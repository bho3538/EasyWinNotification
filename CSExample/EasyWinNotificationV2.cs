using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using static CSExample.EasyWinNotification;

namespace CSExample
{
    public class NotificationTextData
    {
        public string Text { get; private set; }

        public NotificationTextData(string text)
        {
            Text = text;
        }
    }

    public class NotificationButtonData
    {
        public string Text { get; private set; }
        public int ButtonId { get; private set; }

        public NotificationButtonData(string text, int buttonId)
        {
            Text = text;
            ButtonId= buttonId;
        }
    }

    public class NotificationComboBoxItem
    {
        public string Text { get; private set;}
        public string ItemId { get; private set;}

        public NotificationComboBoxItem(string text, string itemId)
        {
            Text= text;
            ItemId= itemId;
        }
    }

    public class NotificationComboBox
    {
        public string ComboBoxId { get; private set; } = "Cmb1";
        public string DefaultItemId { get; private set; }

        private List<NotificationComboBoxItem> _items = new List<NotificationComboBoxItem>();

        public NotificationComboBox AddItem(string text, string itemId)
        {
            _items.Add(new NotificationComboBoxItem(text, itemId));

            return this;
        }

        public NotificationComboBox SetDefaultItemId(string itemId)
        {
            this.DefaultItemId = itemId;
            return this;
        }

        public List<NotificationComboBoxItem> GetItems()
        {
            return _items;
        }
    }

    public class NotificationProgressbar
    {
        public const int INDETERMINATE = -1;
        public string ProgressbarId { get; private set; } = "prog1";
        public string ProgressValueStr { get; private set; }
        public double ProgressValue { get; private set; }
        public string ProgressTitle { get; private set; }
        public string ProgressStatus { get; private set; }

        public NotificationProgressbar(string progressTitle, double progressValue, string progressValueStr, string progressStatus)
        {
            SetValue(progressTitle, progressValue, progressValueStr, progressStatus);
        }

        public void SetValue(string progressTitle, double progressValue, string progressValueStr, string progressStatus)
        {
            ProgressValueStr = progressValueStr;
            ProgressValue = progressValue;
            ProgressTitle = progressTitle;
            ProgressStatus = progressStatus;
        }
    }

    public class NotificationInputbox
    {
        public string InputId { get; private set; } = "input1";
        public string PlaceholderText { get; private set; }

        public NotificationButtonData ShortcutBtn { get; private set; }

        public NotificationInputbox(string placeholderText)
        {
            PlaceholderText = placeholderText;
        }

        public NotificationInputbox SetShortcutButton(NotificationButtonData btn)
        {
            ShortcutBtn = btn;
            return this;
        }
    }

    public class NotificationImage
    {
        public string ImagePath { get; private set; }
        public bool IsLogo { get; private set; }
        public bool UseCircleLogo { get; private set; }

        public NotificationImage(string imagePath)
        {
            ImagePath = imagePath;
        }

        public NotificationImage SetLogo(bool useCircleLogo)
        {
            IsLogo = true;
            UseCircleLogo = useCircleLogo;
            return this;
        }
    }

    public enum XToastTemplateType2 : int
    {
        ToastTemplateType_ToastText01 = 4,
        ToastTemplateType_ToastText02 = 5,
        ToastTemplateType_ToastText03 = 6,
        ToastTemplateType_ToastText04 = 7,
    };

    public enum XToastEventType2 : int
    {
        ActiveWithoutParams = 0,
        ActiveWithParams = 1,
        UserCancel = 2,
        ApplicationHide = 3,
        Timeout = 4
    };

    public class EasyWinNotificationV2
    {
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int NotificationCallbackEx(EasyWinNotificationV2 noty, XToastEventType2 eventType, int args, IntPtr userInputData);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate int _NotificationCallbackExInt(IntPtr pNoty, int eventType, int args, IntPtr userData, IntPtr userInputData);

        private IntPtr pNoty = IntPtr.Zero;
        private NotificationCallbackEx _callbackEx = null;

        private List<NotificationTextData> _textDatas = new List<NotificationTextData>();
        private List<NotificationButtonData> _buttonDatas = new List<NotificationButtonData>();
        private NotificationComboBox _cmbData = null;
        private NotificationProgressbar _progressData = null;
        private NotificationInputbox _inputData = null;
        private NotificationImage _imageData = null;

        private EasyWinNotificationV2()
        {
            pNoty = EasyWinNoty_CreateInstance();
            if (pNoty == IntPtr.Zero)
            {
                throw new Exception("EasyWinNoty_CreateInstance err");
            }
        }

        ~EasyWinNotificationV2()
        {
            if (pNoty != IntPtr.Zero)
            {
                EasyWinNoty_DeleteInstance(pNoty);
            }
        }

        //#error uncomment this function if your program was compiled in 'Any CPU' and 'EasyWinNotification.dll' was located in each cpu type (or remove this)
        // like 'x64' and 'x86' folders
        static EasyWinNotificationV2()
        {
            var myPath = new Uri(typeof(EasyWinNotification).Assembly.CodeBase).LocalPath;
            var myFolder = Path.GetDirectoryName(myPath);

            var is64 = IntPtr.Size == 8;
            var subfolder = is64 ? "\\x64\\" : "\\x86\\";

            LoadLibrary(myFolder + subfolder + "EasyWinNotification.dll");
        }

        public static EasyWinNotificationV2 CreateInstance()
        {
            EasyWinNotificationV2 noty = null;
            try
            {
                noty = new EasyWinNotificationV2();
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

        public EasyWinNotificationV2 Initialize(string programName, string iconPath, string iconBackgroundPath, XToastTemplateType2 notyType)
        {
            EasyWinNoty_InitializeWithoutShortcut(this.pNoty, programName, string.Empty, iconPath, iconBackgroundPath, (int)notyType);
            return this;
        }

        public EasyWinNotificationV2 Initialize(string programName, XToastTemplateType2 notyType)
        {
            return Initialize(programName, string.Empty, string.Empty, notyType);
        }

        public EasyWinNotificationV2 SetText(NotificationTextData textData)
        {
            _textDatas.Add(textData);
            return this;
        }

        public EasyWinNotificationV2 SetButton(NotificationButtonData buttonData)
        {
            _buttonDatas.Add(buttonData);
            return this;
        }

        public EasyWinNotificationV2 SetComboBox(NotificationComboBox comboBoxData)
        {
            _cmbData = comboBoxData;
            return this;
        }

        public EasyWinNotificationV2 SetProgressbar(NotificationProgressbar progressData)
        {
            _progressData = progressData;
            return this;
        }

        public void SetProgressbarValue(string progressTitle, double progressValue, string progressValueStr, string progressStatusMsg)
        {
            _progressData.SetValue(progressTitle, progressValue, progressValueStr, progressStatusMsg);
            EasyWinNoty_SetProgressValue(pNoty, progressTitle, progressValue, progressValueStr, progressStatusMsg);
        }

        public EasyWinNotificationV2 SetInputBox(NotificationInputbox inputBox)
        {
            _inputData = inputBox;
            return this;
        }

        public EasyWinNotificationV2 SetNotificationCallback(NotificationCallbackEx callbackEx)
        {
            _callbackEx = callbackEx;

            EasyWinNoty_SetNotificationCallbackEx(this.pNoty, _CallbackWrapperEx, IntPtr.Zero);

            return this;
        }

        public EasyWinNotificationV2 SetImage(NotificationImage imageData)
        {
            _imageData= imageData;
            return this;
        }

        public string GetSelectedComboBoxId(IntPtr pUserInputData)
        {
            if (_cmbData == null)
            {
                return string.Empty;
            }

            return GetUserInputData(_cmbData.ComboBoxId, pUserInputData);
        }

        public string GetInputboxData(IntPtr pUserInputData)
        {
            if (_inputData == null)
            {
                return string.Empty;
            }

            return GetUserInputData(_inputData.InputId, pUserInputData);
        }

        private string GetUserInputData(string controlId, IntPtr pUserInputData)
        {
            IntPtr pData = EasyWinNoty_GetInputData(controlId, pUserInputData);
            if (pData != IntPtr.Zero)
            {
                string str = Marshal.PtrToStringUni(pData);
                Marshal.FreeCoTaskMem(pData);
                return str;
            }

            return "";
        }

        public int Show()
        {
            Build();


            return EasyWinNoty_Show(this.pNoty);
        }

        private void Build()
        {
            // set text first
            for (int i=0; i< _textDatas.Count;i++)
            {
                EasyWinNoty_SetText(pNoty, _textDatas[i].Text, i);
            }

            if (_progressData != null)
            {
                EasyWinNoty_SetProgressBar(pNoty, _progressData.ProgressbarId);
                EasyWinNoty_SetProgressValue(pNoty, _progressData.ProgressTitle, _progressData.ProgressValue, _progressData.ProgressValueStr, _progressData.ProgressStatus);
            }

            if (_cmbData != null)
            {
                var items = _cmbData.GetItems();

                if (items != null)
                {
                    EasyWinNoty_SetComboBox(pNoty, _cmbData.ComboBoxId, "", _cmbData.DefaultItemId);

                    foreach (var item in items)
                    {
                        EasyWinNoty_SetComboBoxItem(pNoty, _cmbData.ComboBoxId, item.ItemId, item.Text);
                    }
                }
            }

            if (_inputData != null)
            {
                EasyWinNoty_SetInputBox(pNoty, _inputData.InputId, _inputData.PlaceholderText);
                
                if(_inputData.ShortcutBtn != null)
                {
                    EasyWinNoty_SetButtonEx(pNoty, _inputData.ShortcutBtn.Text, _inputData.ShortcutBtn.ButtonId, _inputData.InputId, 0);
                }
            }

            if (_imageData != null)
            {
                EasyWinNoty_SetImage(pNoty, _imageData.ImagePath, _imageData.IsLogo, _imageData.UseCircleLogo);
            }

            for (int i = 0; i < _buttonDatas.Count; i++)
            {
                EasyWinNoty_SetButtonEx(pNoty, _buttonDatas[i].Text, _buttonDatas[i].ButtonId, "", 0);
            }
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

        private int _CallbackWrapperEx(IntPtr pNoty, int eventType, int args, IntPtr userData, IntPtr userInputData)
        {
            if (_callbackEx != null)
            {
                return _callbackEx(this, (XToastEventType2)eventType, args, userInputData);
            }
            return 1;
        }

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr EasyWinNoty_CreateInstance();

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool EasyWinNoty_IsSupportSystem(IntPtr pNoty);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool EasyWinNoty_IsSupportAdvancedFeature(IntPtr pNoty);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_Show(IntPtr pNoty);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_Update(IntPtr pNoty);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_Hide(IntPtr pNoty);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void EasyWinNoty_Cleanup(IntPtr pNoty);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void EasyWinNoty_SetNotificationCallbackEx(IntPtr pNoty, _NotificationCallbackExInt cb, IntPtr userData);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetText(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)] string text, int line);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetButtonEx(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)] string programName, int index, [MarshalAs(UnmanagedType.LPWStr)] string inputId, int reserved);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetProgressBar(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)] string progressId);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetProgressValue(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)] string progressTitle, double progressValue, [MarshalAs(UnmanagedType.LPWStr)] string progressValueStr, [MarshalAs(UnmanagedType.LPWStr)] string progressStatus);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetInputBox(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)] string controlId, [MarshalAs(UnmanagedType.LPWStr)] string placeholderText);

        [DllImport("EasyWinNotification.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int EasyWinNoty_SetImage(IntPtr pNoty, [MarshalAs(UnmanagedType.LPWStr)] string imagePath, [MarshalAs(UnmanagedType.Bool)] bool overrideLogo, [MarshalAs(UnmanagedType.Bool)] bool useCircle);

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
