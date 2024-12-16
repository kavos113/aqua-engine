#ifndef AQUA_BARRIER_H
#define AQUA_BARRIER_H


#include <d3d12.h>
#include "Command.h"

namespace AquaEngine
{
    class Barrier
    {
    public:
        static void Transition(Command *command,
                               ID3D12Resource *resource,
                               D3D12_RESOURCE_STATES before,
                               D3D12_RESOURCE_STATES after);
    private:
        static D3D12_RESOURCE_BARRIER m_Barrier;
    };
}


#endif //AQUA_BARRIER_H
