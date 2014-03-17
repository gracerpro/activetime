/*
 * MainWindow.h
 */
#pragma once

#define WM_TRAY_MESSAGE (WM_USER + 1)

#include <Windows.h>
#include "Document.h"

namespace MainWindow {

HWND CreateMainWindow(HINSTANCE hInst);

void MainWindow_OnDestroy(HWND hWnd);


void MainWindow_OnFileOpen();
void MainWindow_OnFileSaveAs();
void MainWindow_OnFileEditTime();
void MainWindow_OnAppExit();

void MainWindow_OnViewToolbar();
void MainWindow_OnViewStatusbar();
void MainWindow_OnViewGrid();

void MainWindow_OnHelpHelp();
void MainWindow_OnHelpAbout();

void MainWindow_OnShowHide();

HWND GetStatusBar();
HWND GetToolBar();
int FillTable(HWND hwndTvi, const CompTimeStore& timeStore, bool rebuild);

}