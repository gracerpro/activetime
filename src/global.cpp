// global.cpp
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

FILE*     g_hLogFile = NULL;


void Log(const char* message) {
	puts(message);
	fputs(message, g_hLogFile);
	fputs("\n", g_hLogFile);
	fflush(g_hLogFile);
}

void Log(const wchar_t* message) {
	_putws(message);
	fputws(message, g_hLogFile);
	fputws(L"\n", g_hLogFile);
	fflush(g_hLogFile);
}

void LogErr(const char* message) {
	//wprintf(TEXT("Error: %s\n"), message);
	printf("Error: %s\n", message);
}

void LogWarn(LPCTSTR message) {
	printf("Warning: %s\n", message);
}

void LogLastError() {
	DWORD err = GetLastError();
	LPTSTR message = NULL;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
		err, 0, (LPTSTR)&message, 0, NULL);

	Log(message);

	LocalFree(message);
}

TCHAR* GetNearExePath(TCHAR* path, const TCHAR* name) {
	GetModuleFileName(NULL, path, MAX_PATH);

	TCHAR* p = &path[_tcslen(path) - 1];
	while (p >= path && *p != '\\') {
		--p;
	}
	p[1] = 0;
	_tcscat_s(path, MAX_PATH, name); // MAX_PATH * sizeof(TCHAR)

	return path;
}

void ResizeWindow(HWND hWnd) {
	RECT rc;

	if (!hWnd) {
		return;
	}

	if (GetClientRect(hWnd, &rc)) {
		SendMessage(hWnd, WM_SIZE, 0, MAKELPARAM(rc.right, rc.bottom));
	}
}

void SwitchWindowVisible(HWND hWnd) {
	if (IsWindowVisible(hWnd)) {
		ShowWindow(hWnd, SW_HIDE);
	}
	else {
		ShowWindow(hWnd, SW_RESTORE);
		SetForegroundWindow(hWnd);
	}
}