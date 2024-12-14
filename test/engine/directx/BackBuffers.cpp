#include <gtest/gtest.h>
#include <tchar.h>

#include "AquaEngine.h"

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
    }

    void TearDown() override
    {
        delete command;
        Device::Shutdown();
        Factory::Shutdown();
    }

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
        GetModuleHandle(nullptr),
        nullptr
    );
    
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
    
    command->Execute();
}

TEST_F(BackBuffersTest, EndRender)
{
    BackBuffers backBuffers(hwnd, wr, *command);
    backBuffers.EndRender();
    
    command->Execute();
}

TEST_F(BackBuffersTest, Present)
{
    BackBuffers backBuffers(hwnd, wr, *command);
    backBuffers.Present();
}