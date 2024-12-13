#include "directx/BackBuffers.h"
#include "directx/descriptor_heap/GlobalDescriptorHeapManager.h"

#include <memory>

HRESULT BackBuffers::Create()
{
    HRESULT hr = CreateBackBuffers();
    if (FAILED(hr))
    {
        OutputDebugStringW(L"Failed to create back buffers\n");
        return hr;
    }
    
    CreateRenderTargetViews();
    
    return S_OK;
}

HRESULT BackBuffers::CreateBackBuffers()
{
    DXGI_SWAP_CHAIN_DESC1 desc;
    m_swapChain.GetDesc(&desc);
    
    m_backBuffers.resize(desc.BufferCount);
    
    for (int i = 0; i < desc.BufferCount; i++)
    {
        ID3D12Resource* buffer = m_backBuffers[i].GetBuffer();
        HRESULT hr = m_swapChain.Get()->GetBuffer(i, IID_PPV_ARGS(&buffer));
        if (FAILED(hr))
        {
            OutputDebugStringW(L"Failed to get back buffer\n");
            return hr;
        }
    }
    
    return S_OK;
}

void BackBuffers::CreateRenderTargetViews()
{
    DXGI_SWAP_CHAIN_DESC1 desc;
    m_swapChain.GetDesc(&desc);
    
    m_rtvs.resize(desc.BufferCount);
    
    DescriptorHeapSegmentManager& rtvManager = GlobalDescriptorHeapManager::GetCPUHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    
    std::shared_ptr<DescriptorHeapSegment> rtvSegmentPtr = std::make_shared<DescriptorHeapSegment>(rtvManager.Allocate(desc.BufferCount));
    
    for (int i = 0; i < desc.BufferCount; i++)
    {
        m_rtvs[i].SetDescriptorHeapSegment(rtvSegmentPtr, i);
        m_rtvs[i].Create(&m_backBuffers[i]);
    }
    
    return;
}
