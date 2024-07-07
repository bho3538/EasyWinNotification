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
        public delegate int NotificationCallbackEx(EasyWinNotification noty, XToastEventType eventType, int args, IntPtr userData, IntPtr userInputData);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int _NotificationCallbackInt(IntPtr pNoty, int eventType, int args, IntPtr userData);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int _NotificationCallbackExInt(IntPtr pNoty, int eventType, int args, IntPtr userData, IntPtr userInputData);

        private IntPtr pNoty = IntPtr.Zero;
        private NotificationCallback _callback = null;

        private NotificationCallbackEx _callbackEx = null;

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

//#error uncomment this function if your program was compiled in 'Any CPU' and 'EasyWinNotification.dll' was located in each cpu type (or remove this)
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

        [Obsolete("Deprecated. Please use 'InitializeWithoutShortcut'")]
        public int Initialize(string programName, string appId, int notyType)
        {
            return EasyWinNoty_Initialize(this.pNoty, programName, appId, notyType);
        }

        public int InitializeWithoutShortcut(string programName, string activatorClsId, string iconPath, string iconBackgroundPath, XToastTemplateType notyType)
        {
            return EasyWinNoty_InitializeWithoutShortcut(this.pNoty, programName, activatorClsId, iconPath, iconBackgroundPath, (int)notyType);
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

        public void SetNotificationCallbackEx(NotificationCallbackEx callbackEx, IntPtr userData)
        {
            this._callbackEx = callbackEx;
            EasyWinNoty_SetNotificationCallbackEx(this.pNoty, _CallbackWrapperEx, userData);
        }

        public int SetText(string text,int line)
        {
            return EasyWinNoty_SetText(this.pNoty ,text, line);
        }

        public int SetButtonEx(string text, int index, string inputId, int reserved)
        {
            return EasyWinNoty_SetButtonEx(this.pNoty, text, index, inputId, reserved);
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

        public int SetInputBox(string controlId, string placeholderText)
        {
            return EasyWinNoty_SetInputBox(this.pNoty, controlId, placeholderText);
        }

        public int SetImage(string imagePath, bool overrideLogo, bool useCircle)
        {
            return EasyWinNoty_SetImage(this.pNoty, imagePath, overrideLogo ? 1: 0, useCircle ? 1 : 0);
        }

        public int SetComboBox(string controlId, string titleText, string defaultInputItemId)
        {
            return EasyWinNoty_SetComboBox(this.pNoty, controlId, titleText, defaultInputItemId);
        }

        public int SetComboBoxItem(string comboBoxId, string controlId, string itemText)
        {
            return EasyWinNoty_SetComboBoxItem(this.pNoty, comboBoxId, controlId, itemText);
        }

        public string GetUserInputData(string controlId, IntPtr pUserInputData)
        {
            IntPtr pData = EasyWinNoty_GetInputData(controlId, pUserInputData);
            if(pData != IntPtr.Zero)
            {
                string str = Marshal.PtrToStringUni(pData);
                Marshal.FreeCoTaskMem(pData);
                return str;
            }
            return "";
        }

        private int _CallbackWrapper(IntPtr pNoty, int eventType, int args, IntPtr userData)
        {
            if(this._callback != null)
            {
                return this._callback(this, (XToastEventType)eventType, args, userData);
            }
            return 1;
        }

        private int _CallbackWrapperEx(IntPtr pNoty, int eventType, int args, IntPtr userData, IntPtr userInputData)
        {
            if (this._callbackEx != null)
            {
                return this._callbackEx(this, (XToastEventType)eventType, args, userData, userInputData);
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

        //deprecated. please use 'EasyWinNoty_InitializeWithoutShortcut'
        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl), Obsolete("deprecated. please use 'EasyWinNoty_InitializeWithoutShortcut'")]
        private static extern int EasyWinNoty_Initialize(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)]string programName, [MarshalAs(UnmanagedType.LPWStr)]string appId, int notyType);

        //deprecated. DO NOT USE
        //Call this function at program entry.
        //Because if you register(create lnk file at special folder) program with ID, System will know after few time (5~10 sec)
        //if you try show notification before that time (5~10 sec) notification will not showed.
        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl),Obsolete("deprecated. please use 'EasyWinNoty_InitializeWithoutShortcut'")]
        private static extern int EasyWinNoty_RegisterForSystem([MarshalAs(UnmanagedType.LPWStr)]string programName, [MarshalAs(UnmanagedType.LPWStr)]string appId);

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
        private static extern void EasyWinNoty_SetNotificationCallbackEx(IntPtr pNoty, _NotificationCallbackExInt cb, IntPtr userData);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetText(IntPtr pNoty,[MarshalAs(UnmanagedType.LPWStr)]string text,int line);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetButton(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)]string programName,int index);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetButtonEx(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)] string programName, int index, [MarshalAs(UnmanagedType.LPWStr)] string inputId, int reserved);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetProgressBar(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)]string progressId);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetProgressValue(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)]string progressTitle,double progressValue, [MarshalAs(UnmanagedType.LPWStr)]string progressValueStr, [MarshalAs(UnmanagedType.LPWStr)]string progressStatus);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetInputBox(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)] string controlId, [MarshalAs(UnmanagedType.LPWStr)] string placeholderText);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetImage(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)] string imagePath, int overrideLogo, int useCircle);


        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetComboBox(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)] string controlId, [MarshalAs(UnmanagedType.LPWStr)] string titleText, [MarshalAs(UnmanagedType.LPWStr)] string defaultInputId);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetComboBoxItem(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)] string comboBoxId, [MarshalAs(UnmanagedType.LPWStr)] string controlId, [MarshalAs(UnmanagedType.LPWStr)] string itemText);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void EasyWinNoty_DeleteInstance(IntPtr pNoty);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_InitializeWithoutShortcut(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)] string programName, [MarshalAs(UnmanagedType.LPWStr)] string activatorClsId, [MarshalAs(UnmanagedType.LPWStr)] string iconPath, [MarshalAs(UnmanagedType.LPWStr)] string iconBackgroundColor, int notyType);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr EasyWinNoty_GetInputData([MarshalAs(UnmanagedType.LPWStr)] string controlId, IntPtr pUserInputData);

        [DllImport("kernel32.dll")]
        private static extern IntPtr LoadLibrary(string dllToLoad);
    }
}
