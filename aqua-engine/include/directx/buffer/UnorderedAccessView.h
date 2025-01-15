#ifndef UNORDEREDACCESSVIEW_H
#define UNORDEREDACCESSVIEW_H
#include "Buffer.h"
#include "BufferView.h"

namespace AquaEngine
{
    class UnorderedAccessView
    {
    public:
        void Create(Buffer &buffer, const D3D12_UNORDERED_ACCESS_VIEW_DESC &uavDesc) const;

        void SetDescriptorHeapSegment(const std::shared_ptr<DescriptorHeapSegment> &descriptorHeapSegment, int offset)
        {
            m_view.SetDescriptorHeapSegment(descriptorHeapSegment, offset);
        }

        void SetComputeRootDescriptorTable(Command *command) const
        {
            m_view.SetComputeRootDescriptorTable(command);
        }

        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const
        {
            return m_view.GetCPUHandle();
        }

        [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const
        {
            return m_view.GetGPUHandle();
        }

        [[nodiscard]] int CheckSegment() const
        {
            return m_view.CheckSegment();
        }

    private:
        BufferView m_view;
    };
} // AquaEngine

#endif //UNORDEREDACCESSVIEW_H
