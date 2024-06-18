
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
		Sleep(300);
	}

	pNoty->Hide();

	return 0;
}

DWORD __stdcall ToastCallback(PVOID pNoty, DWORD eventType, DWORD args, PVOID userData, PVOID userInputs) {
	CEasyWinNotification* noty = (CEasyWinNotification*)pNoty;
	XToastEventType toastNotyType = (XToastEventType)eventType;

	if (toastNotyType == XToastEventType::ActiveWithParams) {
		printf("user click button id %d\n",args);

		if (userInputs) {
			LPWSTR data = CEasyWinNotification::GetInputData(L"text1", userInputs);
			if (data) {
				printf("user input %ls\n", data);
				CoTaskMemFree(data);
			}

			// user selection
			LPWSTR data2 = CEasyWinNotification::GetInputData(L"menuoption", userInputs);
			if (data2) {
				printf("user select %ls\n", data2);
				CoTaskMemFree(data2);
			}
		}
	}

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

	noty->InitializeWithoutShortCut(L"My Test App CPP", NULL, NULL, NULL, XToastTemplateType::ToastTemplateType_ToastText02);

	noty->SetText(L"Notification First", 0);
	noty->SetText(L"Notification Second", 1);


	noty->SetProgressBar(L"download_progress");
	//set value -1 is indeterminate progress bar
	noty->SetProgressValue(L"Download File", -1, L"0%", L"aaaa.zip");

	noty->SetComboBox(L"menuoption", L"Select one", L"item2");
	noty->SetComboBoxItem(L"menuoption", L"item1", L"Item 1");
	noty->SetComboBoxItem(L"menuoption", L"item2", L"Item 2");
	noty->SetComboBoxItem(L"menuoption", L"item3", L"Item 3");

	noty->SetInputBox(L"text1", L"input details message");


	noty->SetButton(L"Pause", 0);
	noty->SetButton(L"Cancel", 1);
	noty->SetButtonEx(L"Send", 2, L"text1", 0);

	noty->SetImage(L"file://C:\\Users\\bho35\\Pictures\\logo.png", TRUE, TRUE);

	noty->SetNotificationCallbackEx(&ToastCallback, NULL);

	noty->Show();

	//if you use STA, using timer.
	HANDLE hThread = CreateThread(0, 0, &UpdateProgress, noty, 0, 0);

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);

	noty->Cleanup();

	delete noty;

	_CrtDumpMemoryLeaks();

	return 0;
}