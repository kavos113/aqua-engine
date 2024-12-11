#include <gtest/gtest.h>

#include "directx/wrapper/SwapChain.h"
#include "directx/wrapper/Command.h"
#include "directx/wrapper/Factory.h"
#include "directx/wrapper/Device.h"

#include <windows.h>

class SwapChainTest : public ::testing::Test
{
protected:
    SwapChainTest()
    {
        Factory::Init(true);
        Device::GetAdaptors();
        Device::Init(0);
        m_command = new Command();
    }

    ~SwapChainTest() override
    {
        delete m_command;
    }

    Command* m_command;
    HWND m_hwnd = (HWND)0x1234;
    RECT m_rc = { 0, 0, 800, 600 };
};

TEST_F(SwapChainTest, CreateSwapChain)
{
    SwapChain swapChain(m_hwnd, m_rc, *m_command);
    ASSERT_NE(swapChain.Get(), nullptr);
}