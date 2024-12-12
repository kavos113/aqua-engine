#include "directx/descriptor_heap/GlobalDescriptorHeap.h"

#include <tchar.h>

void GlobalDescriptorHeap::Init(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc;
    
    desc.Type = type;
    desc.NodeMask = 0;
    desc.NumDescriptors = kMaxDescriptorHeapSize;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    
    HRESULT hr = m_heap.Create(&desc);
    if (FAILED(hr))
    {
        OutputDebugString(_T("Failed to create descriptor heap\n"));
        return;
    }
    
    m_incrementSize = m_heap.GetIncrementSize();
    m_lastCpuHandle = m_heap.GetCPUHandle();
    
    m_heapManager.SetDescForCPU(kMaxDescriptorHeapSize, m_incrementSize, m_lastCpuHandle);
}

GlobalDescriptorHeap::GlobalDescriptorHeap()
    :
    m_incrementSize(),
    m_lastCpuHandle()
{
}
