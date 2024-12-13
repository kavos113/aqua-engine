#include "directx/BackBuffers.h"
#include "directx/descriptor_heap/GlobalDescriptorHeapManager.h"
#include "directx/wrapper/Barrier.h"

#include <memory>

HRESULT BackBuffers::CreateBackBuffers()
{
    DXGI_SWAP_CHAIN_DESC1 desc;
    m_swapChain.GetDesc(&desc);
    
    m_backBuffers.resize(desc.BufferCount);
    
    for (int i = 0; i < desc.BufferCount; ++i)
    {
        HRESULT hr = m_swapChain.Get()->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i]));
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
        m_rtvs[i].Create(m_backBuffers[i]);
    }
    
    return;
}

D3D12_CPU_DESCRIPTOR_HANDLE BackBuffers::GetCurrentRTVHandle()
{
    UINT index = GetCurrentBackBufferIndex();
    return m_rtvs[index].GetCPUHandle();
}

void BackBuffers::BeginRender()
{
    UINT index = GetCurrentBackBufferIndex();
    Barrier::Transition(
        m_command,
        m_backBuffers[index],
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void BackBuffers::EndRender()
{
    UINT index = GetCurrentBackBufferIndex();
    Barrier::Transition(
        m_command,
        m_backBuffers[index],
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);
}
