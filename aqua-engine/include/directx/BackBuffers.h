#ifndef AQUA_BACKBUFFERS_H
#define AQUA_BACKBUFFERS_H

#include <vector>

#include "directx/wrapper/SwapChain.h"
#include "directx/buffer/Buffer.h"
#include "directx/buffer/RenderTargetView.h"

namespace AquaEngine
{
    class BackBuffers
    {
    public:
        BackBuffers(HWND hwnd, RECT rc, Command &command)
            : m_hwnd(hwnd)
            , m_windowRect(rc)
            , m_swapChain(hwnd, rc, command)
            , m_command(&command)
        {
            HRESULT hr = CreateBackBuffers();
            if (FAILED(hr))
            {
                OutputDebugStringW(L"Failed to create back buffers\n");
                return;
            }

            CreateRenderTargetViews();
        }

        void Present() const
        {
            m_swapChain.Present();
        }

        void GetDesc(DXGI_SWAP_CHAIN_DESC1 *desc) const
        {
            m_swapChain.GetDesc(desc);
        }

        [[nodiscard]] D3D12_RESOURCE_DESC GetBackBufferDesc(UINT index = 0) const
        {
            return m_backBuffers[index]->GetDesc();
        }

        [[nodiscard]] UINT GetCurrentBackBufferIndex() const
        {
            return m_swapChain.GetCurrentBackBufferIndex();
        }

        [[nodiscard]] std::vector<ID3D12Resource *> GetBackBuffers() const
        {
            return m_backBuffers;
        }

        [[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle() const;

        void BeginRender() const;
        void EndRender() const;

    private:
        HWND m_hwnd;
        RECT m_windowRect;

        SwapChain m_swapChain;

        std::vector<ID3D12Resource*> m_backBuffers;
        std::vector<RenderTargetView> m_rtvs;

        HRESULT CreateBackBuffers();
        void CreateRenderTargetViews();

        Command* m_command;
    };
}


#endif //AQUA_BACKBUFFERS_H
