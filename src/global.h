// global.h
#pragma once

#include <stdio.h>
#include <Windows.h>
#include "Application.h"

#define APP_SIGNATURE "AT"

void Log(const char* message);
void Log(const wchar_t* message);

void LogErr(const char* message);
void LogWarn(const char* message);
void LogLastError();

TCHAR* GetNearExePath(TCHAR* path, const TCHAR* name);

void ResizeWindow(HWND hWnd);
void SwitchWindowVisible(HWND hWnd);

extern FILE*           g_hLogFile;