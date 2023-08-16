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
            ushort[] data = new ushort[32];
            int errorCode = GetDataFromRT(0, 0, 0, data, 32);

            IntPtr errorPtr = DecodeError(errorCode);
            string error = Marshal.PtrToStringAnsi(errorPtr);

            Console.WriteLine(error);
            Console.ReadKey();
        }
    }
}
