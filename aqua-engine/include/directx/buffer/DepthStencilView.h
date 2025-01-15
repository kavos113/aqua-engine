#ifndef AQUA_DEPTHSTENCILVIEW_H
#define AQUA_DEPTHSTENCILVIEW_H


#include "Buffer.h"
#include "BufferView.h"

namespace AquaEngine
{
    class DepthStencilView
    {
    public:
        void Create(Buffer &buffer) const;
        void Create(Buffer &buffer, const D3D12_DEPTH_STENCIL_VIEW_DESC &dsvDesc) const;

        void SetDescriptorHeapSegment(const std::shared_ptr<DescriptorHeapSegment> &descriptorHeapSegment, int offset)
        {
            m_view.SetDescriptorHeapSegment(descriptorHeapSegment, offset);
        }

        void SetGraphicsRootDescriptorTable(Command *command) const
        {
            m_view.SetGraphicsRootDescriptorTable(command);
        }

        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const
        {
            return m_view.GetCPUHandle();
        }

        [[nodiscard]] int CheckSegment() const
        {
            return m_view.CheckSegment();
        }

    private:
        BufferView m_view;
    };
}


#endif //AQUA_DEPTHSTENCILVIEW_H
