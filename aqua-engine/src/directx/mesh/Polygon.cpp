#include "directx/mesh/Polygon.h"

void Polygon::Draw(Command &command) const
{
    command.List()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    command.List()->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    command.List()->IASetIndexBuffer(&m_indexBufferView);
    
    command.List()->DrawIndexedInstanced(m_indices.size(), 1, 0, 0, 0);
}

D3D12_VERTEX_BUFFER_VIEW Polygon::GetVertexBufferView() const
{
    return m_vertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW Polygon::GetIndexBufferView() const
{
    return m_indexBufferView;
}
