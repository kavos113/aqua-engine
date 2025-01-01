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

    case WM_MOUSEMOVE:
    {
        if (mouseX == 0.0f && mouseY == 0.0f)
        {
            mouseX = GET_X_LPARAM(lParam);
            mouseY = GET_Y_LPARAM(lParam);
            return 0;
        }

        float x = GET_X_LPARAM(lParam);
        float y = GET_Y_LPARAM(lParam);

        float dx = x - mouseX;
        float dy = y - mouseY;

        std::cout << "dx: " << dx << ", dy: " << dy << std::endl;

        mouseX = x;
        mouseY = y;

        m_graphics->MoveCamera(dx, dy);
    }

    case WM_TIMER:
        m_graphics->Timer(wParam);
        return 0;

    default:
        return DefWindowProc(m_hWnd, message, wParam, lParam);
    }
}
