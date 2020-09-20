#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <cmath>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

using namespace Gdiplus;

#define CL_TRANSPARENT RGB(255,0,0)

static TCHAR szWindowClass[] = _T("DesktopApp");

static TCHAR szTitle[] = _T("By Ralovets Application");

HINSTANCE hInst;
int spriteWidth;
int spriteHeight;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

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

    GdiplusShutdown(gdiplusToken);
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
    static double dX, dY;

    HDC hdc;                 // device context (DC) for window      
    PAINTSTRUCT ps;          // paint data for BeginPaint and EndPaint 

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

        int angle = rand() % 360;

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

            if (y < spriteHeight / 2 + step)
            {
                y = spriteHeight / 2 + step;
            }

            if (y > clientHeight - spriteHeight / 2 - step)
            {
                y = clientHeight - spriteHeight / 2;
            }

            if (x < spriteWidth / 2 - step)
            {
                x = spriteWidth / 2 - step;
            }

            if (x > clientWidth - spriteWidth / 2 + step)
            {
                x = clientWidth - spriteWidth / 2 + step;
            }

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
                SetTimer(hWnd, 1, 42, NULL);
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
                if (y > spriteHeight / 2 + step)
                {
                    y -= step;
                    InvalidateRect(hWnd, NULL, true);
                }
                break;
            }
            case VK_DOWN:
            {
                if (y < clientHeight - spriteHeight / 2 - step)
                {
                    y += step;
                    InvalidateRect(hWnd, NULL, true);
                }
                break;
            }
            case VK_LEFT:
            {
                if (x > spriteWidth / 2 - step)
                {
                    x -= step;
                    InvalidateRect(hWnd, NULL, true);
                }
                break;
            }
            case VK_RIGHT:
            {
                if (x < clientWidth - spriteWidth / 2 + step)
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
                if (x < clientWidth - spriteWidth / 2)
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
                if (y < clientHeight - spriteHeight / 2 - step)
                {
                    y += step;
                    InvalidateRect(hWnd, NULL, true);
                }
            }
            for (; wheelDelta < 0; wheelDelta += WHEEL_DELTA)
            {
                if (y > spriteHeight / 2 + step)
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
        if (x > clientWidth - spriteWidth / 2 + step)
        {
            dX = -abs(dX);
        }

        if (y > clientHeight - spriteHeight / 2 + step)
        {
            dY = -abs(dY);
        }
        
        if (x < spriteWidth / 2 - step)
        {
            dX = abs(dX);
        }

        if (y < spriteHeight / 2 + step)
        {
            dY = abs(dY);
        }

        x += (int) dX;
        y += (int) dY;

        InvalidateRect(hWnd, NULL, true);
    }
    break;
    case WM_PAINT:
    {
        if (started) {
            hdc = BeginPaint(hWnd, &ps);
            Graphics graphics(hdc);
            
            
            Image image(L"Bird.jpg");
            graphics.DrawImage(&image, x - spriteWidth / 2, y - spriteHeight / 2);
            

            //Image image(L"Bird.jpg");
            //Graphics g(hdc);
            //Rect paintRect(0, 0, spriteWidth, spriteHeight);
            //// Создаем временный буфер
            //Bitmap backBuffer(clientWidth, clientHeight, &g);
            //Graphics temp(&backBuffer);
            //// Рисуем в буфер
            //temp.DrawImage(&image, paintRect);
            //// Переносим на экран
            //g.DrawImage(&backBuffer, x - spriteWidth / 2, x - spriteHeight / 2, 0, 0,
            //    clientWidth, clientHeight, UnitPixel);
        }

            EndPaint(hWnd, &ps);
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CREATE:
    {      
        Image temp(L"Bird.jpg");
        spriteHeight = temp.GetHeight();
        spriteWidth = temp.GetWidth();
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return (LRESULT)NULL;
}