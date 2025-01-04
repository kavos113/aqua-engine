#include <gtest/gtest.h>

#include "AquaEngine.h"

using namespace AquaEngine;

class PolygonTextureTest : public testing::Test
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

TEST_F(PolygonTextureTest, Texture)
{
    RectangleTexture rectangle_texture = RectangleTexture(
        GlobalDescriptorHeapManager::CreateShaderManager("rectangle", 10, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
        DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f),
        DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f),
        DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f),
        DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f),
        "icon.png",
        *command
    );
    rectangle_texture.Create();
    auto range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        1,
        0,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    rectangle_texture.CreateShaderResourceView(std::move(range), TODO);
    range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        1,
        0,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    rectangle_texture.CreateMatrixBuffer(std::move(range));
}
