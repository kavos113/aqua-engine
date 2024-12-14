#ifndef AQUA_GPUBUFFER_H
#define AQUA_GPUBUFFER_H


#include "Buffer.h"

namespace AquaEngine
{
    template<typename T>
    class GPUBuffer : public Buffer
    {
    public:
        GPUBuffer()
            : m_mappedBuffer(nullptr)
        {
        }

        ~GPUBuffer() override
        {
            Unmap();
        }

        HRESULT Create(D3D12_HEAP_PROPERTIES heapProperties,
                       D3D12_HEAP_FLAGS heapFlags,
                       D3D12_RESOURCE_DESC resourceDesc,
                       D3D12_RESOURCE_STATES resourceState,
                       D3D12_CLEAR_VALUE *clearValue) override
        {
            HRESULT hr = Buffer::Create(heapProperties, heapFlags, resourceDesc, resourceState, clearValue);
            if (FAILED(hr))
            {
                return hr;
            }

            hr = m_Buffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedBuffer));
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
                m_Buffer->Unmap(0, nullptr);
                m_mappedBuffer = nullptr;
            }
        }
    private:
        T* m_mappedBuffer;
    };
}


#endif //AQUA_GPUBUFFER_H
