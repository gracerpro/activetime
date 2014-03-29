#include "Document.h"
#include "global.h"
#include <stdio.h>
#include <time.h>

namespace Document {

static CompTimeStore g_compTime;

bool GetDivTime(stCompTime& curCompTime, stCompTime& beforeTime, stCompTime& afterTime);


CompTimeStore& GetCompTimeStore() {
	return g_compTime;
}

int LoadTime() {
	TCHAR fileName[MAX_PATH];
	FILE* f = NULL;
	stCompTimeStore timeStore;
	stFileHead header;

	GetNearExePath(fileName, TEXT("comptime.dat"));

	f = _tfopen(fileName, TEXT("rb"));
	if (!f) {
		f = _tfopen(fileName, TEXT("w"));
		if (!f) {
			LogErr("Can't create file: comptime.dat");
			return 0;
		}
		fclose(f);

		return 0;
	}
	fseek(f, 0, SEEK_END);
	long fileSize = ftell(f);
	rewind(f);

	if (!fileSize) {
		goto l_ok;
	}

	if (1 != fread(&header, sizeof(header), 1, f)) {
		goto l_err;
	}
	if (memcmp(header.signature, APP_SIGNATURE, sizeof(header.signature))) {
		goto l_err;
	}
	if ((fileSize - header.dataOffset) % sizeof(stCompTimeStore) != 0) {
		goto l_err;
	}
	// read the comp time
	while (fread(&timeStore, sizeof(timeStore), 1, f)) {
		g_compTime[timeStore.date] = timeStore.compTime;
	}

	goto l_ok;
l_err:
	LogErr("File corrupted");
l_ok:
	fclose(f);

	return g_compTime.size();
}

/*
 Save time in seconds, if user exit from application by menu item
*/
static bool SaveLastTickCount(DWORD seconds) {
	TCHAR fileName[MAX_PATH];

	GetNearExePath(fileName, TEXT("exittime.txt"));

	FILE* stream = _tfopen(fileName, TEXT("w"));
	if (!stream) {
		LogErr("Cann't write exit time to file");
		return false;
	}
	char buf[30];
	buf[0] = 0;
	_itoa(seconds, buf, 10);
	fputs(buf, stream);
	fclose(stream);

	return true;
}

/*
 load time in seconds, when user will exit from application by menu item
*/
static DWORD LoadLastTickCount() {
	TCHAR fileName[MAX_PATH];

	GetNearExePath(fileName, TEXT("exittime.txt"));
	FILE* stream = _tfopen(fileName, TEXT("r"));
	if (!stream) {
		LogErr("Cann't read exit time from file");
		return 0;
	}
	char buf[30];
	buf[0] = 0;
	fgets(buf, 30, stream);
	fclose(stream);

	return atoi(buf);
}

void CleanLastTickCount() {
	TCHAR fileName[MAX_PATH];

	GetNearExePath(fileName, TEXT("exittime.txt"));

	FILE* stream = _tfopen(fileName, TEXT("w"));
	if (stream) {
		fclose(stream);
	}
}

static bool WriteTimeToFile(FILE* stream) {
	stCompTimeStore timeCur;
	DWORD lastTickCount = LoadLastTickCount();
	DWORD tickCount = GetTickCount() / 1000; // in seconds

	timeCur.date = SystemTime::GetCurrentDate();
	timeCur.compTime.activeTime  = tickCount - lastTickCount;
	timeCur.compTime.passiveTime = g_passiveTime;
	timeCur.compTime.sleepTime   = g_sleepTime;

#ifdef _DEBUG
	g_compTime[58] = stCompTime(1, 0, 0);
	g_compTime[59] = stCompTime(12123, 0, 0);
	g_compTime[60] = stCompTime(17123, 0, 0);
	g_compTime[61] = stCompTime(3123, 0, 0);
	g_compTime[62] = stCompTime(12123, 0, 0);
	g_compTime[63] = stCompTime(13123, 0, 0);
	g_compTime[64] = stCompTime(8123, 0, 0);
	g_compTime[65] = stCompTime(7123, 0, 0);
	g_compTime[66] = stCompTime(11321, 0, 0);
	g_compTime[67] = stCompTime(14412, 0, 0);
	g_compTime[68] = stCompTime(12412, 0, 0);
	g_compTime[69] = stCompTime(19412, 0, 0);
	g_compTime[70] = stCompTime(20412, 0, 0);
	g_compTime[71] = stCompTime(4412, 0, 0);
#endif

	stFileHead header;

	memcpy(header.signature, APP_SIGNATURE, sizeof(header.signature));
	header.dataOffset = sizeof(stFileHead);

	rewind(stream);
	fwrite(&header, sizeof(header), 1, stream);

	if (g_compTime.empty()) {
		fwrite(&timeCur, sizeof(timeCur), 1, stream);
	}
	else {
		CompTimeStore::reverse_iterator iterEnd = g_compTime.rbegin();
		stCompTime ctBeforeDiv, ctAfterDiv;

		if (GetDivTime(timeCur.compTime, ctBeforeDiv, ctAfterDiv)) {
			AddTimeToStore(timeCur.date - 1, ctBeforeDiv);
			AddTimeToStore(timeCur.date, ctAfterDiv);
		}
		else {
			AddTimeToStore(timeCur.date, timeCur.compTime);
		}

		CompTimeStore::const_iterator iter = g_compTime.begin();
		for ( ; iter != g_compTime.end(); ++iter) {
			stCompTimeStore timeStore;

			timeStore.date = (*iter).first;
			timeStore.compTime = (*iter).second;

			fwrite(&timeStore, sizeof(timeStore), 1, stream);
		}
	}

	return true;
}

int SaveTime() {
	TCHAR fileName[MAX_PATH];
	FILE* f = NULL;

	GetNearExePath(fileName, TEXT("comptime.dat"));
	f = _tfopen(fileName, TEXT("wb+"));
	if (!f) {
		LogErr("Can't create file: comptime.dat");
		return 0;
	}

	char* tmpFileData = NULL;
	fseek(f, 0, SEEK_END);
	long fileSize = ftell(f);
	try {
		tmpFileData = new char[fileSize];
		fread(tmpFileData, 1, fileSize, f);
	}
	catch (...) {
		LogErr("allocate memory fail");
	}

	bool bWriteSuccess = true;
	try {
		bWriteSuccess = WriteTimeToFile(f);
	}
	catch (...) {
		bWriteSuccess = false;
	}

	if (!bWriteSuccess) {
		LogErr("Save time fail");
		rewind(f);
		fwrite(tmpFileData, 1,fileSize, f);
	}

	fclose(f);

	DWORD tickCount = GetTickCount() / 1000; // in seconds
	SaveLastTickCount(tickCount);

	return 0;
}

/*
 * Return true if current time will be divided
 */
bool GetDivTime(stCompTime& curCompTime, stCompTime& beforeTime, stCompTime& afterTime) {
	time_t curTime = time(NULL);
	time_t startTime = curTime - curCompTime.activeTime; // TODO: total time
	tm  tmDiv = {0};
	tm  tmCur = {0};
	tm  tmStart = {0};
	tm* ptmCur = localtime(&curTime);
	if (ptmCur) {
		memcpy(&tmCur, ptmCur, sizeof(tm));
	}
	tm* ptmStart = localtime(&startTime);
	if (ptmStart) {
		memcpy(&tmStart, ptmStart, sizeof(tm));
	}

	tmDiv.tm_year = tmCur.tm_year;
	tmDiv.tm_mon  = tmCur.tm_mon;
	tmDiv.tm_mday = tmCur.tm_wday;

	time_t timeDiv = mktime(&tmDiv);

	if (tmCur.tm_mday != tmStart.tm_mday) {
		//int secondsAfterDivTime = static_cast<int>(curTime - timeDiv);
		int secondsBeforeDivTime = static_cast<int>(timeDiv - startTime);

		beforeTime.activeTime  = secondsBeforeDivTime;
		beforeTime.passiveTime = curCompTime.passiveTime; // TODO
		beforeTime.sleepTime   = curCompTime.sleepTime;   // TODO

		return true;
	}

	return false;
}

bool AddTimeToStore(Date date, stCompTime& compTime) {
	CompTimeStoreIter findIter = g_compTime.find(date);

	if (findIter != g_compTime.end()) {
		stCompTime& ct = (*findIter).second;

		compTime.activeTime  += ct.activeTime;
		compTime.passiveTime += ct.passiveTime;
		compTime.sleepTime   += ct.sleepTime;
	}
	g_compTime[date] = compTime;

	return true;
}

void FreeTime() {

}

}
