#include <gtest/gtest.h>

#include "AquaEngine.h"

using namespace AquaEngine;

class GPUBufferTest : public ::testing::Test
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

TEST_F(GPUBufferTest, Create)
{
    GPUBuffer<int> buffer;
    HRESULT hr = buffer.Create(BUFFER_DEFAULT(sizeof(int) * 3));
    if (FAILED(hr))
    {
        std::cout << std::hex << hr << std::endl;
    }
    ASSERT_EQ(hr, S_OK);
    ASSERT_NE(buffer.GetBuffer(), nullptr);
    
    int* mappedBuffer = buffer.GetMappedBuffer();
    ASSERT_NE(mappedBuffer, nullptr);
    
    buffer.Unmap();
}
