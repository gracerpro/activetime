#pragma once

#include <Windows.h>
#include <tchar.h>
#include <map>
#include "SystemTime.h"

struct stCompTime {
	stCompTime() {
		activeTime  = 0;
		passiveTime = 0;
		sleepTime   = 0;
	}
	stCompTime(UINT32 active, UINT32 passive, UINT32 sleep) {
		activeTime  = active;
		passiveTime = passive;
		sleepTime   = sleep;
	}
	UINT32 activeTime;
	UINT32 passiveTime;
	UINT32 sleepTime;

public:
	float ActiveToHours() const { return activeTime / (60 * 60.0f); }
	float ActiveToDays() const { return activeTime / (60 * 60 * 24.0f); }
};

struct stCompTimeStore {
	Date        date;
	stCompTime  compTime;
};

typedef std::map<Date, stCompTime>                 CompTimeStore;
typedef std::map<Date, stCompTime>::iterator       CompTimeStoreIter;
typedef std::map<Date, stCompTime>::const_iterator CompTimeStoreConstIter;

namespace Document {

int LoadTime();
int SaveTime();
void FreeTime();

const CompTimeStore& GetCompTimeStore();

}