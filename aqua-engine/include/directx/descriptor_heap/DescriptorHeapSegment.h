#ifndef AQUA_DESCRIPTORHEAPSEGMENT_H
#define AQUA_DESCRIPTORHEAPSEGMENT_H

#include <d3d12.h>

#include "HeapID.h"

class DescriptorHeapSegment
{
public:
    DescriptorHeapSegment() = default;
    
    DescriptorHeapSegment(
        unsigned int numDescriptors,
        int segmentId,
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle,
        D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle,
        UINT incrementSize);
    
    DescriptorHeapSegment(DescriptorHeapSegment &&segment) noexcept;
    DescriptorHeapSegment &operator=(DescriptorHeapSegment &&segment) noexcept;
    
    DescriptorHeapSegment(const DescriptorHeapSegment &segment) = delete;
    DescriptorHeapSegment &operator=(const DescriptorHeapSegment &segment) = delete;
    
    ~DescriptorHeapSegment() = default;
    
    [[nodiscard]] unsigned int GetNumDescriptors() const { return m_numDescriptors; }
    [[nodiscard]] int GetID() const { return m_segmentId; }
    
    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(unsigned int offset = 0) const;
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(unsigned int offset = 0) const;

private:
    unsigned int m_numDescriptors;
    
    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;
    UINT m_incrementSize;
    
    GLOBAL_HEAP_ID m_segmentId;
};


#endif //AQUA_DESCRIPTORHEAPSEGMENT_H
