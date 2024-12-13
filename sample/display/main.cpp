#include <windows.h>
#include <tchar.h>

#include "directx/wrapper/Factory.h"
#include "directx/wrapper/Device.h"
#include "directx/wrapper/Command.h"
#include "directx/BackBuffers.h"
#include "directx/descriptor_heap/GlobalDescriptorHeapManager.h"

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
    Factory::Init(true);
    Device::GetAdaptors();
    Device::Init(1);
    GlobalDescriptorHeapManager::Init();
    Command command;
    BackBuffers backBuffers(hwnd, wr, command);
    
    ShowWindow(hwnd, nCmdShow);
    
    MSG msg = {};
    
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        backBuffers.BeginRender();
        
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = backBuffers.GetCurrentRTVHandle();
        command.List()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
        
        float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
        command.List()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        
        backBuffers.EndRender();
        
        command.Execute();
        
        backBuffers.Present();
    }
    
    Device::Shutdown();
    Factory::Shutdown();
    
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