#include "directx/Display.h"

#include <memory>

#include "directx/descriptor_heap/GlobalDescriptorHeapManager.h"

namespace AquaEngine
{
    Display::Display(HWND hwnd, RECT rc, Command &command)
        : m_backBuffers(hwnd, rc, command)
        , m_command(&command)
        , wr(rc)
    {
        CreateViewport();
        CreateDepthStencilBuffer();
    }

    void Display::BeginRender() const
    {
        m_backBuffers.BeginRender();

        D3D12_CPU_DESCRIPTOR_HANDLE rtv = m_backBuffers.GetCurrentRTVHandle();
        D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_dsv.GetCPUHandle();
        m_command->List()->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

        m_command->List()->ClearRenderTargetView(rtv, m_clearColor, 0, nullptr);
        m_command->List()->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }

    void Display::EndRender() const
    {
        m_backBuffers.EndRender();
    }

    void Display::Present()
    {
        m_backBuffers.Present();
    }

    void Display::SetViewports() const
    {
        m_command->List()->RSSetViewports(1, &m_viewport);
        m_command->List()->RSSetScissorRects(1, &m_scissorRect);
    }

    void Display::CreateViewport()
    {
        m_viewport.Width = static_cast<float>(wr.right - wr.left);
        m_viewport.Height = static_cast<float>(wr.bottom - wr.top);
        m_viewport.TopLeftX = 0;
        m_viewport.TopLeftY = 0;
        m_viewport.MinDepth = 0.0f;
        m_viewport.MaxDepth = 1.0f;

        m_scissorRect.left = 0;
        m_scissorRect.top = 0;
        m_scissorRect.right = m_scissorRect.left + (wr.right - wr.left);
        m_scissorRect.bottom = m_scissorRect.top + (wr.bottom - wr.top);
    }

    void Display::CreateDepthStencilBuffer()
    {
        D3D12_CLEAR_VALUE clearValue = {};

        clearValue.Format = DXGI_FORMAT_D32_FLOAT;
        clearValue.DepthStencil.Depth = 1.0f;

        HRESULT hr = m_depthStencilBuffer.Create(
            Buffer::HeapProperties::Default(),
            D3D12_HEAP_FLAG_NONE,
            Buffer::ResourceDesc::DepthStencil(wr.right - wr.left, wr.bottom - wr.top),
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &clearValue
        );
        if (FAILED(hr))
        {
            OutputDebugStringW(L"Failed to create depth stencil buffer\n");
            return;
        }

        auto& manager = GlobalDescriptorHeapManager::GetCPUHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        auto segment = std::make_shared<DescriptorHeapSegment>(manager.Allocate(1));

        m_dsv.SetDescriptorHeapSegment(segment, 0);
        m_dsv.Create(m_depthStencilBuffer);
    }
}
