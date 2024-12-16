#include "directx/descriptor_heap/ShaderGlobalDescriptorHeap.h"

#include <tchar.h>

namespace AquaEngine
{
    void ShaderGlobalDescriptorHeap::Init(D3D12_DESCRIPTOR_HEAP_TYPE type)
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc;

        desc.Type = type;
        desc.NodeMask = 0;
        desc.NumDescriptors = kMaxDescriptorHeapSize;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        if (type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
        {
            desc.NumDescriptors = kMaxDescriptorHeapSizeSampler;
        }

        HRESULT hr = m_heap.Create(&desc);
        if (FAILED(hr))
        {
            OutputDebugString(_T("Failed to create descriptor heap\n"));
            return;
        }

        m_incrementSize = m_heap.GetIncrementSize();
        m_lastCpuHandle = m_heap.GetCPUHandle();
        m_lastGpuHandle = m_heap.GetGPUHandle();
    }

    ShaderGlobalDescriptorHeap::ShaderGlobalDescriptorHeap()
        : m_lastGpuHandle()
    {
    }

    DescriptorHeapSegmentManager& ShaderGlobalDescriptorHeap::GetHeapManager(const std::string& name)
    {
        return m_heapManagers[name];
    }

    void ShaderGlobalDescriptorHeap::CreateManager(const std::string& name, unsigned int size)
    {
        DescriptorHeapSegmentManager manager(name, size, m_incrementSize, m_lastCpuHandle, m_lastGpuHandle);
        m_heapManagers[name] = manager;

        m_lastCpuHandle.ptr += static_cast<UINT64>(size) * m_incrementSize;
        m_lastGpuHandle.ptr += static_cast<UINT64>(size) * m_incrementSize;
    }

    void ShaderGlobalDescriptorHeap::SetToCommand(Command &command) const
    {
        m_heap.SetToCommand(&command);
    }

}
