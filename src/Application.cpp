/*
 * Application.cpp
 */
#include "Application.h"
#include "MainWindow.h"
#include "GraphicWnd.h"
#include "GraphicPopupWnd.h"
#include "global.h"
#include <windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <locale.h>

using namespace MainWindow;
using namespace GraphicWnd;
using namespace GraphicPopupWnd;

UINT32 g_sleepTime;
UINT32 g_passiveTime;

namespace Application {

static HINSTANCE g_hInst = NULL;
static HWND g_hGraphicWnd = NULL;
static HWND g_hMainWnd = NULL;
static HWND g_hGraphicPopupWnd = NULL;

int InitInstance(HINSTANCE hInst) {
	INITCOMMONCONTROLSEX icce;

	icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icce.dwICC = ICC_PROGRESS_CLASS | ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES | ICC_DATE_CLASSES;
	InitCommonControlsEx(&icce);

	setlocale(LC_ALL, "");

	g_sleepTime   = 0;
	g_passiveTime = 0;

	return 0;
}

int ExitInstance(HINSTANCE hInst) {

	return 0;
}

int Run(HINSTANCE hInst) {
	int res = 0;

	HWND hWnd = CreateMainWindow(hInst);
	if (!hWnd) {
		Log("Can't create main window");
		return -1;
	}
	g_hMainWnd = hWnd;

	g_hGraphicWnd = CreateGraphicWnd(hWnd);
	if (!g_hGraphicWnd) {
		Log("Can't create graphic window");
		return NULL;
	}

	g_hGraphicPopupWnd = CreateGraphicPopupWnd(NULL);
	if (!g_hGraphicPopupWnd) {
		Log("Can't create graphic popup window");
		return NULL;
	}

	NOTIFYICONDATA nid = {0};

	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	nid.hWnd = hWnd;
	_tcscpy_s(nid.szTip, sizeof(nid.szTip) * sizeof(TCHAR), TEXT("Tip"));
	nid.uCallbackMessage = WM_TRAY_MESSAGE;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

	Shell_NotifyIcon(NIM_ADD, &nid);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return res;
}

TCHAR* GetTitle() {
	return TEXT("ActiveTime v1.0");
}

int MessageBoxMy(LPCTSTR message, int flags) {
	return MessageBox(Application::GetMainWindow(), message, GetTitle(), flags);
}

HWND GetMainWindow() {
	return g_hMainWnd;
}

HWND GetGraphicWindow() {
	return g_hGraphicWnd;
}

HWND GetGraphicPopupWindow() {
	return g_hGraphicPopupWnd;
}

HINSTANCE GetInstance() {
	return g_hInst = NULL;
}

void SetInstance(HINSTANCE hInst) {
	g_hInst = hInst;
}

}
