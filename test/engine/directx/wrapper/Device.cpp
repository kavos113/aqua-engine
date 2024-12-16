#include <gtest/gtest.h>

#include "directx/wrapper/Device.h"

using namespace AquaEngine;

class DeviceTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Factory::Init(true);
    }

    void TearDown() override
    {
        Factory::Shutdown();
    }
};

TEST_F(DeviceTest, Init)
{
    Device::Init();
    ASSERT_NE(Device::Get(), nullptr);
    Device::Shutdown();
}

TEST_F(DeviceTest, GetAdaptors)
{
    Device::Init();
    auto adaptors = Device::GetAdaptors();
    for (const auto & adaptor : adaptors)
    {
        std::wcout << "adaptor: " << adaptor << std::endl;
    }

    ASSERT_GT(adaptors.size(), 0);
    Device::Shutdown();
}

TEST_F(DeviceTest, InitWithAdaptor)
{
    auto adaptors = Device::GetAdaptors();
    std::wcout << L"Choose : " << adaptors[0] << std::endl;
    Device::Init(0);
    ASSERT_NE(Device::Get(), nullptr);
    Device::Shutdown();
}