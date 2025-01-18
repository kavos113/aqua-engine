#include "Graphics.h"

#include <iostream>
#include <ostream>

Graphics::Graphics(HWND hwnd, RECT rc)
    : hwnd(hwnd)
    , rc(rc)
    , command(nullptr)
    , display(nullptr)
    , model(nullptr)
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
    camera = std::make_shared<AquaEngine::Camera>(rc);
    camera->Init(
        {0.0f, 0.0f, -2.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    );
    camera->AddManager("texture", std::move(camera_range));

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

    auto& skybox_manager = AquaEngine::GlobalDescriptorHeapManager::CreateShaderManager(
        "skybox",
        10,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
    );
    skybox = std::make_unique<AquaEngine::SkyBox>(
        "sample1.hdr",
        *command,
        skybox_manager
    );
    auto world_range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        1,
        1,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    skybox->CreateMatrixBuffer(std::move(world_range), skybox_manager);
    skybox->Create();
    skybox->SetCamera(camera);
    skybox->ConvertHDRIToCubeMap(*command);
    skybox->CreateCubeMapPipelineState();
    skybox->Scale(1000.0f, 1000.0f, 1000.0f);

    hr = model_rt.Create(display->GetBackBufferResourceDesc(), rc.right - rc.left, rc.bottom - rc.top);
    if (FAILED(hr)) exit(-2);
    model_rt.SetBackgroundColor(1.0f, 1.0f, 1.0f, 1.0f);
    auto& rt_manager = AquaEngine::GlobalDescriptorHeapManager::CreateShaderManager(
        "rt",
        10,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
    );
    auto model_rt_segment = std::make_shared<AquaEngine::DescriptorHeapSegment>(rt_manager.Allocate(1));
    auto model_rt_range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        1,
        0,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    model_rt_segment->SetRootParameter(
        D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        D3D12_SHADER_VISIBILITY_PIXEL,
        std::move(model_rt_range),
        1
    );
    hr = model_rt.CreateShaderResourceView(model_rt_segment, 0);
    if (FAILED(hr)) exit(-3);

    hr = cubemap_rt.Create(display->GetBackBufferResourceDesc(), rc.right - rc.left, rc.bottom - rc.top);
    if (FAILED(hr)) exit(-4);
    cubemap_rt.SetBackgroundColor(1.0f, 1.0f, 1.0f, 1.0f);
    auto cubemap_rt_segment = std::make_shared<AquaEngine::DescriptorHeapSegment>(rt_manager.Allocate(1));
    auto cubemap_rt_range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        1,
        1,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    cubemap_rt_segment->SetRootParameter(
        D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        D3D12_SHADER_VISIBILITY_PIXEL,
        std::move(cubemap_rt_range),
        1
    );
    hr = cubemap_rt.CreateShaderResourceView(cubemap_rt_segment, 0);
    if (FAILED(hr)) exit(-5);

    weightBuffer.Create(BUFFER_DEFAULT(AquaEngine::AlignmentSize(sizeof(Weight), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)));
    weightBuffer.GetMappedBuffer()->weight = weight;
    auto weight_segment = std::make_shared<AquaEngine::DescriptorHeapSegment>(rt_manager.Allocate(1));
    auto weight_range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        1,
        0,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    weight_segment->SetRootParameter(
        D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
        D3D12_SHADER_VISIBILITY_PIXEL,
        std::move(weight_range),
        1
    );
    weightCBV.SetDescriptorHeapSegment(weight_segment, 0);
    weightCBV.Create(weightBuffer.GetBuffer());

    rt_rootSignature.AddStaticSampler(AquaEngine::RootSignature::DefaultStaticSampler());
    rt_rootSignature.SetDescriptorHeapSegmentManager(&rt_manager);
    hr = rt_rootSignature.Create();
    if (FAILED(hr)) exit(-6);
    auto rt_in = AquaEngine::RenderTarget::GetInputElementDescs();

    AquaEngine::ShaderObject rt_vs, rt_ps;
    rt_vs.Load(L"all.hlsl", "vs", "vs_5_0");
    rt_ps.Load(L"all.hlsl", "ps", "ps_5_0");

    rt_pipelineState.SetRootSignature(&rt_rootSignature);
    rt_pipelineState.SetVertexShader(&rt_vs);
    rt_pipelineState.SetPixelShader(&rt_ps);
    rt_pipelineState.SetInputLayout(rt_in.data(), rt_in.size());
    hr = rt_pipelineState.Create();
    if (FAILED(hr)) exit(-7);

    InitD2D();
}

void Graphics::InitD2D()
{
    Microsoft::WRL::ComPtr<ID3D11Device> d3d11Device;
    HRESULT hr = D3D11On12CreateDevice(
        AquaEngine::Device::Get().Get(),
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr,
        0,
        reinterpret_cast<IUnknown**>(command->Queue().GetAddressOf()),
        1,
        0,
        &d3d11Device,
        &d3d11DeviceContext,
        nullptr
    );
    if (FAILED(hr))
    {
        OutputDebugString("Failed to create D3D11On12 device\n");
        return;
    }

    hr = d3d11Device.As(&d3d11On12Device);
    if (FAILED(hr))
    {
        OutputDebugString("Failed to get D3D11On12 device\n");
        return;
    }

    hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        __uuidof(d2dFactory),
        nullptr,
        &d2dFactory
    );
    if (FAILED(hr))
    {
        OutputDebugString("Failed to create D2D factory\n");
        return;
    }

    hr = d3d11On12Device.As(&dxgiDevice);
    if (FAILED(hr))
    {
        OutputDebugString("Failed to get DXGI device\n");
        return;
    }

    hr = d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice);
    if (FAILED(hr))
    {
        OutputDebugString("Failed to create D2D device\n");
        return;
    }

    hr = d2dDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
        &d2dDeviceContext
    );
    if (FAILED(hr))
    {
        OutputDebugString("Failed to create D2D device context\n");
        return;
    }

    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(dwriteFactory),
        reinterpret_cast<IUnknown**>(dwriteFactory.GetAddressOf())
    );
    if (FAILED(hr))
    {
        OutputDebugString("Failed to create DWrite factory\n");
        return;
    }

    // create render targets for D2D
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = display->GetSwapChainDesc();
    auto back_buffers = display->GetBackBufferResouces();
    wrappedBackBuffers.resize(swap_chain_desc.BufferCount);
    d2dRenderTargets.resize(swap_chain_desc.BufferCount);
    for (int i = 0; i < swap_chain_desc.BufferCount; ++i)
    {
        D3D11_RESOURCE_FLAGS flags = {D3D11_BIND_RENDER_TARGET};
        hr = d3d11On12Device->CreateWrappedResource(
            back_buffers[i],
            &flags,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT,
            IID_PPV_ARGS(&wrappedBackBuffers[i])
        );
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create wrapped resource\n");
            return;
        }

        Microsoft::WRL::ComPtr<IDXGISurface> surface;
        hr = wrappedBackBuffers[i].As(&surface);
        if (FAILED(hr))
        {
            OutputDebugString("Failed to get IDXGISurface\n");
            return;
        }

        hr = d2dDeviceContext->CreateBitmapFromDxgiSurface(
            surface.Get(),
            nullptr,
            &d2dRenderTargets[i]
        );
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create D2D bitmap\n");
            return;
        }
    }

    // create d2d resources
    hr = d2dDeviceContext->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Red),
        &d2dBrush
    );
    if (FAILED(hr))
    {
        OutputDebugString("Failed to create D2D brush\n");
        return;
    }

    hr = dwriteFactory->CreateTextFormat(
        L"Arial",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        72.0f,
        L"en-US",
        &dwriteTextFormat
    );
    if (FAILED(hr))
    {
        OutputDebugString("Failed to create DWrite text format\n");
        return;
    }

    hr = dwriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    if (FAILED(hr))
    {
        OutputDebugString("Failed to set text alignment\n");
        return;
    }

    hr = dwriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    if (FAILED(hr))
    {
        OutputDebugString("Failed to set paragraph alignment\n");
        return;
    }
}

void Graphics::OnPress()
{
    if (weight == 0.0f)
    {
        isChangingScene = true;
    }
}

void Graphics::Render()
{
    AquaEngine::GlobalDescriptorHeapManager::SetToCommand(*command);

    if (!isChangingScene && weight == 0.0f)
    {
        display->BeginRender();

        display->SetViewports();
        skybox->Render(*command);

        display->EndRender();
    }

    if (!isChangingScene && weight == 1.0f)
    {
        display->BeginRender();

        pipelineState.SetToCommand(*command);
        rootSignature.SetToCommand(*command);
        display->SetViewports();
        camera->Render(*command, "texture");
        directionLight.Render(*command);
        model->Render(*command);
        model2->Render(*command);

        display->EndRender();
    }

    if (isChangingScene)
    {
        weight += 0.04f;

        if (weight >= 1.0f)
        {
            isChangingScene = false;
            weight = 1.0f;
        }

        weightBuffer.GetMappedBuffer()->weight = weight;

        // models->RotZ(0.01f);
        model2->RotY(-0.1f);

        model_rt.BeginRender(*command);

        pipelineState.SetToCommand(*command);
        rootSignature.SetToCommand(*command);
        display->SetViewports();
        camera->Render(*command, "texture");
        directionLight.Render(*command);
        model->Render(*command);
        model2->Render(*command);

        model_rt.EndRender(*command);

        cubemap_rt.BeginRender(*command);
        display->SetViewports();
        skybox->Render(*command);
        cubemap_rt.EndRender(*command);

        display->BeginRender();

        rt_pipelineState.SetToCommand(*command);
        rt_rootSignature.SetToCommand(*command);
        model_rt.UseAsTexture(*command);
        cubemap_rt.UseAsTexture(*command);
        weightCBV.SetGraphicsRootDescriptorTable(command.get());
        display->SetViewports();
        model_rt.Render(*command);

        display->EndRender();
    }

    HRESULT hr = command->Execute();
    if (FAILED(hr)) exit(-1);

    RenderD2D();

    display->Present();
}

void Graphics::RenderD2D()
{
    UINT index = display->GetCurrentBackBufferIndex();
    D2D1_SIZE_F size = d2dRenderTargets[index]->GetSize();
    D2D1_RECT_F textRect = D2D1::RectF(0, 0, size.width, size.height);

    d3d11On12Device->AcquireWrappedResources(
        wrappedBackBuffers[index].GetAddressOf(),
        1
    );

    static const wchar_t text[] = L"Press any button";

    d2dDeviceContext->SetTarget(d2dRenderTargets[index].Get());
    d2dDeviceContext->BeginDraw();
    d2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
    d2dDeviceContext->DrawTextA(
        text,
        wcslen(text),
        dwriteTextFormat.Get(),
        textRect,
        d2dBrush.Get()
    );
    HRESULT hr = d2dDeviceContext->EndDraw();
    if (FAILED(hr))
    {
        OutputDebugString("Failed to draw text\n");
        return;
    }

    d3d11On12Device->ReleaseWrappedResources(
        wrappedBackBuffers[index].GetAddressOf(),
        1
    );

    d3d11DeviceContext->Flush();
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
