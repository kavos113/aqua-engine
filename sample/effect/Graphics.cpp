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
    display->SetBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);

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
    vs.Load(L"shader.hlsl", "vsMain", "vs_5_0");
    ps.Load(L"shader.hlsl", "psMain", "ps_5_0");
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

    LoadEffect();
}

void Graphics::LoadEffect()
{
    auto format = DXGI_FORMAT_R8G8B8A8_UNORM;
    manager = Effekseer::Manager::Create(8000);
    renderer = EffekseerRendererDX12::Create(
        AquaEngine::Device::Get().Get(),
        command->Queue().Get(),
        2,
        &format,
        1,
        DXGI_FORMAT_D32_FLOAT,
        false,
        8000
    );
    memoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(renderer->GetGraphicsDevice());
    commandList = EffekseerRenderer::CreateCommandList(renderer->GetGraphicsDevice(), memoryPool);
    manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
    manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
    manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
    manager->SetRingRenderer(renderer->CreateRingRenderer());
    manager->SetModelRenderer(renderer->CreateModelRenderer());
    manager->SetTrackRenderer(renderer->CreateTrackRenderer());
    manager->SetTextureLoader(renderer->CreateTextureLoader());
    manager->SetModelLoader(renderer->CreateModelLoader());
    manager->SetMaterialLoader(renderer->CreateMaterialLoader());
    manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

    Effekseer::Matrix44 cameraMatrix;
    Effekseer::Matrix44 projMatrix;
    DirectX::XMMATRIX view = camera.GetView();
    DirectX::XMMATRIX proj = camera.GetProjection();
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            cameraMatrix.Values[i][j] = view.r[i].m128_f32[j];
            projMatrix.Values[i][j] = proj.r[i].m128_f32[j];
        }
    }
    renderer->SetCameraMatrix(cameraMatrix);
    renderer->SetProjectionMatrix(projMatrix);

    renderer->SetCommandList(commandList);

    effect = Effekseer::Effect::Create(manager, u"Laser01.efkefc");
    if (effect == nullptr)
    {
        OutputDebugString("Failed to load effect\n");
        return;
    }
    handle = manager->Play(effect, 0, 0, 0);
}

void Graphics::Render()
{
    AquaEngine::GlobalDescriptorHeapManager::SetToCommand(*command);

    // models->RotZ(0.01f);
    model2->RotY(-0.1f);

    display->BeginRender();

    pipelineState.SetToCommand(*command);
    rootSignature.SetToCommand(*command);
    display->SetViewports();

    camera.Render(*command, "texture");
    directionLight.Render(*command);
    model->Render(*command);
    model2->Render(*command);

    manager->Update();
    memoryPool->NewFrame();
    EffekseerRendererDX12::BeginCommandList(commandList, command->List().Get());
    renderer->BeginRendering();
    manager->Draw();
    renderer->EndRendering();
    EffekseerRendererDX12::EndCommandList(commandList);

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
