#pragma once

#include <Windows.h>
#include "Document.h"

namespace DateFilterDlg {

HWND CreateDateFilterDlg(HWND hWndParent);
void GetSelectedDateRange(Date& minDate, Date& maxDate);

}