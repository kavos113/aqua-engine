#include <gtest/gtest.h>

#include "AquaEngine.h"

using namespace AquaEngine;

class FBXTest : public testing::Test
{
protected:
    void SetUp() override
    {
        Factory::Init(true);
        Device::GetAdaptors();
        Device::Init(0);
        GlobalDescriptorHeapManager::Init();
        FBXManager::Init();
        command = new Command();
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    }

    void TearDown() override
    {
        delete command;
        FBXManager::Shutdown();
        GlobalDescriptorHeapManager::Shutdown();
        Device::Shutdown();
        Factory::Shutdown();
    }

    Command *command{};
};

TEST_F(FBXTest, Init)
{
    ASSERT_NE(FBXManager::Get(), nullptr);
}

TEST_F(FBXTest, Load)
{
    FbxScene *scene;
    int r = FBXManager::ReadFile("isu.fbx", &scene);
    ASSERT_EQ(r, 0);
    ASSERT_NE(scene, nullptr);
}

TEST_F(FBXTest, LoadModel)
{
    FBXModel model("isu.fbx");
    model.Create();
}

TEST_F(FBXTest, LoadModelTexture)
{
    auto &manager = GlobalDescriptorHeapManager::CreateShaderManager(
        "rectangle",
        10,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
    );
    FBXModel model(
        "isu.fbx",
        "isu.png",
        *command
    );
    model.Create();
    auto range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        1,
        0,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    model.SetTexture(std::move(range), manager);
}
