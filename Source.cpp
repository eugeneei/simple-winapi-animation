#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <cmath>

static TCHAR szWindowClass[] = _T("DesktopApp");

static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");

HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    hInst = hInstance;

    HWND hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int clientWidth;
    static int clientHeight;


    static int x, y;
    static int width = 25;
    static double dX, dY;
    static double angle;
    static bool started = false;

    HDC hdc;                 // device context (DC) for window      
    PAINTSTRUCT ps;          // paint data for BeginPaint and EndPaint 
    HPEN pen;

    switch (message)
    {
    case WM_SIZE:
        clientWidth = LOWORD(lParam);
        clientHeight = HIWORD(lParam);
        break;
    case WM_LBUTTONDOWN:
    {
        angle = rand() % 360;

        x = LOWORD(lParam);
        y = HIWORD(lParam);

        SetTimer(hWnd, 1, 25, NULL);

        dX = 5 * cos(angle);
        dY = 5 * sin(angle);

        started = true;
    }
    break;
    case WM_TIMER:
    {      
        if (x > clientWidth - width)
        {
            dX = -abs(dX);
        }

        if (y > clientHeight - width)
        {
            dY = -abs(dY);
        }

        if (x < 0)
        {
            dX = abs(dX);
        }

        if (y < 0)
        {
            dY = abs(dY);
        }

        x += dX;
        y += dY;

        InvalidateRect(hWnd, NULL, true);
    }
    break;
    case WM_PAINT:
        if (started)
        {
            hdc = BeginPaint(hWnd, &ps);

            // drawing
            pen = CreatePen(PS_DOT, 1, RGB(128, 0, 0));
            SelectObject(hdc, pen);

            Rectangle(hdc, x, y, x + width, y + width);
            // end drawing

            DeleteObject(pen);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);        
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return (LRESULT)NULL;
}