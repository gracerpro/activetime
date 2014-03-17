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

	LPTSTR DateToStr(Date date);
	Date SystemTimeToDate(SYSTEMTIME& st);

	LPTSTR TimeToStr(UINT32 time);
	UINT32 StrToTime(LPCTSTR time);

	BOOL GetCurrentWeekRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd);
	BOOL GetCurrentMonthRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd);
	BOOL GetCurrentYearRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd);
	void GetLastWeekRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd);
	void GetLastMonthRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd);
	void GetLastYearRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd);

	void GetMonthRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd, int month = 0, int year = YEAR_SINCE);
	void GetYearRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd, int year = YEAR_SINCE);
}