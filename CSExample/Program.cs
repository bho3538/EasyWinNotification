﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace CSExample
{
    class Program
    {
        private static EasyWinNotification noty = null;

        static void Main(string[] args)
        {
            noty = EasyWinNotification.CreateInstance();
            if (noty == null)
            {
                Console.WriteLine("fail to create instance");
                return;
            }

            noty.InitializeWithoutShortcut("My Test App CSharp", "", "", "", XToastTemplateType.ToastTemplateType_ToastText02);

            noty.SetText("Notification First", 0);
            noty.SetText("Notification Second", 1);

            noty.SetInputBox("input1", "placeholder text");
            noty.SetButton("Pause", 0);
            noty.SetButton("Cancel", 1);
            noty.SetButtonEx("Send", 1 , "input1", 0);

            noty.SetProgressBar("download_progress");
            //set value -1 is indeterminate progress bar
            noty.SetProgressValue("Download File", -1, "0%", "aaaa.zip");

            noty.SetImage("file://C:\\Users\\bho35\\Pictures\\logo.png", true, true);

            noty.SetNotificationCallbackEx(NotificationCallbackEx, IntPtr.Zero);

            noty.Show();

            Thread t = new Thread(Update);
            t.Start();

            t.Join();

            noty.Hide();

            noty.Cleanup();

            
        }

        private static void Update()
        {
            double val = 0;
            for(int i = 0; i < 10; i++)
            {
                val += 0.1;
                noty.SetProgressValue("Download File", val, "10%", "aaaa.zip");
                noty.Update();
                Thread.Sleep(500);
            }
        }

        private static int NotificationCallbackEx(EasyWinNotification noty, XToastEventType eventType, int args, IntPtr userData, IntPtr userInputData)
        {
            string str = noty.GetUserInputData("input1", userInputData);
            Console.WriteLine(str);
            return 0;
        }
    }
}
