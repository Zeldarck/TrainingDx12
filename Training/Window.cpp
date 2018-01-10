#include "Window.h"
#include <Windowsx.h>
#include <windows.h>
#include "resource.h"

int Window::GetWidth()
{
    return m_width;
}

int Window::GetHeight()
{
    return m_height;
}

int Window::GetRotX()
{
    int tmp = m_rotX;
    m_rotX = 0;
    return tmp;
}

int Window::GetRotY()
{
    int tmp = m_rotY;
    m_rotY = 0;
    return tmp;
}

Window::Window(HINSTANCE a_hInstance, int a_nShowCmd, LPCTSTR WindowName /* = L"WindowName" */, LPCTSTR WindowTitle /* = L"WindowTitle" */, int a_width /* =800 */, int a_height /* = 600*/, bool a_fullScreen /*= false*/)
    : m_width(a_width), m_height(a_height), m_fullScreen(a_fullScreen)
{
    if (a_fullScreen)
    {
        HMONITOR hmon = MonitorFromWindow(m_hwnd,
            MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfo(hmon, &mi);

        a_width = mi.rcMonitor.right - mi.rcMonitor.left;
        a_height = mi.rcMonitor.bottom - mi.rcMonitor.top;
        m_width = a_width;
        m_height = a_height;
    }

    HICON ico = LoadIcon(a_hInstance, MAKEINTRESOURCEW(IDI_ICON1));

    WNDCLASSEX wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = a_hInstance;
    wc.hIcon = ico;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WindowName;
    wc.hIconSm = ico;

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"Error registering class",
            L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    m_hwnd = CreateWindowEx(NULL,
        WindowName,
        WindowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        a_width, a_height,
        NULL,
        NULL,
        a_hInstance,
        NULL);

    if (!m_hwnd)
    {
        MessageBox(NULL, L"Error creating window",
            L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    if (a_fullScreen)
    {
        SetWindowLong(m_hwnd, GWL_STYLE, 0);
    }

    ShowWindow(m_hwnd, a_nShowCmd);
    UpdateWindow(m_hwnd);

    SetWindowLongPtrW(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    m_isOpen = true;
}

bool Window::IsOpen() {
    return m_isOpen;
}

void Window::Close() {
    m_isOpen = false;
    DestroyWindow(m_hwnd);
}

LRESULT CALLBACK Window::WndProc(HWND a_hwnd, UINT a_msg, WPARAM a_wParam, LPARAM a_lParam) {
    Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(a_hwnd, GWLP_USERDATA));

    switch (a_msg)
    {
        case WM_KEYDOWN:
            if (a_wParam == VK_ESCAPE) {
                if (MessageBox(0, L"Are you sure you want to exit?",
                    L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
                {
                    if (window != nullptr)
                        window->Close();
                }
            }
            return 0;

        case WM_DESTROY: // x button on top right corner of window was pressed
            if (window != nullptr)
                window->Close();

            PostQuitMessage(0);
            return 0;
        case WM_MOUSEMOVE:
            int xPos = GET_X_LPARAM(a_lParam);
            int yPos = GET_Y_LPARAM(a_lParam);
            if (window->m_lastX != -1) {
                window->m_rotX = xPos - window->m_lastX;
            }
            if (window->m_lastY != -1) {
                window->m_rotY = yPos - window->m_lastY;
            }
            window->m_lastX = xPos;
            window->m_lastY = yPos;
            //RECT rect;
            //GetWindowRect(a_hwnd, &rect);
            //SetCursorPos((rect.left + rect.right)/2, (rect.bottom + rect.top) / 2);
            break;
    }

    return DefWindowProc(a_hwnd,
        a_msg,
        a_wParam,
        a_lParam);
}


void Window::Update()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            break;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}


HWND Window::GetHWND() {
    return m_hwnd;
}

bool Window::IsFullScreen()
{
    return m_fullScreen;
}


Window::~Window()
{
}
