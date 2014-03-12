#include "Document.h"
#include "global.h"
#include <stdio.h>
#include <time.h>

namespace Document {

static CompTimeStore g_compTime;

const CompTimeStore& GetCompTimeStore() {
	return g_compTime;
}

int LoadTime() {
	TCHAR fileName[MAX_PATH];
	FILE* f = NULL;
	stCompTimeStore timeStore;

	GetNearExePath(fileName, TEXT("comptime.dat"));

	_tfopen_s(&f, fileName, TEXT("rb"));
	if (!f) {
		_tfopen_s(&f, fileName, TEXT("w"));
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

	const int signatureSize  = sizeof(APP_SIGNATURE) - 1;
	const int dataOffestSize = sizeof(UINT16);
	char signature[signatureSize + 1]; // +1 for \0
	
	// read head info
	fread(signature, signatureSize, 1, f);
	signature[signatureSize] = 0;
	if (strcmp(signature, APP_SIGNATURE)) {
		goto l_err;
	}

	UINT16 dataTimeOffset;
	fread(&dataTimeOffset, dataOffestSize, 1, f);

	if ((fileSize - dataTimeOffset) % sizeof(stCompTimeStore) != 0) {
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

int SaveTime() {
	TCHAR fileName[MAX_PATH];
	FILE* f = NULL;

	GetNearExePath(fileName, TEXT("comptime.dat"));
	_tfopen_s(&f, fileName, TEXT("w"));
	if (!f) {
		LogErr("Can't create file: comptime.dat");
		return 0;
	}

	stCompTimeStore timeStoreCur;
		
	timeStoreCur.date = SystemTime::GetCurrentDate();
	timeStoreCur.compTime.activeTime  = GetTickCount() / 1000;
	timeStoreCur.compTime.passiveTime = g_passiveTime;
	timeStoreCur.compTime.sleepTime   = g_sleepTime;

#ifdef _DEBUG
	g_compTime[65] = stCompTime(7123, 0, 0);
	g_compTime[66] = stCompTime(11321, 0, 0);
	g_compTime[67] = stCompTime(14412, 0, 0);
#endif

	const int signatureSize  = sizeof(APP_SIGNATURE) - 1;
	const int dataOffsetSize = sizeof(UINT16);

	UINT16 dataTimeOffset = signatureSize + dataOffsetSize;

	fwrite(APP_SIGNATURE, 1, signatureSize, f);
	fwrite((char*)&dataTimeOffset, 1, dataOffsetSize, f);

	if (g_compTime.empty()) {
		fwrite(&timeStoreCur, sizeof(timeStoreCur), 1, f);
	}
	else {
		CompTimeStore::reverse_iterator iterEnd = g_compTime.rbegin();
		Date lastDate = (*iterEnd).first;
		stCompTime& lastTime = (*iterEnd).second;

		if (lastDate == timeStoreCur.date) {
			lastTime.activeTime  += timeStoreCur.compTime.activeTime;
			lastTime.passiveTime += timeStoreCur.compTime.passiveTime;
			lastTime.sleepTime   += timeStoreCur.compTime.sleepTime;
		}
		std::pair<Date, stCompTime> value = std::pair<Date, stCompTime>(timeStoreCur.date, timeStoreCur.compTime);
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

	return 0;
}

void FreeTime() {

}

}