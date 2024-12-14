#ifndef AQUA_FENCE_H
#define AQUA_FENCE_H


#include <d3d12.h>

namespace AquaEngine
{
    class Fence
    {
    public:
        Fence();
        ~Fence();

        HRESULT Wait();

        [[nodiscard]] UINT64 Value() const
        {
            return m_fenceValue;
        }

        [[nodiscard]] UINT64 IncrementValue()
        {
            return ++m_fenceValue;
        }

        [[nodiscard]] ID3D12Fence* Get() const
        {
            return m_fence;
        }
    private:
        ID3D12Fence* m_fence{};
        UINT64 m_fenceValue{};
        HANDLE m_fenceEvent{};

        HRESULT CreateFence();
    };
}


#endif //AQUA_FENCE_H
