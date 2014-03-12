/*
 * SystemTime
 */
#pragma once

#include <Windows.h>

#define YEAR_SINCE 2014

// count of days since YEAR_SINCE/01/01
typedef UINT32 Date;

namespace SystemTime {
	Date GetCurrentDate();
	TCHAR* DateToStr(Date date);
	Date SystemTimeToDate(SYSTEMTIME& st);

	BOOL GetCurrentWeekRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd);
	BOOL GetCurrentMonthRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd);
	BOOL GetCurrentYearRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd);
}