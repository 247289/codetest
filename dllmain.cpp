// dllmain.cpp : Defines the entry point for the DLL application.
//#include "pch.h"
#include "framework.h"
#include "hook.h"
#include <corecrt_wstdio.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	WCHAR szTest[10];
    switch (ul_reason_for_call)
    {
		case DLL_PROCESS_ATTACH:
		{
			run();
			swprintf_s(szTest, 10, L"%d", fnhook());
			MessageBox(0, szTest, L"Keyboard", MB_OKCANCEL);
		}
		case DLL_THREAD_ATTACH:
		{
			swprintf_s(szTest, 10, L"%d", fnhook());
			MessageBox(0, szTest, L"Keyboard", MB_OK);
		}
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

