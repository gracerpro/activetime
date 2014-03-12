/*
 * ActiveTime.cpp
 */
#include "Application.h"
#include "global.h"
#include <share.h>
#include <Windows.h>
#include <tchar.h>


using namespace Application;


enum eCommandLineAction {
	COMMAND_LINE_ACTION_NULL,
	COMMAND_LINE_ACTION_INSTALL,
	COMMAND_LINE_ACTION_UNINSTALL,
	COMMAND_LINE_ACTION_START,
	COMMAND_LINE_ACTION_STOP,
};


eCommandLineAction ParseParameters(int argc, const TCHAR* argv[]) {
	if (argc > 1) {
		const TCHAR* action = argv[1];
		if (!_tcscmp(action, TEXT("install"))) {
			return COMMAND_LINE_ACTION_INSTALL;
		}
		if (!_tcscmp(action, TEXT("uninstall"))) {
			return COMMAND_LINE_ACTION_UNINSTALL;
		}
		if (!_tcscmp(action, TEXT("start"))) {
			return COMMAND_LINE_ACTION_START;
		}
	}

	return COMMAND_LINE_ACTION_NULL;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCommandLine, int nCmdShow) {
	int res = 0;
	TCHAR logPath[MAX_PATH];

	//hInst = GetModuleHandle(NULL);
	Application::SetInstance(hInst);

	GetNearExePath(logPath, TEXT("ActiveTime.log"));
	g_hLogFile = _tfsopen(logPath, TEXT("wt"), SH_DENYWR);
	if (!g_hLogFile) {
		return 1;
	}

//	eCommandLineAction action = ParseParameters(argc, argv);

	InitInstance(hInst);

	Run(hInst);

	ExitInstance(hInst);

#ifdef _DEBUG
	//getchar();
#endif

	fclose(g_hLogFile);

	return res;
}

