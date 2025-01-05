#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <windowsx.h>

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

    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        OutputDebugString("failed to initialize COM\n");
        return -1;
    }

    AquaEngine::Factory::Init(true);
    AquaEngine::Device::GetAdaptors();
    AquaEngine::Device::Init(0);
    AquaEngine::GlobalDescriptorHeapManager::Init();

    AquaEngine::Command command;
    AquaEngine::Display display(hwnd, wr, command);

    auto camera = std::make_shared<AquaEngine::Camera>(wr);
    camera->Init(
        {0.0f, 0.0f, -2.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    );

    auto& skybox_manager = AquaEngine::GlobalDescriptorHeapManager::CreateShaderManager(
        "skybox",
        10,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
    );
    AquaEngine::SkyBox skyBox(
        "sample1.hdr",
        command,
        skybox_manager
    );
    auto world_range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        1,
        1,
        0,
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    );
    skyBox.CreateMatrixBuffer(std::move(world_range), skybox_manager);
    skyBox.Create();
    skyBox.SetCamera(camera);
    skyBox.ConvertHDRIToCubeMap(command);
    skyBox.CreateCubeMapPipelineState();
    skyBox.Scale(1000.0f, 1000.0f, 1000.0f);

    ShowWindow(hwnd, nCmdShow);

    float mouseX = 0.0f;
    float mouseY = 0.0f;

    MSG msg = {};

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        switch (msg.message)
        {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            AquaEngine::GlobalDescriptorHeapManager::SetToCommand(command);

            display.BeginRender();
            display.SetViewports();
            skyBox.Render(command);
            display.EndRender();
            HRESULT hr = command.Execute();
            if (FAILED(hr))
            {
                OutputDebugString("Failed to execute command\n");
                return -1;
            }

            display.Present();

            EndPaint(hwnd, &ps);
            break;
        }

        case WM_MOUSEMOVE:
        {
            if (mouseX == 0.0f && mouseY == 0.0f)
            {
                mouseX = GET_X_LPARAM(msg.lParam);
                mouseY = GET_Y_LPARAM(msg.lParam);
                break;
            }

            float x = GET_X_LPARAM(msg.lParam);
            float y = GET_Y_LPARAM(msg.lParam);

            float dx = x - mouseX;
            float dy = y - mouseY;

            mouseX = x;
            mouseY = y;

            camera->Rotate(dy * -0.005f, dx * -0.005f);
            InvalidateRect(hwnd, &wr, false);
            break;
        }
        }
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
