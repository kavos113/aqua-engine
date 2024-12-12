#include "directx/descriptor_heap/DescriptorHeapSegment.h"

DescriptorHeapSegment::DescriptorHeapSegment(
    const unsigned int numDescriptors,
    const int segmentId,
    const D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle,
    const D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle,
    const UINT incrementSize)
    : m_numDescriptors(numDescriptors)
    , m_cpuHandle(cpu_handle)
    , m_gpuHandle(gpu_handle)
    , m_incrementSize(incrementSize)
    , m_segmentId(segmentId)
{
}

DescriptorHeapSegment::DescriptorHeapSegment(DescriptorHeapSegment&& segment) noexcept
    : m_numDescriptors(segment.m_numDescriptors)
    , m_cpuHandle(segment.m_cpuHandle)
    , m_gpuHandle(segment.m_gpuHandle)
    , m_incrementSize(segment.m_incrementSize)
    , m_segmentId(segment.m_segmentId)
{
}

DescriptorHeapSegment& DescriptorHeapSegment::operator=(DescriptorHeapSegment&& segment) noexcept
{
    m_numDescriptors = segment.m_numDescriptors;
    m_segmentId = segment.m_segmentId;
    m_cpuHandle = segment.m_cpuHandle;
    m_gpuHandle = segment.m_gpuHandle;
    m_incrementSize = segment.m_incrementSize;
    
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
