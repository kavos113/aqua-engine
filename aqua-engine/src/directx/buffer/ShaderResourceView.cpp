#include "directx/buffer/ShaderResourceView.h"
#include "directx/wrapper/Device.h"

void ShaderResourceView::Create(Buffer *buffer)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    
    Device::Get()->CreateShaderResourceView(
        buffer->GetBuffer(),
        &srvDesc,
        m_DescriptorHeapSegment->GetCPUHandle(m_offset)
    );
}
