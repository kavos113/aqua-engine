#include "Graphics.h"

using DirectX::operator*;

Graphics::Graphics(HWND hwnd, RECT rc)
    : hwnd(hwnd)
    , rc(rc)
    , command(nullptr)
    , display(nullptr)
    , rectangle(nullptr)
    , camera(rc)
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
        3,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
    );

    auto camera_range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        1,
        0,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    camera.Init(
        {0.0f, 0.0f, -5.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    );
    camera.AddManager("texture", std::move(camera_range));

    rectangle = std::make_unique<AquaEngine::RectangleTexture>(
        manager,
        DirectX::XMFLOAT3(-1.0f, 1.0f, 0.0f),
        DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f),
        DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f),
        DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f),
        "isu.png",
        *command
    );
    rectangle->Create();
    auto texture_range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        1,
        0,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    rectangle->CreateShaderResourceView(std::move(texture_range));
    auto range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        1,
        1,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    rectangle->CreateMatrixBuffer(std::move(range));
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

void Graphics::Render()
{
    AquaEngine::GlobalDescriptorHeapManager::SetToCommand(*command);

    rectangle->RotY(0.1f);
    rectangle->RotX(0.05f);
    rectangle->RotZ(0.03f);

    display->BeginRender();

    pipelineState.SetToCommand(*command);
    rootSignature.SetToCommand(*command);
    display->SetViewports();

    camera.Render(*command, "texture");
    rectangle->Render(*command);

    display->EndRender();

    command->Execute();

    display->Present();
}
