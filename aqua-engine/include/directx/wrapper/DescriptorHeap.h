#ifndef AQUA_DESCRIPTORHEAP_H
#define AQUA_DESCRIPTORHEAP_H

#include <d3d12.h>
#include "directx/wrapper/Command.h"

namespace AquaEngine
{
    class DescriptorHeap
    {
    public:
        DescriptorHeap();
        ~DescriptorHeap();

        HRESULT Create(const D3D12_DESCRIPTOR_HEAP_DESC* desc);
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const;
        [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const;
        [[nodiscard]] UINT GetIncrementSize() const;
        void SetToCommand(Command* command) const;

        void Release();

    private:
        ID3D12DescriptorHeap* m_descriptorHeap;
        UINT m_incrementSize;
    };
}

#endif //AQUA_DESCRIPTORHEAP_H
