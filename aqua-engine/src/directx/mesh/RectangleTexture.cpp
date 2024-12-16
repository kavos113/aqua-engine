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
        return;
    }

    void RectangleTexture::Draw(Command &command) const
    {
        Mesh::Draw(command);

        m_srv.SetGraphicsRootDescriptorTable(&command);
        command.List()->DrawIndexedInstanced(m_indices.size(), 1, 0, 0, 0);
    }

    void RectangleTexture::CreateVertexBuffer()
    {
        m_vertexBuffer.Create(BUFFER_DEFAULT(sizeof(Vertex) * 4));
        std::ranges::copy(m_vertices, m_vertexBuffer.GetMappedBuffer());
        m_vertexBuffer.Unmap();

        m_vertexBufferView.BufferLocation = m_vertexBuffer.GetBuffer()->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(Vertex);
        m_vertexBufferView.SizeInBytes = sizeof(Vertex) * 4;
    }

    void RectangleTexture::CreateIndexBuffer()
    {
        m_indexBuffer.Create(BUFFER_DEFAULT(sizeof(unsigned short) * 6));
        std::ranges::copy(m_indices, m_indexBuffer.GetMappedBuffer());
        m_indexBuffer.Unmap();

        m_indexBufferView.BufferLocation = m_indexBuffer.GetBuffer()->GetGPUVirtualAddress();
        m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
        m_indexBufferView.SizeInBytes = sizeof(unsigned short) * 6;
    }

    void RectangleTexture::CreateShaderResourceView(const D3D12_DESCRIPTOR_RANGE& texture_param)
    {
        auto segment = std::make_shared<DescriptorHeapSegment>(m_manager->Allocate(1));

        m_srv.SetDescriptorHeapSegment(segment, 0);
        m_srv.Create(m_texture);

        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_SHADER_VISIBILITY_PIXEL,
            &texture_param,
            1
        );
    }
}