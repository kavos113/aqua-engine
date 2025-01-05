#include "directx/buffer/DepthStencilView.h"
#include "directx/wrapper/Device.h"


namespace AquaEngine
{
    void DepthStencilView::Create(Buffer &buffer) const
    {
        if (m_view.CheckSegment() != 0) return;

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

        Device::Get()->CreateDepthStencilView(
            buffer.GetBuffer().Get(),
            &dsvDesc,
            m_view.GetCPUHandle()
        );
    }

    void DepthStencilView::Create(Buffer &buffer, const D3D12_DEPTH_STENCIL_VIEW_DESC &dsvDesc) const
    {
        if (m_view.CheckSegment() != 0) return;

        Device::Get()->CreateDepthStencilView(
            buffer.GetBuffer().Get(),
            &dsvDesc,
            m_view.GetCPUHandle()
        );
    }
}
