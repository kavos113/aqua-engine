#include <gtest/gtest.h>
#include <tchar.h>

#include "AquaEngine.h"

using namespace AquaEngine;

class DisplayTest : public ::testing::Test
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
        UnregisterClass("WindowClass", GetModuleHandle(nullptr));
    }
    
    RECT wr = {0, 0, 800, 600};
    HWND hwnd;
    
    Command* command;
};

TEST_F(DisplayTest, CreateViewport)
{
    Display display(hwnd, wr, *command);
    
    ASSERT_NE(display.GetBackBufferRTV().ptr, 0);
}

TEST_F(DisplayTest, SetViewports)
{
    Display display(hwnd, wr, *command);
    display.SetViewports();
    
    HRESULT hr = command->Execute();
    if (FAILED(hr))
    {
        std::cout << std::hex << hr << std::endl;
    }
    ASSERT_EQ(hr, S_OK);
}

TEST_F(DisplayTest, BeginRender)
{
    Display display(hwnd, wr, *command);
    display.BeginRender();
    
    HRESULT hr = command->Execute();
    if (FAILED(hr))
    {
        std::cout << std::hex << hr << std::endl;
    }
    ASSERT_EQ(hr, S_OK);
}

TEST_F(DisplayTest, EndRender)
{
    Display display(hwnd, wr, *command);
    display.EndRender();
    
    HRESULT hr = command->Execute();
    if (FAILED(hr))
    {
        std::cout << std::hex << hr << std::endl;
    }
    ASSERT_EQ(hr, S_OK);
}

TEST_F(DisplayTest, Present)
{
    Display display(hwnd, wr, *command);
    display.Present();
}