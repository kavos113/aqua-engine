#include "directx/mesh/Triangle.h"

#include <ranges>

namespace AquaEngine
{
    void Triangle::Create()
    {
        CreateVertexBuffer();
        CreateIndexBuffer();
    }

    void Triangle::CreateVertexBuffer()
    {
        m_vertexBuffer.Create(BUFFER_DEFAULT(sizeof(DirectX::XMFLOAT3) * 3));
        std::ranges::copy(m_vertices, m_vertexBuffer.GetMappedBuffer());
        m_vertexBuffer.Unmap();

        m_vertexBufferView.BufferLocation = m_vertexBuffer.GetBuffer()->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(DirectX::XMFLOAT3);
        m_vertexBufferView.SizeInBytes = sizeof(DirectX::XMFLOAT3) * 3;
    }

    void Triangle::CreateIndexBuffer()
    {
        m_indexBuffer.Create(BUFFER_DEFAULT(sizeof(unsigned short) * 3));
        std::ranges::copy(m_indices, m_indexBuffer.GetMappedBuffer());
        m_indexBuffer.Unmap();

        m_indexBufferView.BufferLocation = m_indexBuffer.GetBuffer()->GetGPUVirtualAddress();
        m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
        m_indexBufferView.SizeInBytes = sizeof(unsigned short) * 3;
    }

}
