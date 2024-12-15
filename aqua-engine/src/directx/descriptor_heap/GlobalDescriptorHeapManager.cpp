#include <tchar.h>
#include <array>
#include "directx/descriptor_heap/GlobalDescriptorHeapManager.h"

namespace AquaEngine
{
    std::array<GlobalDescriptorHeap, 2> GlobalDescriptorHeapManager::m_heaps;
    std::array<ShaderGlobalDescriptorHeap, 2> GlobalDescriptorHeapManager::m_shaderHeaps;

    void GlobalDescriptorHeapManager::Init()
    {
        m_shaderHeaps[0].Init(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        m_shaderHeaps[1].Init(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

        m_heaps[0].Init(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        m_heaps[1].Init(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    }

    DescriptorHeapSegmentManager& GlobalDescriptorHeapManager::CreateShaderManager(
        const std::string& name,
        unsigned int size,
        D3D12_DESCRIPTOR_HEAP_TYPE type
    )
    {
        switch (type)
        {
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
            m_shaderHeaps[0].CreateManager(name, size);
            return m_shaderHeaps[0].GetHeapManager(name);

        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
            m_shaderHeaps[1].CreateManager(name, size);
            return m_shaderHeaps[1].GetHeapManager(name);

        case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
        case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
        case D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES:
            OutputDebugString(_T("Invalid descriptor heap type\n"));
        }

        return m_shaderHeaps[0].GetHeapManager(name); // unsafe
    }

    DescriptorHeapSegmentManager& GlobalDescriptorHeapManager::GetCPUHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE type)
    {
        switch (type)
        {
        case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
            return m_heaps[0].GetHeapManager();

        case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
            return m_heaps[1].GetHeapManager();

        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
        case D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES:
            OutputDebugString(_T("Invalid descriptor heap type\n"));
        }

        return m_heaps[0].GetHeapManager(); // unsafe
    }

    // set to command CBV, SRV, UAV heap
    void GlobalDescriptorHeapManager::SetToCommand(Command &command)
    {
        m_shaderHeaps[0].SetToCommand(command);
    }

    DescriptorHeapSegmentManager& GlobalDescriptorHeapManager::GetShaderHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE type, const std::string& name)
    {
        switch (type)
        {
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
            return m_shaderHeaps[0].GetHeapManager(name);

        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
            return m_shaderHeaps[1].GetHeapManager(name);

        case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
        case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
        case D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES:
            OutputDebugString(_T("Invalid descriptor heap type\n"));
        }

        return m_shaderHeaps[0].GetHeapManager(name); // unsafe
    }

    void GlobalDescriptorHeapManager::Shutdown()
    {
        m_shaderHeaps[0].Release();
        m_shaderHeaps[1].Release();

        m_heaps[0].Release();
        m_heaps[1].Release();
    }

}
