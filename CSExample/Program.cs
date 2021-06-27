using System;
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
            }

            noty.Initialize("My Test App CSharp", "My.TEST.APP.AWEF.CSharp", 5);

            noty.SetText("Notification First", 0);
            noty.SetText("Notification Second", 1);

            noty.SetButton("Pause", 0);
            noty.SetButton("Cance", 1);
            noty.SetProgressBar("download_progress");
            noty.SetProgressValue("Download File", -1, "10%", "aaaa.zip");

            //noty.SetNotificationCallback(&ToastCallback, NULL);

            noty.Show();

            Thread.Sleep(2000);

            Thread t = new Thread(Update);
            t.Start();

            t.Join();

            noty.Hide();
            noty.Cleanup();

            Thread.Sleep(5000);
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
    }
}
