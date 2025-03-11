#include <stdio.h>
#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND hButton;

    switch (uMsg) {
	case WM_CREATE:
	    hButton = CreateWindow("BUTTON", "Button", WS_VISIBLE | WS_CHILD,
		    0, 0, 100, 50, hwnd, (HMENU)1,
		    ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	    break;

	case WM_SIZE:
	    if (hButton) {
		int winWidth = LOWORD(lParam);
		int winHeight = HIWORD(lParam);
		int btnWidth = 100;
		int btnHeight = 50;
		int btnX = (winWidth - btnWidth) / 2;
		int btnY = (winHeight - btnHeight) / 2;
		MoveWindow(hButton, btnX, btnY, btnWidth, btnHeight, TRUE);
	    }
	    break;

	case WM_ERASEBKGND:
	    {
		HDC hdc = (HDC)wParam;
		RECT rc;
		GetClientRect(hwnd, &rc);
		FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));
		return 1;
	    }

	case WM_PAINT:
	    {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);
	    }
	    break;

	case WM_DESTROY:
	    PostQuitMessage(0);
	    break;
	default:
	    return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "MyWindowClass";

    RegisterClass(&wc);

    HWND hwnd = CreateWindow("MyWindowClass", "My Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 400, 300, NULL, NULL, hInstance, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
    }

    return 0;
}
