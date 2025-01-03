#ifndef MULTIPASSRENDERER_H
#define MULTIPASSRENDERER_H
#include <DirectXMath.h>
#include <array>

#include "buffer/Buffer.h"
#include "buffer/DepthStencilView.h"
#include "buffer/GPUBuffer.h"
#include "buffer/RenderTargetView.h"
#include "buffer/ShaderResourceView.h"

namespace AquaEngine
{
    class MultiPassRenderer
    {
    public:
        MultiPassRenderer()
            : m_vertexBufferView()
        {
        }

        HRESULT Create(
            const D3D12_RESOURCE_DESC &desc,
            float width,
            float height
        );

        void BeginRender(Command &command);

        void EndRender(Command &command);

        void UseAsTexture(Command &command) const;

        HRESULT CreateShaderResourceView(std::shared_ptr<DescriptorHeapSegment> segment, int offset = 0);

        void SetBackgroundColor(float r, float g, float b, float a)
        {
            m_clearColor[0] = r;
            m_clearColor[1] = g;
            m_clearColor[2] = b;
            m_clearColor[3] = a;
        }

        static std::vector<D3D12_INPUT_ELEMENT_DESC> GetInputElementDescs()
        {
            return m_inputElementDescs;
        }

    private:
        HRESULT CreateVertexBuffer();

        HRESULT CreateRenderBuffer(const D3D12_RESOURCE_DESC &desc);

        HRESULT CreateRenderTargetView();


        HRESULT CreateDepthStencilView(float width, float height);

        struct Vertex
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT2 uv;
        };

        Buffer m_buffer;
        Buffer m_depthBuffer;
        RenderTargetView m_rtv;
        ShaderResourceView m_srv;
        DepthStencilView m_dsv;

        GPUBuffer<Vertex> m_vertexBuffer;
        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;


        std::array<float, 4> m_clearColor = {1.0f, 1.0f, 1.0f, 1.0f};

        static std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputElementDescs;
    };
} // AquaEngine

#endif //MULTIPASSRENDERER_H
