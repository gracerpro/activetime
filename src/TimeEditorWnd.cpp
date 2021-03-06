#include "TimeEditorWnd.h"
#include "Application.h"
#include "MainWindow.h"
#include "resource.h"
#include <CommCtrl.h>

namespace TimeEditorWnd {

static HWND g_hWnd = NULL;
static bool g_bChange = false;

LRESULT WINAPI TimeEditorDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void TimeEditorWnd_OnInitDialog(HWND hWnd, CompTimeStore& timeStore);
void TimeEditorWnd_OnCommand(HWND hWnd, int id, int notifyCode, HWND hwndCtrl);
void TimeEditorWnd_OnSize(HWND hWnd, int cx, int cy, int flags);
void TimeEditorWnd_OnNotify(HWND hWnd, int id, LPNMHDR pNmhdr);

void TimeEditorWnd_OnListViewSelChange(Date date = 0);

void TimeEditorWnd_OnTimeAdd();
void TimeEditorWnd_OnTimeDel();
void TimeEditorWnd_OnTimeDelAll();
void TimeEditorWnd_OnTimeSet();


int CreateTimeEditorWnd(HWND hWndParent, CompTimeStore& timeStore) {
	return DialogBoxParam(Application::GetInstance(), MAKEINTRESOURCE(IDD_TIME_EDITOR), hWndParent,
		(DLGPROC)TimeEditorDlgProc, (LPARAM)&timeStore);
}

bool IsChanged() {
	return g_bChange;
}

LRESULT WINAPI TimeEditorDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_COMMAND:
		TimeEditorWnd_OnCommand(hWnd, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
		break;
	case WM_NOTIFY:
		TimeEditorWnd_OnNotify(hWnd, wParam, (LPNMHDR)lParam);
		break;
	case WM_SIZE:
		TimeEditorWnd_OnSize(hWnd, LOWORD(lParam), HIWORD(lParam), wParam);
		break;
	case WM_INITDIALOG:
		g_hWnd = hWnd;
		CompTimeStore* pTimeStore;
		pTimeStore = (CompTimeStore*)lParam;
		TimeEditorWnd_OnInitDialog(hWnd, *pTimeStore);
		return TRUE;
	case WM_CLOSE:
		if (g_bChange) {
			return 0;
		}
		break;
	}

	return 0;
}

void TimeEditorWnd_OnInitDialog(HWND hWnd, CompTimeStore& timeStore) {
	HWND hwndTvi = GetDlgItem(hWnd, IDC_LVI_TIME);
	MainWindow::FillTable(hwndTvi, timeStore, true);
	ListView_SetExtendedListViewStyleEx(hwndTvi, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	g_bChange = false;
}

void TimeEditorWnd_OnCommand(HWND hWnd, int id, int notifyCode, HWND hwndCtrl) {
	switch (id) {
	case IDOK:
	case IDCANCEL:
		g_hWnd = NULL;
		EndDialog(hWnd, id);
		break;
	case IDC_TIME_ADD:
		TimeEditorWnd_OnTimeAdd();
		break;
	case IDC_TIME_DEL:
		TimeEditorWnd_OnTimeDel();
		break;
	case IDC_TIME_SET:
		TimeEditorWnd_OnTimeSet();
		break;
	case IDC_TIME_DEL_ALL:
		TimeEditorWnd_OnTimeDelAll();
		break;
	}
}

void TimeEditorWnd_OnSize(HWND hWnd, int cx, int cy, int flags) {
	HWND hwndTvi = GetDlgItem(hWnd, IDC_LVI_TIME);

	MoveWindow(hwndTvi, 0, 180, cx, cy - 180, TRUE);
}

void TimeEditorWnd_OnListViewSelChange(Date date) {
	const CompTimeStore& timeStore = Document::GetCompTimeStore();
	CompTimeStoreConstIter iterFind = timeStore.find(date);
	if (iterFind != timeStore.end()) {
		const stCompTime& compTime = (*iterFind).second;
		Date date = (*iterFind).first;

		HWND hwndDtp = GetDlgItem(g_hWnd, IDC_DTP_DATE);
		SYSTEMTIME st;
		SystemTime::DateToSystemTime(st, date);
		DateTime_SetSystemtime(hwndDtp, GDT_VALID, &st);

		LPTSTR szActiveTime = SystemTime::TimeToStr(compTime.activeTime);
		SetDlgItemText(g_hWnd, IDC_EDT_ACTIVE_TIME, szActiveTime);
		LPTSTR szPassiveTime = SystemTime::TimeToStr(compTime.passiveTime);
		SetDlgItemText(g_hWnd, IDC_EDT_PASSIVE_TIME, szPassiveTime);
		LPTSTR szSleepTime = SystemTime::TimeToStr(compTime.sleepTime);
		SetDlgItemText(g_hWnd, IDC_EDT_SLEEP_TIME, szSleepTime);
	}
}

void TimeEditorWnd_OnNotify(HWND hWnd, int id, LPNMHDR pNmhdr) {
	switch (id) {
	case IDC_LVI_TIME:
		NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNmhdr;
		if (pNmhdr->code == LVN_ITEMCHANGED) {
			if (pnmv->uNewState & LVIS_SELECTED) {
				TimeEditorWnd_OnListViewSelChange((Date)pnmv->lParam);
			}
		}
		break;
	}
}

void TimeEditorWnd_OnTimeAdd() {
	stCompTime compTime;
	HWND hWnd = g_hWnd;

	// 2:30
	const int bufLen = 200;
	TCHAR buf[bufLen];
	SYSTEMTIME st;
	CompTimeStore& timeStore = Document::GetCompTimeStore();

	HWND hDtpDate = GetDlgItem(g_hWnd, IDC_DTP_DATE);
	if (!hDtpDate) {
		return;
	}
	DateTime_GetSystemtime(hDtpDate, &st);

	GetDlgItemText(hWnd, IDC_EDT_ACTIVE_TIME, buf, bufLen);
	compTime.activeTime = SystemTime::StrToTime(buf);

	// checking...
	Date date = SystemTime::SystemTimeToDate(st);
	CompTimeStore::const_iterator findIter = timeStore.find(date);
	if (findIter != timeStore.end()) {
		Application::MessageBoxMy(TEXT("�� ���� ���� ����� ����������. �������� ������ �����."));
		return;
	}

	// add
	timeStore[date] = compTime;

	HWND hListView = GetDlgItem(hWnd, IDC_LVI_TIME);
	MainWindow::AddCompTimeToLvi(hListView, date, compTime);

	g_bChange = true;
}

void TimeEditorWnd_OnTimeDel() {
	HWND hListView = GetDlgItem(g_hWnd, IDC_LVI_TIME);

	int index = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
	if (index == -1) {
		return;
	}

	LV_ITEM item = {0};
	item.iItem = index;
	item.mask  = LVIF_PARAM;
	if (!ListView_GetItem(hListView, &item)) {
		return;
	}

	Date date = (Date)item.lParam;
	CompTimeStore& timeStore = Document::GetCompTimeStore();
	CompTimeStore::iterator findIter = timeStore.find(date);
	if (findIter != timeStore.end()) {
		timeStore.erase(findIter);
	}
	ListView_DeleteItem(hListView, index);

	g_bChange = true;
}

void TimeEditorWnd_OnTimeDelAll() {
	if (IDOK != Application::MessageBoxMy(TEXT("������������ �������� ���� ������"), MB_OKCANCEL | MB_ICONQUESTION)) {
		return;
	}

	HWND hListView = GetDlgItem(g_hWnd, IDC_LVI_TIME);
	ListView_DeleteAllItems(hListView);

	CompTimeStore& timeStore = Document::GetCompTimeStore();
	timeStore.clear();

	g_bChange = true;
}

void TimeEditorWnd_OnTimeSet() {
	HWND hListView = GetDlgItem(g_hWnd, IDC_LVI_TIME);

	int index = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
	if (index == -1) {
		return;
	}

	LV_ITEM item = {0};
	item.iItem = index;
	item.mask  = LVIF_PARAM;
	if (!ListView_GetItem(hListView, &item)) {
		return;
	}

	CompTimeStore& timeStore = Document::GetCompTimeStore();
	Date date = (Date)item.lParam;
	CompTimeStore::iterator findIter = timeStore.find(date);
	if (findIter == timeStore.end()) {
		return;
	}

	TCHAR buf[100];
	stCompTime& compTime = (*findIter).second;

	GetDlgItemText(g_hWnd, IDC_EDT_ACTIVE_TIME, buf, 100);
	compTime.activeTime  = SystemTime::StrToTime(buf);
	ListView_SetItemText(hListView, index, 1, SystemTime::TimeToStr(compTime.activeTime));
	GetDlgItemText(g_hWnd, IDC_EDT_PASSIVE_TIME, buf, 100);
	compTime.passiveTime = SystemTime::StrToTime(buf);
	ListView_SetItemText(hListView, index, 2, SystemTime::TimeToStr(compTime.passiveTime));
	GetDlgItemText(g_hWnd, IDC_EDT_SLEEP_TIME, buf, 100);
	compTime.sleepTime   = SystemTime::StrToTime(buf);
	ListView_SetItemText(hListView, index, 3, SystemTime::TimeToStr(compTime.sleepTime));

	g_bChange = true;
}

}
