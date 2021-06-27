
#include "pch.h"

#include <shobjidl_core.h>
#include "../EasyWinNotification/EasyWinNotification.h"

#include <crtdbg.h>

#pragma comment(lib,"../Debug/EasyWinNotification.lib")

using namespace EasyWinNoty;

DWORD WINAPI UpdateProgress(PVOID args) {
	DOUBLE value = 0;
	CEasyWinNotification* pNoty = (CEasyWinNotification*)args;

	for (int i = 0; i < 10; i++) {
		value += 0.1;
		pNoty->SetProgressValue(L"Download File", value, L"10%", L"aaaa.zip");
		pNoty->Update();
		Sleep(500);
	}

	pNoty->Hide();

	return 0;
}

DWORD __stdcall ToastCallback(PVOID pNoty, DWORD eventType, DWORD args, PVOID userData) {
	CEasyWinNotification* noty = (CEasyWinNotification*)pNoty;
	XToastEventType toastNotyType = (XToastEventType)eventType;



	return 0;
}

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	CoInitializeEx(0, COINIT_MULTITHREADED);


	CEasyWinNotification* noty = new CEasyWinNotification();

	if (!noty->IsSupportSystem()) {
		printf("System does not support windows notification.");
	}

	if (!noty->IsSupportAdvancedFeature()) {
		printf("System does not support advanced windows notification.");
	}

	noty->Initialize(L"My Test App CPP", L"My.TEST.APP.AWEF.CPP", XToastTemplateType::ToastTemplateType_ToastText02);

	noty->SetText(L"Notification First", 0);
	noty->SetText(L"Notification Second", 1);

	noty->SetButton(L"Pause", 0);
	noty->SetButton(L"Cancel", 1);
	noty->SetProgressBar(L"download_progress");
	//set value -1 is indeterminate progress bar
	noty->SetProgressValue(L"Download File", -1, L"0%", L"aaaa.zip");

	noty->SetNotificationCallback(&ToastCallback, NULL);

	noty->Show();

	HANDLE hThread = CreateThread(0, 0, &UpdateProgress, noty, 0, 0);

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);

	noty->Cleanup();

	delete noty;

	_CrtDumpMemoryLeaks();

	return 0;
}