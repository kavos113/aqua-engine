#include <gtest/gtest.h>

#include "AquaEngine.h"
#include "directx/buffer/ConstantBufferView.h"
#include "directx/buffer/ShaderResourceView.h"
#include "directx/buffer/DepthStencilView.h"

using namespace AquaEngine;

class BufferViewTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Factory::Init(true);
        Device::GetAdaptors();
        Device::Init(0);
        GlobalDescriptorHeapManager::Init();
        command = new Command();
    }

    void TearDown() override
    {
        if (command)
        {
            delete command;
            command = nullptr;
        }
        GlobalDescriptorHeapManager::Shutdown();
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
    buffer.Create(BUFFER_DEFAULT(256));

    ConstantBufferView view;
    view.SetDescriptorHeapSegment(segment, 0);
    view.Create(buffer);

    ASSERT_NE(view.GetCPUHandle().ptr, 0);
    ASSERT_NE(view.GetGPUHandle().ptr, 0);
}

TEST_F(BufferViewTest, CreateDSV)
{
    DescriptorHeapSegmentManager manager = GlobalDescriptorHeapManager::GetCPUHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    
    std::shared_ptr<DescriptorHeapSegment> segment = std::make_shared<DescriptorHeapSegment>(manager.Allocate(1));
    
    Buffer buffer;
    buffer.Create(Buffer::HeapProperties::Default(),
                  D3D12_HEAP_FLAG_NONE,
                  Buffer::ResourceDesc::DepthStencil(1, 1),
                  D3D12_RESOURCE_STATE_DEPTH_WRITE,
                  nullptr);

    DepthStencilView view;
    view.SetDescriptorHeapSegment(segment, 0);
    view.Create(buffer);
    
    ASSERT_NE(view.GetCPUHandle().ptr, 0);
}