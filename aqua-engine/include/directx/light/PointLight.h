#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include <DirectXMath.h>

#include "directx/TransformMatrix.h"
#include "directx/buffer/ConstantBufferView.h"
#include "directx/buffer/GPUBuffer.h"

namespace AquaEngine
{
    class PointLight
    {
    public:
        PointLight()
            : m_light()
        {
        }

        void Init(
            DirectX::XMFLOAT3 position,
            DirectX::XMFLOAT3 color,
            float range,
            DescriptorHeapSegmentManager &model_heap,
            std::unique_ptr<D3D12_DESCRIPTOR_RANGE> matrix_range
        );

        void Render(Command &command) const;

        void CreateMatrixBuffer(
            std::unique_ptr<D3D12_DESCRIPTOR_RANGE> matrix_range,
            DescriptorHeapSegmentManager &manager
        )
        {
            m_matrix.CreateMatrixBuffer(std::move(matrix_range), manager);
        }

        void CreateMatrixBuffer(
            const std::shared_ptr<DescriptorHeapSegment> &segment,
            const int offset = 0
        )
        {
            m_matrix.CreateMatrixBuffer(segment, offset);
        }

    private:
        struct Light
        {
            DirectX::XMFLOAT3 position;
            float padding;
            DirectX::XMFLOAT3 color;
            float range;
        };

        Light m_light;

        TransformMatrix m_matrix{};

        GPUBuffer<Light> m_buffer{};
        ConstantBufferView m_view{};

        DescriptorHeapSegmentManager *m_manager{};

        void SetBuffer(
            const DirectX::XMFLOAT3 &position,
            const DirectX::XMFLOAT3 &color,
            float range,
            std::unique_ptr<D3D12_DESCRIPTOR_RANGE> light_range
        );
    };
} // AquaEngine

#endif //POINTLIGHT_H
