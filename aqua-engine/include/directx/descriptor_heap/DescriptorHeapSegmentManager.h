#ifndef AQUA_DESCRIPTORHEAPSEGMENTMANAGER_H
#define AQUA_DESCRIPTORHEAPSEGMENTMANAGER_H

#include <d3d12.h>
#include <string>
#include <vector>
#include <memory>

#include "HeapID.h"
#include "directx/wrapper/Command.h"

namespace AquaEngine
{
    class DescriptorHeapSegment;
    class DescriptorHeapSegmentManager
    {
    public:
        DescriptorHeapSegmentManager(
            std::string name,
            unsigned int size,
            UINT increment_size,
            D3D12_CPU_DESCRIPTOR_HANDLE first_cpu_handle,
            D3D12_GPU_DESCRIPTOR_HANDLE first_gpu_handle);

        DescriptorHeapSegmentManager();
        ~DescriptorHeapSegmentManager() = default;

        DescriptorHeapSegment Allocate(unsigned int size);

        void SetDescForCPU(
            unsigned int size,
            UINT increment_size,
            D3D12_CPU_DESCRIPTOR_HANDLE first_cpu_handle);

        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(GLOBAL_HEAP_ID id) const;
        [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(GLOBAL_HEAP_ID id) const;
        [[nodiscard]] unsigned int GetSize(GLOBAL_HEAP_ID id) const;
        [[nodiscard]] UINT GetIncrementSize() const;
        [[nodiscard]] std::vector<D3D12_ROOT_PARAMETER> GetRootParameters() const;

        void SetRootParameter(
            GLOBAL_HEAP_ID id,
            D3D12_ROOT_PARAMETER_TYPE type,
            D3D12_SHADER_VISIBILITY visibility,
            std::unique_ptr<D3D12_DESCRIPTOR_RANGE> descriptor_ranges,
            int num_descriptor_ranges
        );

        void SetFirstGPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle);
        void SetGraphicsRootDescriptorTable(const Command &command, GLOBAL_HEAP_ID id, unsigned int offset = 0) const;

    private:
        std::string m_name;
        unsigned int m_size;
        UINT m_incrementSize;

        GLOBAL_HEAP_ID m_lastId = 0;
        std::vector<unsigned int> m_sizes;
        std::vector<unsigned int> m_offsets;
        std::vector<D3D12_ROOT_PARAMETER> m_rootParameters;

        D3D12_CPU_DESCRIPTOR_HANDLE m_firstCpuHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE m_firstGpuHandle;  // ShaderGlobalDescriptorHeapにおけるhandle
    };
}


#endif //AQUA_DESCRIPTORHEAPSEGMENTMANAGER_H
