#include "directx/buffer/Buffer.h"

#include "directx/Util.h"
#include "directx/wrapper/Device.h"

Buffer::Buffer()
    : m_Buffer(nullptr)
{

}

Buffer::~Buffer()
{
    SafeRelease(&m_Buffer);
}

HRESULT Buffer::Create(D3D12_HEAP_PROPERTIES heapProperties,
                       D3D12_HEAP_FLAGS heapFlags,
                       D3D12_RESOURCE_DESC resourceDesc,
                       D3D12_RESOURCE_STATES resourceState,
                       D3D12_CLEAR_VALUE *clearValue)
{
    HRESULT hr = Device::Get()->CreateCommittedResource(
        &heapProperties,
        heapFlags,
        &resourceDesc,
        resourceState,
        clearValue,
        IID_PPV_ARGS(&m_Buffer)
    );
    if (FAILED(hr))
    {
        OutputDebugStringW(L"Failed to create buffer\n");
        return hr;
    }
 
    return S_OK;
}

ID3D12Resource *Buffer::GetBuffer() const
{
    return m_Buffer;
}
