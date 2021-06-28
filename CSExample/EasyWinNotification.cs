using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace CSExample
{
    public enum XToastTemplateType : int
    {
        //work in progress
        //ToastTemplateType_ToastImageAndText01 = 0,
        //ToastTemplateType_ToastImageAndText02 = 1,
        //ToastTemplateType_ToastImageAndText03 = 2,
        //ToastTemplateType_ToastImageAndText04 = 3,
        ToastTemplateType_ToastText01 = 4,
        ToastTemplateType_ToastText02 = 5,
        ToastTemplateType_ToastText03 = 6,
        ToastTemplateType_ToastText04 = 7,
    };

    public enum XToastEventType : int
    {
        ActiveWithoutParams = 0,
        ActiveWithParams = 1,
        UserCancel = 2,
        ApplicationHide = 3,
        Timeout = 4
    };

    public class EasyWinNotification
    {
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int NotificationCallback(EasyWinNotification noty, XToastEventType eventType,int args,IntPtr userData);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int _NotificationCallbackInt(IntPtr pNoty, int eventType, int args, IntPtr userData);

        private IntPtr pNoty = IntPtr.Zero;
        private NotificationCallback _callback = null;

        private EasyWinNotification()
        {
            pNoty = EasyWinNoty_CreateInstance();
            if(pNoty == IntPtr.Zero)
            {
                throw new Exception("EasyWinNoty_CreateInstance err");
            }
        }

        ~EasyWinNotification()
        {
            if(pNoty != IntPtr.Zero)
            {
                EasyWinNoty_DeleteInstance(pNoty);
            }
        }

        //uncomment this if your program was compiled in 'Any CPU' and 'EasyWinNotification.dll' was located in each cpu type
        // like 'x64' and 'x86' folders
        //static EasyWinNotification()
        //{
        //    var myPath = new Uri(typeof(EasyWinNotification).Assembly.CodeBase).LocalPath;
        //    var myFolder = Path.GetDirectoryName(myPath);

        //    var is64 = IntPtr.Size == 8;
        //    var subfolder = is64 ? "\\x64\\" : "\\x86\\";

        //    LoadLibrary(myFolder + subfolder + "EasyWinNotification.dll");
        //}

        public static EasyWinNotification CreateInstance()
        {
            EasyWinNotification noty = null;
            try
            {
                noty = new EasyWinNotification();
            }
            catch
            {
                
            }

            return noty;
        }

        public bool IsSupportSystem()
        {
            return EasyWinNoty_IsSupportSystem(this.pNoty);
        }

        public bool IsSupportAdvancedFeature()
        {
            return EasyWinNoty_IsSupportAdvancedFeature(this.pNoty);
        }

        public int Initialize(string programName, string appId, int notyType)
        {
            return EasyWinNoty_Initialize(this.pNoty, programName, appId, notyType);
        }

        public int Show()
        {
            return EasyWinNoty_Show(this.pNoty);
        }

        public int Update()
        {
            return EasyWinNoty_Update(this.pNoty);
        }

        public int Hide()
        {
            return EasyWinNoty_Hide(this.pNoty);
        }

        public void Cleanup()
        {
            EasyWinNoty_Cleanup(this.pNoty);
        }

        public void RemoveShortcut()
        {
            EasyWinNoty_RemoveShortcut(this.pNoty);
        }

        public void SetNotificationCallback(NotificationCallback callback,IntPtr userData)
        {
            this._callback = callback;
            EasyWinNoty_SetNotificationCallback(this.pNoty, _CallbackWrapper, userData);
        }

        public int SetText(string text,int line)
        {
            return EasyWinNoty_SetText(this.pNoty ,text, line);
        }

        public int SetButton(string text,int index)
        {
            return EasyWinNoty_SetButton(this.pNoty, text, index);
        }

        public int SetProgressBar(string progressId)
        {
            return EasyWinNoty_SetProgressBar(this.pNoty, progressId);
        }

        public int SetProgressValue(string progressTitle,double progressValue,string progressValueStr,string progressStatus)
        {
            return EasyWinNoty_SetProgressValue(this.pNoty, progressTitle, progressValue, progressValueStr, progressStatus);
        }

        private int _CallbackWrapper(IntPtr pNoty, int eventType, int args, IntPtr userData)
        {
            if(this._callback != null)
            {
                return this._callback(this, (XToastEventType)eventType, args, userData);
            }
            return 1;
        }

        //---------------

        [DllImport("EasyWinNotification.dll",CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr EasyWinNoty_CreateInstance();

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool EasyWinNoty_IsSupportSystem(IntPtr pNoty);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool EasyWinNoty_IsSupportAdvancedFeature(IntPtr pNoty);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_Initialize(IntPtr pNoty,[MarshalAs(UnmanagedType.LPWStr)]string programName, [MarshalAs(UnmanagedType.LPWStr)]string appId,int notyType);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_Show(IntPtr pNoty);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_Update(IntPtr pNoty);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_Hide(IntPtr pNoty);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void EasyWinNoty_Cleanup(IntPtr pNoty);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void EasyWinNoty_RemoveShortcut(IntPtr pNoty);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void EasyWinNoty_SetNotificationCallback(IntPtr pNoty, _NotificationCallbackInt cb,IntPtr userData);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetText(IntPtr pNoty,[MarshalAs(UnmanagedType.LPWStr)]string text,int line);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetButton(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)]string programName,int index);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetProgressBar(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)]string progressId);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetProgressValue(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)]string progressTitle,double progressValue, [MarshalAs(UnmanagedType.LPWStr)]string progressValueStr, [MarshalAs(UnmanagedType.LPWStr)]string progressStatus);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void EasyWinNoty_DeleteInstance(IntPtr pNoty);

        [DllImport("kernel32.dll")]
        private static extern IntPtr LoadLibrary(string dllToLoad);
    }
}
