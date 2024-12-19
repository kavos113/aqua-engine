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

    camera.Init(
        manager,
        {0.0f, 0.0f, -5.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {
            .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
            .NumDescriptors = 1,
            .BaseShaderRegister = 0,
            .RegisterSpace = 0,
            .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
        }
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
        .BaseShaderRegister = 1,
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


    world = DirectX::XMMatrixIdentity();
    DirectX::XMFLOAT3 eye(0, 0, -5);
    DirectX::XMFLOAT3 target(0, 0, 0);
    DirectX::XMFLOAT3 up(0, 1, 0);
    view = DirectX::XMMatrixLookAtLH(
        XMLoadFloat3(&eye),
        XMLoadFloat3(&target),
        XMLoadFloat3(&up)
    );
    projection = DirectX::XMMatrixPerspectiveFovLH(
        DirectX::XM_PIDIV2,
        static_cast<float>(rc.right - rc.left) / static_cast<float>(rc.bottom - rc.top),
        0.1f,
        100.0f
    );
    rectangle->SetMatrix(world * view * projection);
}

void Graphics::Render()
{
    AquaEngine::GlobalDescriptorHeapManager::SetToCommand(*command);

    world = DirectX::XMMatrixRotationY(0.1f) * world;
    //rectangle->SetMatrix(world * view * projection);
    rectangle->SetMatrix(world);

    display->BeginRender();

    D3D12_CPU_DESCRIPTOR_HANDLE rtv = display->GetBackBufferRTV();
    command->List()->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

    float clear_color[] = {0.0f, 0.2f, 0.4f, 1.0f};
    command->List()->ClearRenderTargetView(rtv, clear_color, 0, nullptr);

    pipelineState.SetToCommand(*command);
    rootSignature.SetToCommand(*command);
    display->SetViewports();

    camera.Render(*command);
    rectangle->Render(*command);

    display->EndRender();

    command->Execute();

    display->Present();
}
