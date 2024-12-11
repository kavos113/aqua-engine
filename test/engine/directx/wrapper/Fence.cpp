#include <gtest/gtest.h>

#include "directx/wrapper/Device.h"
#include "directx/wrapper/Fence.h"

class FenceTest : public ::testing::Test
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

TEST_F(FenceTest, Init)
{
    Fence fence;
    ASSERT_NE(fence.Get(), nullptr);
}