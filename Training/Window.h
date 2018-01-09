#pragma once
#include <d3d12.h>

class Window
{

private:

    // Handle to the window
    HWND m_hwnd = NULL;
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
    Window(HINSTANCE a_hInstance, int a_nShowCmd, LPCTSTR WindowName = L"WindowName", LPCTSTR WindowTitle = L"WindowTitle", int a_width = 800, int a_height = 600, bool a_fullScreen = false);
    void Close();
    ~Window();

};

