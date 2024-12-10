#pragma once

#include <dxgi1_6.h>

class Factory
{
public:
    static void Init()
    {
        CreateFactory();
    }

    static IDXGIFactory6* Get()
    {
        return m_dxgiFactory;
    }

    static void Shutdown()
    {
        if (m_dxgiFactory)
        {
            m_dxgiFactory->Release();
            m_dxgiFactory = nullptr;
        }
    }
private:
    static IDXGIFactory6* m_dxgiFactory;

    static HRESULT CreateFactory()
    {
        HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&m_dxgiFactory));
        if (FAILED(hr))
        {
            MessageBox(
                nullptr,
                L"Failed to create DXGI factory.",
                L"Error",
                MB_OK | MB_ICONERROR
            );
            return hr;
        }

        return S_OK;
    }
};
