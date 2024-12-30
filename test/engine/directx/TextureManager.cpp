#include <gtest/gtest.h>

#include "AquaEngine.h"

using namespace AquaEngine;

class TextureManagerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Factory::Init(true);
        Device::GetAdaptors();
        Device::Init(0);
        GlobalDescriptorHeapManager::Init();
        command = new Command();
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    }

    void TearDown() override
    {
        delete command;
        GlobalDescriptorHeapManager::Shutdown();
        Device::Shutdown();
        Factory::Shutdown();
    }

    Command *command{};
};

TEST_F(TextureManagerTest, LoadFile)
{
    auto resource = TextureManager::LoadTextureFromFile("icon.png", *command);
    ASSERT_NE(resource, nullptr);
}
