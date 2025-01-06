#include "../../../include/directx/buffer/UnorderedAccessView.h"

#include "../../../include/directx/wrapper/Device.h"

namespace AquaEngine
{
    void UnorderedAccessView::Create(Buffer &buffer, const D3D12_UNORDERED_ACCESS_VIEW_DESC &uavDesc) const
    {
        if (m_view.CheckSegment() != 0) return;

        Device::Get()->CreateUnorderedAccessView(
            buffer.GetBuffer().Get(),
            nullptr,
            &uavDesc,
            m_view.GetCPUHandle()
        );
    }
} // AquaEngine
