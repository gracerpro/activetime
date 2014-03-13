#include "GraphicWnd.h"
#include "DateFilterDlg.h"
#include "global.h"
#include "resource.h"
#include <math.h>
#include <CommCtrl.h>

namespace GraphicWnd {

static HWND g_hWnd = NULL;
static Date g_selectedDate = 0;

static LRESULT WINAPI GraphicWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void GraphicWnd_OnPaint(HWND hWnd, HDC hDC);
HBRUSH GraphicWnd_OnEraseBkgnd(HWND hWnd, HDC hDC);
void GraphicWnd_OnMouseMove(int flags, int x, int y);


HWND CreateGraphicWnd(HWND hWndParent, int style) {
	HWND hWnd = 0;

	hWnd = CreateDialog(Application::GetInstance(), MAKEINTRESOURCE(IDD_GRAPHIC), hWndParent, (DLGPROC)GraphicWndProc);
	if (!hWnd) {
		return NULL;
	}
	g_hWnd = hWnd;

	return hWnd;
}

HWND GetHwnd() {
	return g_hWnd;
}

LRESULT WINAPI GraphicWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hDC;

		hDC = BeginPaint(hWnd, &ps);
		GraphicWnd_OnPaint(hWnd, hDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_MOUSEMOVE:
		GraphicWnd_OnMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));

	//	SendMessage(g_hwndTooltip, TTM_RELAYEVENT, 0, (LPARAM) (LPMSG) &msg);
		break;
	//case WM_ERASEBKGND:
	//	return (LRESULT)GraphicWnd_OnEraseBkgnd(hWnd, (HDC)wParam);
	case WM_CTLCOLORDLG:
		return (LRESULT)GraphicWnd_OnEraseBkgnd(hWnd, (HDC)wParam);
	case WM_INITDIALOG:
		g_hWnd = hWnd;
		return TRUE;
	case WM_COMMAND:

		break;
	//default:
	//	return DefDlgProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void SetParentWnd(bool toMainWin) {
	if (toMainWin) {
		if (GetParent(g_hWnd) == Application::GetMainWindow()) {
			return;
		}
		SetParent(g_hWnd, Application::GetGraphicPopupWindow());
		ResizeWindow(Application::GetGraphicPopupWindow());
	}
	else {
		if (GetParent(g_hWnd) == Application::GetGraphicPopupWindow()) {
			return;
		}
		SetParent(g_hWnd, Application::GetMainWindow());
		ResizeWindow(Application::GetMainWindow());
	}
}

void SetSelectedDate(Date date) {
	g_selectedDate = date;
}

Date GetSelectedDate() {
	return g_selectedDate;
}

void RectangleWare(HDC hDC, int x1, int y1, int x2, int y2) {
	MoveToEx(hDC, x1, y1, NULL);
	LineTo(hDC, x2, y1);
	LineTo(hDC, x2, y2);
	LineTo(hDC, x1, y2);
	LineTo(hDC, x1, y1);
}

const int margin = 30;
const int textHeight = 20;

static void DrawAxes(HDC hDC, RECT& clientRect) {
	MoveToEx(hDC, margin, margin, NULL);
	LineTo(hDC, margin, clientRect.bottom - margin);
	LineTo(hDC, clientRect.right - margin, clientRect.bottom - margin);

	TCHAR timeTitle[] = TEXT("time, (h)");
	TextOut(hDC, 5, 5, timeTitle, 9);
	TCHAR dateTitle[] = TEXT("date, (d)");
	TextOut(hDC, clientRect.right - 60, clientRect.bottom - textHeight, dateTitle, 9);
}

static void DrawHorizontalLines(HDC hDC, RECT& clientRect, int gistohramHeight) {
	TCHAR buf[10]; // 1, 2, 3 .. 24 numbers
	HPEN penHorz = CreatePen(PS_DASH, 1, RGB(255, 255, 255));
	HPEN penHorzOld = (HPEN)SelectObject(hDC, penHorz);

	int y = clientRect.bottom - margin;
	const int maxGridY = 24;

	for (size_t i = 3; i <= maxGridY; i += 3) {
		int y2 = y - i * gistohramHeight / maxGridY;
		MoveToEx(hDC, margin + 1, y2, NULL);
		LineTo(hDC, clientRect.right - margin, y2);

		_itot(i, buf, 10);
		TextOut(hDC, 5, y2, buf, _tcslen(buf));
	}
	SelectObject(hDC, penHorzOld);
	DeleteObject(penHorz);
}

int GetVertLineCount(Date minDate, Date maxDate) {
	int count = 1;
	int daysCount = maxDate - minDate;

	if (daysCount > 7) {
		if (daysCount < 31) {
			count = 3;
		}
		else if (daysCount < 100) {
			count = 8;
		}
		else {
			count = 16;
		}
	}

	return count;
}

static void DrawVerticalLines(HDC hDC, RECT& clientRect, int gistohramWidth, int gistohramHeight,
	Date minDate, Date maxDate) {

	TCHAR buf[10]; // "1", "2" ... "24" numbers
	HPEN penHorz = CreatePen(PS_DASH, 1, RGB(255, 255, 255));
	HPEN penHorzOld = (HPEN)SelectObject(hDC, penHorz);

	int y = clientRect.bottom - margin;
	const int maxGridY = 24;
	const int daysCount = maxDate - minDate;
	const int lineCount = GetVertLineCount(minDate, maxDate);
	float dx = gistohramWidth * lineCount / (float)(daysCount);
	float x = dx + margin;

	int day = lineCount;
	while (x < gistohramWidth) {
		MoveToEx(hDC, static_cast<int>(x), margin, NULL);
		LineTo(hDC, static_cast<int>(x), clientRect.bottom - margin);

		_itot(day, buf, 10);
		// week   1 ... 7
		// month  1 ... 31
		// year   1 ... 12
		// range ...
		TextOut(hDC, static_cast<int>(x), clientRect.bottom - margin, buf, _tcslen(buf));

		x += dx;
		day += lineCount;
	}
	SelectObject(hDC, penHorzOld);
	DeleteObject(penHorz);
}

static CompTimeStoreConstIter FindStartDate(const CompTimeStore& timeStore, Date minDate, Date maxDate) {
	// try find fast
	if (timeStore.empty()) {
		return timeStore.end();
	}
	CompTimeStoreConstIter iter = timeStore.find(minDate);
	if (iter != timeStore.end()) {
		return iter;
	}
	for (iter = timeStore.begin(); iter != timeStore.end(); ++iter) {
		Date date = (*iter).first;
		//stCompTime& compTime = (*iter).second;
		if (date > maxDate) {
			return timeStore.end();
		}
		if (date >= minDate) {
			return iter;
		}
	}

	return timeStore.begin();
}

/*
(x, y) -- left bottom point of rectangle
*/
static void DrawHostogram(HDC hDC, RECT& clientRect, int gistohramWidth, int gistohramHeight,
	const CompTimeStore& timeStore, Date minDate, Date maxDate ) {

	int dayCount = maxDate - minDate;
	if (dayCount <= 1) {
		dayCount = 30; // TODO: get days in month
	}
	HPEN penGistogram = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
	HPEN penGistogramOld = (HPEN)SelectObject(hDC, penGistogram);

	float selectedDayX = 0;
	const int maxHours = 24;
	float yGridLenInvert = 1 / (float)maxHours * gistohramHeight;
	float dx = gistohramWidth / (float)dayCount;
	float x = margin + dx;
	int y = clientRect.bottom - margin;
	bool bBeginLine = true;

	CompTimeStoreConstIter iter = FindStartDate(timeStore, minDate, maxDate);
	if (iter == timeStore.end()) {
		return;
	}
	for (Date i = minDate; i < maxDate; ++i, x += dx) {
		//  ; iter != timeStore.end(); ++iter, x += dx
		if (iter == timeStore.end()) {
			break;
		}
		Date date = (*iter).first;
		const stCompTime& compTime = (*iter).second;

		if (date != i) {
			bBeginLine = true;
			continue;
		}

		if (date == g_selectedDate) {
			selectedDayX = x;
			//continue;
		}
		float hours = compTime.ActiveToHours();
		if (hours > maxHours) {
			hours = maxHours;
		}

		int yTop = y - static_cast<int>(hours * yGridLenInvert);
		int xMid = static_cast<int>(x);

		//RectangleWare(hDC, static_cast<int>(x), y, static_cast<int>(x + dx), yTop);
		if (bBeginLine) {
			MoveToEx(hDC, xMid, yTop, NULL);
			bBeginLine = false;
		}
		else {
			LineTo(hDC, xMid, yTop);
		}
		Rectangle(hDC, xMid - 2, yTop - 2, xMid + 2, yTop + 2);

		TCHAR buf[20];
		double modInt, modFloat;
		modFloat = modf(hours, &modInt);
		_stprintf(buf, TEXT("%d:%02d"), static_cast<int>(modInt), static_cast<int>(modFloat * 60));
		TextOut(hDC, static_cast<int>(x), yTop - textHeight, buf, _tcslen(buf));

		iter++;
	}

	if (selectedDayX > 0) {
		CompTimeStoreConstIter findIter = timeStore.find(g_selectedDate);
		Date date = (*findIter).first;
		const stCompTime& compTime = (*findIter).second;

		float hours = compTime.ActiveToHours();
		int yTop = y - static_cast<int>(hours * yGridLenInvert);
		int xMid = static_cast<int>(selectedDayX);
		Rectangle(hDC, xMid - 5, yTop - 5, xMid + 5, yTop + 5);
	}

	//MoveToEx(hDC, margin + dx * dayCount, margin, NULL);
	//LineTo(hDC, margin + dx * dayCount, clientRect.bottom - margin);

	SelectObject(hDC, penGistogramOld);
	DeleteObject(penGistogram);
}

/*
 Date minDate, Date maxDate -- valid allways
*/
void DrawCompTimeStore(HWND hWnd, HDC hDC, const CompTimeStore& timeStore, Date minDate, Date maxDate) {
	RECT clientRect;
	TCHAR buf[500];

	GetClientRect(hWnd, &clientRect);
	SetBkMode(hDC, TRANSPARENT);

	// element's count / gistogramWidth;
	//int dx = 40;
	int gistohramHeight = clientRect.bottom - 2 * margin;
	int gistohramWidth = clientRect.right - 2 * margin;

	DrawAxes(hDC, clientRect);

	DrawHorizontalLines(hDC, clientRect, gistohramHeight);

	DrawVerticalLines(hDC, clientRect, gistohramWidth, gistohramHeight, minDate, maxDate);

	if (!timeStore.empty()) {
		//RangeInTimeStorage(timeStore, minDate, maxDate);
		DrawHostogram(hDC, clientRect, gistohramWidth, gistohramHeight, timeStore, minDate, maxDate);
	}

/*	if (selectedDayX) {
		const stCompTime& compTime = timeStore.at(g_selectedDate);
		HPEN hSelectedPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
		HPEN hSelectedPenOld= (HPEN)SelectObject(hDC, hSelectedPen);
		int heightTime = compTime.ActiveToHours();
		if (heightTime > 24) {
			heightTime = 24;
		}
		selectedDayX -= dx / 2.0;
		RectangleWare(hDC, selectedDayX, y, selectedDayX + dx, y - heightTime * gistohramHeight / 24);
		SelectObject(hDC, hSelectedPenOld);
		DeleteObject(hSelectedPen);
	}*/

/*	TCHAR* pStr;

	pStr = SystemTime::DateToStr(minDate);
	TextOut(hDC, margin, clientRect.bottom - textHeight - 5, pStr, _tcslen(pStr));
	pStr = SystemTime::DateToStr(maxDate);
	TextOut(hDC, clientRect.right - margin - 100, clientRect.bottom - textHeight - 5, pStr, _tcslen(pStr));*/
}

void GraphicWnd_OnPaint(HWND hWnd, HDC hDC) {
	RECT rc;

	GetClientRect(hWnd, &rc);

	MoveToEx(hDC, 0, 0, NULL);
	LineTo(hDC, rc.right - 1, 0);
	LineTo(hDC, rc.right - 1, rc.bottom - 1);
	LineTo(hDC, 0, rc.bottom - 1);
	LineTo(hDC, 0, 0);

	Date minDate, maxDate;

	DateFilterDlg::GetSelectedDateRange(minDate, maxDate);

	DrawCompTimeStore(hWnd, hDC, Document::GetCompTimeStore(), minDate, maxDate);
}

HBRUSH GraphicWnd_OnEraseBkgnd(HWND hWnd, HDC hDC) {
	static HBRUSH hbr = NULL;

	if (!hbr) {
		hbr = CreateSolidBrush(RGB(100, 200, 50));
	}

	return hbr;
}

void GraphicWnd_OnMouseMove(int flags, int x, int y) {

}

}
