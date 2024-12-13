#include <gtest/gtest.h>

#include "directx/descriptor_heap/GlobalDescriptorHeapManager.h"
#include "directx/wrapper/Device.h"
#include "directx/wrapper/Factory.h"
#include "directx/descriptor_heap/DescriptorHeapSegment.h"

class GlobalDescriptorHeapManagerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Factory::Init(true);
        Device::GetAdaptors();
        Device::Init(0);
        GlobalDescriptorHeapManager::Init();
    }

    void TearDown() override
    {
        Device::Shutdown();
        Factory::Shutdown();
    }
};

TEST_F(GlobalDescriptorHeapManagerTest, CreateShaderManager)
{
    auto &manager = GlobalDescriptorHeapManager::CreateShaderManager("test", 1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    ASSERT_NE(&manager, nullptr);
    
    auto &manager2 = GlobalDescriptorHeapManager::CreateShaderManager("test2", 1, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    ASSERT_NE(&manager2, nullptr);
    
    auto &manager3 = GlobalDescriptorHeapManager::GetShaderHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, "test");
    ASSERT_NE(&manager3, nullptr);
    
    auto &manager4 = GlobalDescriptorHeapManager::GetShaderHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, "test2");
    ASSERT_NE(&manager4, nullptr);
}

TEST_F(GlobalDescriptorHeapManagerTest, GetCPUHeapManager)
{
    auto &manager = GlobalDescriptorHeapManager::GetCPUHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    ASSERT_NE(&manager, nullptr);
    
    auto &manager2 = GlobalDescriptorHeapManager::GetCPUHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    ASSERT_NE(&manager2, nullptr);
}

TEST_F(GlobalDescriptorHeapManagerTest, SegmentManager)
{
    auto &manager = GlobalDescriptorHeapManager::CreateShaderManager("managerTest", 1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    ASSERT_NE(&manager, nullptr);
    
    ASSERT_EQ(manager.GetIncrementSize(), Device::Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
}

TEST_F(GlobalDescriptorHeapManagerTest, Segment)
{
    auto &manager = GlobalDescriptorHeapManager::CreateShaderManager("shaderTest", 10, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    ASSERT_NE(&manager, nullptr);
    
    auto segment = manager.Allocate(1);
    ASSERT_EQ(segment.GetID(), 0);
    
    auto segment2 = manager.Allocate(3);
    ASSERT_EQ(segment2.GetID(), 1);
    
    auto segment3 = manager.Allocate(6);
    ASSERT_EQ(segment3.GetID(), 2);
    
    ASSERT_EQ(segment.GetNumDescriptors(), 1);
    ASSERT_EQ(segment2.GetNumDescriptors(), 3);
    ASSERT_EQ(segment3.GetNumDescriptors(), 6);
    
    ASSERT_EQ(manager.GetSize(0), 1);
    ASSERT_EQ(manager.GetSize(1), 3);
    ASSERT_EQ(manager.GetSize(2), 6);
    
    auto cpuHandle = manager.GetCPUHandle(0);
    ASSERT_NE(cpuHandle.ptr, 0);
    
    auto gpuHandle = manager.GetGPUHandle(0);
    ASSERT_NE(gpuHandle.ptr, 0);
    
    auto cpuHandle2 = manager.GetCPUHandle(1);
    ASSERT_NE(cpuHandle2.ptr, 0);
    
    auto gpuHandle2 = manager.GetGPUHandle(1);
    ASSERT_NE(gpuHandle2.ptr, 0);
    
    auto cpuHandle3 = manager.GetCPUHandle(2);
    ASSERT_NE(cpuHandle3.ptr, 0);
    
    auto gpuHandle3 = manager.GetGPUHandle(2);
    ASSERT_NE(gpuHandle3.ptr, 0);
}