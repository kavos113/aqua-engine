#ifndef MESH_H
#define MESH_H
#include <DirectXMath.h>
#include <vector>

#include "directx/buffer/ConstantBufferView.h"
#include "directx/buffer/GPUBuffer.h"
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

            if (m_matrixBuffer.GetBuffer() != nullptr)
            {
                m_matrixCBV.SetGraphicsRootDescriptorTable(&command);
            }
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

        void CreateMatrixBuffer(const D3D12_DESCRIPTOR_RANGE &matrix_range)
        {
            m_matrixBuffer.Create(BUFFER_DEFAULT(sizeof(TransformMatrix)));
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix;

            auto segment = std::make_shared<DescriptorHeapSegment>(m_manager->Allocate(1));

            m_matrixCBV.SetDescriptorHeapSegment(segment, 0);
            m_matrixCBV.Create(m_matrixBuffer);

            segment->SetRootParameter(
                D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
                D3D12_SHADER_VISIBILITY_VERTEX,
                &matrix_range,
                1
            );
        }

        void RotationX(float angle)
        {
            m_transformMatrix *= DirectX::XMMatrixRotationX(angle);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix;
        }

        void RotationY(float angle)
        {
            m_transformMatrix *= DirectX::XMMatrixRotationY(angle);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix;
        }

        void RotationZ(float angle)
        {
            m_transformMatrix *= DirectX::XMMatrixRotationZ(angle);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix;
        }

        void SetRotation(float x, float y, float z)
        {
            m_transformMatrix = DirectX::XMMatrixRotationRollPitchYaw(x, y, z);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix;
        }


    protected:
        struct TransformMatrix
        {
            DirectX::XMMATRIX world;
        };

        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
        D3D12_INDEX_BUFFER_VIEW m_indexBufferView{};

        static std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputElementDescs;
        
        DirectX::XMMATRIX m_transformMatrix = DirectX::XMMatrixIdentity();
        DescriptorHeapSegmentManager* m_manager{};
        GPUBuffer<TransformMatrix> m_matrixBuffer;
        ConstantBufferView m_matrixCBV;

    };
}

#endif //MESH_H
