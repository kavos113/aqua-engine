#include <gtest/gtest.h>

#include "directx/wrapper/Factory.h"
#include "directx/wrapper/Device.h"
#include "directx/wrapper/Command.h"
#include "directx/buffer/Buffer.h"

using namespace AquaEngine;

class BufferTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Factory::Init(true);
        Device::GetAdaptors();
        Device::Init(0);
    }

    void TearDown() override
    {
        Device::Shutdown();
        Factory::Shutdown();
    }
};

TEST_F(BufferTest, Create)
{
    Buffer buffer;
    HRESULT hr = buffer.Create(BUFFER_DEFAULT(256));
    if (FAILED(hr))
    {
        std::cout << std::hex << hr << std::endl;
    }
    ASSERT_EQ(hr, S_OK);
    ASSERT_NE(buffer.GetBuffer(), nullptr);
}

TEST_F(BufferTest, CreateDSBuffer)
{
    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth = 1.0f;
    
    Buffer buffer;
    HRESULT hr = buffer.Create(
        Buffer::HeapProperties::Default(),
        D3D12_HEAP_FLAG_NONE,
        Buffer::ResourceDesc::DepthStencil(1, 1),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue
        );
    if (FAILED(hr))
    {
        std::cout << std::hex << hr << std::endl;
    }
    
    ASSERT_EQ(hr, S_OK);
    ASSERT_NE(buffer.GetBuffer(), nullptr);
}