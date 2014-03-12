#pragma once

#include <Windows.h>
#include "Document.h"

namespace GraphicWnd {

HWND GetHwnd();

HWND CreateGraphicWnd(HWND hWndParent, int style = WS_CHILD | WS_VISIBLE);
void SetParentWnd(bool toMainWin);

void SetSelectedDate(Date date);
Date GetSelectedDate();

void DrawCompTimeStore(HWND hWnd, HDC hDC, const CompTimeStore& timeStore, Date minDate, Date maxDate);

}