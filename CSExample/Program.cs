using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace CSExample
{
    class Program
    {
        private static EasyWinNotificationV2 noty = null;

        static void Main(string[] args)
        {
            noty = EasyWinNotificationV2.CreateInstance();
            if (noty == null)
            {
                Console.WriteLine("fail to create instance");
                return;
            }

            noty.Initialize("My Test App CSharp", XToastTemplateType2.ToastTemplateType_ToastText02)
                .SetText(new NotificationTextData("First Line"))
                .SetText(new NotificationTextData("Second Line"))
                .SetButton(new NotificationButtonData("OK", 1))
                .SetButton(new NotificationButtonData("Cancel", 2))
                .SetComboBox(new NotificationComboBox()
                    .AddItem("Item 1", "item1")
                    .AddItem("Item 2", "item2")
                    .AddItem("Item 3", "item3")
                    .SetDefaultItemId("item2")
                    )
                .SetInputBox(new NotificationInputbox("Input comment here")
                    .SetShortcutButton(new NotificationButtonData("Send", 0))
                    )
                .SetProgressbar(new NotificationProgressbar("Downloading File", NotificationProgressbar.INDETERMINATE, "Loading", "aaaa.zip"))
                .SetImage(new NotificationImage("C:\\Users\\bho35\\Pictures\\logo.png")
                    .SetLogo(true)
                    )
                .SetNotificationCallback(NotificationCallback)
                .Show();

            Thread.Sleep(3000);

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
                noty.SetProgressbarValue("Download File", val, string.Format("{0}%", val * 100), "aaaa.zip");
                noty.Update();
                Thread.Sleep(500);
            }
            noty.Hide();
        }

        private static int NotificationCallback(EasyWinNotificationV2 noty, XToastEventType2 eventType, int args, IntPtr userInputData)
        {
            string str = noty.GetInputboxData(userInputData);
            Console.WriteLine($"input data : {str}");

            str = noty.GetSelectedComboBoxId(userInputData);
            Console.WriteLine($"combobox selected id : {str}");

            if (eventType == XToastEventType2.ActiveWithParams)
            {
                Console.WriteLine($"user click button id : {args}");
            }

            return 0;
        }

    }
}
