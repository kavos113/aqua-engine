#ifndef MESH_H
#define MESH_H
#include <DirectXMath.h>
#include <vector>

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
        explicit Mesh(DescriptorHeapSegmentManager& manager)
            : m_manager(&manager)
        {

        }
        virtual ~Mesh() = default;

        virtual void Create() = 0;
        virtual void Render(Command& command) const
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
            m_matrixBuffer.Create(BUFFER_DEFAULT(AlignmentSize(sizeof(TransformMatrix), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)));
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

        // if use shared segment
        void CreateMatrixBuffer(
            const std::shared_ptr<DescriptorHeapSegment>& segment,
            const int offset = 0
            )
        {
            m_matrixBuffer.Create(BUFFER_DEFAULT(AlignmentSize(sizeof(TransformMatrix), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)));
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix;

            m_matrixCBV.SetDescriptorHeapSegment(segment, offset);
            m_matrixCBV.Create(m_matrixBuffer);
        }

        void RotX(float angle)
        {
            m_transformMatrix *= DirectX::XMMatrixRotationX(angle);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix * m_coordinateMatrix;
        }

        void RotY(float angle)
        {
            m_transformMatrix *= DirectX::XMMatrixRotationY(angle);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix * m_coordinateMatrix;
        }

        void RotZ(float angle)
        {
            m_transformMatrix *= DirectX::XMMatrixRotationZ(angle);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix * m_coordinateMatrix;
        }

        void SetRot(float x, float y, float z)
        {
            m_transformMatrix = DirectX::XMMatrixRotationRollPitchYaw(x, y, z);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix * m_coordinateMatrix;
        }

        void RotOriginX(float angle)
        {
            m_coordinateMatrix *= DirectX::XMMatrixRotationX(angle);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix * m_coordinateMatrix;
        }

        void RotOriginY(float angle)
        {
            m_coordinateMatrix *= DirectX::XMMatrixRotationY(angle);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix * m_coordinateMatrix;
        }

        void RotOriginZ(float angle)
        {
            m_coordinateMatrix *= DirectX::XMMatrixRotationZ(angle);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix * m_coordinateMatrix;
        }

        void SetRotOrigin(float x, float y, float z)
        {
            m_coordinateMatrix = DirectX::XMMatrixRotationRollPitchYaw(x, y, z);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix * m_coordinateMatrix;
        }

        void Move(float x, float y, float z)
        {
            m_coordinateMatrix *= DirectX::XMMatrixTranslation(x, y, z);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix * m_coordinateMatrix;
        }

        void SetPos(float x, float y, float z)
        {
            m_coordinateMatrix = DirectX::XMMatrixTranslation(x, y, z);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix * m_coordinateMatrix;
        }

        void Scale(float x, float y, float z)
        {
            m_transformMatrix *= DirectX::XMMatrixScaling(x, y, z);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix * m_coordinateMatrix;
        }

        void SetScale(float x, float y, float z)
        {
            m_transformMatrix = DirectX::XMMatrixScaling(x, y, z);
            m_matrixBuffer.GetMappedBuffer()->world = m_transformMatrix * m_coordinateMatrix;
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
        DirectX::XMMATRIX m_coordinateMatrix = DirectX::XMMatrixIdentity();
        DescriptorHeapSegmentManager* m_manager{};
        GPUBuffer<TransformMatrix> m_matrixBuffer;
        ConstantBufferView m_matrixCBV;

    };
}

#endif //MESH_H
