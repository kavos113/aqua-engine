#ifndef AQUA_SWAPCHAIN_H
#define AQUA_SWAPCHAIN_H


#include <dxgi1_6.h>

#include "directx/wrapper/Command.h"

class SwapChain
{
public:
    SwapChain(HWND hwnd, RECT rc, Command &command);
    ~SwapChain();
    
    void Present()
    {
        m_swapChain->Present(1, 0);
    }
    
    void GetDesc(DXGI_SWAP_CHAIN_DESC1* desc)
    {
        m_swapChain->GetDesc1(desc);
    }
    
    UINT GetCurrentBackBufferIndex()
    {
        return m_swapChain->GetCurrentBackBufferIndex();
    }
    
    [[nodiscard]] IDXGISwapChain4* Get() const { return m_swapChain; }
private:
    IDXGISwapChain4* m_swapChain{};
    RECT m_windowRect;
    HWND m_hwnd;
    
    Command& m_command;
    
    HRESULT CreateSwapChain();
};


#endif //AQUA_SWAPCHAIN_H