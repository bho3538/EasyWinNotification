#include "pch.h"
#include "EasyWinNotyCWrapper.h"

#include "EasyWinNotification.h"

using namespace EasyWinNoty;

typedef struct _EasyWinNotyInt {
	int unused;
	CEasyWinNotification* pNoty;
	PVOID userData;
	EasyWinNoty_NotificationCB cb;
	EasyWinNoty_NotificationCBEx cbEx;
} EASYWINNOTYINT, *PEASYWINNOTYINT;

DWORD __stdcall _EasyWinNoty_CallbackWrapper(PVOID pNoty, DWORD eventType, DWORD args, PVOID userData);
DWORD __stdcall _EasyWinNoty_CallbackWrapperEx(PVOID pNoty, DWORD eventType, DWORD args, PVOID userData, PVOID inputDatas);

__declspec(dllexport) PEASYWINNOTY __cdecl EasyWinNoty_CreateInstance() {
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)malloc(sizeof(EASYWINNOTYINT));
	if (pEasyNotyInstance) {
		pEasyNotyInstance->unused = 1235;
		pEasyNotyInstance->pNoty = new CEasyWinNotification();
		pEasyNotyInstance->userData = NULL;
		pEasyNotyInstance->cb = NULL;
	}
	return (PEASYWINNOTY)pEasyNotyInstance;
}
__declspec(dllexport) BOOL __cdecl EasyWinNoty_IsSupportSystem(PEASYWINNOTY pNoty) {
	BOOL re = FALSE;
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		re = pEasyNotyInstance->pNoty->IsSupportSystem();
	}
	return re;
}
__declspec(dllexport) BOOL __cdecl EasyWinNoty_IsSupportAdvancedFeature(PEASYWINNOTY pNoty) {
	BOOL re = FALSE;
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		re = pEasyNotyInstance->pNoty->IsSupportAdvancedFeature();
	}
	return re;
}

__declspec(dllexport) BOOL __cdecl EasyWinNoty_IsSystemSupportInputBox(PEASYWINNOTY pNoty) {
	BOOL re = FALSE;
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		re = pEasyNotyInstance->pNoty->IsSystemSupportInputBox();
	}
	return re;
}

__declspec(dllexport) HRESULT __cdecl EasyWinNoty_RegisterForSystem(LPCWSTR programName, LPCWSTR appId) {
	return CEasyWinNotification::RegisterForSystem(programName, appId);
}

__declspec(dllexport) HRESULT __cdecl EasyWinNoty_Initialize(PEASYWINNOTY pNoty, LPCWSTR programName, LPCWSTR appId, DWORD notyType) {
	HRESULT hr = S_OK;
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		hr = pEasyNotyInstance->pNoty->Initialize(programName, appId, (XToastTemplateType)notyType);
	}
	return hr;
}

__declspec(dllexport) HRESULT __cdecl EasyWinNoty_InitializeWithoutShortcut(PEASYWINNOTY pNoty, LPCWSTR programName, LPCWSTR activatorClsId, LPCWSTR iconPath, LPCWSTR iconBackgroundColor, DWORD notyType) {
	HRESULT hr = S_OK;
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		hr = pEasyNotyInstance->pNoty->InitializeWithoutShortCut(programName, activatorClsId, iconPath, iconBackgroundColor, (XToastTemplateType)notyType);
	}
	return hr;
}

__declspec(dllexport) HRESULT __cdecl EasyWinNoty_Show(PEASYWINNOTY pNoty) {
	HRESULT hr = S_OK;
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		hr = pEasyNotyInstance->pNoty->Show();
	}
	return hr;
}
__declspec(dllexport) HRESULT __cdecl EasyWinNoty_Update(PEASYWINNOTY pNoty) {
	HRESULT hr = S_OK;
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		hr = pEasyNotyInstance->pNoty->Update();
	}
	return hr;
}
__declspec(dllexport) HRESULT __cdecl EasyWinNoty_Hide(PEASYWINNOTY pNoty) {
	HRESULT hr = S_OK;
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		hr = pEasyNotyInstance->pNoty->Hide();
	}
	return hr;
}
__declspec(dllexport) void __cdecl EasyWinNoty_Cleanup(PEASYWINNOTY pNoty) {
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		pEasyNotyInstance->pNoty->Cleanup();
	}
}
__declspec(dllexport) void __cdecl EasyWinNoty_RemoveShortcut(PEASYWINNOTY pNoty) {
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		pEasyNotyInstance->pNoty->RemoveShortcut();
	}
}
__declspec(dllexport) void __cdecl EasyWinNoty_SetNotificationCallback(PEASYWINNOTY pNoty, EasyWinNoty_NotificationCB cb, PVOID userData) {
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		pEasyNotyInstance->userData = userData;
		pEasyNotyInstance->cb = cb;
		pEasyNotyInstance->pNoty->SetNotificationCallback(&_EasyWinNoty_CallbackWrapper, pEasyNotyInstance);
	}
}

__declspec(dllexport) void __cdecl EasyWinNoty_SetNotificationCallbackEx(PEASYWINNOTY pNoty, EasyWinNoty_NotificationCBEx cbEx, PVOID userData) {
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		pEasyNotyInstance->userData = userData;
		pEasyNotyInstance->cbEx = cbEx;
		pEasyNotyInstance->pNoty->SetNotificationCallbackEx(&_EasyWinNoty_CallbackWrapperEx, pEasyNotyInstance);
	}
}

__declspec(dllexport) HRESULT __cdecl EasyWinNoty_SetText(PEASYWINNOTY pNoty, LPCWSTR text, DWORD line) {
	HRESULT hr = S_OK;
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		hr = pEasyNotyInstance->pNoty->SetText(text,line);
	}
	return hr;
}
__declspec(dllexport) HRESULT __cdecl EasyWinNoty_SetButton(PEASYWINNOTY pNoty, LPCWSTR text, DWORD index) {
	HRESULT hr = S_OK;
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		hr = pEasyNotyInstance->pNoty->SetButton(text, index);
	}
	return hr;
}

__declspec(dllexport) HRESULT __cdecl EasyWinNoty_SetButtonEx(PEASYWINNOTY pNoty, LPCWSTR text, DWORD index, LPCWSTR inputId, DWORD dwReserved) {
	HRESULT hr = S_OK;
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		hr = pEasyNotyInstance->pNoty->SetButtonEx(text, index, inputId, dwReserved);
	}
	return hr;
}

__declspec(dllexport) HRESULT __cdecl EasyWinNoty_SetProgressBar(PEASYWINNOTY pNoty, LPCWSTR progressId) {
	HRESULT hr = S_OK;
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		hr = pEasyNotyInstance->pNoty->SetProgressBar(progressId);
	}
	return hr;
}
__declspec(dllexport) HRESULT __cdecl EasyWinNoty_SetProgressValue(PEASYWINNOTY pNoty, LPCWSTR progressTitle, DOUBLE progressValue, LPCWSTR progressValueStr, LPCWSTR progressStatus) {
	HRESULT hr = S_OK;
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		hr = pEasyNotyInstance->pNoty->SetProgressValue(progressTitle,progressValue,progressValueStr,progressStatus);
	}
	return hr;
}

__declspec(dllexport) HRESULT __cdecl EasyWinNoty_SetInputBox(PEASYWINNOTY pNoty, LPCWSTR controlId, LPCWSTR placeholderText) {
	HRESULT hr = S_OK;
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		hr = pEasyNotyInstance->pNoty->SetInputBox(controlId, placeholderText);
	}
	return hr;
}

__declspec(dllexport) HRESULT __cdecl EasyWinNoty_SetImage(PEASYWINNOTY pNoty, LPCWSTR imagePath, BOOL overrideLogo, BOOL useCircle) {
	HRESULT hr = S_OK;
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		hr = pEasyNotyInstance->pNoty->SetImage(imagePath, overrideLogo, useCircle);
	}
	return hr;
}

__declspec(dllexport) PVOID __cdecl EasyWinNoty_GetRawTemplate(PEASYWINNOTY pNoty) {
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		return (PVOID)pEasyNotyInstance->pNoty->GetRawTemplate();
	}
	return NULL;
}
__declspec(dllexport) void __cdecl EasyWinNoty_DeleteInstance(PEASYWINNOTY pNoty) {
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)pNoty;
	if (pEasyNotyInstance) {
		delete pEasyNotyInstance->pNoty;
		free(pEasyNotyInstance);
	}
}

DWORD __stdcall _EasyWinNoty_CallbackWrapper(PVOID pNoty, DWORD eventType, DWORD args, PVOID userData) {
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)userData;
	if (!pEasyNotyInstance) {
		return 1; //abnormal
	}
	if (pEasyNotyInstance->cb) {
		return pEasyNotyInstance->cb(pEasyNotyInstance, eventType, args, pEasyNotyInstance->userData);
	}
	return 0;
}

DWORD __stdcall _EasyWinNoty_CallbackWrapperEx(PVOID pNoty, DWORD eventType, DWORD args, PVOID userData, PVOID inputDatas) {
	PEASYWINNOTYINT pEasyNotyInstance = (PEASYWINNOTYINT)userData;
	if (!pEasyNotyInstance) {
		return 1; //abnormal
	}
	if (pEasyNotyInstance->cbEx) {
		return pEasyNotyInstance->cbEx(pEasyNotyInstance, eventType, args, pEasyNotyInstance->userData, inputDatas);
	}
	return 0;
}

__declspec(dllexport) LPWSTR __cdecl EasyWinNoty_GetInputData(LPCWSTR controlId, PVOID userInputs) {
	return CEasyWinNotification::GetInputData(controlId, userInputs);
}