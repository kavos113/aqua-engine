#include "directx/descriptor_heap/DescriptorHeapSegment.h"

namespace AquaEngine
{
    DescriptorHeapSegment::DescriptorHeapSegment(unsigned int numDescriptors,
                                                 int segmentId,
                                                 D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle,
                                                 D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle,
                                                 UINT incrementSize,
                                                 DescriptorHeapSegmentManager* manager)
        : m_numDescriptors(numDescriptors)
        , m_cpuHandle(cpu_handle)
        , m_gpuHandle(gpu_handle)
        , m_incrementSize(incrementSize)
        , m_segmentId(segmentId)
        , m_manager(manager)
    {
    }

    DescriptorHeapSegment::DescriptorHeapSegment(DescriptorHeapSegment&& segment) noexcept
        : m_numDescriptors(segment.m_numDescriptors)
        , m_cpuHandle(segment.m_cpuHandle)
        , m_gpuHandle(segment.m_gpuHandle)
        , m_incrementSize(segment.m_incrementSize)
        , m_segmentId(segment.m_segmentId)
        , m_manager(segment.m_manager)
    {
    }

    DescriptorHeapSegment& DescriptorHeapSegment::operator=(DescriptorHeapSegment&& segment) noexcept
    {
        m_numDescriptors = segment.m_numDescriptors;
        m_segmentId = segment.m_segmentId;
        m_cpuHandle = segment.m_cpuHandle;
        m_gpuHandle = segment.m_gpuHandle;
        m_incrementSize = segment.m_incrementSize;
        m_manager = segment.m_manager;

        return *this;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapSegment::GetCPUHandle(const unsigned int offset) const
    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cpuHandle;
        handle.ptr += static_cast<UINT64>(offset) * m_incrementSize;

        return handle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapSegment::GetGPUHandle(unsigned int offset) const
    {
        D3D12_GPU_DESCRIPTOR_HANDLE handle = m_gpuHandle;
        handle.ptr += static_cast<UINT64>(offset) * m_incrementSize;

        return handle;
    }

    void DescriptorHeapSegment::SetGraphicsRootDescriptorTable(Command *command, unsigned int offset) const
    {
        m_manager->SetGraphicsRootDescriptorTable(*command, m_segmentId, offset);
    }

    void DescriptorHeapSegment::SetRootParameter(
        D3D12_ROOT_PARAMETER_TYPE type,
        D3D12_SHADER_VISIBILITY visibility,
        std::unique_ptr<D3D12_DESCRIPTOR_RANGE> descriptorRanges,
        int numDescriptorRanges
    ) const
    {
        m_manager->SetRootParameter(m_segmentId, type, visibility, std::move(descriptorRanges), numDescriptorRanges);
    }
}
