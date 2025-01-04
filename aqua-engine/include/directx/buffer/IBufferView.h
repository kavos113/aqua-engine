#ifndef AQUA_BUFFERVIEW_H
#define AQUA_BUFFERVIEW_H

#include <d3d12.h>
#include <memory>
#include <utility>

#include "directx/descriptor_heap/DescriptorHeapSegment.h"
#include "Buffer.h"

namespace AquaEngine
{
    class IBufferView
    {
    public:
        virtual ~IBufferView() = default;

        virtual void Create(Buffer &buffer) = 0;

        void SetDescriptorHeapSegment(std::shared_ptr<DescriptorHeapSegment> descriptorHeapSegment, int offset)
        {
            m_DescriptorHeapSegment = std::move(descriptorHeapSegment);
            this->m_offset = offset;
        }

        void SetGraphicsRootDescriptorTable(Command *command) const
        {
            m_DescriptorHeapSegment->SetGraphicsRootDescriptorTable(command, m_offset);
        }

        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const
        {
            return m_DescriptorHeapSegment->GetCPUHandle(m_offset);
        }

        [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const
        {
            return m_DescriptorHeapSegment->GetGPUHandle(m_offset);
        }

    protected:
        std::shared_ptr<DescriptorHeapSegment> m_DescriptorHeapSegment;
        int m_offset = 0;

        int CheckSegment() const
        {
            if (!m_DescriptorHeapSegment->IsEnable())
            {
                OutputDebugString("descriptor heap segment is not set\n");
                return -1;
            }

            return 0;
        }
    };
}

#endif //AQUA_BUFFERVIEW_H