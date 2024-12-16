#include <gtest/gtest.h>

#include "directx/wrapper/Factory.h"
#include "directx/wrapper/Device.h"
#include "directx/wrapper/Command.h"
#include "directx/wrapper/Barrier.h"
#include "directx/buffer/Buffer.h"

using namespace AquaEngine;

class BarrierTest : public testing::Test
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
        Device::Shutdown();
        Factory::Shutdown();
        delete command;
    }
    
    Command* command;
};

TEST_F(BarrierTest, Transition)
{
    Buffer buffer;
    buffer.Create(BUFFER_DEFAULT(1));

    Barrier barrier;
    barrier.Transition(command, buffer.GetBuffer(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    command->Execute();
}