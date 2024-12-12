#ifndef AQUA_GLOBALDESCRIPTORHEAP_H
#define AQUA_GLOBALDESCRIPTORHEAP_H

#include <d3d12.h>

#include "DescriptorHeapSegmentManager.h"
#include "directx/wrapper/DescriptorHeap.h"


class GlobalDescriptorHeap
{
public:
    virtual void Init(D3D12_DESCRIPTOR_HEAP_TYPE type);
    GlobalDescriptorHeap();
    virtual ~GlobalDescriptorHeap() = default;
    
    DescriptorHeapSegmentManager& GetHeapManager() { return m_heapManager; }

protected:
    DescriptorHeap m_heap;
    UINT m_incrementSize;
    
    D3D12_CPU_DESCRIPTOR_HANDLE m_lastCpuHandle;

private:
    DescriptorHeapSegmentManager m_heapManager;
    
    constexpr static unsigned int kMaxDescriptorHeapSize = 65536;
};


#endif //AQUA_GLOBALDESCRIPTORHEAP_H
