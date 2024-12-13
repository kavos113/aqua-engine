#ifndef AQUA_BACKBUFFERS_H
#define AQUA_BACKBUFFERS_H

#include <vector>

#include "directx/wrapper/SwapChain.h"
#include "directx/buffer/Buffer.h"
#include "directx/buffer/RenderTargetView.h"

class BackBuffers
{
public:
    BackBuffers(HWND hwnd, RECT rc, Command &command)
        : m_hwnd(hwnd)
        , m_windowRect(rc)
        , m_swapChain(hwnd, rc, command)
    {
    }
    
    HRESULT Create();
    
    void Present()
    {
        m_swapChain.Present();
    }
    
    void GetDesc(DXGI_SWAP_CHAIN_DESC1* desc)
    {
        m_swapChain.GetDesc(desc);
    }
    
    UINT GetCurrentBackBufferIndex()
    {
        return m_swapChain.GetCurrentBackBufferIndex();
    }
    
private:
    HWND m_hwnd;
    RECT m_windowRect;
    
    SwapChain m_swapChain;
    
    std::vector<Buffer> m_backBuffers;
    std::vector<RenderTargetView> m_rtvs;
    
    HRESULT CreateBackBuffers();
    void CreateRenderTargetViews();
};


#endif //AQUA_BACKBUFFERS_H
