#include <gtest/gtest.h>
#include "directx/wrapper/Factory.h"
#include "directx/wrapper/Device.h"
#include "directx/wrapper/Command.h"
#include "directx/wrapper/DescriptorHeap.h"

class DescriptorHeapTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Factory::Init(true);
        Device::GetAdaptors();
        Device::Init(0);
        
        command = new Command();
    }

    void TearDown() override
    {
        delete command;
        Device::Shutdown();
        Factory::Shutdown();
    }
    
    Command* command{};
};

TEST_F(DescriptorHeapTest, Create)
{
    DescriptorHeap heap;
    D3D12_DESCRIPTOR_HEAP_DESC desc;
    
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NodeMask = 0;
    desc.NumDescriptors = 1024;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    
    ASSERT_EQ(heap.Create(&desc), S_OK);
}

TEST_F(DescriptorHeapTest, GetCPUHandle)
{
    DescriptorHeap heap;
    D3D12_DESCRIPTOR_HEAP_DESC desc;
    
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NodeMask = 0;
    desc.NumDescriptors = 1024;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    
    ASSERT_EQ(heap.Create(&desc), S_OK);
    
    D3D12_CPU_DESCRIPTOR_HANDLE handle = heap.GetCPUHandle();
    ASSERT_NE(handle.ptr, 0);
}

TEST_F(DescriptorHeapTest, GetGPUHandle)
{
    DescriptorHeap heap;
    D3D12_DESCRIPTOR_HEAP_DESC desc;
    
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NodeMask = 0;
    desc.NumDescriptors = 1024;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    
    ASSERT_EQ(heap.Create(&desc), S_OK);
    
    D3D12_GPU_DESCRIPTOR_HANDLE handle = heap.GetGPUHandle();
    ASSERT_NE(handle.ptr, 0);
}

TEST_F(DescriptorHeapTest, GetIncrementSize)
{
    DescriptorHeap heap;
    D3D12_DESCRIPTOR_HEAP_DESC desc;
    
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NodeMask = 0;
    desc.NumDescriptors = 1024;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    
    ASSERT_EQ(heap.Create(&desc), S_OK);
    
    UINT incrementSize = heap.GetIncrementSize();
    ASSERT_NE(incrementSize, 0);
}