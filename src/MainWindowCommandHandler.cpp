#include "MainWindow.h"
#include "resource.h"
#include "global.h"
#include "AboutDlg.h"


namespace MainWindow {

void MainWindow_OnFileOpen() {

}

void MainWindow_OnFileSaveAs() {

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

void MainWindow_OnViewToolbar() {

}

void MainWindow_OnViewStatusbar() {

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