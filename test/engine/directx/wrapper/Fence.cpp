#include <gtest/gtest.h>

#include "directx/wrapper/Device.h"
#include "directx/wrapper/Fence.h"

class FenceTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Factory::Init(true);
        Device::Init();
    }

    void TearDown() override
    {
        Device::Shutdown();
        Factory::Shutdown();
    }
};

TEST_F(FenceTest, Init)
{
    Fence fence;
    ASSERT_NE(fence.Get(), nullptr);
}