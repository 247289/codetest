// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the HOOK_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// HOOK_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include "framework.h"

#ifdef HOOK_EXPORTS
#define HOOK_API __declspec(dllexport)
#else
#define HOOK_API __declspec(dllimport)
#endif

extern HOOK_API int nhook;

HOOK_API DWORD fnhook(void);
HOOK_API bool getconchar(KEY_EVENT_RECORD& krec);
HOOK_API LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);

//
int CaptureScreen();

HOOK_API int run();