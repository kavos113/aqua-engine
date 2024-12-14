#include <gtest/gtest.h>
#include <tchar.h>

#include "AquaEngine.h"

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
        nullptr,
        nullptr
    );
    
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
    
    command->Execute();
}

TEST_F(DisplayTest, BeginRender)
{
    Display display(hwnd, wr, *command);
    display.BeginRender();
    
    command->Execute();
}

TEST_F(DisplayTest, EndRender)
{
    Display display(hwnd, wr, *command);
    display.EndRender();
    
    command->Execute();
}

TEST_F(DisplayTest, Present)
{
    Display display(hwnd, wr, *command);
    display.Present();
}