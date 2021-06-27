#pragma once
//C Style wrapper

#ifndef EasyWinNoty_TemplateType_Text01
#define EasyWinNoty_TemplateType_Text01 4
#define EasyWinNoty_TemplateType_Text02 5
#define EasyWinNoty_TemplateType_Text03 6
#define EasyWinNoty_TemplateType_Text04 7

#define EasyWinNoty_EventType_ActiveWithoutParams 0
#define EasyWinNoty_EventType_ActiveWithParams 1
#define EasyWinNoty_EventType_UserCancel 2
#define EasyWinNoty_EventType_ApplicationHide 3
#define EasyWinNoty_EventType_Timeout 4
#endif

#if __cplusplus
extern "C" {
#endif
	typedef struct _EasyWinNoty {
		int unused;
	} EASYWINNOTY, *PEASYWINNOTY;

	typedef DWORD(__stdcall *EasyWinNoty_NotificationCB)(PVOID pNoty, DWORD eventType, DWORD args, PVOID userData);

	__declspec(dllexport) PEASYWINNOTY __cdecl EasyWinNoty_CreateInstance();
	__declspec(dllexport) BOOL __cdecl EasyWinNoty_IsSupportSystem(PEASYWINNOTY pNoty);
	__declspec(dllexport) BOOL __cdecl EasyWinNoty_IsSupportAdvancedFeature(PEASYWINNOTY pNoty);

	__declspec(dllexport) HRESULT __cdecl EasyWinNoty_Initialize(PEASYWINNOTY pNoty,LPCWSTR programName, LPCWSTR appId, DWORD notyType);

	__declspec(dllexport) HRESULT __cdecl EasyWinNoty_Show(PEASYWINNOTY pNoty);
	__declspec(dllexport) HRESULT __cdecl EasyWinNoty_Update(PEASYWINNOTY pNoty);
	__declspec(dllexport) HRESULT __cdecl EasyWinNoty_Hide(PEASYWINNOTY pNoty);
	__declspec(dllexport) void __cdecl EasyWinNoty_Cleanup(PEASYWINNOTY pNoty);
	__declspec(dllexport) void __cdecl EasyWinNoty_RemoveShortcut(PEASYWINNOTY pNoty);
	__declspec(dllexport) void __cdecl EasyWinNoty_SetNotificationCallback(PEASYWINNOTY pNoty, EasyWinNoty_NotificationCB cb, PVOID userData);

	__declspec(dllexport) HRESULT __cdecl EasyWinNoty_SetText(PEASYWINNOTY pNoty,LPCWSTR text, DWORD line);
	__declspec(dllexport) HRESULT __cdecl EasyWinNoty_SetButton(PEASYWINNOTY pNoty,LPCWSTR text, DWORD index);
	__declspec(dllexport) HRESULT __cdecl EasyWinNoty_SetProgressBar(PEASYWINNOTY pNoty,LPCWSTR progressId);
	__declspec(dllexport) HRESULT __cdecl EasyWinNoty_SetProgressValue(PEASYWINNOTY pNoty,LPCWSTR progressTitle, DOUBLE progressValue, LPCWSTR progressValueStr, LPCWSTR progressStatus);

	__declspec(dllexport) PVOID __cdecl EasyWinNoty_GetRawTemplate(PEASYWINNOTY pNoty);
	__declspec(dllexport) void __cdecl EasyWinNoty_DeleteInstance(PEASYWINNOTY pNoty);

#if __cplusplus
}
#endif