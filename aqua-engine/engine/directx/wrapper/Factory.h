#pragma once

#include <dxgi1_6.h>
#include <wrl.h>

class Factory
{
public:
    static void Init(bool enableDebugLayer)
    {
        if (enableDebugLayer)
        {
            CreateFactoryWithDebug();
        }
        else
        {
            CreateFactory();
        }
    }

    static IDXGIFactory6* Get()
    {
        return m_dxgiFactory.Get();
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
    static Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgiFactory;

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

    static HRESULT CreateFactoryWithDebug()
    {
        HRESULT hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory));
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
