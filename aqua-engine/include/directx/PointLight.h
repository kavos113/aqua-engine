#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include <DirectXMath.h>

#include "buffer/ConstantBufferView.h"
#include "buffer/GPUBuffer.h"

namespace AquaEngine
{
    class PointLight
    {
    public:
        PointLight()
            : m_light()
            , m_manager(nullptr)
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

    private:
        struct Light
        {
            DirectX::XMFLOAT3 position;
            float padding;
            DirectX::XMFLOAT3 color;
            float range;
        };

        struct TransformMatrix
        {
            DirectX::XMMATRIX world;
        };

        Light m_light;

        GPUBuffer<Light> m_buffer;
        ConstantBufferView m_view;

        DescriptorHeapSegmentManager *m_manager;

        void SetBuffer(
            const DirectX::XMFLOAT3 &position,
            const DirectX::XMFLOAT3 &color,
            float range,
            std::unique_ptr<D3D12_DESCRIPTOR_RANGE> matrix_range
        );
    };
} // AquaEngine

#endif //POINTLIGHT_H
