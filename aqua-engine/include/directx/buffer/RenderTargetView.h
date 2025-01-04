#ifndef AQUA_RENDERTARGETVIEW_H
#define AQUA_RENDERTARGETVIEW_H


#include "BufferView.h"
#include "Buffer.h"

namespace AquaEngine
{
    class RenderTargetView
    {
    public:
        void Create(Buffer &buffer) const;
        void Create(ID3D12Resource* resource) const;
        void Create(Buffer &buffer, const D3D12_RENDER_TARGET_VIEW_DESC &rtvDesc) const;

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



#endif //AQUA_RENDERTARGETVIEW_H
