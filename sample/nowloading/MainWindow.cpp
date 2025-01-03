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

        long width = wr.right - wr.left;
        long height = wr.bottom - wr.top;
        m_progressBarBorder.left = width / 6;
        m_progressBarBorder.right = width - m_progressBarBorder.left;
        m_progressBarBorder.top = height / 2;
        m_progressBarBorder.bottom = m_progressBarBorder.top + 20;

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
        }
        else
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
        std::cout << "loading: " << m_progress << std::endl;
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
        std::cout << "right: " << right << std::endl;
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
        D2D1::RectF(m_progressBarBorder.left - 1, m_progressBarBorder.top - 1, m_progressBarBorder.right + 1, m_progressBarBorder.bottom + 1),
        m_progressBarGeometry.GetAddressOf()
        );
    if (FAILED(hr))
    {
        OutputDebugString("failed to create D2D rectangle geometry\n");
        return hr;
    }

    long right = (m_progressBarBorder.right - m_progressBarBorder.left) * m_progress + m_progressBarBorder.left;
    std::cout << "right: " << right << std::endl;
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
    hr = m_d2dRenderTarget->EndDraw();
    if (FAILED(hr))
    {
        OutputDebugString("failed to end draw\n");
        return hr;
    }

    return S_OK;
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
