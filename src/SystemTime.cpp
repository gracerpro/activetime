#include "SystemTime.h"
#include <stdint.h>
#include <time.h>
#include <tchar.h>

namespace SystemTime {

int64_t SystemTimeToSeconds(SYSTEMTIME& st);
SYSTEMTIME SecondsToSystemTime(int64_t seconds);

int DaysInMonth(int month, int year);
int DaysInYear(int year);


void SystemTimeAdd(SYSTEMTIME& st, int secondCount) {
	tm sttm = {0};

	sttm.tm_mday = st.wDay;
	sttm.tm_mon  = st.wMonth - 1;
	sttm.tm_year = st.wYear - 1900;

	time_t seconds = mktime(&sttm);
	seconds += secondCount;

	tm* timeinfo = localtime(&seconds);

	st.wDay   = timeinfo->tm_mday;
	st.wMonth = timeinfo->tm_mon + 1;
	st.wYear  = timeinfo->tm_year + 1900;
}

int DaysInMonth(int month, int year) {
	int arrDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if (month < 0 || month > 11) {
		return 0;
	}
	if (month == 1 && (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))) {
		return 29;
	}

	return arrDays[month];
}

int DaysInYear(int year) {
	if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)) {
		return 366;
	}

	return 365;
}

BOOL GetCurrentWeekRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd) {
	GetSystemTime(&stBegin);
	stBegin.wMilliseconds = 0;
	stBegin.wSecond = 0;
	stBegin.wMinute = 0;
	stBegin.wHour = 0;

	int daySinceMonday = stBegin.wDayOfWeek == 0 ? 6 : stBegin.wDayOfWeek - 1;
	SystemTimeAdd(stBegin, -daySinceMonday * 60 * 60 * 24);

	stEnd = stBegin;
	SystemTimeAdd(stEnd, 60 * 60 * 24 * 7 - 1);

	return TRUE;
}

BOOL GetCurrentMonthRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd) {
	GetSystemTime(&stBegin);
	stBegin.wMilliseconds = 0;
	stBegin.wSecond = 0;
	stBegin.wMinute = 0;
	stBegin.wHour = 0;
	stBegin.wDay = 1;

	stEnd = stBegin;
	SystemTimeAdd(stEnd, DaysInMonth(stEnd.wMonth, stEnd.wYear) * 60 * 60 * 24 - 1);

	return TRUE;
}

BOOL GetCurrentYearRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd) {
	GetSystemTime(&stBegin);
	stBegin.wMilliseconds = 0;
	stBegin.wSecond = 0;
	stBegin.wMinute = 0;
	stBegin.wHour = 0;
	stBegin.wDay = 1;
	stBegin.wMonth = 1;

	stEnd = stBegin;
	SystemTimeAdd(stEnd, DaysInYear(stEnd.wYear) * 60 * 60 * 24 - 1);

	return TRUE;
}

void GetLastWeekRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd) {
	GetSystemTime(&stEnd);
	stEnd.wMilliseconds = 0;
	stEnd.wSecond = 0;
	stEnd.wMinute = 0;
	stEnd.wHour = 0;

	stBegin = stEnd;
	SystemTimeAdd(stBegin, -6 * 60 * 60 * 24);
}

void GetLastMonthRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd) {
	GetSystemTime(&stEnd);
	stEnd.wMilliseconds = 0;
	stEnd.wSecond = 0;
	stEnd.wMinute = 0;
	stEnd.wHour = 0;

	stBegin = stEnd;
	if (stBegin.wMonth == 1) {
		stBegin.wMonth = 12;
		stBegin.wYear--;
	}
	else {
		stBegin.wMonth--;
	}
}

void GetLastYearRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd) {
	GetSystemTime(&stEnd);
	stEnd.wMilliseconds = 0;
	stEnd.wSecond = 0;
	stEnd.wMinute = 0;
	stEnd.wHour = 0;

	stBegin = stEnd;
	stBegin.wYear--;
}

void GetMonthRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd, int month, int year) {
	memset(&stBegin, 0, sizeof(SYSTEMTIME));
	stBegin.wMonth = month;
	stBegin.wYear = year;
	stBegin.wDay = 1;

	memset(&stEnd, 0, sizeof(SYSTEMTIME));
	stEnd.wYear = year;
	stEnd.wMonth = month;
	stEnd.wDay = DaysInMonth(month - 1, year);
}

void GetYearRange(SYSTEMTIME& stBegin, SYSTEMTIME& stEnd, int year) {
	stBegin.wYear = year;

	stEnd.wYear = year;
	stEnd.wMonth = 11;
	stEnd.wDay = 31;
}

Date GetCurrentDate() {
	time_t timeCur = time(NULL); // 1970/01/01
	tm tmSince = {0};
	tmSince.tm_mday = 1;
	tmSince.tm_year = YEAR_SINCE - 1900;
	time_t timeSince = mktime(&tmSince); // YEAR_SINCE/01/01

	if (timeSince > timeCur) {
		return 0;
	}
	time_t diff = timeCur - timeSince;

	return Date(diff / 60 / 60 / 24); // skip time
}

LPTSTR DateToStr(Date date) {
	static TCHAR buf[100];

	tm timeinfo = {0};
	timeinfo.tm_mday = 1;
	timeinfo.tm_year = YEAR_SINCE - 1900;

	time_t t = mktime(&timeinfo);
	t += date * 60 * 60 * 24;

	tm* pTm = localtime(&t);
	_tcsftime(buf, 100, TEXT("%d/%m/%Y"), pTm);

	return buf;
}

LPTSTR TimeToStr(UINT32 time) {
	static TCHAR buf[100];

	wsprintf(buf, TEXT("%02d:%02d:%02d"), time / 60 / 60, time / 60 % 60, time % 60);

	return buf;
}

UINT32 StrToTime(LPCTSTR time) {
	UINT32 res = 0;
	TCHAR buf[20];

	LPCTSTR pHourBeg = time;
	LPCTSTR pHourEnd = _tcschr(pHourBeg, ':');
	if (!pHourEnd) {
		return res;
	}
	int len = pHourEnd - pHourBeg;
	_tcsncpy(buf, pHourBeg, len);
	buf[len] = 0;
	res = _ttoi(buf) * 60 * 60; // hours

	LPCTSTR pMinBeg = pHourEnd + 1;
	LPCTSTR pMinEnd = _tcschr(pMinBeg, ':');
	if (!pMinEnd) {
		return res;
	}
	len = pMinEnd - pMinBeg;
	_tcsncpy(buf, pMinBeg, len);
	buf[len] = 0;
	res += _ttoi(buf) * 60; // minutes

	LPCTSTR pSecBeg = pMinEnd + 1;
	res += _ttoi(pSecBeg);

	return res;
}

Date SystemTimeToDate(SYSTEMTIME& st) {
	tm tmCur = {0};
	tmCur.tm_year = st.wYear - 1900;
	tmCur.tm_mon  = st.wMonth - 1;
	tmCur.tm_mday = st.wDay;
	time_t timeCur = mktime(&tmCur);

	tm tmSince = {0};
	tmSince.tm_mday = 1;
	tmSince.tm_year = YEAR_SINCE - 1900;
	time_t timeSince = mktime(&tmSince); // YEAR_SINCE/01/01

	if (timeSince > timeCur) {
		return 0;
	}
	time_t diff = timeCur - timeSince;

	return Date(diff / 60 / 60 / 24);
}

LPSYSTEMTIME DateToSystemTime(SYSTEMTIME& st, Date date) {
	memset(&st, 0, sizeof(SYSTEMTIME));

	st.wYear = YEAR_SINCE;
	st.wMonth = 1;
	st.wDay = 1;

	SystemTimeAdd(st, 60 * 60 * 24 * date);

	return &st;
}

}
