#include "Application.h"
#include "DateFilterDlg.h"
#include "global.h"
#include "resource.h"
#include "SystemTime.h"
#include <tchar.h>
#include <CommCtrl.h>

namespace DateFilterDlg {

HWND g_hWnd = NULL;

LRESULT WINAPI DateFilterDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void DateFilterDlg_OnSelectRangeDate(HWND hWnd, int id);
void DateFilterDlg_OnNotify(HWND hWnd, int id, LPNMHDR pNmdr);
void DateFilterDlg_OnInitDialog(HWND hWnd);
void DateFilterDlg_OnCommand(HWND hWnd, int id, int notifyCode, HWND hWndCtrl);

HWND CreateDateFilterDlg(HWND hWndParent) {
	HWND hWnd;

	hWnd = CreateDialog(Application::GetInstance(), MAKEINTRESOURCE(IDD_DATE_FILTER), hWndParent, (DLGPROC)DateFilterDlgProc);

	return hWnd;
}

LRESULT WINAPI DateFilterDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		DateFilterDlg_OnInitDialog(hWnd);
		return TRUE;
	case WM_COMMAND:
		DateFilterDlg_OnCommand(hWnd, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
		break;
	case WM_NOTIFY:
		DateFilterDlg_OnNotify(hWnd, wParam, (LPNMHDR)lParam);
		break;
	}

	return 0;
}

void GetSelectedDateRange(Date& minDate, Date& maxDate) {
	minDate = 0;
	maxDate = 0;

	if (!g_hWnd) {
		return;
	}

	SYSTEMTIME stMin, stMax;
	HWND hDtpMin = GetDlgItem(g_hWnd, IDC_DTP_MIN);
	HWND hDtpMax = GetDlgItem(g_hWnd, IDC_DTP_MAX);

	DateTime_GetSystemtime(hDtpMin, &stMin);
	DateTime_GetSystemtime(hDtpMax, &stMax);

	minDate = SystemTime::SystemTimeToDate(stMin);
	maxDate = SystemTime::SystemTimeToDate(stMax);
}

void DateFilterDlg_OnInitDialog(HWND hWnd) {
	g_hWnd = hWnd;

	const TCHAR* arrMonthName[] = {
		TEXT("Январь"), TEXT("Февраль"), TEXT("Март"), TEXT("Апрель"),
		TEXT("Май"), TEXT("Июнь"), TEXT("Июль"), TEXT("Август"),
		TEXT("Сентябрь"), TEXT("Октябрь"), TEXT("Ноябрь"), TEXT("Декабрь")
	};

	HWND hMonthList = GetDlgItem(hWnd, IDC_LST_MONTHS);
	SendMessage(hMonthList, LB_RESETCONTENT, 0, 0);
	for (size_t i = 0; i < 12; ++i) {
		SendMessage(hMonthList, LB_ADDSTRING, 0, (LPARAM)arrMonthName[i]);
	}

	CheckDlgButton(hWnd, IDC_RBN_CURRENT_MONTH, MF_CHECKED); // OnCommand don't occur
	DateFilterDlg_OnSelectRangeDate(hWnd, IDC_RBN_CURRENT_MONTH);
}

void DateFilterDlg_OnSelectRangeDate(HWND hWnd, int id) {
	SYSTEMTIME stMin, stMax;
	HWND hDtpMin = GetDlgItem(hWnd, IDC_DTP_MIN);
	HWND hDtpMax = GetDlgItem(hWnd, IDC_DTP_MAX);

	switch (id) {
	case IDC_RBN_CURRENT_WEEK:
		SystemTime::GetCurrentWeekRange(stMin, stMax);
		break;
	case IDC_RBN_CURRENT_MONTH:
		SystemTime::GetCurrentMonthRange(stMin, stMax);
		break;
	case IDC_RBN_CURRENT_YEAR:
		SystemTime::GetCurrentYearRange(stMin, stMax);
		break;
	case IDC_RBN_LAST_WEEK:
		SystemTime::GetLastWeekRange(stMin, stMax);
		break;
	case IDC_RBN_LAST_MONTH:
		SystemTime::GetLastMonthRange(stMin, stMax);
		break;
	case IDC_RBN_LAST_YEAR:
		SystemTime::GetLastYearRange(stMin, stMax);
		break;
	default:
		return;
	}

	DateTime_SetSystemtime(hDtpMin, GDT_VALID, &stMin);
	DateTime_SetSystemtime(hDtpMax, GDT_VALID, &stMax);
	InvalidateRect(Application::GetGraphicWindow(), NULL, FALSE);
}

void DateFilterDlg_OnCommand(HWND hWnd, int id, int notifyCode, HWND hWndCtrl) {
	if (notifyCode == LBN_SELCHANGE && id == IDC_LST_MONTHS) {
		int month = SendDlgItemMessage(hWnd, IDC_LST_MONTHS, LB_GETCURSEL, 0, 0);
		if (month != LB_ERR) {
			HWND hDtpMin = GetDlgItem(hWnd, IDC_DTP_MIN);
			HWND hDtpMax = GetDlgItem(hWnd, IDC_DTP_MAX);
			SYSTEMTIME stMin, stMax, st;
			GetSystemTime(&st);
			SystemTime::GetMonthRange(stMin, stMax, month + 1, st.wYear);
			DateTime_SetSystemtime(hDtpMin, GDT_VALID, &stMin);
			DateTime_SetSystemtime(hDtpMax, GDT_VALID, &stMax);
			InvalidateRect(Application::GetGraphicWindow(), NULL, FALSE);
		}
		return;
	}
	switch (id) {
	case IDC_RBN_CURRENT_WEEK:
	case IDC_RBN_CURRENT_MONTH:
	case IDC_RBN_CURRENT_YEAR:
	case IDC_RBN_LAST_WEEK:
	case IDC_RBN_LAST_MONTH:
	case IDC_RBN_LAST_YEAR:
		DateFilterDlg_OnSelectRangeDate(hWnd, id);
		break;
	}
}

void DateFilterDlg_OnNotify(HWND hWnd, int id, LPNMHDR pNmdr) {
	switch (id) {
	case IDC_DTP_MIN:
	case IDC_DTP_MAX:
		if (pNmdr->code == DTN_DATETIMECHANGE) {
			// hack, disable second DTN_DATETIMECHANGE
			static DWORD firstEventTime = 0;
			DWORD secondEventTime = GetTickCount();
			if (secondEventTime - firstEventTime < 100) {
				return;
			}
			firstEventTime = secondEventTime;

		//	LPNMDATETIMECHANGE lpChange = (LPNMDATETIMECHANGE)pNmdr;
			InvalidateRect(Application::GetGraphicWindow(), NULL, FALSE);
		}
		break;
	}
}

}
