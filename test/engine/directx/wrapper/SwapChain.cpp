#include <gtest/gtest.h>

#include "directx/wrapper/SwapChain.h"
#include "directx/wrapper/Command.h"
#include "directx/wrapper/Factory.h"
#include "directx/wrapper/Device.h"

#include <windows.h>

using namespace AquaEngine;

class SwapChainTest : public ::testing::Test
{
protected:
    SwapChainTest()
    {
        Factory::Init(true);
        Device::GetAdaptors();
        Device::Init(0);
        m_command = new Command();
        m_hwnd = CreateWindowW(L"STATIC", L"Test", WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, nullptr, nullptr, nullptr, nullptr);
    }

    ~SwapChainTest() override
    {
        delete m_command;
        DestroyWindow(m_hwnd);
        Device::Shutdown();
        Factory::Shutdown();
    }

    Command* m_command;
    HWND m_hwnd;
    RECT m_rc = { 0, 0, 800, 600 };
};

TEST_F(SwapChainTest, CreateSwapChain)
{
    SwapChain swapChain(m_hwnd, m_rc, *m_command);
    ASSERT_NE(swapChain.Get(), nullptr);
}