#include "MainWindow.h"

#include <iostream>
#include <windowsx.h>

LRESULT MainWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_d2dFactory.GetAddressOf());
        if (FAILED(hr))
        {
            OutputDebugString("failed to create D2D factory\n");
            return -1;
        }

        hr = CreateDWriteResources();
        if (FAILED(hr))
        {
            OutputDebugString("failed to create DWrite resources\n");
            return -1;
        }

        long width = wr.right - wr.left;
        long height = wr.bottom - wr.top;
        m_progressBarBorder.left = width / 6;
        m_progressBarBorder.right = width - m_progressBarBorder.left;
        m_progressBarBorder.top = height / 2;
        m_progressBarBorder.bottom = m_progressBarBorder.top + 20;

        m_textBorder.left = m_progressBarBorder.left;
        m_textBorder.right = m_progressBarBorder.right;
        m_textBorder.top = m_progressBarBorder.top - 50;
        m_textBorder.bottom = m_progressBarBorder.top;

        return 0;
    }

    case WM_SHOWWINDOW:
        OnPaint();
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(m_hwnd, &ps);

        if (!m_isInitialized)
        {
            OnPaint();
        } else
        {
            m_graphics->Render();
        }

        EndPaint(m_hwnd, &ps);
        return 0;
    }

    case WM_SIZE:
        OnResize();
        return 0;

    case WM_TIMER:
        m_graphics->Timer(wParam);
        return 0;

    case WM_AQUA_LOADING:
    {
        auto *p = reinterpret_cast<Graphics::Progress *>(lParam);
        m_progress = p->progress;
        SendMessage(m_hwnd, WM_PAINT, 0, 0);
        return 0;
    }

    default:
        return DefWindowProc(m_hwnd, message, wParam, lParam);
    }
}

HRESULT MainWindow::CreateDeviceResources()
{
    if (m_d2dRenderTarget != nullptr)
    {
        long right = (m_progressBarBorder.right - m_progressBarBorder.left) * m_progress + m_progressBarBorder.left;
        HRESULT hr = m_d2dFactory->CreateRectangleGeometry(
            D2D1::RectF(m_progressBarBorder.left, m_progressBarBorder.top, right, m_progressBarBorder.bottom),
            m_finishedProgressBarGeometry.GetAddressOf()
        );
        if (FAILED(hr))
        {
            OutputDebugString("failed to create D2D rectangle geometry\n");
            return hr;
        }

        return S_OK;
    }

    HRESULT hr = CreateD2D1Resources();
    if (FAILED(hr))
    {
        OutputDebugString("failed to create D2D1 resources\n");
        return hr;
    }

    return S_OK;
}

HRESULT MainWindow::CreateD2D1Resources()
{
    RECT rc;
    GetClientRect(m_hwnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

    HRESULT hr = m_d2dFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(m_hwnd, size),
        m_d2dRenderTarget.GetAddressOf()
    );
    if (FAILED(hr))
    {
        OutputDebugString("failed to create D2D render target\n");
        return hr;
    }

    D2D1_COLOR_F green = D2D1::ColorF(0.0f, 1.0f, 0.0f);
    hr = m_d2dRenderTarget->CreateSolidColorBrush(green, m_d2dGreenBrush.GetAddressOf());
    if (FAILED(hr))
    {
        OutputDebugString("failed to create D2D solid color brush\n");
        return hr;
    }

    D2D1_COLOR_F black = D2D1::ColorF(0.0f, 0.0f, 0.0f);
    hr = m_d2dRenderTarget->CreateSolidColorBrush(black, m_d2dBlackBrush.GetAddressOf());
    if (FAILED(hr))
    {
        OutputDebugString("failed to create D2D solid color brush\n");
        return hr;
    }

    hr = m_d2dFactory->CreateRectangleGeometry(
        D2D1::RectF(
            m_progressBarBorder.left - 1,
            m_progressBarBorder.top - 1,
            m_progressBarBorder.right + 1,
            m_progressBarBorder.bottom + 1
        ),
        m_progressBarGeometry.GetAddressOf()
    );
    if (FAILED(hr))
    {
        OutputDebugString("failed to create D2D rectangle geometry\n");
        return hr;
    }

    long right = (m_progressBarBorder.right - m_progressBarBorder.left) * m_progress + m_progressBarBorder.left;
    hr = m_d2dFactory->CreateRectangleGeometry(
        D2D1::RectF(m_progressBarBorder.left, m_progressBarBorder.top, right, m_progressBarBorder.bottom),
        m_finishedProgressBarGeometry.GetAddressOf()
    );
    if (FAILED(hr))
    {
        OutputDebugString("failed to create D2D rectangle geometry\n");
        return hr;
    }

    return S_OK;
}

HRESULT MainWindow::CreateDWriteResources()
{
    HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &m_dwriteFactory);
    if (FAILED(hr))
    {
        OutputDebugString("failed to create DWrite factory\n");
        return hr;
    }

    m_loadingText = L"Now Loading...";
    m_loadingTextLength = static_cast<UINT32>(wcslen(m_loadingText));

    hr = m_dwriteFactory->CreateTextFormat(
        L"Arial",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        24.0f,
        L"en-us",
        &m_dwriteTextFormat
    );
    if (FAILED(hr))
    {
        OutputDebugString("failed to create DWrite text format\n");
        return hr;
    }

    hr = m_dwriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    if (FAILED(hr))
    {
        OutputDebugString("failed to set text alignment\n");
        return hr;
    }

    hr = m_dwriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    if (FAILED(hr))
    {
        OutputDebugString("failed to set paragraph alignment\n");
        return hr;
    }

    return S_OK;
}

HRESULT MainWindow::OnPaint()
{
    HRESULT hr = CreateDeviceResources();
    if (FAILED(hr))
    {
        OutputDebugString("failed to create device resources\n");
        return hr;
    }

    m_d2dRenderTarget->BeginDraw();
    m_d2dRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
    m_d2dRenderTarget->DrawGeometry(m_progressBarGeometry.Get(), m_d2dBlackBrush.Get(), 2.0f);
    m_d2dRenderTarget->FillGeometry(m_finishedProgressBarGeometry.Get(), m_d2dGreenBrush.Get());
    PaintText();
    hr = m_d2dRenderTarget->EndDraw();
    if (FAILED(hr))
    {
        OutputDebugString("failed to end draw\n");
        return hr;
    }

    return S_OK;
}

void MainWindow::PaintText() const
{
    D2D1_RECT_F textRect = D2D1::RectF(
        static_cast<FLOAT>(m_textBorder.left),
        static_cast<FLOAT>(m_textBorder.top),
        static_cast<FLOAT>(m_textBorder.right),
        static_cast<FLOAT>(m_textBorder.bottom)
    );

    m_d2dRenderTarget->DrawText(
        m_loadingText,
        m_loadingTextLength,
        m_dwriteTextFormat.Get(),
        textRect,
        m_d2dBlackBrush.Get()
    );
}

void MainWindow::OnResize()
{
    if (m_d2dRenderTarget != nullptr)
    {
        GetClientRect(m_hwnd, &wr);
        m_d2dRenderTarget->Resize(D2D1::SizeU(wr.right - wr.left, wr.bottom - wr.top));
        SendMessage(m_hwnd, WM_PAINT, 0, 0);
    }
}
