using System;
using System.Runtime.InteropServices;

namespace console_test
{
    internal class Program
    {
        [DllImport("MIL-STD-1553-BC.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Initialize(int deviceAddress);

        [DllImport("MIL-STD-1553-BC.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Close();

        [DllImport("MIL-STD-1553-BC.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr DecodeError(int error);

        [DllImport("MIL-STD-1553-BC.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int SendDataToRT(
            byte channel,
            byte remoteTerminalAddress,
            byte subAddress,
            ushort[] data,
            byte length);

        [DllImport("MIL-STD-1553-BC.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetDataFromRT(
            byte channel,
            byte remoteTerminalAddress,
            byte subAddress,
            [Out] ushort[] data,
            byte length);

        static void Main(string[] args)
        {
            int errorCode;

            errorCode = Initialize(0);
            PrintError("Initialize", errorCode);

            ushort[] data = new ushort[0x20];
            errorCode = GetDataFromRT(0x80, 0x1C, 0x01, data, (byte)data.Length);
            PrintError("GetDataFromRT", errorCode);

            errorCode = Close();
            PrintError("Close", errorCode);
            Console.ReadKey();
        }

        private static void PrintError(string operationName, int errorCode)
        {
            IntPtr errorPtr = DecodeError(errorCode);
            string errorDescription = Marshal.PtrToStringAnsi(errorPtr);

            Console.WriteLine($"{operationName} error:{errorCode:X4} description:{errorDescription}");
        }
    }
}