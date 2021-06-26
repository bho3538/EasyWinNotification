
#include "pch.h"

#include <shobjidl_core.h>
#include "EasyWinNotification.h"

#include <crtdbg.h>

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

DWORD __stdcall ToastCallback(PVOID pNoty,DWORD eventType,DWORD args,PVOID userData) {
	CEasyWinNotification* noty = (CEasyWinNotification*)pNoty;
	XToastEventType toastNotyType = (XToastEventType)eventType;



	return 0;
}

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//_CrtSetBreakAlloc(81);

	CoInitializeEx(0, COINIT_MULTITHREADED);

	CEasyWinNotification* noty = new CEasyWinNotification();

	noty->Initialize(L"My Test App", L"My.TEST.APP.AWEF", XToastTemplateType::ToastTemplateType_ToastText02);

	noty->SetText(L"Notification First", 0);
	noty->SetText(L"Notification Second", 1);

	noty->SetButton(L"Pause", 0);
	noty->SetButton(L"Cancel", 1);
	noty->SetProgressBar(L"download_progress");
	noty->SetProgressValue(L"Download File", 0.1, L"10%", L"aaaa.zip");

	noty->SetNotificationCallback(&ToastCallback,NULL);

	noty->Show();

	HANDLE hThread = CreateThread(0, 0, &UpdateProgress, noty, 0, 0);

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);

	noty->Cleanup();
	//noty->RemoveShortcut();

	delete noty;

	
	

	_CrtDumpMemoryLeaks();

	return 0;
}