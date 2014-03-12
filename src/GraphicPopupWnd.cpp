#include "GraphicPopupWnd.h"
#include "global.h"
#include "resource.h"

namespace GraphicPopupWnd {

LRESULT WINAPI GraphicPopupWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void GraphicPopupWnd_OnCommand(int id, int notifyCode, HWND hwndCtrl);


HWND CreateGraphicPopupWnd(HWND hParent) {
	HWND hWnd;

	hWnd = CreateDialog(Application::GetInstance(), MAKEINTRESOURCE(IDD_GRAPHIC_POPUP), hParent, (DLGPROC)GraphicPopupWndProc);

	return hWnd;
}

LRESULT WINAPI GraphicPopupWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hDC;

		hDC = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
		break;
	case WM_COMMAND:
		GraphicPopupWnd_OnCommand(LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
		break;
	case WM_CLOSE:
		SwitchWindowVisible(hWnd);
		break;
	case WM_INITDIALOG:
		
		return TRUE;
	}

	return 0;
}

void GraphicPopupWnd_OnCommand(int id, int notifyCode, HWND hwndCtrl) {
	switch (id) {
		
	}

}

}