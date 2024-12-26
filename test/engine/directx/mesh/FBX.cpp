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
    }

    void TearDown() override
    {
        delete command;
        FBXManager::Shutdown();
        GlobalDescriptorHeapManager::Shutdown();
        Device::Shutdown();
        Factory::Shutdown();
    }

    Command* command{};
};

TEST_F(FBXTest, Init)
{
    ASSERT_NE(FBXManager::Get(), nullptr);
}

TEST_F(FBXTest, Load)
{
    FbxScene* scene;
    int r = FBXManager::ReadFile("isu.fbx", &scene);
    ASSERT_EQ(r, 0);
    ASSERT_NE(scene, nullptr);
}

TEST_F(FBXTest, LoadModel)
{
    FBXModel model(
        GlobalDescriptorHeapManager::CreateShaderManager("rectangle", 10, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
        "isu.fbx");
    model.Create();
}

TEST_F(FBXTest, LoadModelTexture)
{
    FBXModel model(
        GlobalDescriptorHeapManager::CreateShaderManager("rectangle", 10, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
        "isu.fbx",
        "isu.png",
        *command);
    model.Create();
    model.SetTexture({
        .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        .NumDescriptors = 1,
        .BaseShaderRegister = 0,
        .RegisterSpace = 0,
        .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    });
}