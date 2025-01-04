#include "../../../include/directx/mesh/RectangleTexture.h"

#include <memory>

namespace AquaEngine
{
    std::vector<D3D12_INPUT_ELEMENT_DESC> Mesh<RectangleTexture>::m_inputElementDescs = {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
        {
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        }
    };

    void RectangleTexture::Create()
    {
        CreateVertexBuffer();
        CreateIndexBuffer();
    }

    void RectangleTexture::Render(Command &command)
    {
        Mesh::Render(command);

        m_srv.SetGraphicsRootDescriptorTable(&command);
        command.List()->DrawIndexedInstanced(m_indices.size(), 1, 0, 0, 0);
    }

    void RectangleTexture::CreateVertexBuffer()
    {
        m_vertexBuffer.Create(BUFFER_DEFAULT(sizeof(Vertex) * 4));
        std::ranges::copy(m_vertices, m_vertexBuffer.GetMappedBuffer());
        m_vertexBuffer.Unmap();

        m_vertexBufferView.BufferLocation = m_vertexBuffer.GetResource()->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(Vertex);
        m_vertexBufferView.SizeInBytes = sizeof(Vertex) * 4;
    }

    void RectangleTexture::CreateIndexBuffer()
    {
        m_indexBuffer.Create(BUFFER_DEFAULT(sizeof(unsigned short) * 6));
        std::ranges::copy(m_indices, m_indexBuffer.GetMappedBuffer());
        m_indexBuffer.Unmap();

        m_indexBufferView.BufferLocation = m_indexBuffer.GetResource()->GetGPUVirtualAddress();
        m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
        m_indexBufferView.SizeInBytes = sizeof(unsigned short) * 6;
    }

    void RectangleTexture::CreateShaderResourceView(std::unique_ptr<D3D12_DESCRIPTOR_RANGE> texture_range, DescriptorHeapSegmentManager &manager)
    {
        auto segment = std::make_shared<DescriptorHeapSegment>(manager.Allocate(1));

        m_srv.SetDescriptorHeapSegment(segment, 0);
        m_srv.Create(m_texture);

        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_SHADER_VISIBILITY_PIXEL,
            std::move(texture_range),
            1
        );
    }
}
