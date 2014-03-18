/*
 * MainWindow.cpp
 */
#include "Application.h"
#include "MainWindow.h"
#include "DateFilterDlg.h"
#include "GraphicWnd.h"
#include "global.h"
#include "resource.h"
#include <CommCtrl.h>
#include <tchar.h>

static const TCHAR g_className[] = TEXT("ActiveTimeMainWindow");
static const TCHAR g_Title[]     = TEXT("ActiveTime");
static HWND g_hwndStatusBar = NULL;
static HWND g_hwndToolBar = NULL;
static HWND g_hwndDateFilterWnd = NULL;
static HWND g_hwndLviTime = NULL;

using namespace DateFilterDlg;
using namespace GraphicWnd;

namespace MainWindow {

const int ID_MAIN_STATUSBAR = 1000;
const int ID_TIME_LISTBOX   = 1001;

bool RegisterMainWindowClass();
LRESULT WINAPI MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void MainWindow_OnCreate(HWND hWnd);
void MainWindow_OnSize(HWND hWnd, int cx, int cy, int action);
void MainWindow_OnPaint(HWND hWnd, HDC hDC);
void MainWindow_OnTray(HWND hWnd, int notifyEvent, int iconId, UINT mouseMessage);
void MainWindow_OnCommand(HWND hWnd, int id, int notifyCode, HWND hWndCtrl);
void MainWindow_OnNotify(int id, LPNMHDR pNMDR);
void MainWindow_OnPowerBroadcast(HWND hWnd, int eventId, LPARAM param);
void MainWindow_OnClose(HWND hWnd);
void MainWindow_OnGetMinMaxInfo(LPMINMAXINFO mmi);
void MainWindow_OnMouseMove(int flags, int x, int y);
void MainWindow_OnActivate(HWND hWnd);

bool RegisterMainWindowClass() {
	WNDCLASS wc = {0};

	HINSTANCE hInst = GetModuleHandle(NULL);

	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance     = hInst;
	wc.lpfnWndProc   = MainWindowProc;
	wc.lpszClassName = g_className;
	wc.style         = CS_HREDRAW | CS_VREDRAW;

	return RegisterClass(&wc) != 0;
}

HWND CreateMainWindow(HINSTANCE hInst) {
	if (!RegisterMainWindowClass()) {
		Log("Can't register main window class");
		return NULL;
	}

	// TODO: read window placement...

	HMENU hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
	if (!hMenu) {
		LogLastError();
	}

	HWND hWnd = CreateWindow(g_className, g_Title,
		WS_BORDER | WS_SIZEBOX | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, hMenu, hInst, 0);
	if (!hWnd) {
		return NULL;
	}

	return hWnd;
}

void MainWindow_OnCommand(HWND hWnd, int id, int notifyCode, HWND hWndCtrl) {
	switch (id) {
	case IDC_FILE_OPEN:
		MainWindow_OnFileOpen();
		break;
	case IDC_FILE_CLOSE:
		MainWindow_OnClose(hWnd);
		break;
	case IDC_FILE_SAVEAS:
		MainWindow_OnFileSaveAs();
		break;
	case IDC_FILE_EDIT_TIME:
		MainWindow_OnFileEditTime();
		break;
	case IDC_VIEW_STATUSBAR:
		MainWindow_OnViewStatusbar();
		break;
	case IDC_VIEW_TOOLBAR:
		MainWindow_OnViewToolbar();
		break;
	case IDC_VIEW_GRID:
		MainWindow_OnViewGrid();
		break;
	case IDC_HELP_ABOUT:
		MainWindow_OnHelpAbout();
		break;
	case IDC_HELP_HELP:
		MainWindow_OnHelpHelp();
		break;
	case IDC_APP_EXIT:
		MainWindow_OnAppExit();
		break;
	case IDC_SHOW_HIDE:
		MainWindow_OnShowHide();
		break;
	}
}

void AddCompTimeToLvi(HWND hwndTvi, Date itemDate, const stCompTime& compTime) {
	LV_ITEM item = {0};

	item.mask = LVIF_TEXT | LVIF_PARAM;

	item.pszText = SystemTime::DateToStr(itemDate);
	item.cchTextMax = _tcslen(item.pszText);
	item.lParam = itemDate;
	ListView_InsertItem(hwndTvi, &item);

	LPTSTR szActiveTime = SystemTime::TimeToStr(compTime.activeTime);
	ListView_SetItemText(hwndTvi, 0, 1, szActiveTime);
	LPTSTR szPassiveTime = SystemTime::TimeToStr(compTime.passiveTime);
	ListView_SetItemText(hwndTvi, 0, 2, szPassiveTime);
	LPTSTR szSleepTime = SystemTime::TimeToStr(compTime.sleepTime);
	ListView_SetItemText(hwndTvi, 0, 3, szSleepTime);
}

int FillTable(HWND hwndTvi, const CompTimeStore& timeStore, bool rebuild) {
	int addedCount = 0;
	LV_COLUMN col = {0};

	if (!IsWindow(hwndTvi)) {
		return 0;
	}

	// Add columns
	if (rebuild) {
		memset(&col, 0, sizeof(col));
		col.cx = 70;
		col.fmt = LVCFMT_CENTER;
		col.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_FMT;
		col.cchTextMax = 10;
		col.pszText = TEXT("Date");
		ListView_InsertColumn(hwndTvi, 0, &col);
		col.cx = 60;
		col.pszText = TEXT("Active");
		ListView_InsertColumn(hwndTvi, 1, &col);
		col.pszText = TEXT("Passive");
		ListView_InsertColumn(hwndTvi, 2, &col);
		col.pszText = TEXT("Sleep");
		ListView_InsertColumn(hwndTvi, 3, &col);
	}

	ListView_DeleteAllItems(hwndTvi);
	// Add data
	CompTimeStoreConstIter iter = timeStore.begin();
	for ( ; iter != timeStore.end(); ++iter) {
		Date itemDate = (*iter).first;
		const stCompTime& compTime = (*iter).second;

		AddCompTimeToLvi(hwndTvi, itemDate, compTime);
	}

	return addedCount;
}

void MainWindow_OnCreate(HWND hWnd) {
//	g_hWnd = hWnd;
	g_hwndDateFilterWnd = CreateDateFilterDlg(hWnd);
	if (!g_hwndDateFilterWnd) {
		Log("Can't create date filter dialog");
		return;
	}

	g_hwndStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, NULL, hWnd, ID_MAIN_STATUSBAR);

	g_hwndLviTime = CreateWindowEx(0, WC_LISTVIEW, NULL,
		WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | WS_TABSTOP,
		0, 0, 100, 100,
		hWnd, (HMENU)ID_TIME_LISTBOX, Application::GetInstance(), 0);
	ListView_SetExtendedListViewStyleEx(g_hwndLviTime, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	Document::LoadTime();
	FillTable(g_hwndLviTime, Document::GetCompTimeStore(), true);
}

void MainWindow_OnDestroy(HWND hWnd) {
	Document::SaveTime();
	Document::FreeTime();

	NOTIFYICONDATA nid = {0};
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	Shell_NotifyIcon(NIM_DELETE, &nid);

	PostQuitMessage(0);
}

void MainWindow_OnSize(HWND hWnd, int cx, int cy, int action) {
	RECT rc;

	if (action == SIZE_MINIMIZED) {
		Application::SwapRam();
		MainWindow_OnClose(hWnd);
		return; // do not process
	}

	const int lviWidth = 70 + 60 * 3;
	int toolbarHeight = 0;
	int statusbarHeight = 0;

	if (g_hwndStatusBar && IsWindowVisible(g_hwndStatusBar)) {
		SendMessage(g_hwndStatusBar, WM_SIZE, 0, 0);
		GetWindowRect(g_hwndStatusBar, &rc);
		statusbarHeight = rc.bottom - rc.top;
	}
	if (g_hwndToolBar && IsWindowVisible(g_hwndToolBar)) {
		SendMessage(g_hwndToolBar, TB_AUTOSIZE, 0, 0);
		GetWindowRect(g_hwndToolBar, &rc);
		toolbarHeight = rc.bottom - rc.top;
	}

	int filterHeight = 0;
	if (g_hwndDateFilterWnd) {
		GetWindowRect(g_hwndDateFilterWnd, &rc);
		filterHeight = rc.bottom - rc.top;
		MoveWindow(g_hwndDateFilterWnd, 0, toolbarHeight, cx, filterHeight, TRUE);
	}

	int clientHeight = cy - toolbarHeight - filterHeight - statusbarHeight;
	if (g_hwndLviTime) {
		MoveWindow(g_hwndLviTime, 0, filterHeight + toolbarHeight, lviWidth, clientHeight, TRUE);
	}

	if (HWND hGraphicWnd = Application::GetGraphicWindow()) {
		if (hWnd == GetParent(hGraphicWnd)) {
			MoveWindow(hGraphicWnd, lviWidth, filterHeight, cx - lviWidth, clientHeight, TRUE);
		}
	}
}

void MainWindow_OnPaint(HWND hWnd, HDC hDC) {

}

void MainWindow_OnTray(HWND hWnd, int notifyEvent, int iconId, WPARAM wParam) {
	if (notifyEvent == WM_LBUTTONDOWN) {
		SwitchWindowVisible(hWnd);
	}
	if (notifyEvent == WM_RBUTTONDOWN) {
		HMENU hMenu = LoadMenu(Application::GetInstance(), MAKEINTRESOURCE(IDR_TRAY_MENU));
		HMENU hSubMenu = GetSubMenu(hMenu, 0);
		POINT pt;

		GetCursorPos(&pt);
		SetForegroundWindow(hWnd);
		TrackPopupMenu(hSubMenu, 0, pt.x, pt.y, 0, hWnd, NULL);

		DestroyMenu(hMenu);
	}
#ifdef _DEBUG
	TCHAR buf[200];

	SYSTEMTIME st;

	GetSystemTime(&st);

	wsprintf(buf, TEXT("%02d:%02d:%02d:%03d %04d/%02d/%02d tray event %d"),
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
		st.wYear, st.wMonth, st.wDay, notifyEvent);

	Log(buf);
#endif
}

void MainWindow_OnNotify(int id, LPNMHDR pNMDR) {
	switch (id) {
	case ID_TIME_LISTBOX:
		NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMDR;
		if (pNMDR->code == LVN_ITEMCHANGED) {
			if (pnmv->uNewState & LVIS_SELECTED) {
				const CompTimeStore& timeStore = Document::GetCompTimeStore();
				Date date = (Date)pnmv->lParam;
				CompTimeStoreConstIter iterFind = timeStore.find(date);
				if (iterFind != timeStore.end()) {
					GraphicWnd::SetSelectedDate(date);
					InvalidateRect(Application::GetGraphicWindow(), NULL, FALSE);
				}
			}
		}
		break;
	}
}

void MainWindow_OnPowerBroadcast(HWND hWnd, int eventId, LPARAM param) {
	TCHAR buf[200];
	SYSTEMTIME st;

	GetSystemTime(&st);

	wsprintf(buf, TEXT("%02d:%02d:%02d:%03d %04d/%02d/%02d %d %d"), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
		st.wYear, st.wMonth, st.wDay,
		eventId, param);

	Log(buf);
	/*
	10:28:26:950 2014/03/11 0 1
	10:28:27:043 2014/03/11 4 0
	11:17:54:625 2014/03/11 18 0
	11:17:54:640 2014/03/11 7 0
	*/
	static DWORD ticksBeforeSleep = 0;

	switch (eventId) {
	case PBT_APMQUERYSUSPEND: // sleep
		break;
	case PBT_APMSUSPEND:
		ticksBeforeSleep = GetTickCount();
		wsprintf(buf, TEXT("PBT_APMSUSPEND (to sleep): %d"), ticksBeforeSleep);
		Log(buf);
		break;
	case PBT_APMRESUMEAUTOMATIC:
		break;
	case PBT_APMRESUMESUSPEND:
		DWORD ticksAfterSleep;

		ticksAfterSleep = GetTickCount();

		g_sleepTime += (ticksAfterSleep - ticksBeforeSleep) / 1000;

		wsprintf(buf, TEXT("PBT_APMRESUMESUSPEND (from sleep): %d"), ticksAfterSleep);
		Log(buf);
		wsprintf(buf, TEXT("time in sleep: %d seconds"), (ticksAfterSleep - ticksBeforeSleep) / 1000);
		Log(buf);
		wsprintf(buf, TEXT("g_sleepTime: %d"), g_sleepTime);
		Log(buf);

		break;
	}
}

void MainWindow_OnClose(HWND hWnd) {
#ifdef _DEBUG
	MainWindow_OnDestroy(hWnd);
#else
	MainWindow_OnShowHide();
#endif
}

void MainWindow_OnGetMinMaxInfo(LPMINMAXINFO mmi) {
	mmi->ptMinTrackSize.x = 400;
	mmi->ptMinTrackSize.y = 300;
}

void MainWindow_OnActivate(HWND hWnd) {
	TCHAR buf[200];
	DWORD systemTime = GetTickCount() / 1000;

	_tcscpy(buf, g_Title);
	_tcscat(buf, TEXT(" "));
	_tcscat(buf, SystemTime::TimeToStr(systemTime));
	SetWindowText(hWnd,  buf);
}

HWND GetStatusBar() {
	return g_hwndStatusBar;
}

HWND GetToolBar() {
	return g_hwndToolBar;
}

HWND GetLviTime() {
	return g_hwndLviTime;
}

LRESULT WINAPI MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hDC;

		hDC = BeginPaint(hWnd, &ps);
		MainWindow_OnPaint(hWnd, hDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_ERASEBKGND:
		return 0;
	case WM_SIZE:
		MainWindow_OnSize(hWnd, LOWORD(lParam), HIWORD(lParam), wParam);
		break;
	case WM_GETMINMAXINFO:
		MainWindow_OnGetMinMaxInfo((LPMINMAXINFO)lParam);
		break;
	case WM_CREATE:
		MainWindow_OnCreate(hWnd);
		break;
	case WM_TRAY_MESSAGE:
		MainWindow_OnTray(hWnd, LOWORD(lParam), HIWORD(lParam), wParam);
		break;
	case WM_COMMAND:
		MainWindow_OnCommand(hWnd, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
		break;
	case WM_NOTIFY:
		MainWindow_OnNotify(wParam, (LPNMHDR)lParam);
		break;
	case WM_CLOSE:
		MainWindow_OnClose(hWnd);
		break;
	case WM_POWERBROADCAST:
		MainWindow_OnPowerBroadcast(hWnd, wParam, lParam);
		break;
	case WM_DESTROY:
		MainWindow_OnDestroy(hWnd);
		break;
	case WM_ACTIVATE:
		MainWindow_OnActivate(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

}
