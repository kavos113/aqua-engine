#include "directx/wrapper/DescriptorHeap.h"

#include "directx/wrapper/Device.h"

namespace AquaEngine
{
    DescriptorHeap::DescriptorHeap()
        : m_descriptorHeap(nullptr)
        , m_incrementSize(0)
    {

    }

    HRESULT DescriptorHeap::Create(const D3D12_DESCRIPTOR_HEAP_DESC *desc)
    {
        HRESULT hr = Device::Get()->CreateDescriptorHeap(
            desc,
            IID_PPV_ARGS(&m_descriptorHeap)
        );
        if (FAILED(hr))
        {
            OutputDebugStringW(L"Failed to create descriptor heap\n");
            return hr;
        }

        m_incrementSize = Device::Get()->GetDescriptorHandleIncrementSize(desc->Type);

        return S_OK;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandle() const
    {
        return m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHandle() const
    {
        return m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    }

    UINT DescriptorHeap::GetIncrementSize() const
    {
        return m_incrementSize;
    }

    void DescriptorHeap::SetToCommand(Command *command) const
    {
        command->List()->SetDescriptorHeaps(1, m_descriptorHeap.GetAddressOf());
    }

}
