#include "../../include/directx/MultiPassRenderer.h"

#include <array>

#include "../../include/directx/Util.h"
#include "../../include/directx/descriptor_heap/GlobalDescriptorHeapManager.h"
#include "../../include/directx/wrapper/Barrier.h"

namespace AquaEngine
{
    HRESULT MultiPassRenderer::Create(
        const D3D12_RESOURCE_DESC &desc,
        float width,
        float height
    )
    {
        HRESULT hr = CreateVertexBuffer();
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create vertex buffer\n");
            return hr;
        }

        hr = CreateRenderBuffer(desc);
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create render buffer\n");
            return hr;
        }

        hr = CreateRenderTargetView();
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create render target view\n");
            return hr;
        }

        hr = CreateDepthStencilView(width, height);
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create depth stencil view\n");
            return hr;
        }

        return S_OK;
    }

    void MultiPassRenderer::BeginRender(Command &command)
    {
        Barrier::Transition(
            &command,
            m_buffer.GetBuffer().Get(),
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );

        auto rtv = m_rtv.GetCPUHandle();
        auto dsv = m_dsv.GetCPUHandle();

        command.List()->OMSetRenderTargets(1, &rtv, false, &dsv);

        command.List()->ClearRenderTargetView(rtv, m_clearColor.data(), 0, nullptr);
        command.List()->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }

    void MultiPassRenderer::EndRender(Command &command)
    {
        Barrier::Transition(
            &command,
            m_buffer.GetBuffer().Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
    }

    void MultiPassRenderer::UseAsTexture(Command &command) const
    {
        m_srv.SetGraphicsRootDescriptorTable(&command);
    }

    HRESULT MultiPassRenderer::CreateVertexBuffer()
    {
        std::array vertices = {
            Vertex{DirectX::XMFLOAT3{-1.0f, -1.0f, 0.0f}, DirectX::XMFLOAT2{0.0f, 1.0f}},
            Vertex{DirectX::XMFLOAT3{-1.0f, 1.0f, 0.0f}, DirectX::XMFLOAT2{0.0f, 0.0f}},
            Vertex{DirectX::XMFLOAT3{1.0f, -1.0f, 0.0f}, DirectX::XMFLOAT2{1.0f, 1.0f}},
            Vertex{DirectX::XMFLOAT3{1.0f, 1.0f, 0.0f}, DirectX::XMFLOAT2{1.0f, 0.0f}},
        };
        HRESULT hr = m_vertexBuffer.Create(
            BUFFER_DEFAULT(AlignmentSize(vertices.size() * 4, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT))
        );
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create vertex buffer\n");
            return hr;
        }
        std::ranges::copy(vertices, m_vertexBuffer.GetMappedBuffer());
        m_vertexBuffer.Unmap();

        m_vertexBufferView.BufferLocation = m_vertexBuffer.GetResource()->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(Vertex);
        m_vertexBufferView.SizeInBytes = sizeof(Vertex) * vertices.size();

        return S_OK;
    }

    HRESULT MultiPassRenderer::CreateRenderBuffer(const D3D12_RESOURCE_DESC &desc)
    {
        D3D12_CLEAR_VALUE clear_value = {
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM
        };
        for (int i = 0; i < 4; ++i)
        {
            clear_value.Color[i] = m_clearColor[i];
        }

        HRESULT hr = m_buffer.Create(
            Buffer::HeapProperties::Default(),
            D3D12_HEAP_FLAG_NONE,
            desc,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            &clear_value
        );
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create render buffer\n");
            return hr;
        }

        return S_OK;
    }

    HRESULT MultiPassRenderer::CreateRenderTargetView()
    {
        auto rtv_manager = GlobalDescriptorHeapManager::GetCPUHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        auto rtv_segment = std::make_shared<DescriptorHeapSegment>(rtv_manager.Allocate(1));

        m_rtv.SetDescriptorHeapSegment(rtv_segment, 0);
        m_rtv.Create(m_buffer);

        return S_OK;
    }

    HRESULT MultiPassRenderer::CreateShaderResourceView(std::shared_ptr<DescriptorHeapSegment> segment, int offset)
    {
        m_srv.SetDescriptorHeapSegment(segment, offset);
        m_srv.Create(m_buffer);

        return S_OK;
    }

    HRESULT MultiPassRenderer::CreateDepthStencilView(float width, float height)
    {
        D3D12_CLEAR_VALUE clear_value = {
            .Format = DXGI_FORMAT_D32_FLOAT,
            .DepthStencil = {
                .Depth = 1.0f,
                .Stencil = 0
            }
        };

        HRESULT hr = m_depthBuffer.Create(
            Buffer::HeapProperties::Default(),
            D3D12_HEAP_FLAG_NONE,
            Buffer::ResourceDesc::DepthStencil(width, height),
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &clear_value
        );
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create depth buffer\n");
            return hr;
        }

        auto dsv_manager = GlobalDescriptorHeapManager::GetCPUHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        auto dsv_segment = std::make_shared<DescriptorHeapSegment>(dsv_manager.Allocate(1));

        m_dsv.SetDescriptorHeapSegment(dsv_segment, 0);
        m_dsv.Create(m_depthBuffer);

        return S_OK;
    }

    std::vector<D3D12_INPUT_ELEMENT_DESC> MultiPassRenderer::m_inputElementDescs = {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
        {
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            12,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
    };
} // AquaEngine
