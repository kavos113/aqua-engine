#include "directx/buffer/RenderTargetView.h"

#include "directx/wrapper/Device.h"

namespace AquaEngine
{
    void RenderTargetView::Create(Buffer &buffer)
    {
        if (CheckSegment() != 0) return;

        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        Device::Get()->CreateRenderTargetView(
            buffer.GetBuffer(),
            &rtvDesc,
            m_DescriptorHeapSegment->GetCPUHandle(m_offset)
        );
    }

    void RenderTargetView::Create(ID3D12Resource *resource) const
    {
        if (CheckSegment() != 0) return;

        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        Device::Get()->CreateRenderTargetView(
            resource,
            &rtvDesc,
            m_DescriptorHeapSegment->GetCPUHandle(m_offset)
        );
    }

    void RenderTargetView::Create(const Buffer &buffer, const D3D12_RENDER_TARGET_VIEW_DESC &rtvDesc) const
    {
        if (CheckSegment() != 0) return;

        Device::Get()->CreateRenderTargetView(
            buffer.GetBuffer(),
            &rtvDesc,
            m_DescriptorHeapSegment->GetCPUHandle(m_offset)
        );
    }
}
