#include "directx/buffer/DepthStencilView.h"
#include "directx/wrapper/Device.h"


namespace AquaEngine
{
    void DepthStencilView::Create(Buffer *buffer)
    {
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

        Device::Get()->CreateDepthStencilView(
            buffer->GetBuffer(),
            &dsvDesc,
            m_DescriptorHeapSegment->GetCPUHandle(m_offset)
        );
    }

}
