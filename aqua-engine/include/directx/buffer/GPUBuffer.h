#ifndef AQUA_GPUBUFFER_H
#define AQUA_GPUBUFFER_H


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
                       const D3D12_CLEAR_VALUE *clearValue)
        {
            HRESULT hr = Device::Get()->CreateCommittedResource(
                &heapProperties,
                heapFlags,
                &resourceDesc,
                resourceState,
                clearValue,
                IID_PPV_ARGS(&m_buffer)
            );
            if (FAILED(hr))
            {
                return hr;
            }

            hr = m_buffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedBuffer));
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
                m_buffer->Unmap(0, nullptr);
                m_mappedBuffer = nullptr;
            }
        }

        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> GetBuffer()
        {
            return m_buffer;
        }

        [[nodiscard]] bool IsActive() const
        {
            return m_buffer != nullptr;
        }
    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_buffer;
        T* m_mappedBuffer;
    };
}


#endif //AQUA_GPUBUFFER_H
