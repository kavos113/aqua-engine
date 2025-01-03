#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <d2d1.h>
#include <dwrite.h>

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

        return S_OK;
    }

    void SetUp()
    {
        m_graphics = std::make_unique<Graphics>(m_hwnd, wr);
        m_graphics->SetUp();

        m_isInitialized = true;
    }

private:
    [[nodiscard]] LPCSTR ClassName() const override
    {
        return "MainWindow";
    }

    LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) override;

    std::unique_ptr<Graphics> m_graphics;
    bool m_isInitialized = false;

    float m_progress = 0.0f;

    HRESULT CreateDeviceResources();

    HRESULT CreateD2D1Resources();

    HRESULT CreateDWriteResources();

    HRESULT OnPaint();

    void PaintText() const;

    void OnResize();

    Microsoft::WRL::ComPtr<ID2D1Factory> m_d2dFactory = nullptr;
    Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> m_d2dRenderTarget = nullptr;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_d2dGreenBrush = nullptr;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_d2dBlackBrush = nullptr;
    Microsoft::WRL::ComPtr<ID2D1RectangleGeometry> m_progressBarGeometry = nullptr;
    Microsoft::WRL::ComPtr<ID2D1RectangleGeometry> m_finishedProgressBarGeometry = nullptr;
    Microsoft::WRL::ComPtr<IDWriteFactory> m_dwriteFactory = nullptr;
    Microsoft::WRL::ComPtr<IDWriteTextFormat> m_dwriteTextFormat = nullptr;

    RECT m_progressBarBorder = {0, 0, 0, 0};
    RECT m_textBorder = {0, 0, 0, 0};

    const wchar_t *m_loadingText = nullptr;
    UINT32 m_loadingTextLength = 0;
};


#endif //MAINWINDOW_H
