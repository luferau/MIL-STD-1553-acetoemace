// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "stdace.h"
#include <string>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

BuConfStruct Conf;

extern "C" __declspec(dllexport) BuError_t Initialize(int deviceAddress)
{
    Conf.ConfDev = deviceAddress; // Set device address

    BuError_t Err = BuOpen32(&Conf);
    if (Err != BU_SUCCESS)
    {
        return Err;
    }

    Err = BuBCOpen();
    return Err;
}

extern "C" __declspec(dllexport) const char* DecodeError(int errorCode)
{
    return BuErrorStr(errorCode);
}

extern "C" __declspec(dllexport) BuError_t Close()
{
    /* closes bus controller mode */
    BuError_t Err = BuBCClose();

    if (Err != BU_SUCCESS)
    {
        return Err;
    }

    /* must call at end of ACE library use */
    Err = BuClose();

    return Err;
}

extern "C" __declspec(dllexport) BuError_t SendDataToRT(
    unsigned char channel,
    unsigned char rt,
    unsigned char sa,
    unsigned short* data,
    unsigned char length)
{
    return BuBCSendData(channel, rt, sa, data, length);
}

extern "C" __declspec(dllexport) BuError_t GetDataFromRT(
    unsigned char channel,
    unsigned char rt,
    unsigned char sa,
    unsigned short* data,
    unsigned char length)
{
    return BuBCGetData(channel, rt, sa, data, length);
}
