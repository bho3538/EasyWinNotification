# EasyWinNotification

Easy Windows Toast Notification for pure C/C++/C#(for old .NET Framework version)

UPDATE (20231003) \
Does not require program shortcut file(.lnk file in 'startup program' folder) \
Support 'input box' control

![Win10](/image/capture.png)

![Win11](https://github.com/bho3538/EasyWinNotification/assets/12496720/e2e8ee40-74d5-4747-91fd-ea0b0c8df499)


## How to use
### C++
```c++

using namespace EasyWinNoty;

DWORD WINAPI UpdateProgress(PVOID args) {
	DOUBLE value = 0;
	CEasyWinNotification* pNoty = (CEasyWinNotification*)args;

	for (int i = 0; i < 10; i++) {
		value += 0.1;
		//update progress bar
		pNoty->SetProgressValue(L"Download File", value, L"10%", L"aaaa.zip");
		pNoty->Update();
		Sleep(500);
	}

	pNoty->Hide();

	return 0;
}
//notification callback (extended)
DWORD __stdcall ToastCallback(PVOID pNoty, DWORD eventType, DWORD args, PVOID userData, PVOID userInputs) {
	CEasyWinNotification* noty = (CEasyWinNotification*)pNoty;
	XToastEventType toastNotyType = (XToastEventType)eventType;

	if (userInputs) {
		//first parameter is inputbox control id
		LPWSTR data = CEasyWinNotification::GetInputData(L"text1", userInputs);
		if (data) {
			//user input data (at input text box)
			printf("%ls", data);
			CoTaskMemFree(data);
		}
	}

	return 0;
}

int main() {
	CoInitializeEx(0, COINIT_MULTITHREADED);

	CEasyWinNotification* noty = new CEasyWinNotification();

	if (!noty->IsSupportSystem()) {
		printf("System does not support windows notification.");
		return;
	}

	if (!noty->IsSupportAdvancedFeature()) {
		printf("System does not support advanced windows notification.");
		return;
	}

	//change initliaze method (does not create lnk file)
	//activationClsId is currently not implemented
	noty->InitializeWithoutShortCut(L"My Test App CPP", NULL, NULL, NULL, XToastTemplateType::ToastTemplateType_ToastText02);

	noty->SetText(L"Notification First", 0);
	noty->SetText(L"Notification Second", 1);


	noty->SetProgressBar(L"download_progress");
	//set value -1 is indeterminate progress bar
	noty->SetProgressValue(L"Download File", -1, L"0%", L"aaaa.zip");

	//create inputbox (id = 'text1')
	//You must create inputbox first before create button.
	noty->SetInputBox(L"text1", L"placeholder text");
	noty->SetButton(L"Pause", 0);
	noty->SetButton(L"Cancel", 1);

	noty->SetNotificationCallbackEx(&ToastCallback, NULL);

	noty->Show();

	//if you use STA, using timer.
	HANDLE hThread = CreateThread(0, 0, &UpdateProgress, noty, 0, 0);

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);

	noty->Cleanup();

	delete noty;
	
	return 0;
}
```
Copy 2 files ('EasyWinNotification.cpp' and 'EasyWinNotification.h') to your projects.\
Or Compile 'EasyWinNotification' project to dll or static library(need set 0 at first #if in 'EasyWinNotification.h') and use it.
### C
``` C
DWORD WINAPI UpdateProgress(PVOID args) {
	DOUBLE value = 0;
	PEASYWINNOTY pNoty = (PEASYWINNOTY)args;

	for (int i = 0; i < 10; i++) {
		value += 0.1;
		//update progress bar
		EasyWinNoty_SetProgressValue(pNoty, L"Download File", value, L"10%", L"aaaa.zip");
		EasyWinNoty_Update(pNoty);
		Sleep(500);
	}

	EasyWinNoty_Hide(pNoty);

	return 0;
}
//notification callback (extended)
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
	CoInitializeEx(0, COINIT_MULTITHREADED);

	PEASYWINNOTY noty = EasyWinNoty_CreateInstance();

	if (!EasyWinNoty_IsSupportSystem(noty)) {
		printf("System does not support windows notification.");
	}
	
	if (!EasyWinNoty_IsSupportAdvancedFeature(noty)) {
		printf("System does not support advanced windows notification.");
	}

	//change initliaze method (does not create lnk file)
	//activationClsId is currently not implemented
	EasyWinNoty_InitializeWithoutShortcut(noty, L"My Test App C", NULL, NULL, NULL, EasyWinNoty_TemplateType_Text02);

	EasyWinNoty_SetText(noty, L"Notification First", 0);
	EasyWinNoty_SetText(noty, L"Notification Second", 1);

	//create inputbox (id = 'input1')
	//You must create inputbox first before create button.
	EasyWinNoty_SetInputBox(noty, L"input1", L"placeholder text");

	EasyWinNoty_SetButton(noty, L"Pause", 0);
	EasyWinNoty_SetButton(noty, L"Cancel", 1);

	EasyWinNoty_SetProgressBar(noty, L"download_progress");
	//set value -1 is indeterminate progress bar
	EasyWinNoty_SetProgressValue(noty, L"Download File", -1, L"0%", L"aaaa.zip");

	EasyWinNoty_SetNotificationCallbackEx(noty, &ToastCallback, NULL);

	EasyWinNoty_Show(noty);

	//update progress bar at thread.
	HANDLE hThread = CreateThread(0, 0, &UpdateProgress, noty, 0, 0);
	//do something or wait thread.
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);

	//cleanup
	EasyWinNoty_Cleanup(noty);
	EasyWinNoty_DeleteInstance(noty);

	return 0;
}

```
Compile 'EasyWinNotification' project to dll or static library.\
And include 'EasyWinNotyCWrapper.h' to your projects and use it.
### C# (For old .NET Framework like .NET 3.5)
``` C#
class Program
{
    private static EasyWinNotification noty = null;

    static void Main(string[] args)
    {
        noty = EasyWinNotification.CreateInstance();
        if (noty == null)
        {
            Console.WriteLine("fail to create instance");
        }

	//change initliaze method (does not create lnk file)
	//activationClsId is currently not implemented
	noty.InitializeWithoutShortcut("My Test App CSharp", "", "", "", XToastTemplateType.ToastTemplateType_ToastText02);

	noty.SetText("Notification First", 0);
	noty.SetText("Notification Second", 1);

	//create inputbox (id = 'input1')
	//You must create inputbox first before create button.
	noty.SetInputBox("input1", "placeholder text");

	noty.SetButton("Pause", 0);
	noty.SetButton("Cance", 1);
	noty.SetProgressBar("download_progress");
	//set value -1 is indeterminate progress bar
	noty.SetProgressValue("Download File", -1, "0%", "aaaa.zip");

	noty.SetNotificationCallbackEx(NotificationCallbackEx, IntPtr.Zero);

	noty.Show();

	//update progress bar at thread. 
        Thread t = new Thread(Update);
        t.Start();
	//wait or do something
        t.Join();

        noty.Cleanup();
    }

    private static void Update()
    {
        double val = 0;
        for (int i = 0; i < 10; i++)
        {
            val += 0.1;
            noty.SetProgressValue("Download File", val, "10%", "aaaa.zip");
            noty.Update();
            Thread.Sleep(500);
        }
		noty.Hide();
    }

    private static int NotificationCallbackEx(EasyWinNotification noty, XToastEventType eventType, int args, IntPtr userData, IntPtr userInputData)
    {
        string str = noty.GetUserInputData("input1", userInputData);
	Console.WriteLine(str);
	return 0;
    }
}
```
Copy 'EasyWinNotification.cs' file to your projects. (DllImport wrapper class)\
And Compile 'EasyWinNotification' project to dll or download compiled dll.\
Put 'EasyWinNotification.dll' into same directory into c# exe file.\
(or uncomment static constructor in 'EasyWinNotification' if your c# project compiled in 'Any CPU'

## TODO
More Template support (like image, dropdown menu...)

License : MIT
