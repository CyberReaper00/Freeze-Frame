#include <windows.h>
#include <stdio.h>  // For file operations
#include <stdlib.h> // For atoi
#include <commctrl.h>

#define ID_TIMER_START 1
#define ID_TIMER_STOP 2
#define ID_TIMER_SAVE 3
#define ID_TIMER_DELETE 4
#define ID_LISTBOX_TIMERS 5

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int timer_running = 0;
int seconds_remaining = 0;
HWND countdown, input_field, timer_list;

const char* filename = "timers.txt";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "TimerApp";

    WNDCLASS wc = {0};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&icex);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Freeze Frame",
        WS_OVERLAPPEDWINDOW & ~WS_BORDER, // Remove border style
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void SaveTimers() {
    FILE *file = fopen(filename, "w");
    if (file != NULL) {
        int itemCount = SendMessage(timer_list, LB_GETCOUNT, 0, 0);
        for (int i = 0; i < itemCount; i++) {
            char buffer[50];
            SendMessage(timer_list, LB_GETTEXT, i, (LPARAM)buffer);
            fprintf(file, "%s\n", buffer);
        }
        fclose(file);
    }
}

void LoadTimers(HWND timer_list) {
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        char buffer[50];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            // Remove newline character
            buffer[strcspn(buffer, "\n")] = 0;
            SendMessage(timer_list, LB_ADDSTRING, 0, (LPARAM)buffer);
        }
        fclose(file);
    }
}

void DeleteSelectedTimer(HWND timer_list) {
    int itemIndex = SendMessage(timer_list, LB_GETCURSEL, 0, 0);
    if (itemIndex != LB_ERR) {
        SendMessage(timer_list, LB_DELETESTRING, itemIndex, 0);
        SaveTimers(); // Save updated timers to file
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CTLCOLORLISTBOX: {
            HDC hdcListBox = (HDC)wParam;
            SetBkColor(hdcListBox, RGB(0, 100, 255)); // Set background color to light gray
            return (INT_PTR)CreateSolidBrush(RGB(0, 100, 255)); // Return a handle to the brush
        }

        case WM_CREATE: {
            RECT rect;
            GetClientRect(hwnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;
            int leftSectionWidth = (int)(width * 0.3);
            int rightSectionWidth = (int)(width * 0.7);

            timer_list = CreateWindowEx(0, "LISTBOX", "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
                                       10, 10, leftSectionWidth - 20, height - 100, hwnd, (HMENU) ID_LISTBOX_TIMERS, 
                                       (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            input_field = CreateWindow("EDIT", "", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_NUMBER,
                                     leftSectionWidth + 10, 10, rightSectionWidth - 20, 20, hwnd, NULL, 
                                     (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            CreateWindow("BUTTON", "Start", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         leftSectionWidth + 10, 50, 100, 30, hwnd, (HMENU) ID_TIMER_START, 
                         (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            CreateWindow("BUTTON", "Stop", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         leftSectionWidth + 120, 50, 100, 30, hwnd, (HMENU) ID_TIMER_STOP, 
                         (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            // Create "Save Timer" button in the left section
            CreateWindow("BUTTON", "Save", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                        10, height - 80, leftSectionWidth - 20, 30, hwnd, (HMENU) ID_TIMER_SAVE, 
                        (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            // Create "Delete Timer" button in the left section
            CreateWindow("BUTTON", "Delete", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                        10, height - 40, leftSectionWidth - 20, 30, hwnd, (HMENU) ID_TIMER_DELETE, 
                        (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            countdown = CreateWindow("STATIC", "Timer: 0s", WS_VISIBLE | WS_CHILD,
                                     leftSectionWidth + 10, 140, 100, 20, hwnd, NULL, 
                                     (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);

            LoadTimers(timer_list);
            break;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_TIMER_START) {
                if (!timer_running) {
                    char buffer[10] = {0};
                    GetWindowText(input_field, buffer, 10);

                    // Convert the input text to an integer
                    seconds_remaining = atoi(buffer);

                    // Ensure a valid non-zero timer
                    if (seconds_remaining > 0) {
                        timer_running = 1;
                        SetTimer(hwnd, 1, 1000, NULL); // 1-second interval
                    } else {
                        MessageBox(hwnd, "Please enter a valid time!", "Error", MB_OK);
                    }
                }
            }
            if (LOWORD(wParam) == ID_TIMER_STOP) {
                if (timer_running) {
                    KillTimer(hwnd, 1);
                    timer_running = 0;
                }
            }
            if (LOWORD(wParam) == ID_TIMER_SAVE) {
                char buffer[10] = {0};
                GetWindowText(input_field, buffer, 10);
                int seconds = atoi(buffer);
                if (seconds > 0) {
                    SendMessage(timer_list, LB_ADDSTRING, -1, (LPARAM)buffer);
                    SaveTimers();
                } else {
                    MessageBox(hwnd, "Please enter a valid time to save!", "Error", MB_OK);
                }
            }
            if (LOWORD(wParam) == ID_TIMER_DELETE) {
                DeleteSelectedTimer(timer_list);
            }
            if (LOWORD(wParam) == ID_LISTBOX_TIMERS && HIWORD(wParam) == LBN_SELCHANGE) {
                int itemIndex = SendMessage(timer_list, LB_GETCURSEL, 0, 0);
                if (itemIndex != LB_ERR) {
                    char buffer[10] = {0};
                    SendMessage(timer_list, LB_GETTEXT, itemIndex, (LPARAM)buffer);
                    SetWindowText(input_field, buffer);
                }
            }
            break;
        }
        case WM_TIMER: {
            if (seconds_remaining > 0) {
                seconds_remaining--;
                char buffer[50];
                wsprintf(buffer, "Timer: %ds", seconds_remaining);
                SetWindowText(countdown, buffer);
            } else {
                KillTimer(hwnd, 1);
                timer_running = 0;
        PlaySound(TEXT("song2.wav"), NULL, SND_FILENAME | SND_ASYNC);
            }
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
