#ifndef AQUA_SHADERRESOURCEVIEW_H
#define AQUA_SHADERRESOURCEVIEW_H


#include "BufferView.h"

namespace AquaEngine
{
    class ShaderResourceView
    {
    public:
        void Create(Buffer &buffer) const;
        void Create(Buffer &buffer, const D3D12_SHADER_RESOURCE_VIEW_DESC &srvDesc) const;

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
}

#endif //AQUA_SHADERRESOURCEVIEW_H
