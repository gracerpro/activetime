#include "Document.h"
#include "global.h"
#include <stdio.h>
#include <time.h>

namespace Document {

static CompTimeStore g_compTime;

CompTimeStore& GetCompTimeStore() {
	return g_compTime;
}

int LoadTime() {
	TCHAR fileName[MAX_PATH];
	FILE* f = NULL;
	stCompTimeStore timeStore;
	stFileHear header;

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

int SaveTime() {
	TCHAR fileName[MAX_PATH];
	FILE* f = NULL;

	GetNearExePath(fileName, TEXT("comptime.dat"));
	f = _tfopen(fileName, TEXT("w"));
	if (!f) {
		LogErr("Can't create file: comptime.dat");
		return 0;
	}

	stCompTimeStore timeCur;
	DWORD lastTickCount = LoadLastTickCount();
	DWORD tickCount = GetTickCount() / 1000; // in seconds

	timeCur.date = SystemTime::GetCurrentDate();
	timeCur.compTime.activeTime  = tickCount - lastTickCount;
	timeCur.compTime.passiveTime = g_passiveTime;
	timeCur.compTime.sleepTime   = g_sleepTime;

#ifdef _DEBUG
	g_compTime[65] = stCompTime(7123, 0, 0);
	g_compTime[66] = stCompTime(11321, 0, 0);
	g_compTime[67] = stCompTime(14412, 0, 0);
#endif

	stFileHear header;

	memcpy(header.signature, APP_SIGNATURE, sizeof(header.signature));
	header.dataOffset = sizeof(stFileHear);

	fwrite(&header, sizeof(header), 1, f);

	if (g_compTime.empty()) {
		fwrite(&timeCur, sizeof(timeCur), 1, f);
	}
	else {
		CompTimeStore::reverse_iterator iterEnd = g_compTime.rbegin();
		Date lastDate = (*iterEnd).first;
		stCompTime& lastTime = (*iterEnd).second;

		if (lastDate == timeCur.date) {
			lastTime.activeTime  += timeCur.compTime.activeTime;
			lastTime.passiveTime += timeCur.compTime.passiveTime;
			lastTime.sleepTime   += timeCur.compTime.sleepTime;
		}
		std::pair<Date, stCompTime> value = std::pair<Date, stCompTime>(timeCur.date, timeCur.compTime);
		std::pair<CompTimeStore::iterator, bool> insertRes = g_compTime.insert(value);

	/*	if (insertRes.second) {

		}*/

		CompTimeStore::const_iterator iter = g_compTime.begin();
		for ( ; iter != g_compTime.end(); ++iter) {
			stCompTimeStore timeStore;

			timeStore.date = (*iter).first;
			timeStore.compTime = (*iter).second;

			fwrite(&timeStore, sizeof(timeStore), 1, f);
		}
	}
	fclose(f);

	SaveLastTickCount(tickCount);

	return 0;
}

void FreeTime() {

}

}
