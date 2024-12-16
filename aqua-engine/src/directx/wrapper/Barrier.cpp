#include "directx/wrapper/Barrier.h"

namespace AquaEngine
{
    D3D12_RESOURCE_BARRIER Barrier::m_Barrier = {
        .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
        .Transition = {
            .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
        }
    };

    void Barrier::Transition(Command *command,
                             ID3D12Resource *resource,
                             D3D12_RESOURCE_STATES before,
                             D3D12_RESOURCE_STATES after)
    {
        m_Barrier.Transition.pResource = resource;
        m_Barrier.Transition.StateBefore = before;
        m_Barrier.Transition.StateAfter = after;

        command->List()->ResourceBarrier(1, &m_Barrier);
    }

}
