#ifndef MESH_H
#define MESH_H
#include <DirectXMath.h>
#include <vector>

#include "directx/TransformMatrix.h"
#include "directx/Util.h"
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
        Mesh() = default;

        virtual ~Mesh() = default;

        virtual void Create() = 0;
        virtual void Render(Command& command)
        {
            command.List()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            command.List()->IASetVertexBuffers(0, 1, &m_vertexBufferView);
            command.List()->IASetIndexBuffer(&m_indexBufferView);

            m_matrixBuffer.SetToCommand(command);
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

        void CreateMatrixBuffer(
            std::unique_ptr<D3D12_DESCRIPTOR_RANGE> matrix_range,
            DescriptorHeapSegmentManager &manager
        )
        {
            m_matrixBuffer.CreateMatrixBuffer(std::move(matrix_range), manager);
        }

        // if use shared segment
        void CreateMatrixBuffer(
            const std::shared_ptr<DescriptorHeapSegment>& segment,
            const int offset = 0
        )
        {
            m_matrixBuffer.CreateMatrixBuffer(segment, offset);
        }

        bool IsHit(const Mesh &mesh)
        {
            return DirectX::XMVectorGetX(
                       DirectX::XMVector3Length(DirectX::XMVectorSubtract(GetPos(), mesh.GetPos()))
                   ) < m_radius + mesh.m_radius;
        }

        [[nodiscard]] DirectX::XMVECTOR GetPos() const
        {
            DirectX::XMFLOAT3 pos = {
                m_coordinateMatrix.r[3].m128_f32[0],
                m_coordinateMatrix.r[3].m128_f32[1],
                m_coordinateMatrix.r[3].m128_f32[2]
            };
            return XMLoadFloat3(&pos);
        }

        void RotX(float angle)
        {
            m_transformMatrix *= DirectX::XMMatrixRotationX(angle);
            m_matrixBuffer.SetMatrixBuffer(m_transformMatrix * m_coordinateMatrix);
        }

        void RotY(float angle)
        {
            m_transformMatrix *= DirectX::XMMatrixRotationY(angle);
            m_matrixBuffer.SetMatrixBuffer(m_transformMatrix * m_coordinateMatrix);
        }

        void RotZ(float angle)
        {
            m_transformMatrix *= DirectX::XMMatrixRotationZ(angle);
            m_matrixBuffer.SetMatrixBuffer(m_transformMatrix * m_coordinateMatrix);
        }

        void SetRot(float x, float y, float z)
        {
            m_transformMatrix = DirectX::XMMatrixRotationRollPitchYaw(x, y, z);
            m_matrixBuffer.SetMatrixBuffer(m_transformMatrix * m_coordinateMatrix);
        }

        void RotOriginX(float angle)
        {
            m_coordinateMatrix *= DirectX::XMMatrixRotationX(angle);
            m_matrixBuffer.SetMatrixBuffer(m_transformMatrix * m_coordinateMatrix);
        }

        void RotOriginY(float angle)
        {
            m_coordinateMatrix *= DirectX::XMMatrixRotationY(angle);
            m_matrixBuffer.SetMatrixBuffer(m_transformMatrix * m_coordinateMatrix);
        }

        void RotOriginZ(float angle)
        {
            m_coordinateMatrix *= DirectX::XMMatrixRotationZ(angle);
            m_matrixBuffer.SetMatrixBuffer(m_transformMatrix * m_coordinateMatrix);
        }

        void SetRotOrigin(float x, float y, float z)
        {
            m_coordinateMatrix = DirectX::XMMatrixRotationRollPitchYaw(x, y, z);
            m_matrixBuffer.SetMatrixBuffer(m_transformMatrix * m_coordinateMatrix);
        }

        void Move(float x, float y, float z)
        {
            m_coordinateMatrix *= DirectX::XMMatrixTranslation(x, y, z);
            m_matrixBuffer.SetMatrixBuffer(m_transformMatrix * m_coordinateMatrix);
        }

        void SetPos(float x, float y, float z)
        {
            m_coordinateMatrix = DirectX::XMMatrixTranslation(x, y, z);
            m_matrixBuffer.SetMatrixBuffer(m_transformMatrix * m_coordinateMatrix);
        }

        void Scale(float x, float y, float z)
        {
            m_transformMatrix *= DirectX::XMMatrixScaling(x, y, z);
            m_matrixBuffer.SetMatrixBuffer(m_transformMatrix * m_coordinateMatrix);
        }

        void SetScale(float x, float y, float z)
        {
            m_transformMatrix = DirectX::XMMatrixScaling(x, y, z);
            m_matrixBuffer.SetMatrixBuffer(m_transformMatrix * m_coordinateMatrix);
        }

        [[nodiscard]] DirectX::XMMATRIX GetCoordinate() const
        {
            return m_coordinateMatrix;
        }


    protected:
        virtual void CreateIndexBuffer() = 0;

        virtual void CreateVertexBuffer() = 0;

        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
        D3D12_INDEX_BUFFER_VIEW m_indexBufferView{};

        static std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputElementDescs;
        
        DirectX::XMMATRIX m_transformMatrix = DirectX::XMMatrixIdentity();
        DirectX::XMMATRIX m_coordinateMatrix = DirectX::XMMatrixIdentity();

        TransformMatrix m_matrixBuffer;

        float m_radius = 0.0f;

    };
}

#endif //MESH_H
