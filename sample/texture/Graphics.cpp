#include "Graphics.h"

Graphics::Graphics(HWND hwnd, RECT rc)
    : hwnd(hwnd)
    , rc(rc)
    , command(nullptr)
    , display(nullptr)
    , rectangle(nullptr)
{
    AquaEngine::Factory::Init(true);
    AquaEngine::Device::GetAdaptors();
    AquaEngine::Device::Init(0);
    AquaEngine::GlobalDescriptorHeapManager::Init();
}

void Graphics::SetUp()
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        OutputDebugString("failed to initialize COM\n");
        return;
    }

    command = std::make_unique<AquaEngine::Command>();
    display = std::make_unique<AquaEngine::Display>(hwnd, rc, *command);

    auto& manager = AquaEngine::GlobalDescriptorHeapManager::CreateShaderManager(
        "texture",
        2,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
    );

    rectangle = std::make_unique<AquaEngine::RectangleTexture>(
        manager,
        DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f),
        DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f),
        DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f),
        DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f),
        "icon.png",
        *command
    );
    rectangle->Create();
    rectangle->CreateShaderResourceView({
        .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        .NumDescriptors = 1,
        .BaseShaderRegister = 0,
        .RegisterSpace = 0,
        .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    });
    rectangle->CreateMatrixBuffer({
        .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        .NumDescriptors = 1,
        .BaseShaderRegister = 0,
        .RegisterSpace = 0,
        .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    });
    auto inputElement = rectangle->GetInputElementDescs();

    rootSignature.AddStaticSampler({
        .Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
        .AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        .AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        .AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        .MipLODBias = 0,
        .MaxAnisotropy = 0,
        .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
        .BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
        .MinLOD = 0.0f,
        .MaxLOD = D3D12_FLOAT32_MAX,
        .ShaderRegister = 0,
        .RegisterSpace = 0,
        .ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL
    });
    rootSignature.SetDescriptorHeapSegmentManager(&manager);
    rootSignature.Create();

    AquaEngine::ShaderObject vs, ps;
    vs.Load(L"vs.hlsl", "vsMain", "vs_5_0");
    ps.Load(L"ps.hlsl", "psMain", "ps_5_0");

    pipelineState.SetRootSignature(&rootSignature);
    pipelineState.SetVertexShader(&vs);
    pipelineState.SetPixelShader(&ps);
    pipelineState.SetInputLayout(inputElement.data(), inputElement.size());
    pipelineState.Create();
}

void Graphics::Render() const
{
    AquaEngine::GlobalDescriptorHeapManager::SetToCommand(*command);

    rectangle->RotationY(0.01f);

    display->BeginRender();

    D3D12_CPU_DESCRIPTOR_HANDLE rtv = display->GetBackBufferRTV();
    command->List()->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

    float clear_color[] = {0.0f, 0.2f, 0.4f, 1.0f};
    command->List()->ClearRenderTargetView(rtv, clear_color, 0, nullptr);

    pipelineState.SetToCommand(*command);
    rootSignature.SetToCommand(*command);
    display->SetViewports();

    rectangle->Draw(*command);

    display->EndRender();

    command->Execute();

    display->Present();
}
