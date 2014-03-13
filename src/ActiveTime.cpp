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


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCommandLine, int nCmdShow) {
	int res = 0;
	TCHAR logPath[MAX_PATH];

	//hInst = GetModuleHandle(NULL);
	//Application::SetInstance(hInst);

	GetNearExePath(logPath, TEXT("ActiveTime.log"));
	g_hLogFile = _tfsopen(logPath, TEXT("wt"), SH_DENYWR);
	if (!g_hLogFile) {
		return 1;
	}

//	eCommandLineAction action = ParseParameters(argc, argv);

	InitInstance(hInst);

	int argc = 0;
	int commandLineLen = strlen(lpCommandLine);
	LPWSTR commandLine = new WCHAR[commandLineLen + 1];
	MultiByteToWideChar(CP_ACP, 0, lpCommandLine, commandLineLen, commandLine, commandLineLen + 1);
	commandLine[commandLineLen] = 0;
	LPWSTR* argv = CommandLineToArgvW(commandLine, &argc);
	if (argc && !wcscmp(argv[0], L"-start")) {
		Document::CleanLastTickCount();
	}
	delete[] commandLine;

	Run(hInst);

	LocalFree(argv);

	ExitInstance(hInst);

#ifdef _DEBUG
	//getchar();
#endif

	fclose(g_hLogFile);

	return res;
}

