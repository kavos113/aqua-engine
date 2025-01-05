#include <windows.h>
#include <tchar.h>

#include "AquaEngine.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    WNDCLASSEX wc = {};

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("WindowClass");

    RegisterClassEx(&wc);

    RECT wr = {0, 0, 800, 600};

    HWND hwnd = CreateWindowEx(
        0,
        _T("WindowClass"),
        _T("Hello, Engine!"),
        WS_OVERLAPPEDWINDOW,
        wr.left,
        wr.top,
        wr.right - wr.left,
        wr.bottom - wr.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
    if (hwnd == nullptr)
    {
        return -1;
    }

    // d3d init
    AquaEngine::Factory::Init(true);
    AquaEngine::Device::GetAdaptors();
    AquaEngine::Device::Init(0);
    AquaEngine::GlobalDescriptorHeapManager::Init();

    AquaEngine::Command command;
    AquaEngine::Display display(hwnd, wr, command);

    AquaEngine::Triangle triangle(
        {0.0f, 0.5f, 0.0f},
        {0.5f, -0.5f, 0.0f},
        {-0.5f, -0.5f, 0.0f}
        );
    triangle.Create();

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs = AquaEngine::Triangle::GetInputElementDescs();

    AquaEngine::ShaderObject vs, ps;
    vs.Load(L"vs.hlsl", "vsMain", "vs_5_0");
    ps.Load(L"ps.hlsl", "psMain", "ps_5_0");

    AquaEngine::RootSignature rootSignature;
    rootSignature.Create();

    AquaEngine::PipelineState pipelineState;
    pipelineState.SetRootSignature(&rootSignature);
    pipelineState.SetPixelShader(&ps);
    pipelineState.SetVertexShader(&vs);
    pipelineState.SetInputLayout(inputElementDescs.data(), inputElementDescs.size());
    pipelineState.Create();

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        AquaEngine::GlobalDescriptorHeapManager::SetToCommand(command);

        display.BeginRender();

        pipelineState.SetToCommand(command);
        rootSignature.SetToCommand(command);
        display.SetViewports();

        triangle.Render(command);

        display.EndRender();

        command.Execute();

        display.Present();
    }

    AquaEngine::GlobalDescriptorHeapManager::Shutdown();
    AquaEngine::Device::Shutdown();
    AquaEngine::Factory::Shutdown();

    UnregisterClass(_T("WindowClass"), wc.hInstance);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}