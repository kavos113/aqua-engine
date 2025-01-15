#ifndef AQUA_BUFFERVIEW_H
#define AQUA_BUFFERVIEW_H

#include <d3d12.h>
#include <memory>
#include <utility>

#include "directx/descriptor_heap/DescriptorHeapSegment.h"

namespace AquaEngine
{
    class BufferView
    {
    public:
        BufferView() = default;
        ~BufferView() = default;

        void SetDescriptorHeapSegment(std::shared_ptr<DescriptorHeapSegment> descriptorHeapSegment, int offset)
        {
            m_descriptorHeapSegment = std::move(descriptorHeapSegment);
            this->m_offset = offset;
        }

        void SetGraphicsRootDescriptorTable(Command *command) const
        {
            m_descriptorHeapSegment->SetGraphicsRootDescriptorTable(command, m_offset);
        }

        void SetComputeRootDescriptorTable(Command *command) const
        {
            m_descriptorHeapSegment->SetComputeRootDescriptorTable(command, m_offset);
        }

        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const
        {
            return m_descriptorHeapSegment->GetCPUHandle(m_offset);
        }

        [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const
        {
            return m_descriptorHeapSegment->GetGPUHandle(m_offset);
        }

        [[nodiscard]] int CheckSegment() const
        {
            if (!m_descriptorHeapSegment->IsEnable())
            {
                OutputDebugString("descriptor heap segment is not set\n");
                return -1;
            }

            return 0;
        }

    private:
        std::shared_ptr<DescriptorHeapSegment> m_descriptorHeapSegment;
        int m_offset = 0;
    };
}

#endif //AQUA_BUFFERVIEW_H