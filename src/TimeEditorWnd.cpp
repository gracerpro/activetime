#include "TimeEditorWnd.h"
#include "Application.h"
#include "MainWindow.h"
#include "resource.h"
#include <CommCtrl.h>

namespace TimeEditorWnd {

static HWND g_hWnd = NULL;

LRESULT WINAPI TimeEditorDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void TimeEditorWnd_OnInitDialog(HWND hWnd, CompTimeStore& timeStore);
void TimeEditorWnd_OnCommand(HWND hWnd, int id, int notifyCode, HWND hwndCtrl);
void TimeEditorWnd_OnSize(HWND hWnd, int cx, int cy, int flags);
void TimeEditorWnd_OnNotify(HWND hWnd, int id, LPNMHDR pNmhdr);


int CreateTimeEditorWnd(HWND hWndParent, CompTimeStore& timeStore) {
	return DialogBoxParam(Application::GetInstance(), MAKEINTRESOURCE(IDD_TIME_EDITOR), hWndParent,
		(DLGPROC)TimeEditorDlgProc, (LPARAM)&timeStore);
}

LRESULT WINAPI TimeEditorDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_COMMAND:
		TimeEditorWnd_OnCommand(hWnd, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
		break;
	case WM_SIZE:
		TimeEditorWnd_OnSize(hWnd, LOWORD(lParam), HIWORD(lParam), wParam);
		break;
	case WM_INITDIALOG:
		g_hWnd = hWnd;
		CompTimeStore* pTimeStore = (CompTimeStore*)lParam;
		TimeEditorWnd_OnInitDialog(hWnd, *pTimeStore);
		return TRUE;
	}

	return 0;
}

void TimeEditorWnd_OnInitDialog(HWND hWnd, CompTimeStore& timeStore) {
	HWND hwndTvi = GetDlgItem(hWnd, IDC_TVI_TIME);
	MainWindow::FillTable(hwndTvi, timeStore, true);
}

void TimeEditorWnd_OnCommand(HWND hWnd, int id, int notifyCode, HWND hwndCtrl) {
	switch (id) {
	case IDOK:
	case IDCANCEL:
		g_hWnd = NULL;
		EndDialog(hWnd, id);
		break;
	}
}

void TimeEditorWnd_OnSize(HWND hWnd, int cx, int cy, int flags) {
	HWND hwndTvi = GetDlgItem(hWnd, IDC_TVI_TIME);

	MoveWindow(hwndTvi, 0, 100, cx, cy - 150, TRUE);
}

void TimeEditorWnd_OnNotify(HWND hWnd, int id, LPNMHDR pNmhdr) {
	switch (id) {
	case IDC_TVI_TIME:
		NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNmhdr;
		if (pNmhdr->code == LVN_ITEMCHANGED) {
			if (pnmv->uNewState & LVIS_SELECTED) {
				const CompTimeStore& timeStore = Document::GetCompTimeStore();
				Date date = (Date)pnmv->lParam;
				CompTimeStoreConstIter iterFind = timeStore.find(date);
				if (iterFind != timeStore.end()) {
					// Item to controls
				}
			}
		}
		break;
	}
}

}