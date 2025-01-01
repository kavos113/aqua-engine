#ifndef AQUA_COMMAND_H
#define AQUA_COMMAND_H

#include <d3d12.h>
#include <wrl/client.h>

#include "Fence.h"

namespace AquaEngine
{
    class Command
    {
    public:
        Command();
        ~Command() = default;

        HRESULT Execute();

        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12CommandQueue> Queue()
        {
            return m_commandQueue;
        }

        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12CommandAllocator> Allocator()
        {
            return m_commandAllocator;
        }

        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> List()
        {
            return m_commandList;
        }
    private:
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue{};
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator{};
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList{};

        Fence m_fence;

        HRESULT CreateCommandQueue();
        HRESULT CreateCommandAllocator();
        HRESULT CreateCommandList();
    };
}


#endif //AQUA_COMMAND_H
