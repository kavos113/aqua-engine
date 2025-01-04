#include <gtest/gtest.h>

#include "AquaEngine.h"

class PolygonTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        AquaEngine::Factory::Init(true);
        AquaEngine::Device::GetAdaptors();
        AquaEngine::Device::Init(0);
        AquaEngine::GlobalDescriptorHeapManager::Init();
        command = new AquaEngine::Command();
    }

    void TearDown() override
    {
        delete command;
        AquaEngine::GlobalDescriptorHeapManager::Shutdown();
        AquaEngine::Device::Shutdown();
        AquaEngine::Factory::Shutdown();
    }

    AquaEngine::Command *command{};
};

TEST_F(PolygonTest, Triangle)
{
    AquaEngine::Triangle triangle(
        {0.0f, 0.5f, 0.0f},
        {0.5f, -0.5f, 0.0f},
        {-0.5f, -0.5f, 0.0f}
    );
    triangle.Create();
}

TEST_F(PolygonTest, Rectangle)
{
    AquaEngine::Rectangle rectangle(
        DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f),
        DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f),
        DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f),
        DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f)
    );
    rectangle.Create();
}
