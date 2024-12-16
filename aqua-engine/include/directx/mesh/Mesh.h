#ifndef MESH_H
#define MESH_H
#include <DirectXMath.h>
#include <vector>

#include "directx/descriptor_heap/DescriptorHeapSegmentManager.h"
#include "directx/wrapper/Command.h"

namespace AquaEngine
{
    template<class DerivedClass>
    class Mesh
    {
    public:
        Mesh(DescriptorHeapSegmentManager& manager)
            : m_manager(&manager)
        {

        }
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

        [[nodiscard]] static std::vector<D3D12_INPUT_ELEMENT_DESC> GetInputElementDescs()
        {
            return m_inputElementDescs;
        }

    protected:
        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
        D3D12_INDEX_BUFFER_VIEW m_indexBufferView{};

        static std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputElementDescs;
        
        DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();

        DescriptorHeapSegmentManager* m_manager{};
    };
}

#endif //MESH_H
