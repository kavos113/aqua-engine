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
        return 0;
    }
    
    // d3d init
    AquaEngine::Factory::Init(true);
    AquaEngine::Device::GetAdaptors();
    AquaEngine::Device::Init(1);
    AquaEngine::GlobalDescriptorHeapManager::Init();
    AquaEngine::Command command;
    AquaEngine::Display display(hwnd, wr, command);
    
    ShowWindow(hwnd, nCmdShow);
    
    MSG msg = {};
    
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        display.BeginRender();
        
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = display.GetBackBufferRTV();
        command.List()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
        
        float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
        command.List()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        
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