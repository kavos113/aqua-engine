#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "BaseWindow.h"
#include "Graphics.h"

class MainWindow : public BaseWindow
{
public:
    HRESULT Create(
        DWORD dwStyle,
        LPCSTR lpWindowName,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = CW_USEDEFAULT,
        int nHeight = CW_USEDEFAULT,
        DWORD dwExStyle = 0,
        HWND hWndParent = nullptr,
        HMENU hMenu = nullptr
    ) override
    {
        HRESULT hr = BaseWindow::Create(dwStyle, lpWindowName, x, y, nWidth, nHeight, dwExStyle, hWndParent, hMenu);
        if (FAILED(hr))
        {
            return hr;
        }

        m_graphics = std::make_unique<Graphics>(m_hwnd, wr);
        m_graphics->SetUp();

        return S_OK;
    }

private:
    [[nodiscard]] LPCSTR ClassName() const override
    {
        return "MainWindow";
    }

    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) override;

    std::unique_ptr<Graphics> m_graphics;
};


#endif //MAINWINDOW_H
