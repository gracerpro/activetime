#pragma once

#include <Windows.h>
#include "Document.h"

namespace TimeEditorWnd {

int CreateTimeEditorWnd(HWND hWndParent, CompTimeStore& timeStore);

bool IsChanged();

}
