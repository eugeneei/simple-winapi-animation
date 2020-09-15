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
    static bool started = false;
    static bool paused = false;
    static bool mousePressed = false;

    static int clientWidth;
    static int clientHeight;

    static int step = 5;
    static int x, y;
    static int radius = 15;
    static double dX, dY;
    static double angle;

    HDC hdc;                 // device context (DC) for window      
    PAINTSTRUCT ps;          // paint data for BeginPaint and EndPaint 
    
    static HDC hdcBitmap;    // compatible HDC for bitmap
    static HBITMAP hBitmap;
    static BITMAP bitmap;

    static HPEN pen;
    static HBRUSH brush;


    switch (message)
    {
    case WM_SIZE:
    {
        clientWidth = LOWORD(lParam);
        clientHeight = HIWORD(lParam);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        KillTimer(hWnd, 1);
        started = true;
        paused = true;
        mousePressed = true;

        angle = rand() % 360;

        x = LOWORD(lParam);
        y = HIWORD(lParam);

        dX = 5 * cos(angle);
        dY = 5 * sin(angle);

        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (mousePressed)
        {
            x = LOWORD(lParam);
            y = HIWORD(lParam);
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    }
    case WM_LBUTTONUP:
    {
        mousePressed = false;
        break;
    }
    case WM_KEYDOWN:
    {
        if (wParam == VK_SPACE)
        {
            if (paused)
            {
                SetTimer(hWnd, 1, 25, NULL);
                paused = false;
            }
            else
            {
                KillTimer(hWnd, 1);
                paused = true;
            }
        }

        if (paused)
        {
            switch (wParam)
            {
            case VK_UP:
            {
                if (y > step)
                {
                    y -= step;
                    InvalidateRect(hWnd, NULL, true);
                }
                break;
            }
            case VK_DOWN:
            {
                if (y < clientHeight - radius - step)
                {
                    y += step;
                    InvalidateRect(hWnd, NULL, true);
                }
                break;
            }
            case VK_LEFT:
            {
                if (x > step)
                {
                    x -= step;
                    InvalidateRect(hWnd, NULL, true);
                }
                break;
            }
            case VK_RIGHT:
            {
                if (x < clientWidth - radius - step)
                {
                    x += step;
                    InvalidateRect(hWnd, NULL, true);
                }
                break;
            }
            break;
            }
        }
    case WM_MOUSEWHEEL:
    {
        int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (LOWORD(wParam) == MK_CONTROL)
        {
            for (; wheelDelta > WHEEL_DELTA; wheelDelta -= WHEEL_DELTA)
            {
                if (x < clientWidth - radius - step)
                {
                    x += step;
                    InvalidateRect(hWnd, NULL, true);
                }
            }
            for (; wheelDelta < 0; wheelDelta += WHEEL_DELTA)
            {
                if (x > step)
                {
                    x -= step;
                    InvalidateRect(hWnd, NULL, true);
                }
            }
        }
        else
        {
            for (; wheelDelta > WHEEL_DELTA; wheelDelta -= WHEEL_DELTA)
            {
                if (y < clientHeight - radius - step)
                {
                    y += step;
                    InvalidateRect(hWnd, NULL, true);
                }
            }
            for (; wheelDelta < 0; wheelDelta += WHEEL_DELTA)
            {
                if (y > step)
                {
                    y -= step;
                    InvalidateRect(hWnd, NULL, true);
                }
            }
        }
        break;
    }
    case WM_TIMER:
    {
        if (x > clientWidth - radius)
        {
            dX = -abs(dX);
        }

        if (y > clientHeight - radius)
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
    {
        if (started) {
            hdc = BeginPaint(hWnd, &ps);

            // drawing

            BitBlt(
                hdc,
                x,
                y,
                bitmap.bmWidth,
                bitmap.bmHeight,
                hdcBitmap,
                0,
                0,
                SRCCOPY
            );

            /*pen = CreatePen(PS_DOT, 1, RGB(128, 0, 0));
            SelectObject(hdc, pen);

            brush = CreateSolidBrush(RGB(255, 255, 0));
            SelectObject(hdc, brush);

            Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);*/
            
            // end drawing

            //DeleteObject(pen);
            EndPaint(hWnd, &ps);
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CREATE:
    {
        hBitmap = (HBITMAP)LoadImage(
            NULL,
            _T("Sprite.bmp"),
            IMAGE_BITMAP,
            0, 0, LR_LOADFROMFILE  | LR_CREATEDIBSECTION
            );

        if (hBitmap == NULL)
        {
            MessageBox(hWnd, _T("Файл не найден"), _T("Загрузка изображения"), MB_OK | MB_ICONHAND);
            DestroyWindow(hWnd);
            return 1;
        }
        
        GetObject(hBitmap, sizeof(bitmap), &bitmap);
        hdc = GetDC(hWnd);
        hdcBitmap = CreateCompatibleDC(hdc);
        SelectObject(hdcBitmap, hBitmap);
        ReleaseDC(hWnd, hdc);
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return (LRESULT)NULL;
    }
}