#ifndef UNICODE
#define UNICODE
#endif

#include "directx/wrapper/Fence.h"
#include "directx/wrapper/Device.h"

namespace AquaEngine
{
    Fence::Fence()
    {
        CreateFence();
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    }

    Fence::~Fence()
    {
        SafeRelease(&m_fence);
    }

    HRESULT Fence::Wait()
    {
        if (m_fence->GetCompletedValue() != m_fenceValue)
        {
            HRESULT hr = m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
            if (FAILED(hr))
            {
                OutputDebugString(L"Failed to set event on completion.\n");
                return hr;
            }

            WaitForSingleObject(m_fenceEvent, INFINITE);
        }

        return S_OK;
    }

    HRESULT Fence::CreateFence()
    {
        HRESULT hr = Device::Get()->CreateFence(
            0,
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&m_fence)
        );
        if (FAILED(hr))
        {
            MessageBox(
                nullptr,
                L"Failed to create fence.",
                L"Error",
                MB_OK
            );
            return hr;
        }

        return hr;
    }

}
