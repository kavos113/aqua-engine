#include <gtest/gtest.h>

#include "AquaEngine.h"

using namespace AquaEngine;

class RootSignatureTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Factory::Init(true);
        Device::GetAdaptors();
        Device::Init(0);
        GlobalDescriptorHeapManager::Init();
        command = new Command();
    }
    
    void TearDown() override
    {
        delete command;
        GlobalDescriptorHeapManager::Shutdown();
        Device::Shutdown();
        Factory::Shutdown();
    }
    
    Command* command;
};

TEST_F(RootSignatureTest, Create)
{
    RootSignature rootSignature;
    rootSignature.Create();
}

TEST_F(RootSignatureTest, SetToCommand)
{
    auto manager = GlobalDescriptorHeapManager::CreateShaderManager(
        "RootSignatureTest",
        1,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
    );
    auto segment = std::make_shared<DescriptorHeapSegment>(manager.Allocate(1));
    auto range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        1,
        0,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    segment->SetRootParameter(
        D3D12_ROOT_PARAMETER_TYPE_CBV,
        D3D12_SHADER_VISIBILITY_ALL,
        std::move(range),
        1
    );
    
    RootSignature rootSignature;
    rootSignature.SetDescriptorHeapSegmentManager(&manager);
    HRESULT hr = rootSignature.Create();
    if (FAILED(hr))
    {
        std::cout << std::hex << hr << std::endl;
    }
    ASSERT_EQ(hr, S_OK);
    
    rootSignature.SetToCommand(*command);
    
    ASSERT_NE(rootSignature.GetRootSignature(), nullptr);
    
    hr = command->Execute();
    if (FAILED(hr))
    {
        std::cout << std::hex << hr << std::endl;
    }
    ASSERT_EQ(hr, S_OK);
}