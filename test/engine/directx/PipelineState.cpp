#include <gtest/gtest.h>

#include "AquaEngine.h"

using namespace AquaEngine;

class PipelineStateTest : public ::testing::Test
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

TEST_F(PipelineStateTest, Create)
{
    ShaderObject vs;
    vs.Load(L"vs.hlsl", "vsMain", "vs_5_0");
    
    ShaderObject ps;
    ps.Load(L"ps.hlsl", "psMain", "ps_5_0");
    
    RootSignature rootSignature;
    rootSignature.Create();
    
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        {
            .SemanticName = "POSITION",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        }
    };
    
    PipelineState pipelineState;
    pipelineState.SetRootSignature(&rootSignature);
    pipelineState.SetInputLayout(inputElementDescs, 1);
    pipelineState.SetPixelShader(&ps);
    pipelineState.SetVertexShader(&vs);
    
    HRESULT hr = pipelineState.Create();
    if (FAILED(hr))
    {
        std::cout << std::hex << hr << std::endl;
    }
    
    EXPECT_EQ(hr, S_OK);
}

TEST_F(PipelineStateTest, CreateWithNoRootSignature)
{
    ShaderObject vs;
    vs.Load(L"vs.hlsl", "vsMain", "vs_5_0");
    
    ShaderObject ps;
    ps.Load(L"ps.hlsl", "psMain", "ps_5_0");
    
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        {
            .SemanticName = "POSITION",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        }
    };
    
    PipelineState pipelineState;
    pipelineState.SetInputLayout(inputElementDescs, 1);
    pipelineState.SetPixelShader(&ps);
    pipelineState.SetVertexShader(&vs);
    
    HRESULT hr = pipelineState.Create();
    
    EXPECT_EQ(hr, E_INVALIDARG);
}

TEST_F(PipelineStateTest, CreateWithNoVertexShader)
{
    ShaderObject ps;
    ps.Load(L"ps.hlsl", "psMain", "ps_5_0");
    
    RootSignature rootSignature;
    rootSignature.Create();
    
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        {
            .SemanticName = "POSITION",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        }
    };
    
    PipelineState pipelineState;
    pipelineState.SetRootSignature(&rootSignature);
    pipelineState.SetInputLayout(inputElementDescs, 1);
    pipelineState.SetPixelShader(&ps);
    
    HRESULT hr = pipelineState.Create();
    
    EXPECT_EQ(hr, E_INVALIDARG);
}

TEST_F(PipelineStateTest, CreateWithNoPixelShader)
{
    ShaderObject vs;
    vs.Load(L"vs.hlsl", "vsMain", "vs_5_0");
    
    RootSignature rootSignature;
    rootSignature.Create();
    
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        {
            .SemanticName = "POSITION",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        }
    };
    
    PipelineState pipelineState;
    pipelineState.SetRootSignature(&rootSignature);
    pipelineState.SetInputLayout(inputElementDescs, 1);
    pipelineState.SetVertexShader(&vs);
    
    HRESULT hr = pipelineState.Create();
    
    EXPECT_EQ(hr, S_OK);
}

TEST_F(PipelineStateTest, CreateWithNoInputLayout)
{
    ShaderObject vs;
    vs.Load(L"vs.hlsl", "vsMain", "vs_5_0");
    
    ShaderObject ps;
    ps.Load(L"ps.hlsl", "psMain", "ps_5_0");
    
    RootSignature rootSignature;
    rootSignature.Create();
    
    PipelineState pipelineState;
    pipelineState.SetRootSignature(&rootSignature);
    pipelineState.SetPixelShader(&ps);
    pipelineState.SetVertexShader(&vs);
    
    HRESULT hr = pipelineState.Create();
    
    EXPECT_EQ(hr, E_INVALIDARG);
}

TEST_F(PipelineStateTest, Command)
{
    ShaderObject vs;
    vs.Load(L"vs.hlsl", "vsMain", "vs_5_0");
    
    ShaderObject ps;
    ps.Load(L"ps.hlsl", "psMain", "ps_5_0");
    
    RootSignature rootSignature;
    rootSignature.Create();
    
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        {
            .SemanticName = "POSITION",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        }
    };
    
    PipelineState pipelineState;
    pipelineState.SetRootSignature(&rootSignature);
    pipelineState.SetInputLayout(inputElementDescs, 1);
    pipelineState.SetPixelShader(&ps);
    pipelineState.SetVertexShader(&vs);
    
    HRESULT hr = pipelineState.Create();
    if (FAILED(hr))
    {
        std::cout << std::hex << hr << std::endl;
    }
    
    EXPECT_EQ(hr, S_OK);
    
    pipelineState.SetToCommand(*command);
    
    hr = command->Execute();
    if (FAILED(hr))
    {
        std::cout << std::hex << hr << std::endl;
    }
    ASSERT_EQ(hr, S_OK);
}