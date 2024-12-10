#pragma once

#include <d3d12.h>
#include <wrl.h>

class Device
{
public:
    static void Init()
    {
        CreateDevice();
    }

    static ID3D12Device* Get()
    {
        return m_device.Get();
    }

    static void Shutdown()
    {
        if (m_device)
        {
            m_device->Release();
            m_device = nullptr;
        }
    }

private:
    static Microsoft::WRL::ComPtr<ID3D12Device> m_device;

    static HRESULT CreateDevice()
    {
        D3D_FEATURE_LEVEL levels[] =
        {
            D3D_FEATURE_LEVEL_12_2,
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_0,
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0
        };

        for (auto level : levels)
        {
            HRESULT hr = D3D12CreateDevice(nullptr, level, IID_PPV_ARGS(&m_device));
            if (SUCCEEDED(hr))
            {
                return S_OK;
            }
        }

        if (m_device == nullptr)
        {
            MessageBox(
                nullptr,
                L"Failed to create D3D12 device.",
                L"Error",
                MB_OK | MB_ICONERROR
            );
            return E_FAIL;
        }

        return S_OK;
    }
};
