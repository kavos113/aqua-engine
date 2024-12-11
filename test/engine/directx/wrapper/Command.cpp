#include <gtest/gtest.h>

#include "directx/wrapper/Command.h"
#include "directx/wrapper/Device.h"
#include "directx/wrapper/Factory.h"

class CommandTest : public ::testing::Test
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

TEST_F(CommandTest, Init)
{
    Command command;
    ASSERT_NE(command.Queue(), nullptr);
    ASSERT_NE(command.Allocator(), nullptr);
    ASSERT_NE(command.List(), nullptr);
}