#ifndef MESH_H
#define MESH_H
#include "directx/wrapper/Command.h"

namespace AquaEngine
{
    class Mesh
    {
    public:
        virtual ~Mesh() = default;

        virtual void Create() = 0;
        virtual void Draw(Command& command) const
        {
            command.List()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            command.List()->IASetVertexBuffers(0, 1, &m_vertexBufferView);
            command.List()->IASetIndexBuffer(&m_indexBufferView);
        }

        [[nodiscard]] D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const
        {
            return m_vertexBufferView;
        }

        [[nodiscard]] D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const
        {
            return m_indexBufferView;
        }

    protected:
        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
        D3D12_INDEX_BUFFER_VIEW m_indexBufferView{};
    };
}

#endif //MESH_H
