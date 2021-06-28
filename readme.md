# EasyWinNotification

Easy Windows Toast Notification for pure C/C++/C#(for old .NET Framework version)

![Alt text](/image/capture.png)

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
//notification callback
DWORD __stdcall ToastCallback(PVOID pNoty, DWORD eventType, DWORD args, PVOID userData) {
	CEasyWinNotification* noty = (CEasyWinNotification*)pNoty;
	XToastEventType toastNotyType = (XToastEventType)eventType;

	return 0;
}

int main() {
	CoInitializeEx(0, COINIT_MULTITHREADED);

	CEasyWinNotification* noty = new CEasyWinNotification();
	if (!noty->IsSupportSystem()) {
		printf("System does not support windows notification.");
		delete noty;
		return -1;
	}
	if (!noty->IsSupportAdvancedFeature()) {
		printf("System does not support advanced windows notification.");
	}
	//initialize notification with app name and app id
	//template type description in this link : https://docs.microsoft.com/en-us/uwp/api/windows.ui.notifications.toasttemplatetype?view=winrt-20348
	noty->Initialize(L"My Test App CPP", L"My.TEST.APP.AWEF.CPP", XToastTemplateType::ToastTemplateType_ToastText02);

	//set notification text
	noty->SetText(L"Notification First", 0);
	noty->SetText(L"Notification Second", 1);

	//set notification button
	noty->SetButton(L"Pause", 0);
	noty->SetButton(L"Cancel", 1);
	
	//initialize progress bar
	noty->SetProgressBar(L"download_progress");
	//set value -1 is indeterminate progress bar
	noty->SetProgressValue(L"Download File", -1, L"0%", L"aaaa.zip");

	noty->SetNotificationCallback(&ToastCallback, NULL);

	noty->Show();
	//update progress bar at thread.
	HANDLE hThread = CreateThread(0, 0, &UpdateProgress, noty, 0, 0);
	//do something or wait thread.
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);

	noty->Cleanup();
	delete noty;
	
	return 0;
}
```
Copy 2 files ('EasyWinNotification.cpp' and 'EasyWinNotification.h') to your projects.\
Or Compile 'EasyWinNotification' project to dll or static library and use it.
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
//notification callback
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
	CoInitializeEx(0, COINIT_MULTITHREADED);

	PEASYWINNOTY noty = EasyWinNoty_CreateInstance();

	if (!EasyWinNoty_IsSupportSystem(noty)) {
		printf("System does not support windows notification.");
	}
	
	if (!EasyWinNoty_IsSupportAdvancedFeature(noty)) {
		printf("System does not support advanced windows notification.");
	}
	//initialize notification with app name and app id
	//template type description in this link : https://docs.microsoft.com/en-us/uwp/api/windows.ui.notifications.toasttemplatetype?view=winrt-20348
	EasyWinNoty_Initialize(noty, L"My Test App C", L"My.TEST.APP.AWEF.C", EasyWinNoty_TemplateType_Text02);
	
	//set notification text
	EasyWinNoty_SetText(noty, L"Notification First", 0);
	EasyWinNoty_SetText(noty, L"Notification Second", 1);
	
	//set notification button
	EasyWinNoty_SetButton(noty, L"Pause", 0);
	EasyWinNoty_SetButton(noty, L"Cancel", 1);

	EasyWinNoty_SetProgressBar(noty, L"download_progress");
	//set value -1 is indeterminate progress bar
	EasyWinNoty_SetProgressValue(noty, L"Download File", -1, L"0%", L"aaaa.zip");

	EasyWinNoty_SetNotificationCallback(noty, &ToastCallback, NULL);
	
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

		//initialize notification with app name and app id
        noty.Initialize("My Test App CSharp", "My.TEST.APP.AWEF.CSharp", 5);

		//set notification text
        noty.SetText("Notification First", 0);
        noty.SetText("Notification Second", 1);

		//set notification button
        noty.SetButton("Pause", 0);
        noty.SetButton("Cance", 1);
		
		//initialize progress bar
        noty.SetProgressBar("download_progress");
        //set value -1 is indeterminate progress bar
        noty.SetProgressValue("Download File", -1, "0%", "aaaa.zip");

        noty.SetNotificationCallback(NotificationCallback, IntPtr.Zero);
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

    private static int NotificationCallback(EasyWinNotification noty, XToastEventType eventType, int args, IntPtr userData)
    {
        return 0;
    }
}
```
Copy 'EasyWinNotification.cs' file to your projects. (DllImport wrapper class)\
And Compile 'EasyWinNotification' project to dll or download compiled dll.\
Put 'EasyWinNotification.dll' into same directory into c# exe file.\
(or uncomment static constructor in 'EasyWinNotification' if your c# project compiled in 'Any CPU'

## TODO
More Template support (like image,text box,dropdown menu...)

License : MIT
