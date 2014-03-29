/*
 * Application.h
 */
#pragma once

#include <Windows.h>

extern UINT32 g_sleepTime;
extern UINT32 g_passiveTime;

namespace Application {

HINSTANCE GetInstance();
void SetInstance(HINSTANCE hInst);
HWND GetMainWindow();
HWND GetGraphicWindow();
HWND GetGraphicPopupWindow();

int InitInstance(HINSTANCE hInst);
int ExitInstance(HINSTANCE hInst);
int Run(HINSTANCE hInst);

const TCHAR* GetTitle();

int SwapRam();

int MessageBoxMy(LPCTSTR message, int flags = MB_OK | MB_ICONINFORMATION);

}
