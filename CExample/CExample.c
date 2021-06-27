#include "pch.h"
#include <stdio.h>

#include "../EasyWinNotification/EasyWinNotyCWrapper.h"

#pragma comment(lib,"../Debug/EasyWinNotification.lib")

#include <crtdbg.h>

DWORD WINAPI UpdateProgress(PVOID args) {
	DOUBLE value = 0;
	PEASYWINNOTY pNoty = (PEASYWINNOTY)args;

	for (int i = 0; i < 10; i++) {
		value += 0.1;
		EasyWinNoty_SetProgressValue(pNoty, L"Download File", value, L"10%", L"aaaa.zip");
		EasyWinNoty_Update(pNoty);
		Sleep(500);
	}

	EasyWinNoty_Hide(pNoty);

	return 0;
}

DWORD __stdcall ToastCallback(PVOID pNoty, DWORD eventType, DWORD args, PVOID userData) {
	PEASYWINNOTY noty = (PEASYWINNOTY)pNoty;

	switch (eventType) {
	case EasyWinNoty_EventType_ActiveWithoutParams: break;
	case EasyWinNoty_EventType_ActiveWithParams: break;
	case EasyWinNoty_EventType_UserCancel: break;
	case EasyWinNoty_EventType_ApplicationHide: break;
	case EasyWinNoty_EventType_Timeout: break;
	}

	return 0;
}

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	CoInitializeEx(0, COINIT_MULTITHREADED);

	PEASYWINNOTY noty = EasyWinNoty_CreateInstance();

	if (!EasyWinNoty_IsSupportSystem(noty)) {
		printf("System does not support windows notification.");
	}
	
	if (!EasyWinNoty_IsSupportAdvancedFeature(noty)) {
		printf("System does not support advanced windows notification.");
	}

	EasyWinNoty_Initialize(noty, L"My Test App C", L"My.TEST.APP.AWEF.C", EasyWinNoty_TemplateType_Text02);

	EasyWinNoty_SetText(noty, L"Notification First", 0);
	EasyWinNoty_SetText(noty, L"Notification Second", 1);

	EasyWinNoty_SetButton(noty, L"Pause", 0);
	EasyWinNoty_SetButton(noty, L"Cancel", 1);

	EasyWinNoty_SetProgressBar(noty, L"download_progress");
	//set value -1 is indeterminate progress bar
	EasyWinNoty_SetProgressValue(noty, L"Download File", -1, L"0%", L"aaaa.zip");

	EasyWinNoty_SetNotificationCallback(noty, &ToastCallback, NULL);

	EasyWinNoty_Show(noty);

	Sleep(3000);


	HANDLE hThread = CreateThread(0, 0, &UpdateProgress, noty, 0, 0);

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);

	EasyWinNoty_Cleanup(noty);
	EasyWinNoty_DeleteInstance(noty);


	_CrtDumpMemoryLeaks();

	return 0;
}