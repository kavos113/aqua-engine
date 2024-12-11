#ifndef AQUA_FACTORY_H
#define AQUA_FACTORY_H

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "directx/Util.h"

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
        return m_dxgiFactory;
    }

    static void Shutdown()
    {
        SafeRelease(&m_dxgiFactory);
    }

    static void EnableDebugLayer()
    {
        ID3D12Debug5* debugController;
        HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
        if (SUCCEEDED(hr))
        {
            debugController->EnableDebugLayer();
            debugController->SetEnableGPUBasedValidation(TRUE);
            debugController->SetEnableAutoName(TRUE);
        }
        OutputDebugStringW(L"Debug layer enabled.\n");
        debugController->Release();
    }

private:
    static IDXGIFactory6* m_dxgiFactory;

    static HRESULT CreateFactory()
    {
        HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&m_dxgiFactory));
        if (FAILED(hr))
        {
            MessageBoxW(
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
            MessageBoxW(
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

#endif //AQUA_FACTORY_H