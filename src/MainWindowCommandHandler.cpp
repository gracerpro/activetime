#include "MainWindow.h"
#include "resource.h"
#include "global.h"
#include "AboutDlg.h"
#include "TimeEditorWnd.h"

using namespace TimeEditorWnd;

namespace MainWindow {

void MainWindow_OnFileOpen() {

}

void MainWindow_OnFileSaveAs() {

}

void MainWindow_OnFileEditTime() {
	CompTimeStore& timeStore = Document::GetCompTimeStore();
	CreateTimeEditorWnd(Application::GetMainWindow(), timeStore);
}

void MainWindow_OnAppExit() {
#ifdef _RELEASE
	if (MBOK != Application::MessageBoxMy(TEXT("Вы уверены?"),
			MB_OKCANCEL | MB_ICONQUESTION))	{
		return;
	}
#endif
	MainWindow_OnDestroy(Application::GetMainWindow());
}

static void ShowHideToolBars(HWND hWndBar, int id) {
	HMENU hMenu = GetMenu(Application::GetMainWindow());
	UINT cmdShow, cmdCheck;

	if (IsWindowVisible(hWndBar)) {
		cmdShow =  SW_HIDE;
		cmdCheck = MF_UNCHECKED;
	}
	else {
		cmdShow =  SW_SHOW;
		cmdCheck = MF_CHECKED;
	}

	ShowWindow(hWndBar, cmdShow);
	CheckMenuItem(hMenu, id, cmdCheck);

	ResizeWindow(Application::GetMainWindow());
	InvalidateRect(Application::GetMainWindow(), NULL, FALSE);
}

void MainWindow_OnViewToolbar() {
	ShowHideToolBars(GetToolBar(), IDC_VIEW_TOOLBAR);
}

void MainWindow_OnViewStatusbar() {
	ShowHideToolBars(GetStatusBar(), IDC_VIEW_STATUSBAR);
}

void MainWindow_OnViewGrid() {
	HMENU hMenu = GetMenu(Application::GetMainWindow());
	UINT state = GetMenuState(hMenu, IDC_VIEW_GRID, MF_BYCOMMAND);
	UINT newState = state & MF_CHECKED ? MF_UNCHECKED : MF_CHECKED;
	CheckMenuItem(hMenu, IDC_VIEW_GRID, newState);
	InvalidateRect(Application::GetGraphicWindow(), NULL, FALSE);
}

void MainWindow_OnHelpHelp() {

}

void MainWindow_OnHelpAbout() {
	AboutDlg::ShowAboutDlg(Application::GetMainWindow());
}

void MainWindow_OnShowHide() {
	SwitchWindowVisible(Application::GetMainWindow());
}

}