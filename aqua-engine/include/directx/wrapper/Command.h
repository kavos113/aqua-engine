#ifndef AQUA_COMMAND_H
#define AQUA_COMMAND_H

#include <d3d12.h>
#include "Fence.h"

class Command
{
public:
    Command();
    ~Command();

    HRESULT Execute();

    [[nodiscard]] ID3D12CommandQueue* Queue() const
    {
        return m_commandQueue;
    }

    [[nodiscard]] ID3D12CommandAllocator* Allocator() const
    {
        return m_commandAllocator;
    }

    [[nodiscard]] ID3D12GraphicsCommandList* List() const
    {
        return m_commandList;
    }
private:
    ID3D12CommandQueue* m_commandQueue{};
    ID3D12CommandAllocator* m_commandAllocator{};
    ID3D12GraphicsCommandList* m_commandList{};
    
    Fence m_fence;
    
    HRESULT CreateCommandQueue();
    HRESULT CreateCommandAllocator();
    HRESULT CreateCommandList();
};


#endif //AQUA_COMMAND_H
