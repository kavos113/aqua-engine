#include "directx/buffer/ConstantBufferView.h"
#include "directx/wrapper/Device.h"

namespace AquaEngine
{
    void ConstantBufferView::Create(Buffer *buffer)
    {
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = buffer->GetBuffer()->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = buffer->GetBuffer()->GetDesc().Width;

        Device::Get()->CreateConstantBufferView(&cbvDesc, m_DescriptorHeapSegment->GetCPUHandle(m_offset));
    }

}
