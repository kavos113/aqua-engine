#include "directx/descriptor_heap/DescriptorHeapSegmentManager.h"

#include "directx/descriptor_heap/DescriptorHeapSegment.h"
#include "directx/wrapper/Command.h"

namespace AquaEngine
{
    DescriptorHeapSegmentManager::DescriptorHeapSegmentManager(
        std::string name,
        const unsigned int size,
        const UINT increment_size,
        const D3D12_CPU_DESCRIPTOR_HANDLE first_cpu_handle,
        const D3D12_GPU_DESCRIPTOR_HANDLE first_gpu_handle) :
                                                            m_name(std::move(name)),
                                                            m_size(size),
                                                            m_incrementSize(increment_size),
                                                            m_firstCpuHandle(first_cpu_handle),
                                                            m_firstGpuHandle(first_gpu_handle)
    {
    }

    DescriptorHeapSegmentManager::DescriptorHeapSegmentManager()
        :
        m_size(0),
        m_incrementSize(0),
        m_firstCpuHandle(),
        m_firstGpuHandle()
    {

    }

    void DescriptorHeapSegmentManager::SetDescForCPU(unsigned int size,
                                                     UINT increment_size,
                                                     D3D12_CPU_DESCRIPTOR_HANDLE first_cpu_handle)
    {
        m_size = size;
        m_incrementSize = increment_size;
        m_firstCpuHandle = first_cpu_handle;
    }

    DescriptorHeapSegment DescriptorHeapSegmentManager::Allocate(const unsigned int size)
    {
        m_sizes.push_back(size);

        if (m_lastId == 0)
        {
            m_offsets.push_back(0);
        }
        else
        {
            m_offsets.push_back(m_offsets[m_lastId - 1] + m_sizes[m_lastId - 1]);
        }

        GLOBAL_HEAP_ID id = m_lastId;
        m_lastId++;



        return {m_sizes[id],
            id,
            GetCPUHandle(id),
            GetGPUHandle(id),
            m_incrementSize,
            this};
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapSegmentManager::GetCPUHandle(const GLOBAL_HEAP_ID id) const
    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = m_firstCpuHandle;
        handle.ptr += static_cast<UINT64>(m_offsets[id]) * m_incrementSize;

        return handle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapSegmentManager::GetGPUHandle(const GLOBAL_HEAP_ID id) const
    {
        D3D12_GPU_DESCRIPTOR_HANDLE handle = m_firstGpuHandle;
        handle.ptr += static_cast<UINT64>(m_offsets[id]) * m_incrementSize;

        return handle;
    }

    unsigned int DescriptorHeapSegmentManager::GetSize(const GLOBAL_HEAP_ID id) const
    {
        return m_sizes[id];
    }

    UINT DescriptorHeapSegmentManager::GetIncrementSize() const
    {
        return m_incrementSize;
    }

    std::pair<D3D12_ROOT_PARAMETER*, size_t> DescriptorHeapSegmentManager::GetRootParameters() const
    {
        auto root_parameters = new D3D12_ROOT_PARAMETER[m_rootParameters.size()];
        for (size_t i = 0; i < m_rootParameters.size(); i++)
        {
            root_parameters[i] = m_rootParameters[i];
        }

        return std::make_pair(root_parameters, m_rootParameters.size());
    }

    void DescriptorHeapSegmentManager::SetRootParameter(
        const GLOBAL_HEAP_ID id,
        const D3D12_ROOT_PARAMETER_TYPE type,
        const D3D12_SHADER_VISIBILITY visibility,
        const D3D12_DESCRIPTOR_RANGE* descriptor_ranges,
        const int num_descriptor_ranges)
    {
        D3D12_ROOT_PARAMETER root_parameter;

        root_parameter.ParameterType = type;
        root_parameter.ShaderVisibility = visibility;
        root_parameter.DescriptorTable.NumDescriptorRanges = num_descriptor_ranges;
        root_parameter.DescriptorTable.pDescriptorRanges = descriptor_ranges;

        if (static_cast<GLOBAL_HEAP_ID>(m_rootParameters.size()) <= id)
        {
            m_rootParameters.resize(id + 1);
        }

        m_rootParameters[id] = root_parameter;
    }

    void DescriptorHeapSegmentManager::SetFirstGPUHandle(const D3D12_GPU_DESCRIPTOR_HANDLE handle)
    {
        m_firstGpuHandle = handle;
    }

    void DescriptorHeapSegmentManager::SetGraphicsRootDescriptorTable(
        Command *command,
        GLOBAL_HEAP_ID id,
        unsigned int offset) const
    {
        D3D12_GPU_DESCRIPTOR_HANDLE handle = GetGPUHandle(id);
        handle.ptr += static_cast<UINT64>(offset) * m_incrementSize;

        command->List()->SetGraphicsRootDescriptorTable(id, handle);
    }

}
