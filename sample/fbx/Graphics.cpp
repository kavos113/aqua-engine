#include "Graphics.h"

Graphics::Graphics(HWND hwnd, RECT rc)
    : hwnd(hwnd)
    , rc(rc)
    , command(nullptr)
    , display(nullptr)
    , model(nullptr)
    , camera(rc)
{
    AquaEngine::Factory::Init(true);
    AquaEngine::Device::GetAdaptors();
    AquaEngine::Device::Init(0);
    AquaEngine::GlobalDescriptorHeapManager::Init();

    AquaEngine::FBXManager::Init();
}

void Graphics::SetUp()
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        OutputDebugString("failed to initialize COM\n");
        return;
    }

    OutputDebugString("[Message] COM initialized\n");

    command = std::make_unique<AquaEngine::Command>();
    display = std::make_unique<AquaEngine::Display>(hwnd, rc, *command);
    display->SetBackgroundColor(1.0f, 1.0f, 1.0f, 1.0f);

    auto& manager = AquaEngine::GlobalDescriptorHeapManager::CreateShaderManager(
        "texture",
        10,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
    );

    camera.Init(
        manager,
        {0.0f, 0.0f, -2.0f},
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

    OutputDebugString("[Message] Camera initialized\n");

    auto matrix_segment = std::make_shared<AquaEngine::DescriptorHeapSegment>(manager.Allocate(2));
    D3D12_DESCRIPTOR_RANGE matrix_range = {
        .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        .NumDescriptors = 1,
        .BaseShaderRegister = 1,
        .RegisterSpace = 0,
        .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    };
    matrix_segment->SetRootParameter(
        D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        D3D12_SHADER_VISIBILITY_ALL,
        &matrix_range,
        1
    );

    auto texture_segment = std::make_shared<AquaEngine::DescriptorHeapSegment>(manager.Allocate(2));
    D3D12_DESCRIPTOR_RANGE texture_range = {
        .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        .NumDescriptors = 1,
        .BaseShaderRegister = 0,
        .RegisterSpace = 0,
        .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    };
    texture_segment->SetRootParameter(
        D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        D3D12_SHADER_VISIBILITY_ALL,
        &texture_range,
        1
    );

    auto material_segment = std::make_shared<AquaEngine::DescriptorHeapSegment>(manager.Allocate(2));
    D3D12_DESCRIPTOR_RANGE material_range = {
        .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        .NumDescriptors = 1,
        .BaseShaderRegister = 2,
        .RegisterSpace = 0,
        .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    };
    material_segment->SetRootParameter(
        D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        D3D12_SHADER_VISIBILITY_ALL,
        &material_range,
        1
    );

    model = std::make_unique<AquaEngine::FBXModel>(manager, "ninja.fbx", "ninja.png", *command);
    model->Create();
    model->CreateMatrixBuffer(matrix_segment,0);
    model->SetTexture(texture_segment, 0);
    model->CreateMaterialBufferView(material_segment, 0);
    OutputDebugString("[Message] Model loaded\n");

    model2 = std::make_unique<AquaEngine::FBXModel>(manager,"isu.fbx", "isu.png", *command);
    model2->Create();
    model2->CreateMatrixBuffer(matrix_segment, 1);
    model2->SetTexture(texture_segment, 1);
    model2->CreateMaterialBufferView(material_segment, 1);
    OutputDebugString("[Message] Model2 loaded\n");

    auto inputElement = model->GetInputElementDescs();

    rootSignature.AddStaticSampler(
        {
            .Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            .AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            .AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            .AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
            .MinLOD = 0.0f,
            .MaxLOD = D3D12_FLOAT32_MAX,
            .ShaderRegister = 0,
            .RegisterSpace = 0,
            .ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL
        }
    );
    rootSignature.SetDescriptorHeapSegmentManager(&manager);
    hr = rootSignature.Create();
    if (FAILED(hr)) exit(-1);

    AquaEngine::ShaderObject vs, ps;
    vs.Load(L"vs.hlsl", "vsMain", "vs_5_0");
    ps.Load(L"ps.hlsl", "psMain", "ps_5_0");
    OutputDebugString("[Message] Shader loaded\n");

    pipelineState.SetRootSignature(&rootSignature);
    pipelineState.SetVertexShader(&vs);
    pipelineState.SetPixelShader(&ps);
    pipelineState.SetInputLayout(inputElement.data(), inputElement.size());
    hr = pipelineState.Create();
    if (FAILED(hr)) exit(-1);

    OutputDebugString("[Message] Pipeline state created\n");

    SetTimer(nullptr, 0, model->GetFrameCount(), nullptr);
    SetTimer(nullptr, 1, model2->GetFrameCount(), nullptr);

    model->RotX(-DirectX::XM_PIDIV2);
    model2->RotX(-DirectX::XM_PIDIV2);

    model->Move(-1.5f, -1.0f, 0.0f);
    model2->Move(1.5f, -1.0f, 0.0f);

    model->Scale(2.0f, 2.0f, 2.0f);
    model2->Scale(2.0f, 2.0f, 2.0f);

    model->SetCurrentAnimStack(0);
}

void Graphics::Render() const
{
    AquaEngine::GlobalDescriptorHeapManager::SetToCommand(*command);

    //model->RotY(0.01f);
    //model2->RotY(-0.1f);

    display->BeginRender();

    pipelineState.SetToCommand(*command);
    rootSignature.SetToCommand(*command);
    display->SetViewports();

    camera.Render(*command);
    model->Render(*command);
    model2->Render(*command);

    display->EndRender();

    HRESULT hr = command->Execute();
    if (FAILED(hr)) exit(-1);

    display->Present();
}

void Graphics::Timer(int id) const
{
    switch (id)
    {
    case 0:
        model->Timer();
        break;

    case 1:
        model2->Timer();
        break;
    }
}
