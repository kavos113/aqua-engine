#include "directx/wrapper/SwapChain.h"
#include "directx/wrapper/Factory.h"

namespace AquaEngine
{
    SwapChain::SwapChain(HWND hwnd, RECT rc, Command &command)
        : m_windowRect(rc)
        , m_hwnd(hwnd)
        , m_command(&command)
    {
        CreateSwapChain();
    }

    SwapChain::~SwapChain() = default;

    HRESULT SwapChain::CreateSwapChain()
    {
        DXGI_SWAP_CHAIN_DESC1 desc;

        desc.Width = m_windowRect.right - m_windowRect.left;
        desc.Height = m_windowRect.bottom - m_windowRect.top;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Stereo = FALSE;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = 2;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        HRESULT hr = Factory::Get()->CreateSwapChainForHwnd(
            m_command->Queue().Get(),
            m_hwnd,
            &desc,
            nullptr,
            nullptr,
            reinterpret_cast<IDXGISwapChain1**>(m_swapChain.GetAddressOf())
        );
        if (FAILED(hr))
        {
            OutputDebugStringW(L"Failed to create swap chain\n");
            return hr;
        }

        return S_OK;
    }

}
