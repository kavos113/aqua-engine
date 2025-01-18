#include "Graphics.h"

#include <iostream>
#include <ostream>

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

    Progress p = {0.05f, L"Initialized COM"};
    SendMessage(hwnd, WM_AQUA_LOADING, 0, reinterpret_cast<LPARAM>(&p));

    command = std::make_unique<AquaEngine::Command>();
    display = std::make_unique<AquaEngine::Display>(hwnd, rc, *command);
    display->SetBackgroundColor(1.0f, 1.0f, 1.0f, 1.0f);

    auto &manager = AquaEngine::GlobalDescriptorHeapManager::CreateShaderManager(
        "texture",
        10,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
    );

    auto camera_range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        1,
        2,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    camera.Init(
        {0.0f, 0.0f, -2.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    );
    camera.AddManager("texture", std::move(camera_range));

    Progress p2 = {0.1f, L"Created Camera"};
    SendMessage(hwnd, WM_AQUA_LOADING, 0, reinterpret_cast<LPARAM>(&p2));

    auto light_range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        1,
        3,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    directionLight.Init(
        {-1.0f, -1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        manager,
        std::move(light_range)
    );

    auto matrix_segment = std::make_shared<AquaEngine::DescriptorHeapSegment>(manager.Allocate(2));
    auto matrix_range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        1,
        0,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    matrix_segment->SetRootParameter(
        D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        D3D12_SHADER_VISIBILITY_ALL,
        std::move(matrix_range),
        1
    );

    auto texture_segment = std::make_shared<AquaEngine::DescriptorHeapSegment>(manager.Allocate(2));
    auto texture_range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        1,
        0,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    texture_segment->SetRootParameter(
        D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        D3D12_SHADER_VISIBILITY_ALL,
        std::move(texture_range),
        1
    );

    auto material_segment = std::make_shared<AquaEngine::DescriptorHeapSegment>(manager.Allocate(2));
    auto material_range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        1,
        1,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    material_segment->SetRootParameter(
        D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        D3D12_SHADER_VISIBILITY_ALL,
        std::move(material_range),
        1
    );
    Progress p25 = {0.15f, L"Created Descriptor Heap Segments"};
    SendMessage(hwnd, WM_AQUA_LOADING, 0, reinterpret_cast<LPARAM>(&p25));

    model = std::make_unique<AquaEngine::FBXModel>("ninja.fbx", "ninja.png", *command);
    model->Create();
    model->CreateMatrixBuffer(matrix_segment, 0);
    model->SetTexture(texture_segment, 0);
    model->CreateMaterialBufferView(material_segment, 0);
    Progress p3 = {0.4f, L"Loaded Model1: ninja.fbx"};
    SendMessage(hwnd, WM_AQUA_LOADING, 0, reinterpret_cast<LPARAM>(&p3));

    model2 = std::make_unique<AquaEngine::FBXModel>("isu.fbx", "isu.png", *command);
    model2->Create();
    model2->CreateMatrixBuffer(matrix_segment, 1);
    model2->SetTexture(texture_segment, 1);
    model2->CreateMaterialBufferView(material_segment, 1);
    Progress p4 = {0.7f, L"Loaded Model2: isu.fbx"};
    SendMessage(hwnd, WM_AQUA_LOADING, 0, reinterpret_cast<LPARAM>(&p4));

    auto inputElement = model->GetInputElementDescs();

    rootSignature.AddStaticSampler(AquaEngine::RootSignature::DefaultStaticSampler());
    rootSignature.SetDescriptorHeapSegmentManager(&manager);
    hr = rootSignature.Create();
    if (FAILED(hr)) exit(-1);

    AquaEngine::ShaderObject vs, ps;
    vs.Load(L"shaders/shader.hlsl", "vsMain", "vs_5_0");
    ps.Load(L"shaders/shader.hlsl", "psMain", "ps_5_0");
    Progress p5 = {0.8f, L"Loaded Shaders"};
    SendMessage(hwnd, WM_AQUA_LOADING, 0, reinterpret_cast<LPARAM>(&p5));

    pipelineState.SetRootSignature(&rootSignature);
    pipelineState.SetVertexShader(&vs);
    pipelineState.SetPixelShader(&ps);
    pipelineState.SetInputLayout(inputElement.data(), inputElement.size());
    hr = pipelineState.Create();
    if (FAILED(hr)) exit(-1);

    Progress p6 = {0.95f, L"Created Pipeline State"};
    SendMessage(hwnd, WM_AQUA_LOADING, 0, reinterpret_cast<LPARAM>(&p6));

    auto anims = model->GetAnimStackNames();
    for (auto &anim: anims)
    {
        std::cout << anim << std::endl;
    }

    model->PlayAnimation("metarig|hirou", AquaEngine::FBXModel::AnimationMode::LOOP);
    SetTimer(hwnd, TIMER_MODEL1, model->GetFrameCount(), nullptr);
    SetTimer(hwnd, TIMER_MODEL2, model2->GetFrameCount(), nullptr);

    model->RotX(-DirectX::XM_PIDIV2);
    model->RotY(DirectX::XM_PI);
    model2->RotX(-DirectX::XM_PIDIV2);

    model->Move(-1.5f, -1.0f, 0.0f);
    model2->Move(1.5f, -1.0f, 0.0f);

    model->Scale(2.0f, 2.0f, 2.0f);
    model2->Scale(2.0f, 2.0f, 2.0f);

    Progress p7 = {1.0f, L"Finished"};
    SendMessage(hwnd, WM_AQUA_LOADING, 0, reinterpret_cast<LPARAM>(&p7));

    hr = rt1.Create(display->GetBackBufferResourceDesc(), rc.right - rc.left, rc.bottom - rc.top);
    if (FAILED(hr)) exit(-2);
    rt1.SetBackgroundColor(1.0f, 1.0f, 1.0f, 1.0f);
    auto &rt_manager = AquaEngine::GlobalDescriptorHeapManager::CreateShaderManager(
        "rt1",
        5,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
    );
    auto segment = std::make_shared<AquaEngine::DescriptorHeapSegment>(rt_manager.Allocate(2));
    auto rt1_range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        1,
        0,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    segment->SetRootParameter(
        D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        D3D12_SHADER_VISIBILITY_PIXEL,
        std::move(rt1_range),
        1
    );
    hr = rt1.CreateShaderResourceView(segment, 0);
    if (FAILED(hr)) exit(-3);

    hr = rt2.Create(display->GetBackBufferResourceDesc(), rc.right - rc.left, rc.bottom - rc.top);
    if (FAILED(hr)) exit(-12);
    rt2.SetBackgroundColor(1.0f, 1.0f, 1.0f, 1.0f);
    hr = rt2.CreateShaderResourceView(segment, 1);
    if (FAILED(hr)) exit(-13);

    std::array<float, 8> blur_weights{};
    float total = 0.0f;
    float sigma = 1.0f;
    for (int i = 0; i < blur_weights.size(); ++i)
    {
        blur_weights[i] = expf(-0.5f * (i * i) / (sigma * sigma));
        total += blur_weights[i] * (i == 0 ? 1.0f : 2.0f);
    }
    for (auto &weight: blur_weights)
    {
        weight /= total;
    }
    blurBuffer.Create(BUFFER_DEFAULT(AquaEngine::AlignmentSize(sizeof(float) * blur_weights.size(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)));
    std::ranges::copy(blur_weights, blurBuffer.GetMappedBuffer());
    blurBuffer.Unmap();
    auto blur_segment = std::make_shared<AquaEngine::DescriptorHeapSegment>(rt_manager.Allocate(1));
    auto blur_range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        1,
        0,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    blur_segment->SetRootParameter(
        D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        D3D12_SHADER_VISIBILITY_PIXEL,
        std::move(blur_range),
        1
    );
    blurCBV.SetDescriptorHeapSegment(blur_segment, 0);
    blurCBV.Create(blurBuffer.GetBuffer());

    rt1_rootSignature.AddStaticSampler(AquaEngine::RootSignature::DefaultStaticSampler());
    rt1_rootSignature.SetDescriptorHeapSegmentManager(&rt_manager);
    hr = rt1_rootSignature.Create();
    if (FAILED(hr)) exit(-4);
    auto rt1_in = AquaEngine::RenderTarget::GetInputElementDescs();

    AquaEngine::ShaderObject rt1_vs, rt1_ps;
    rt1_vs.Load(L"shaders/rt1.hlsl", "vs", "vs_5_0");
    rt1_ps.Load(L"shaders/rt1.hlsl", "pshor", "ps_5_0");

    rt1_pipelineState.SetRootSignature(&rt1_rootSignature);
    rt1_pipelineState.SetVertexShader(&rt1_vs);
    rt1_pipelineState.SetPixelShader(&rt1_ps);
    rt1_pipelineState.SetInputLayout(rt1_in.data(), rt1_in.size());
    hr = rt1_pipelineState.Create();
    if (FAILED(hr)) exit(-5);

    rt2_rootSignature.AddStaticSampler(AquaEngine::RootSignature::DefaultStaticSampler());
    rt2_rootSignature.SetDescriptorHeapSegmentManager(&rt_manager);
    hr = rt2_rootSignature.Create();
    if (FAILED(hr)) exit(-14);
    auto rt2_in = AquaEngine::RenderTarget::GetInputElementDescs();

    AquaEngine::ShaderObject rt2_vs, rt2_ps;
    rt2_vs.Load(L"rt1.hlsl", "vs", "vs_5_0");
    rt2_ps.Load(L"rt1.hlsl", "psver", "ps_5_0");

    rt2_pipelineState.SetRootSignature(&rt2_rootSignature);
    rt2_pipelineState.SetVertexShader(&rt2_vs);
    rt2_pipelineState.SetPixelShader(&rt2_ps);
    rt2_pipelineState.SetInputLayout(rt2_in.data(), rt2_in.size());
    hr = rt2_pipelineState.Create();
    if (FAILED(hr)) exit(-15);
}

void Graphics::Render()
{
    AquaEngine::GlobalDescriptorHeapManager::SetToCommand(*command);

    // models->RotZ(0.01f);
    model2->RotY(-0.1f);

    rt1.BeginRender(*command);

    pipelineState.SetToCommand(*command);
    rootSignature.SetToCommand(*command);
    display->SetViewports();

    camera.Render(*command, "texture");
    directionLight.Render(*command);
    model->Render(*command);
    model2->Render(*command);

    rt1.EndRender(*command);

    rt2.BeginRender(*command);

    rt1_pipelineState.SetToCommand(*command);
    rt1_rootSignature.SetToCommand(*command);
    rt1.UseAsTexture(*command);
    blurCBV.SetGraphicsRootDescriptorTable(command.get());
    display->SetViewports();
    rt1.Render(*command);

    rt2.EndRender(*command);

    display->BeginRender();

    rt2_pipelineState.SetToCommand(*command);
    rt2_rootSignature.SetToCommand(*command);
    rt2.UseAsTexture(*command);
    blurCBV.SetGraphicsRootDescriptorTable(command.get());
    display->SetViewports();
    rt2.Render(*command);

    display->EndRender();

    HRESULT hr = command->Execute();
    if (FAILED(hr)) exit(-1);

    display->Present();
}

void Graphics::Timer(int id) const
{
    switch (id)
    {
    case TIMER_MODEL1:
        model->Timer();
        InvalidateRect(hwnd, &rc, false);
        break;

    case TIMER_MODEL2:
        model2->Timer();
        InvalidateRect(hwnd, &rc, false);
        break;
    }
}
