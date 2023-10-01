#pragma once
#include "pch.h"

#include <shobjidl_core.h>

#include <windows.ui.notifications.h>
#include <atlstr.h>


//set 0 if you compile to 'static library'
#if 1
#if EasyWin_EXPORTS 
#define EasyWin_API __declspec(dllexport)
#else
#define EasyWin_API __declspec(dllimport)
#endif
#else
#define EasyWin_API
#endif

using namespace ABI::Windows::UI::Notifications;
using namespace ABI::Windows::Data::Xml::Dom;

namespace EasyWinNoty {
	typedef DWORD(__stdcall *TNotificationCB)(PVOID pNoty, DWORD eventType, DWORD args, PVOID userData);

	enum XToastTemplateType : int
	{
		//work in progress
		ToastTemplateType_ToastImageAndText01 = 0,
		ToastTemplateType_ToastImageAndText02 = 1,
		ToastTemplateType_ToastImageAndText03 = 2,
		ToastTemplateType_ToastImageAndText04 = 3,
		ToastTemplateType_ToastText01 = 4,
		ToastTemplateType_ToastText02 = 5,
		ToastTemplateType_ToastText03 = 6,
		ToastTemplateType_ToastText04 = 7,
	};

	enum XToastEventType : int {
		ActiveWithoutParams = 0,
		ActiveWithParams = 1,
		UserCancel = 2,
		ApplicationHide = 3,
		Timeout = 4
	};

	class EasyWin_API CEasyWinNotification {
	public:
		BOOL IsSupportSystem();
		BOOL IsSupportAdvancedFeature();

		static HRESULT RegisterForSystem(LPCWSTR programName, LPCWSTR appId);
		static HRESULT SetStartupShortcut(LPCWSTR exePath, LPCWSTR programName, LPCWSTR appId);
		static BOOL ValidateStartupShortcut(LPCWSTR lnkPath, LPCWSTR appId);

		HRESULT Initialize(LPCWSTR programName,LPCWSTR appId, XToastTemplateType notyType);
		//show noty without shortcut
		HRESULT InitializeWithoutShortCut(LPCWSTR programName, LPCWSTR activatorClsId, LPCWSTR iconPath, LPCWSTR iconBackgroundColor, XToastTemplateType notyType);


		HRESULT Show();
		HRESULT Update();
		HRESULT Hide();
		void Cleanup();
		void RemoveShortcut();
		void SetNotificationCallback(TNotificationCB cb, PVOID userData);

		HRESULT SetText(LPCWSTR text, DWORD line);
		HRESULT SetButton(LPCWSTR text, DWORD index);
		HRESULT SetProgressBar(LPCWSTR progressId);
		HRESULT SetProgressValue(LPCWSTR progressTitle, DOUBLE progressValue, LPCWSTR progressValueStr, LPCWSTR progressStatus);

		//work in progress
		//HRESULT LoadCustomTemplate(LPCWSTR xmlDoc);
		//HRESULT LoadCustomTemplateFromFile(LPCWSTR xmlPath);


		//need Release outside (for advanced template)
		//callback arguments must be 'INT' type
		IXmlDocument* GetRawTemplate();

	private:

		LPWSTR _GetAppId();

		DWORD _WriteRegistryForNoShortcutMode(LPCWSTR appId, LPCWSTR programName, LPCWSTR activatorId, LPCWSTR iconUri, LPCWSTR iconBackground);
		DWORD _RemoveRegistryForNoShortcutMode();

		void _NotyActivatedEventHander(IToastNotification* noty, IInspectable* ins);
		void _NotyDismissedEventHander(IToastNotification* noty, IToastDismissedEventArgs* args);

		BOOL _Initialized = FALSE;
		HSTRING _appId = NULL;
		TNotificationCB cb = NULL;
		PVOID userData = NULL;

		//progressValue
		HSTRING _progressValue = NULL;
		HSTRING _progrsssValueStr = NULL;
		HSTRING _progressTitle = NULL;
		HSTRING _progressStatus = NULL;
		HSTRING _progressTag = NULL;

		IToastNotificationManagerStatics* _notyManager = NULL;
		IToastNotifier* toastNotifier = NULL;
		IToastNotification* noty = NULL;

		//event token
		EventRegistrationToken activatedToken = { 0, };
		EventRegistrationToken dismissedToken = { 0, };

		//data
		IXmlDocument* _notyTemplate = NULL;

		//for shortcut
		CString _currentExePath;
		CString _programName;

		//for registry
		HKEY _key = NULL;

		//----------HSTRING HELPER------------
		class _HSTRINGHelper {
		public:
			_HSTRINGHelper(LPCWSTR str);
			_HSTRINGHelper(DWORD val);

			~_HSTRINGHelper();

			static HSTRING CreateHStr(LPCWSTR str);
			static HSTRING CreateHStr(DOUBLE val);

			HSTRING GetHStr() {
				return tmp;
			}

		private:
			HSTRING tmp = NULL;
		};
	};
}
