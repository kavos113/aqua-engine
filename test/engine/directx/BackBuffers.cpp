#include <gtest/gtest.h>
#include <tchar.h>

#include "AquaEngine.h"

using namespace AquaEngine;

class BackBuffersTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Factory::Init(true);
        Device::GetAdaptors();
        Device::Init(0);
        GlobalDescriptorHeapManager::Init();
        command = new Command();
        
        WNDCLASSEX wc = {};
        
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = DefWindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.lpszClassName = _T("WindowClass");
        
        RegisterClassEx(&wc);
        
        hwnd = CreateWindowEx(
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
            nullptr,
            nullptr
        );
    }

    void TearDown() override
    {
        delete command;
        GlobalDescriptorHeapManager::Shutdown();
        Device::Shutdown();
        Factory::Shutdown();
        
        DestroyWindow(hwnd);
        UnregisterClass(_T("WindowClass"), GetModuleHandle(nullptr));
    }

    RECT wr = {0, 0, 800, 600};
    HWND hwnd;
    
    Command* command;
};

TEST_F(BackBuffersTest, Create)
{
    BackBuffers backBuffers(hwnd, wr, *command);
    
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    backBuffers.GetDesc(&swapChainDesc);
    
    ASSERT_NE(swapChainDesc.Width, 0);
}

TEST_F(BackBuffersTest, BeginRender)
{
    BackBuffers backBuffers(hwnd, wr, *command);
    backBuffers.BeginRender();
    
    HRESULT hr = command->Execute();
    if (FAILED(hr))
    {
        std::cout << std::hex << hr << std::endl;
    }
    ASSERT_EQ(hr, S_OK);
}

TEST_F(BackBuffersTest, EndRender)
{
    BackBuffers backBuffers(hwnd, wr, *command);
    backBuffers.BeginRender();
    backBuffers.EndRender();
    
    HRESULT hr = command->Execute();
    if (FAILED(hr))
    {
        std::cout << std::hex << hr << std::endl;
    }
    ASSERT_EQ(hr, S_OK);
}

TEST_F(BackBuffersTest, Present)
{
    BackBuffers backBuffers(hwnd, wr, *command);
    backBuffers.Present();
}