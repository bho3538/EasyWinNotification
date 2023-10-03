﻿#include "pch.h"
#include "EasyWinNotification.h"

#include <propsys.h>
#include <propkey.h>
#include <propvarutil.h>
#include <strsafe.h>
#include <wrl/event.h>
#include <VersionHelpers.h>

#include <shlobj_core.h>


interface IApplicationResolver : public IUnknown
{
	STDMETHOD(dummy)();
	STDMETHOD(dummy2)();
	STDMETHOD(dummy3)();
	STDMETHOD(GetAppIDForProcess)(DWORD dwProcessId, LPWSTR* ppszAppId, PBOOL pfPinningPrevented, PBOOL pfExplicitAppID, PBOOL pfEmbeddedShortcutValid);
};

using namespace EasyWinNoty;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::Foundation;

typedef HRESULT(__stdcall *TRoGetActivationFactory)(HSTRING activatableClassId, REFIID iid, void ** factory);
typedef HRESULT(__stdcall *TWindowsCreateString)(PCWSTR sourceString, UINT32 length, HSTRING * string);
typedef HRESULT(__stdcall *TWindowsDeleteString)(HSTRING string);
typedef LPCWSTR(__stdcall *TWindowsGetStringRawBuffer)(HSTRING hstr, UINT* length);
typedef NTSTATUS(__stdcall *TRtlGetVersion)(OSVERSIONINFOEXW *ovf);

//-----DLL load function
TRoGetActivationFactory _RoGetActivationFactory = NULL;
TWindowsCreateString _WindowsCreateString = NULL;
TWindowsDeleteString _WindowsDeleteString = NULL;
TWindowsGetStringRawBuffer _WindowsGetStringRawBuffer = NULL;
TRtlGetVersion _RtlGetVersion = NULL;

BOOL CEasyWinNotification::IsSupportSystem() {
	if (IsWindows8OrGreater()) {
		return TRUE;
	}

	return FALSE;
}
BOOL CEasyWinNotification::IsSupportAdvancedFeature() {
	OSVERSIONINFOEXW verInfo = { 0, };
	NTSTATUS status = 0;

	if (!_RtlGetVersion) {
		_RtlGetVersion = (TRtlGetVersion)GetProcAddress(LoadLibraryW(L"ntdll.dll"), "RtlGetVersion");
		if (!_RtlGetVersion) {
			return FALSE;
		}
	}

	_RtlGetVersion(&verInfo);

	if (verInfo.dwMajorVersion >= 10) {
		return TRUE;
	}

	return FALSE;
}

HRESULT CEasyWinNotification::RegisterForSystem(LPCWSTR programName, LPCWSTR appId) {
	WCHAR currentPath[MAX_PATH];
	if (!appId || wcslen(appId) == 0 || !programName || wcslen(programName) == 0) {
		return E_INVALIDARG;
	}

	GetModuleFileNameW(NULL, currentPath, MAX_PATH);

	return CEasyWinNotification::SetStartupShortcut(currentPath, programName, appId);
}

HRESULT CEasyWinNotification::InitializeWithoutShortCut(LPCWSTR programName, LPCWSTR activatorClsId, LPCWSTR iconPath, LPCWSTR iconBackgroundColor, XToastTemplateType notyType) {
	HRESULT hr = 0;
	LPWSTR appId = _GetAppId();

	this->_programName = programName;

	//write registry at system
	_WriteRegistryForNoShortcutMode(appId, programName, activatorClsId, iconPath, iconBackgroundColor);

	//for check unsupported os
	if (!_WindowsCreateString) {
		_WindowsCreateString = (TWindowsCreateString)GetProcAddress(LoadLibraryW(L"Combase.dll"), "WindowsCreateString");
		if (!_WindowsCreateString) {
			hr = E_FAIL;
			goto escapeArea;
		}
	}

	if (!_RoGetActivationFactory) {
		_RoGetActivationFactory = (TRoGetActivationFactory)GetProcAddress(LoadLibraryW(L"Combase.dll"), "RoGetActivationFactory");
		if (!_RoGetActivationFactory) {
			hr = E_FAIL;
			goto escapeArea;
		}
	}

	if (!_WindowsDeleteString) {
		_WindowsDeleteString = (TWindowsDeleteString)GetProcAddress(LoadLibraryW(L"Combase.dll"), "WindowsDeleteString");
		if (!_WindowsDeleteString) {
			hr = E_FAIL;
			goto escapeArea;
		}
	}

	if (!_WindowsGetStringRawBuffer) {
		_WindowsGetStringRawBuffer = (TWindowsGetStringRawBuffer)GetProcAddress(LoadLibraryW(L"Combase.dll"), "WindowsGetStringRawBuffer");
		if (!_WindowsGetStringRawBuffer) {
			hr = E_FAIL;
			goto escapeArea;
		}
	}

	_WindowsCreateString(appId, wcslen(appId), &this->_appId);
	if (!this->_appId) {
		hr = E_FAIL;
		goto escapeArea;
	}

	if (this->_notyManager) {
		this->_notyManager->Release();
		this->_notyManager = NULL;
	}

	hr = _RoGetActivationFactory(_HSTRINGHelper(RuntimeClass_Windows_UI_Notifications_ToastNotificationManager).GetHStr(), IID_IToastNotificationManagerStatics, (PVOID*)&this->_notyManager);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = this->_notyManager->GetTemplateContent((ABI::Windows::UI::Notifications::ToastTemplateType)notyType, &_notyTemplate);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	_Initialized = 2;

escapeArea:

	CoTaskMemFree(appId);

	return hr;
}

DWORD CEasyWinNotification::_WriteRegistryForNoShortcutMode(LPCWSTR appId, LPCWSTR programName, LPCWSTR activatorId, LPCWSTR iconUri, LPCWSTR iconBackground) {
	if (!appId || !programName) {
		return 1;
	}
	DWORD len = 0;
	CString mainKey = L"Software\\Classes\\AppUserModelId\\";
	mainKey += appId;


	RegCreateKeyExW(HKEY_CURRENT_USER, mainKey.GetBuffer(), 0, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &this->_key, NULL);
	if (this->_key == NULL) {
		return 1;
	}

	if (!activatorId || wcslen(activatorId) == 0) {
		activatorId = L"{00000000-0000-0000-0000-000000000000}";
	}

	len = ((DWORD)wcslen(activatorId) + 1) * 2;
	RegSetKeyValueW(this->_key, NULL, L"CustomActivator", REG_SZ, activatorId, len);

	len = ((DWORD)wcslen(programName) + 1) * 2;
	RegSetKeyValueW(this->_key, NULL, L"DisplayName", REG_SZ, programName, len);

	return 0;
}
DWORD CEasyWinNotification::_RemoveRegistryForNoShortcutMode() {
	if (!this->_appId) {
		return 1;
	}

	if (this->_key) {
		RegDeleteKeyW(this->_key, L"");
		RegCloseKey(this->_key);
		this->_key = NULL;
	}

	return 0;
}

LPWSTR CEasyWinNotification::_GetAppId() {
	GUID CLSID_ApplicationResolver = { 0x660b90c8,0x73a9,0x4b58,{0x8c,0xae,0x35,0x5b,0x7f,0x55,0x34,0x1b} };
	GUID IID_IApplicationResolver = { 0xde25675a,0x72de,0x44b4,{0x93,0x73,0x05,0x17,0x04,0x50,0xc1,0x40} };

	IApplicationResolver* pResolver = NULL;
	if (CoCreateInstance(CLSID_ApplicationResolver, NULL, CLSCTX_INPROC_SERVER, IID_IApplicationResolver, (PVOID*)&pResolver) != S_OK) {
		return NULL;
	}

	LPWSTR id = NULL;
	pResolver->GetAppIDForProcess(GetCurrentProcessId(), &id, NULL, NULL, NULL);

	pResolver->Release();

	if (!id) {
		return NULL;
	}

	DWORD len = (DWORD)wcslen(id);
	for (DWORD i = 0; i < len; i++) {
		if (id[i] == L'\\') {
			id[i] = L'/';
		}
	}

	return id;
}

HRESULT CEasyWinNotification::Initialize(LPCWSTR programName, LPCWSTR appId, XToastTemplateType notyType) {
	HRESULT hr = 0;
	WCHAR currentPath[MAX_PATH];

	if (!appId || wcslen(appId) == 0 || !programName || wcslen(programName) == 0) {
		hr = E_INVALIDARG;
		goto escapeArea;
	}

	SetCurrentProcessExplicitAppUserModelID(appId);

	GetModuleFileNameW(NULL, currentPath, MAX_PATH);

	this->_programName = programName;
	this->_currentExePath = currentPath;

	//for check unsupported os
	if (!_WindowsCreateString) {
		_WindowsCreateString = (TWindowsCreateString)GetProcAddress(LoadLibraryW(L"Combase.dll"), "WindowsCreateString");
		if (!_WindowsCreateString) {
			hr = E_FAIL;
			goto escapeArea;
		}
	}

	if (!_RoGetActivationFactory) {
		_RoGetActivationFactory = (TRoGetActivationFactory)GetProcAddress(LoadLibraryW(L"Combase.dll"), "RoGetActivationFactory");
		if (!_RoGetActivationFactory) {
			hr = E_FAIL;
			goto escapeArea;
		}
	}

	if (!_WindowsDeleteString) {
		_WindowsDeleteString = (TWindowsDeleteString)GetProcAddress(LoadLibraryW(L"Combase.dll"), "WindowsDeleteString");
		if (!_WindowsDeleteString) {
			hr = E_FAIL;
			goto escapeArea;
		}
	}

	if (!_WindowsGetStringRawBuffer) {
		_WindowsGetStringRawBuffer = (TWindowsGetStringRawBuffer)GetProcAddress(LoadLibraryW(L"Combase.dll"), "WindowsGetStringRawBuffer");
		if (!_WindowsGetStringRawBuffer) {
			hr = E_FAIL;
			goto escapeArea;
		}
	}

	_WindowsCreateString(appId, wcslen(appId), &this->_appId);
	if (!this->_appId) {
		hr = E_FAIL;
		goto escapeArea;
	}

	hr = SetStartupShortcut(this->_currentExePath, this->_programName, _WindowsGetStringRawBuffer(this->_appId,NULL));
	if (FAILED(hr)) {
		goto escapeArea;
	}

	if (this->_notyManager) {
		this->_notyManager->Release();
		this->_notyManager = NULL;
	}

	hr = _RoGetActivationFactory(_HSTRINGHelper(RuntimeClass_Windows_UI_Notifications_ToastNotificationManager).GetHStr(), IID_IToastNotificationManagerStatics,(PVOID*)&this->_notyManager);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = this->_notyManager->GetTemplateContent((ABI::Windows::UI::Notifications::ToastTemplateType)notyType, &_notyTemplate);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	_Initialized = 1;

	escapeArea:

	return hr;
}

void CEasyWinNotification::SetNotificationCallback(TNotificationCB cb, PVOID userData) {
	this->cb = cb;
	this->userData = userData;
}

void CEasyWinNotification::SetNotificationCallbackEx(TNotificationCBEx cbEx, PVOID userData) {
	this->cbEx = cbEx;
	this->userData = userData;
}

HRESULT CEasyWinNotification::Show() {
	HRESULT hr = E_FAIL;
	IToastNotificationFactory* notificationFactory = NULL;
	IToastNotification2* noty2 = NULL;

	if (!_Initialized) {
		return hr;
	}

	if (!toastNotifier) {
		hr = this->_notyManager->CreateToastNotifierWithId(this->_appId, &toastNotifier);

		if (FAILED(hr)) {
			goto escapeArea;
		}
	}

	hr = _RoGetActivationFactory(_HSTRINGHelper(RuntimeClass_Windows_UI_Notifications_ToastNotification).GetHStr(), IID_IToastNotificationFactory, (PVOID*)&notificationFactory);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	if (this->noty) {
		this->noty->remove_Activated(this->activatedToken);
		this->noty->remove_Dismissed(this->dismissedToken);

		this->noty->Release();
		this->noty = NULL;
	}

	hr = notificationFactory->CreateToastNotification(this->_notyTemplate, &noty);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	if (this->_progressTag) {
		hr = noty->QueryInterface(IID_IToastNotification2, (PVOID*)&noty2);
		if (SUCCEEDED(hr)) {
			noty2->put_Tag(this->_progressTag);
		}
	}


	hr = noty->add_Activated(
		Callback < Implements < RuntimeClassFlags<ClassicCom>,
		ITypedEventHandler<ToastNotification*, IInspectable* >> >(
		[this](IToastNotification* noty, IInspectable* ins)
	{
		this->_NotyActivatedEventHander(noty, ins);
		return S_OK;
	}).Get(), &activatedToken);

	hr = noty->add_Dismissed(
		Callback < Implements < RuntimeClassFlags<ClassicCom>,
		ITypedEventHandler<ToastNotification*, ToastDismissedEventArgs* >> >(
			[this](IToastNotification* noty, IToastDismissedEventArgs* ins)
	{
		this->_NotyDismissedEventHander(noty, ins);
		return S_OK;
	}).Get(), &dismissedToken);


	hr = toastNotifier->Show(noty);

	if (this->_progressValue) {
		Update();
	}


escapeArea:

	if (noty2) {
		noty2->Release();
	}

	if (notificationFactory) {
		notificationFactory->Release();
	}

	return hr;
}
HRESULT CEasyWinNotification::Update() {
	HRESULT hr = S_OK;
	INotificationDataFactory* dataFactory = NULL;
	INotificationData* data = NULL;
	IToastNotifier2* notifier2 = NULL;
	__FIMap_2_HSTRING_HSTRING* value = NULL;

	if (!_Initialized) {
		return E_FAIL;
	}

	hr = _RoGetActivationFactory(_HSTRINGHelper(L"Windows.UI.Notifications.NotificationData").GetHStr(),IID_INotificationDataFactory, (PVOID*)&dataFactory);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = dataFactory->CreateNotificationDataWithValues(NULL, &data);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = data->get_Values(&value);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	if (this->_progressValue) {
		boolean replaced = false;
		value->Insert(_HSTRINGHelper(L"progressValue").GetHStr(), this->_progressValue, &replaced);
		value->Insert(_HSTRINGHelper(L"progressValueString").GetHStr(), this->_progrsssValueStr, &replaced);
		value->Insert(_HSTRINGHelper(L"progressTitle").GetHStr(), this->_progressTitle, &replaced);
		value->Insert(_HSTRINGHelper(L"progressStatus").GetHStr(), this->_progressStatus, &replaced);
	}

	if (this->toastNotifier) {
		toastNotifier->QueryInterface(IID_IToastNotifier2, (PVOID*)&notifier2);
		if (notifier2) {
			NotificationUpdateResult re;
			notifier2->UpdateWithTag(data, this->_progressTag, &re);

			if (re == NotificationUpdateResult::NotificationUpdateResult_Succeeded) {
				hr = S_OK;
			}
			else {
				hr = E_FAIL;
			}

			notifier2->Release();
		}
	}

escapeArea:

	if (value) {
		value->Release();
	}
	if (data) {
		data->Release();
	}
	if (dataFactory) {
		dataFactory->Release();
	}

	return hr;
}

HRESULT CEasyWinNotification::Hide() {
	if (this->noty) {
		this->toastNotifier->Hide(noty);
	}

	return S_OK;
}

void CEasyWinNotification::Cleanup() {
	if (!_Initialized) {
		return;
	}

	if (this->_notyManager) {
		this->_notyManager->Release();
		this->_notyManager = NULL;
	}

	if (this->toastNotifier) {
		this->toastNotifier->Release();
		this->toastNotifier = NULL;
	}

	if (this->_notyTemplate) {
		this->_notyTemplate->Release();
		this->_notyTemplate = NULL;
	}

	if (this->noty) {
		this->noty->remove_Activated(this->activatedToken);
		this->noty->remove_Dismissed(this->dismissedToken);
		this->noty->Release();
		this->noty = NULL;
	}

	if (_Initialized == 2) {
		_RemoveRegistryForNoShortcutMode();
	}

	_WindowsDeleteString(this->_appId);
	_appId = NULL;

	_WindowsDeleteString(this->_progressTag);
	_progressTag = NULL;

	_WindowsDeleteString(this->_progressValue);
	_progressValue = NULL;
	_WindowsDeleteString(this->_progrsssValueStr);
	_progrsssValueStr = NULL;
	_WindowsDeleteString(this->_progressTitle);
	_progressTitle = NULL;
	_WindowsDeleteString(this->_progressStatus);
	_progressStatus = NULL;

	_Initialized = FALSE;
}

HRESULT CEasyWinNotification::SetText(LPCWSTR text, DWORD line) {
	HRESULT hr = E_FAIL;
	IXmlNodeList* nodeList = NULL;
	IXmlNode* node = NULL;
	IXmlText* textNode = NULL;
	IXmlNode* newNode = NULL;
	IXmlNode* dummy = NULL;

	if (!_Initialized) {
		goto escapeArea;
	}

	if (!this->_notyTemplate) {
		goto escapeArea;
	}

	
	hr = this->_notyTemplate->GetElementsByTagName(_HSTRINGHelper(L"text").GetHStr(), &nodeList);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = nodeList->Item(line, &node);
	if (FAILED(hr) || !node) {
		goto escapeArea;
	}

	hr = this->_notyTemplate->CreateTextNode(_HSTRINGHelper(text).GetHStr(), &textNode);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = textNode->QueryInterface(IID_IXmlNode, (PVOID*)&newNode);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = node->AppendChild(newNode, &dummy);

escapeArea:

	if (newNode) {
		newNode->Release();
	}
	if (textNode) {
		textNode->Release();
	}
	if (node) {
		node->Release();
	}
	if (nodeList) {
		nodeList->Release();
	}
	if (dummy) {
		dummy->Release();
	}

	return hr;
}

HRESULT CEasyWinNotification::SetButton(LPCWSTR text, DWORD args) {
	HRESULT hr = E_FAIL;
	IXmlNodeList* nodeList = NULL;
	IXmlNode* node = NULL;
	IXmlElement* actionElement = NULL;
	IXmlNode* newNode = NULL;
	IXmlNode* dummy = NULL;

	if (!_Initialized) {
		goto escapeArea;
	}

	if (!this->_notyTemplate) {
		goto escapeArea;
	}

	hr = this->_notyTemplate->GetElementsByTagName(_HSTRINGHelper(L"actions").GetHStr(), &nodeList);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = nodeList->Item(0, &node);
	if (FAILED(hr)) {
		goto escapeArea;
	}
	else if (!node) {
		//actions node does not exist (firsttime need create)
		IXmlNode* createNode = NULL;
		IXmlElement* actionsElement = NULL;
		IXmlNode* actionsNode = NULL;

		nodeList->Release();
		hr = this->_notyTemplate->GetElementsByTagName(_HSTRINGHelper(L"toast").GetHStr(), &nodeList);
		if (FAILED(hr)) {
			goto escapeArea;
		}

		hr = nodeList->Item(0, &createNode);
		if (FAILED(hr) || !createNode) {
			goto escapeArea;
		}

		hr = createNode->QueryInterface(IID_IXmlElement, (PVOID*)&actionsElement);
		if (FAILED(hr)) {
			goto escapeArea;
		}

		hr = actionsElement->SetAttribute(_HSTRINGHelper(L"template").GetHStr(), _HSTRINGHelper(L"ToastGeneric").GetHStr());

		actionsElement->Release();


		this->_notyTemplate->CreateElement(_HSTRINGHelper(L"actions").GetHStr(), &actionsElement);

		hr = actionsElement->QueryInterface(IID_IXmlNode, (PVOID*)&actionsNode);
		if (FAILED(hr)) {
			goto escapeArea;
		}

		hr = createNode->AppendChild(actionsNode, &node);
		if (FAILED(hr)) {
			goto escapeArea;
		}

		if (createNode) {
			createNode->Release();
		}

		if (actionsElement) {
			actionsElement->Release();
		}

		if (actionsNode) {
			actionsNode->Release();
		}
	}

	hr = this->_notyTemplate->CreateElement(_HSTRINGHelper(L"action").GetHStr(), &actionElement);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	actionElement->SetAttribute(_HSTRINGHelper(L"content").GetHStr(), _HSTRINGHelper(text).GetHStr());
	actionElement->SetAttribute(_HSTRINGHelper(L"arguments").GetHStr(), _HSTRINGHelper(args).GetHStr());


	hr = actionElement->QueryInterface(IID_IXmlNode, (PVOID*)&newNode);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = node->AppendChild(newNode, &dummy);

escapeArea:

	if (newNode) {
		newNode->Release();
	}
	if (actionElement) {
		actionElement->Release();
	}
	if (node) {
		node->Release();
	}
	if (nodeList) {
		nodeList->Release();
	}
	if (dummy) {
		dummy->Release();
	}

	return hr;
}

HRESULT CEasyWinNotification::SetProgressBar(LPCWSTR progressId) {
	HRESULT hr = E_FAIL;
	IXmlNodeList* nodeList = NULL;
	IXmlNode* node = NULL;
	IXmlElement* progressElement = NULL;
	IXmlNode* newNode = NULL;
	IXmlNode* dummy = NULL;

	if (!_Initialized) {
		goto escapeArea;
	}

	if (!this->_notyTemplate) {
		goto escapeArea;
	}


	hr = this->_notyTemplate->GetElementsByTagName(_HSTRINGHelper(L"binding").GetHStr(), &nodeList);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = nodeList->Item(0, &node);
	if (FAILED(hr) || !node) {
		goto escapeArea;
	}

	hr = node->QueryInterface(IID_IXmlElement, (PVOID*)&progressElement);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	progressElement->SetAttribute(_HSTRINGHelper(L"template").GetHStr(), _HSTRINGHelper(L"ToastGeneric").GetHStr());

	progressElement->Release();
	progressElement = NULL;


	hr = this->_notyTemplate->CreateElement(_HSTRINGHelper(L"progress").GetHStr(), &progressElement);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	progressElement->SetAttribute(_HSTRINGHelper(L"title").GetHStr(), _HSTRINGHelper(L"{progressTitle}").GetHStr());
	progressElement->SetAttribute(_HSTRINGHelper(L"value").GetHStr(), _HSTRINGHelper(L"{progressValue}").GetHStr());
	progressElement->SetAttribute(_HSTRINGHelper(L"valueStringOverride").GetHStr(), _HSTRINGHelper(L"{progressValueString}").GetHStr());
	progressElement->SetAttribute(_HSTRINGHelper(L"status").GetHStr(), _HSTRINGHelper(L"{progressStatus}").GetHStr());

	hr = progressElement->QueryInterface(IID_IXmlNode, (PVOID*)&newNode);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = node->AppendChild(newNode, &dummy);

	if (this->_progressTag) {
		_WindowsDeleteString(this->_progressTag);
	}
	this->_progressTag = _HSTRINGHelper::CreateHStr(progressId);

escapeArea:

	if (newNode) {
		newNode->Release();
	}
	if (progressElement) {
		progressElement->Release();
	}
	if (node) {
		node->Release();
	}
	if (nodeList) {
		nodeList->Release();
	}
	if (dummy) {
		dummy->Release();
	}

	return hr;
}
HRESULT CEasyWinNotification::SetProgressValue(LPCWSTR progressTitle, DOUBLE progressValue, LPCWSTR progressValueStr, LPCWSTR progressStatus) {
	if (_progressTitle) {
		_WindowsDeleteString(_progressTitle);
	}
	this->_progressTitle = _HSTRINGHelper::CreateHStr(progressTitle);

	if (_progressValue) {
		_WindowsDeleteString(_progressValue);
	}
	if (progressValue < 0) {
		this->_progressValue = _HSTRINGHelper::CreateHStr(L"indeterminate");
	}
	else {
		this->_progressValue = _HSTRINGHelper::CreateHStr(progressValue);
	}

	if (_progrsssValueStr) {
		_WindowsDeleteString(_progrsssValueStr);
	}
	this->_progrsssValueStr = _HSTRINGHelper::CreateHStr(progressValueStr);

	if (_progressStatus) {
		_WindowsDeleteString(_progressStatus);
	}
	this->_progressStatus = _HSTRINGHelper::CreateHStr(progressStatus);

	return S_OK;
}

HRESULT CEasyWinNotification::SetInputBox(LPCWSTR controlId, LPCWSTR placeholderText) {
	HRESULT hr = E_FAIL;
	IXmlNodeList* nodeList = NULL;
	IXmlNode* node = NULL;
	IXmlElement* inputElement = NULL;
	IXmlNode* newNode = NULL;
	IXmlNode* dummy = NULL;

	if (!_Initialized || !controlId) {
		goto escapeArea;
	}

	if (!this->_notyTemplate) {
		goto escapeArea;
	}

	hr = this->_notyTemplate->GetElementsByTagName(_HSTRINGHelper(L"actions").GetHStr(), &nodeList);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = nodeList->Item(0, &node);
	if (FAILED(hr)) {
		goto escapeArea;
	}
	else if (!node) {
		//actions node does not exist (firsttime need create)
		IXmlNode* createNode = NULL;
		IXmlElement* actionsElement = NULL;
		IXmlNode* actionsNode = NULL;

		nodeList->Release();
		hr = this->_notyTemplate->GetElementsByTagName(_HSTRINGHelper(L"toast").GetHStr(), &nodeList);
		if (FAILED(hr)) {
			goto escapeArea;
		}

		hr = nodeList->Item(0, &createNode);
		if (FAILED(hr) || !createNode) {
			goto escapeArea;
		}

		hr = createNode->QueryInterface(IID_IXmlElement, (PVOID*)&actionsElement);
		if (FAILED(hr)) {
			goto escapeArea;
		}

		hr = actionsElement->SetAttribute(_HSTRINGHelper(L"template").GetHStr(), _HSTRINGHelper(L"ToastGeneric").GetHStr());

		actionsElement->Release();


		this->_notyTemplate->CreateElement(_HSTRINGHelper(L"actions").GetHStr(), &actionsElement);

		hr = actionsElement->QueryInterface(IID_IXmlNode, (PVOID*)&actionsNode);
		if (FAILED(hr)) {
			goto escapeArea;
		}

		hr = createNode->AppendChild(actionsNode, &node);
		if (FAILED(hr)) {
			goto escapeArea;
		}

		if (createNode) {
			createNode->Release();
		}

		if (actionsElement) {
			actionsElement->Release();
		}

		if (actionsNode) {
			actionsNode->Release();
		}
	}

	hr = this->_notyTemplate->CreateElement(_HSTRINGHelper(L"input").GetHStr(), &inputElement);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	inputElement->SetAttribute(_HSTRINGHelper(L"id").GetHStr(), _HSTRINGHelper(controlId).GetHStr());
	inputElement->SetAttribute(_HSTRINGHelper(L"type").GetHStr(), _HSTRINGHelper(L"text").GetHStr());

	if (placeholderText) {
		inputElement->SetAttribute(_HSTRINGHelper(L"placeHolderContent").GetHStr(), _HSTRINGHelper(placeholderText).GetHStr());
	}


	hr = inputElement->QueryInterface(IID_IXmlNode, (PVOID*)&newNode);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = node->AppendChild(newNode, &dummy);

escapeArea:

	if (newNode) {
		newNode->Release();
	}
	if (inputElement) {
		inputElement->Release();
	}
	if (node) {
		node->Release();
	}
	if (nodeList) {
		nodeList->Release();
	}
	if (dummy) {
		dummy->Release();
	}

	return hr;
}

ABI::Windows::Data::Xml::Dom::IXmlDocument* CEasyWinNotification::GetRawTemplate() {
	if (this->_notyTemplate) {
		this->_notyTemplate->AddRef();
	}

	//HSTRING xml;
	//ABI::Windows::Data::Xml::Dom::IXmlNodeSerializer* ser;


	//this->_notyTemplate->QueryInterface(ABI::Windows::Data::Xml::Dom::IID_IXmlNodeSerializer, (PVOID*)&ser);

	//ser->GetXml(&xml);
	return this->_notyTemplate;
}


CEasyWinNotification::_HSTRINGHelper::_HSTRINGHelper(LPCWSTR str) {
	if (_WindowsCreateString) {
		_WindowsCreateString(str, wcslen(str), &tmp);
	}
}

CEasyWinNotification::_HSTRINGHelper::_HSTRINGHelper(DWORD val) {
	WCHAR buffer[24];
	StringCchPrintfW(buffer, sizeof(buffer) / sizeof(WCHAR), L"%d", val);

	if (_WindowsCreateString) {
		_WindowsCreateString(buffer, wcslen(buffer), &tmp);
	}
}

CEasyWinNotification::_HSTRINGHelper::~_HSTRINGHelper() {
	_WindowsDeleteString(tmp);
}

HSTRING CEasyWinNotification::_HSTRINGHelper::CreateHStr(LPCWSTR str) {
	HSTRING aa = NULL;

	if (str && _WindowsCreateString) {
		_WindowsCreateString(str, wcslen(str), &aa);
	}

	return aa;
}

HSTRING CEasyWinNotification::_HSTRINGHelper::CreateHStr(DOUBLE val) {
	WCHAR buffer[24];
	StringCchPrintfW(buffer, sizeof(buffer) / sizeof(WCHAR), L"%lf", val);

	return CreateHStr(buffer);
}

void CEasyWinNotification::_NotyActivatedEventHander(IToastNotification* noty, IInspectable* ins) {
	IToastActivatedEventArgs *args = NULL;
	IToastActivatedEventArgs2* args2 = NULL;
	ABI::Windows::Foundation::Collections::IMap<HSTRING, IInspectable*>* userInputDatas = NULL;
	ABI::Windows::Foundation::Collections::IPropertySet* userInput = NULL;

	ins->QueryInterface(IID_IToastActivatedEventArgs, (PVOID*)&args);

	if (args) {
		HSTRING argument = NULL;
		args->get_Arguments(&argument);

		if (argument) {
			LPCWSTR rawStr = _WindowsGetStringRawBuffer(argument, NULL);

			DWORD dwArgs = wcstoul(rawStr, NULL, 0);


			args->QueryInterface(IID_IToastActivatedEventArgs2, (PVOID*)&args2);
			if (args2) {
				args2->get_UserInput(&userInput);
				if (userInput) {
					userInput->QueryInterface<ABI::Windows::Foundation::Collections::IMap<HSTRING, IInspectable*>>(&userInputDatas);
					userInput->Release();
				}
				args2->Release();
			}


			if (cbEx) {
				cbEx(this, XToastEventType::ActiveWithParams, dwArgs, userData, userInputDatas);
			}
			else if (cb) {
				cb(this, XToastEventType::ActiveWithParams, dwArgs, userData);
			}

			if (userInputDatas) {
				userInputDatas->Release();
			}

			_WindowsDeleteString(argument);
		}
		else {
			if (cbEx) {
				cbEx(this, XToastEventType::ActiveWithoutParams, 0, userData, NULL);
			}
			else if (cb) {
				cb(this, XToastEventType::ActiveWithoutParams, 0, userData);
			}
		}
		args->Release();
	}

}

void CEasyWinNotification::_NotyDismissedEventHander(IToastNotification* noty, IToastDismissedEventArgs* args) {
	ToastDismissalReason reason;
	args->get_Reason(&reason);

	if (cb) {
		XToastEventType eventType = XToastEventType::ApplicationHide;
		if (reason == ToastDismissalReason::ToastDismissalReason_TimedOut) {
			eventType = XToastEventType::Timeout;
		}
		else if (reason == ToastDismissalReason::ToastDismissalReason_UserCanceled) {
			eventType = XToastEventType::UserCancel;
		}

		cb(this, eventType, 0, userData);
	}
}

HRESULT CEasyWinNotification::SetStartupShortcut(LPCWSTR exePath, LPCWSTR programName, LPCWSTR appId) {
	IShellLinkW* shellLink = NULL;
	IPersistFile *pf = NULL;
	IPropertyStore* ps = NULL;
	PROPVARIANT appIdInfo = { 0, };
	HRESULT hr = E_FAIL;
	CString path;

#pragma warning(suppress: 4996)
	path = getenv("AppData");
	path += L"\\Microsoft\\Windows\\Start Menu\\Programs\\";
	path += programName;
	path += L".lnk";

	if (ValidateStartupShortcut(path, appId)) {
		hr = S_OK;
		goto escapeArea;
	}

	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW,(LPVOID*)&shellLink);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = shellLink->SetPath(exePath);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = shellLink->QueryInterface(IID_IPropertyStore, (PVOID*)&ps);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = InitPropVariantFromString(appId, &appIdInfo);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = ps->SetValue(PKEY_AppUserModel_ID, appIdInfo);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	ps->Commit();

	hr = shellLink->QueryInterface(IID_IPersistFile, (PVOID*)&pf);
	if (FAILED(hr)) {
		goto escapeArea;
	}

	hr = pf->Save(path.GetString(),TRUE);

	pf->SaveCompleted(path.GetString());

escapeArea:

	PropVariantClear(&appIdInfo);

	if (ps) {
		ps->Release();
	}

	if (pf) {
		pf->Release();
	}

	if (shellLink) {
		shellLink->Release();
	}

	return hr;
}

BOOL CEasyWinNotification::ValidateStartupShortcut(LPCWSTR lnkPath, LPCWSTR appId) {
	BOOL re = FALSE;
	IShellFolder* pSF = NULL;
	IPropertyStore* ps = NULL;
	LPITEMIDLIST pItem = NULL;
	PROPVARIANT prop = { 0, };

	if (GetFileAttributesW(lnkPath) == INVALID_FILE_ATTRIBUTES) {
		goto escapeArea;
	}

	SHGetDesktopFolder(&pSF);
	if (!pSF) {
		goto escapeArea;
	}

	pSF->ParseDisplayName(NULL, NULL, (LPWSTR)lnkPath, NULL, &pItem, NULL);
	if (!pItem) {
		goto escapeArea;
	}

	pSF->BindToObject(pItem, NULL, IID_IPropertyStore, (void**)&ps);
	if (!ps) {
		goto escapeArea;
	}

	ps->GetValue(PKEY_AppUserModel_ID, &prop);
	if (prop.vt != VT_LPWSTR) {
		goto escapeArea;
	}

	if (wcscmp(prop.pwszVal, appId)) {
		goto escapeArea;
	}

	re = TRUE;
escapeArea:

	if (pItem) {
		ILFree(pItem);
	}

	PropVariantClear(&prop);
	if (ps) {
		ps->Release();
	}
	if (pSF) {
		pSF->Release();
	}

	return re;
}

void CEasyWinNotification::RemoveShortcut() {
	CString path;
#pragma warning(suppress: 4996)
	path = getenv("AppData");
	path += L"\\Microsoft\\Windows\\Start Menu\\Programs\\";
	path += _programName;
	path += L".lnk";

	DWORD fileAttr = GetFileAttributesW(path.GetString());
	if (fileAttr == INVALID_FILE_ATTRIBUTES) {
		return;
	}

	DeleteFileW(path);
}

LPWSTR CEasyWinNotification::GetInputData(LPCWSTR controlId, PVOID userInputs) {
	ABI::Windows::Foundation::Collections::IMap<HSTRING, IInspectable*>* userInputDatas = NULL;
	ABI::Windows::Foundation::IPropertyValue* val = NULL;
	IInspectable* tmp = NULL;
	LPWSTR data = NULL;

	if (!controlId || !userInputs) {
		return NULL;
	}

	userInputDatas = (ABI::Windows::Foundation::Collections::IMap<HSTRING, IInspectable*>*)userInputs;
	userInputDatas->Lookup(_HSTRINGHelper(controlId).GetHStr(), &tmp);

	if (tmp) {
		tmp->QueryInterface<ABI::Windows::Foundation::IPropertyValue>(&val);
		if (val) {
			HSTRING str = NULL;
			val->GetString(&str);

			UINT dataLen = 0;
			LPCWSTR strData = _WindowsGetStringRawBuffer(str, &dataLen);
			if (strData) {
				data = (LPWSTR)CoTaskMemAlloc((dataLen + 1) * sizeof(WCHAR));
				if (data) {
					wcscpy_s(data, dataLen + 1, strData);
				}

				_WindowsDeleteString(str);
			}
			val->Release();
		}
		tmp->Release();
	}

	return data;
}