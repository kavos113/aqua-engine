#ifndef AQUA_GPUBUFFER_H
#define AQUA_GPUBUFFER_H

#include <d3d12.h>
#include <wrl.h>

#include "Buffer.h"

namespace AquaEngine
{
    template<typename T>
    class GPUBuffer
    {
    public:
        GPUBuffer()
            : m_mappedBuffer(nullptr)
        {
        }

        ~GPUBuffer()
        {
            Unmap();
        }

        HRESULT Create(const D3D12_HEAP_PROPERTIES &heapProperties,
                       const D3D12_HEAP_FLAGS heapFlags,
                       const D3D12_RESOURCE_DESC &resourceDesc,
                       const D3D12_RESOURCE_STATES resourceState,
                       D3D12_CLEAR_VALUE *clearValue
        )
        {
            HRESULT hr = m_buffer.Create(heapProperties, heapFlags, resourceDesc, resourceState, clearValue);
            if (FAILED(hr))
            {
                OutputDebugStringW(L"Failed to create buffer\n");
                return hr;
            }

            hr = m_buffer.GetBuffer()->Map(0, nullptr, reinterpret_cast<void **>(&m_mappedBuffer));
            if (FAILED(hr))
            {
                OutputDebugStringW(L"Failed to map buffer\n");
                return hr;
            }

            return S_OK;
        }

        T* GetMappedBuffer() const
        {
            return m_mappedBuffer;
        }

        void Unmap()
        {
            if (m_mappedBuffer)
            {
                m_buffer.GetBuffer()->Unmap(0, nullptr);
                m_mappedBuffer = nullptr;
            }
        }

        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> GetResource()
        {
            return m_buffer.GetBuffer();
        }

        Buffer &GetBuffer()
        {
            return m_buffer;
        }

        [[nodiscard]] bool IsActive() const
        {
            return m_buffer.IsActive();
        }

    private:
        Buffer m_buffer{};
        T* m_mappedBuffer;
    };
}


#endif //AQUA_GPUBUFFER_H
