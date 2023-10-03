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

DWORD __stdcall ToastCallback(PVOID pNoty, DWORD eventType, DWORD args, PVOID userData, PVOID userInputData) {
	PEASYWINNOTY noty = (PEASYWINNOTY)pNoty;

	switch (eventType) {
		case EasyWinNoty_EventType_ActiveWithoutParams: {
			printf("user click noty\n");
		}; break;
		case EasyWinNoty_EventType_ActiveWithParams: {
			printf("user click button %d\n",args);
		}; break;
		case EasyWinNoty_EventType_UserCancel: {
			printf("user close noty\n");
		}; break;
		case EasyWinNoty_EventType_ApplicationHide: {
			printf("noty closed by program\n");
		}; break;
		case EasyWinNoty_EventType_Timeout: {
			printf("noty closed by timeout\n");
		}; break;
	}

	if (userInputData) {
		LPWSTR text = EasyWinNoty_GetInputData(L"input1", userInputData);
		printf("%ls\n", text);
		CoTaskMemFree(text);
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

	EasyWinNoty_InitializeWithoutShortcut(noty, L"My Test App C", NULL, NULL, NULL, EasyWinNoty_TemplateType_Text02);

	EasyWinNoty_SetText(noty, L"Notification First", 0);
	EasyWinNoty_SetText(noty, L"Notification Second", 1);

	EasyWinNoty_SetInputBox(noty, L"input1", L"placeholder text");

	EasyWinNoty_SetButton(noty, L"Pause", 0);
	EasyWinNoty_SetButton(noty, L"Cancel", 1);

	EasyWinNoty_SetProgressBar(noty, L"download_progress");
	//set value -1 is indeterminate progress bar
	EasyWinNoty_SetProgressValue(noty, L"Download File", -1, L"0%", L"aaaa.zip");

	EasyWinNoty_SetNotificationCallbackEx(noty, &ToastCallback, NULL);

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