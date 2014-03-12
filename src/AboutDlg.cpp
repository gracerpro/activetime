/*
 * AboutDlg.cpp
 */
#include "AboutDlg.h"
#include "global.h"
#include "resource.h"

namespace AboutDlg {

LRESULT WINAPI AboutDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int ShowAboutDlg(HWND hParentWnd) {
	return DialogBox(Application::GetInstance(), MAKEINTRESOURCE(IDD_ABOUT), hParentWnd, (DLGPROC)AboutDlgProc);
}

LRESULT WINAPI AboutDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:

		return TRUE;
	case WM_COMMAND:
		int id;

		id = LOWORD(wParam);
		if (id == IDOK || id == IDCANCEL) {
			return EndDialog(hWnd, id);
		}
		break;
	}

	return 0;
}



}