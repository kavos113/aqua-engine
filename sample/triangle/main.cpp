#include <windows.h>
#include <tchar.h>

#include "directx/wrapper/Factory.h"
#include "directx/wrapper/Device.h"
#include "directx/wrapper/Command.h"
#include "directx/Display.h"
#include "directx/descriptor_heap/GlobalDescriptorHeapManager.h"
#include "directx/mesh/Triangle.h"
#include "directx/ShaderObject.h"
#include "directx/PipelineState.h"
#include "directx/RootSignature.h"

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
    Display display(hwnd, wr, command);
    
    Triangle triangle({0.0f, 0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {-0.5f, -0.5f, 0.0f});
    triangle.Create();
    
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        {
            .SemanticName = "POSITION",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        }
    };
    
    ShaderObject vs, ps;
    vs.CompileFromFile(L"sample/triangle/vs.hlsl", "VSMain", "vs_5_0");
    ps.CompileFromFile(L"sample/triangle/ps.hlsl", "PSMain", "ps_5_0");
    
    RootSignature rootSignature;
    rootSignature.Create();
    
    PipelineState pipelineState;
    pipelineState.SetRootSignature(&rootSignature);
    pipelineState.SetPixelShader(&ps);
    pipelineState.SetVertexShader(&vs);
    pipelineState.SetInputLayout(inputElementDescs, 1);
    pipelineState.Create();
    
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
        
        pipelineState.SetToCommand(command);
        rootSignature.SetToCommand(command);
        display.SetViewports();
        
        triangle.Draw(command);
        
        display.EndRender();
        
        command.Execute();
        
        display.Present();
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