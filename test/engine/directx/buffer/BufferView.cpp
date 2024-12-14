#include <gtest/gtest.h>

#include "AquaEngine.h"
#include "directx/buffer/ConstantBufferView.h"
#include "directx/buffer/ShaderResourceView.h"
#include "directx/buffer/DepthStencilView.h"
#include "directx/buffer/RenderTargetView.h"

class BufferViewTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Factory::Init(true);
        Device::GetAdaptors();
        Device::Init(0);
        command = new Command();
        GlobalDescriptorHeapManager::Init();
    }

    void TearDown() override
    {
        delete command;
        Device::Shutdown();
        Factory::Shutdown();
    }

    Command* command{};
};

TEST_F(BufferViewTest, CreateCBV)
{
    DescriptorHeapSegmentManager manager = GlobalDescriptorHeapManager::CreateShaderManager(
        "CBV",
        1,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
        );
    
    std::shared_ptr<DescriptorHeapSegment> segment = std::make_shared<DescriptorHeapSegment>(manager.Allocate(1));
    
    Buffer buffer;
    buffer.Create(BUFFER_DEFAULT(1));

    ConstantBufferView view;
    view.SetDescriptorHeapSegment(segment, 0);
    view.Create(&buffer);
    
    ASSERT_NE(view.GetCPUHandle().ptr, 0);
    ASSERT_NE(view.GetGPUHandle().ptr, 0);
    
    GlobalDescriptorHeapManager::SetToCommand(nullptr);
    view.SetGraphicsRootDescriptorTable(command);
    
    command->Execute();
}

TEST_F(BufferViewTest, CreateSRV)
{
    DescriptorHeapSegmentManager manager = GlobalDescriptorHeapManager::CreateShaderManager(
        "SRV",
        1,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
        );
    
    std::shared_ptr<DescriptorHeapSegment> segment = std::make_shared<DescriptorHeapSegment>(manager.Allocate(1));
    
    Buffer buffer;
    buffer.Create(BUFFER_DEFAULT(1));

    ShaderResourceView view;
    view.SetDescriptorHeapSegment(segment, 0);
    view.Create(&buffer);
    
    ASSERT_NE(view.GetCPUHandle().ptr, 0);
    ASSERT_NE(view.GetGPUHandle().ptr, 0);
    
    GlobalDescriptorHeapManager::SetToCommand(nullptr);
    view.SetGraphicsRootDescriptorTable(command);
    
    command->Execute();
}

TEST_F(BufferViewTest, CreateDSV)
{
    DescriptorHeapSegmentManager manager = GlobalDescriptorHeapManager::GetCPUHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    
    std::shared_ptr<DescriptorHeapSegment> segment = std::make_shared<DescriptorHeapSegment>(manager.Allocate(1));
    
    Buffer buffer;
    buffer.Create(BUFFER_DEFAULT(1));

    DepthStencilView view;
    view.SetDescriptorHeapSegment(segment, 0);
    view.Create(&buffer);
    
    ASSERT_NE(view.GetCPUHandle().ptr, 0);
    ASSERT_NE(view.GetGPUHandle().ptr, 0);
    
    GlobalDescriptorHeapManager::SetToCommand(nullptr);
    view.SetGraphicsRootDescriptorTable(command);
    
    command->Execute();
}

TEST_F(BufferViewTest, CreateRTV)
{
    DescriptorHeapSegmentManager manager = GlobalDescriptorHeapManager::GetCPUHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    
    std::shared_ptr<DescriptorHeapSegment> segment = std::make_shared<DescriptorHeapSegment>(manager.Allocate(1));
    
    Buffer buffer;
    buffer.Create(BUFFER_DEFAULT(1));

    RenderTargetView view;
    view.SetDescriptorHeapSegment(segment, 0);
    view.Create(&buffer);
    
    ASSERT_NE(view.GetCPUHandle().ptr, 0);
    ASSERT_NE(view.GetGPUHandle().ptr, 0);
    
    GlobalDescriptorHeapManager::SetToCommand(nullptr);
    view.SetGraphicsRootDescriptorTable(command);
    
    command->Execute();
}