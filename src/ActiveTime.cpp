/*
 * ActiveTime.cpp
 */
#include "Application.h"
#include "Document.h"
#include "global.h"
#include <Windows.h>
#include <share.h>
#include <tchar.h>
#include <wchar.h>


using namespace Application;

bool ParseCommandLine(LPSTR lpCommandLine) {
	int argc = 0;
	int commandLineLen = strlen(lpCommandLine);
	LPWSTR commandLine;
	try {
		commandLine = new WCHAR[commandLineLen + 1];
	}
	catch (...) {
		LogErr("memory allocate fail");
		return false;
	}
	MultiByteToWideChar(CP_ACP, 0, lpCommandLine, commandLineLen, commandLine, commandLineLen + 1);
	commandLine[commandLineLen] = 0;
	LPWSTR* argv = CommandLineToArgvW(commandLine, &argc);
	if (argc && !wcscmp(argv[0], L"-start")) {
		Document::CleanLastTickCount();
	}
	delete[] commandLine;

	LocalFree(argv);

	return true;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCommandLine, int nCmdShow) {
	int res = 0;
	TCHAR logPath[MAX_PATH];

	GetNearExePath(logPath, TEXT("ActiveTime.log"));
	g_hLogFile = _tfsopen(logPath, TEXT("wt"), SH_DENYWR);
	if (!g_hLogFile) {
		return 1;
	}

	ParseCommandLine(lpCommandLine);

	InitInstance(hInst);

	Run(hInst);

	ExitInstance(hInst);

	fclose(g_hLogFile);

	return res;
}

