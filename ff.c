#include <stdio.h>
#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND btn1;
    static HBRUSH brush1, brush2;

    switch (uMsg) {
	case WM_CREATE:
	    btn1 = CreateWindow("BUTTON", "Button", WS_VISIBLE | WS_CHILD,
		    0, 0, 100, 50, hwnd, (HMENU)1,
		    ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	    
	    brush1 = CreateSolidBrush(RGB(255, 100, 100));
	    brush2 = CreateSolidBrush(RGB(100, 255, 100));
	    break;

	case WM_SIZE:
	    if (btn1) {
		int winWidth = LOWORD(lParam);
		int winHeight = HIWORD(lParam);

		int sectionWidth = winWidth * 0.3;
		int sectionHeight = winHeight;

		int btnWidth = 100,
		    btnHeight = 50;

		int btnX = (sectionWidth - btnWidth) * 0.5;
		int btnY = (sectionHeight - btnHeight) * 0.5;

		MoveWindow(btn1, btnX, btnY, btnWidth, btnHeight, TRUE);
		InvalidateRect(hwnd, NULL, TRUE);
	    }
	    break;

	case WM_ERASEBKGND:
	    {
		HDC hdc = (HDC)wParam;
		RECT rc;
		GetClientRect(hwnd, &rc);

		RECT section1 = {rc.left, rc.top , rc.right * 0.3, rc.bottom};
		RECT section2 = {rc.right * 0.3, rc.top , rc.right, rc.bottom};

		FillRect(hdc, &section1, brush1);
		FillRect(hdc, &section2, brush2);
		return 1;
	    }

	case WM_PAINT:
	    {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
	    }
	    break;

	case WM_DESTROY:
	    DeleteObject(brush1);
	    DeleteObject(brush2);
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

    HWND hwnd = CreateWindow("MyWindowClass", "My Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 600, 400, NULL, NULL, hInstance, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
    }

    return 0;
}
