#ifndef AQUA_DESCRIPTORHEAPSEGMENT_H
#define AQUA_DESCRIPTORHEAPSEGMENT_H

#include <d3d12.h>
#include <memory>

#include "HeapID.h"
#include "DescriptorHeapSegmentManager.h"

class DescriptorHeapSegment
{
public:
    DescriptorHeapSegment(unsigned int numDescriptors,
                          GLOBAL_HEAP_ID segmentId,
                          D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle,
                          D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle,
                          UINT incrementSize,
                          DescriptorHeapSegmentManager* manager);
    
    DescriptorHeapSegment(DescriptorHeapSegment &&segment) noexcept;
    DescriptorHeapSegment &operator=(DescriptorHeapSegment &&segment) noexcept;
    
    DescriptorHeapSegment(const DescriptorHeapSegment &segment) = delete;
    DescriptorHeapSegment &operator=(const DescriptorHeapSegment &segment) = delete;
    
    ~DescriptorHeapSegment() = default;
    
    [[nodiscard]] unsigned int GetNumDescriptors() const { return m_numDescriptors; }
    [[nodiscard]] int GetID() const { return m_segmentId; }
    
    [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(unsigned int offset = 0) const;
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(unsigned int offset = 0) const;
    
    void SetGraphicsRootDescriptorTable(Command *command, unsigned int offset = 0) const;
    void SetRootParameter(D3D12_ROOT_PARAMETER_TYPE type,
                          D3D12_SHADER_VISIBILITY visibility,
                          const D3D12_DESCRIPTOR_RANGE* descriptorRanges,
                          int numDescriptorRanges);

private:
    unsigned int m_numDescriptors;
    
    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;
    UINT m_incrementSize;
    
    GLOBAL_HEAP_ID m_segmentId;
    
    DescriptorHeapSegmentManager* m_manager;
};


#endif //AQUA_DESCRIPTORHEAPSEGMENT_H
