#ifndef AQUA_DEVICE_H
#define AQUA_DEVICE_H

#ifndef UNICODE
#define UNICODE
#endif

#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include <vector>
#include <wrl.h>

#include "Factory.h"
#include "directx/Util.h"

// single adapter only now
class Device
{
public:
    static void Init(int adaptor_index = -1)
    {
        CreateDevice(adaptor_index);
    }

    static ID3D12Device* Get()
    {
        return m_device;
    }

    static void Shutdown()
    {
        SafeRelease(&m_device);
    }

    static std::vector<std::wstring> GetAdaptors()
    {
        std::vector<std::wstring> adaptors;
        IDXGIAdapter* adaptor;

        for (
            int i = 0;
            Factory::Get()->EnumAdapters(i, &adaptor) != DXGI_ERROR_NOT_FOUND;
            ++i
        )
        {
            m_adaptors.push_back(adaptor);

            DXGI_ADAPTER_DESC desc;
            adaptor->GetDesc(&desc);

            adaptors.emplace_back(desc.Description);
        }

        SafeRelease(&adaptor);

        return adaptors;
    }

private:
    static ID3D12Device* m_device;
    static std::vector<IDXGIAdapter*> m_adaptors;

    static HRESULT CreateDevice(int adaptor_index)
    {
        bool use_adaptor = adaptor_index >= 0 && adaptor_index < static_cast<int>(m_adaptors.size());

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
            HRESULT hr;

            if (use_adaptor)
            {
                hr = D3D12CreateDevice(m_adaptors[adaptor_index], level, IID_PPV_ARGS(&m_device));
            }
            else
            {
                hr = D3D12CreateDevice(nullptr, level, IID_PPV_ARGS(&m_device));
            }

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

#endif //AQUA_DEVICE_H