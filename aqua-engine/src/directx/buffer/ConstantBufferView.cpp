#include "directx/buffer/ConstantBufferView.h"
#include "directx/wrapper/Device.h"

namespace AquaEngine
{
    void ConstantBufferView::Create(Buffer &buffer)
    {
        if (CheckSegment() != 0) return;

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = buffer.GetBuffer()->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = static_cast<UINT>(buffer.GetBuffer()->GetDesc().Width);

        if (cbvDesc.SizeInBytes % D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT != 0)
        {
            OutputDebugString("Constant buffer size must be a multiple of 256 bytes : D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT.\n");
            return;
        }

        Device::Get()->CreateConstantBufferView(&cbvDesc, m_DescriptorHeapSegment->GetCPUHandle(m_offset));
    }

}
