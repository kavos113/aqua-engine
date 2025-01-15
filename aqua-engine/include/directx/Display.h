#ifndef AQUA_DISPLAY_H
#define AQUA_DISPLAY_H

#include <d3d12.h>

#include "buffer/DepthStencilView.h"
#include "directx/BackBuffers.h"

namespace AquaEngine
{
    class Display
    {
    public:
        Display(HWND hwnd, RECT rc, Command& command);

        void BeginRender() const;
        void EndRender() const;
        void Present();

        void SetViewports() const;

        [[nodiscard]] UINT GetCurrentBackBufferIndex() const
        {
            return m_backBuffers.GetCurrentBackBufferIndex();
        }

        // TODO zantei
        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferRTV() const
        {
            return m_backBuffers.GetCurrentRTVHandle();
        }

        [[nodiscard]] D3D12_RESOURCE_DESC GetBackBufferResourceDesc() const
        {
            return m_backBuffers.GetBackBufferDesc();
        }

        [[nodiscard]] DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const
        {
            DXGI_SWAP_CHAIN_DESC1 desc;
            m_backBuffers.GetDesc(&desc);
            return desc;
        }

        [[nodiscard]] std::vector<ID3D12Resource *> GetBackBufferResouces() const
        {
            return m_backBuffers.GetBackBuffers();
        }

        void SetBackgroundColor(float r, float g, float b, float a)
        {
            m_clearColor[0] = r;
            m_clearColor[1] = g;
            m_clearColor[2] = b;
            m_clearColor[3] = a;
        }
    private:
        BackBuffers m_backBuffers;
        Command* m_command;

        Buffer m_depthStencilBuffer;
        DepthStencilView m_dsv;

        D3D12_VIEWPORT m_viewport{};
        D3D12_RECT m_scissorRect{};
        RECT wr;

        void CreateViewport();
        void CreateDepthStencilBuffer();

        float m_clearColor[4] = {0.0f, 0.2f, 0.4f, 1.0f};
    };
}


#endif //AQUA_DISPLAY_H
