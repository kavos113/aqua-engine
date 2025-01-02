#include "MainWindow.h"

#include <iostream>
#include <ostream>
#include <windowsx.h>

LRESULT MainWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hWnd, &ps);

        m_graphics->Render();

        EndPaint(m_hWnd, &ps);
        return 0;
    }

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_LEFT:
            m_graphics->Move(-0.1f);
            break;

        case VK_RIGHT:
            m_graphics->Move(0.1f);
            break;
        }
        InvalidateRect(m_hWnd, &wr, false);
        return 0;

    case WM_TIMER:
        m_graphics->Timer(wParam);
        return 0;

    default:
        return DefWindowProc(m_hWnd, message, wParam, lParam);
    }
}
