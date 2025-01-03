#ifndef BASEWINDOW_H
#define BASEWINDOW_H
#include <Windows.h>

#include "AquaEngine.h"


class BaseWindow
{
public:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        BaseWindow *pThis = nullptr;

        if (message == WM_CREATE)
        {
            auto *pCreateStruct = reinterpret_cast<CREATESTRUCT *>(lParam);
            pThis = static_cast<BaseWindow *>(pCreateStruct->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));

            pThis->m_hwnd = hWnd;
        }
        else
        {
            pThis = reinterpret_cast<BaseWindow *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        if (pThis)
        {
            return pThis->HandleMessage(message, wParam, lParam);
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    BaseWindow()
        : m_hwnd(nullptr)
        , wr()
    {
    }

    virtual ~BaseWindow()
    {
        if (m_hwnd)
        {
            DestroyWindow(m_hwnd);
        }
        m_hwnd = nullptr;
    }

    virtual HRESULT Create(
        DWORD dwStyle,
        LPCSTR lpWindowName,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = CW_USEDEFAULT,
        int nHeight = CW_USEDEFAULT,
        DWORD dwExStyle = 0,
        HWND hWndParent = nullptr,
        HMENU hMenu = nullptr
    )
    {

        WNDCLASSEX wc = {};

        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = ClassName();

        auto result = RegisterClassEx(&wc);
        if (result == 0)
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        m_hwnd = CreateWindowEx(
            dwExStyle,
            ClassName(),
            lpWindowName,
            dwStyle,
            x,
            y,
            nWidth,
            nHeight,
            hWndParent,
            hMenu,
            GetModuleHandle(nullptr),
            this
        );

        if (m_hwnd == nullptr)
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        wr = {0, 0, nWidth, nHeight};

        return S_OK;
    }

    [[nodiscard]] HWND GetWindow() const { return m_hwnd; }

protected:

    [[nodiscard]] virtual LPCSTR ClassName() const = 0;
    virtual LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam) = 0;

    HWND m_hwnd;
    RECT wr;

};



#endif //BASEWINDOW_H
