#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H
#include <DirectXMath.h>

#include "buffer/ConstantBufferView.h"
#include "buffer/GPUBuffer.h"

namespace AquaEngine
{
    class DirectionLight
    {
    public:
        void Init(
            const DirectX::XMFLOAT3 &direction,
            const DirectX::XMFLOAT3 &color,
            DescriptorHeapSegmentManager &model_heap,
            std::unique_ptr<D3D12_DESCRIPTOR_RANGE> matrix_range
        );

        void Render(Command &command) const;

        DirectionLight()
            : m_light()
            , m_manager(nullptr)
        {
        }

    private:
        struct Light
        {
            DirectX::XMFLOAT3 direction;
            float padding;
            DirectX::XMFLOAT3 color;
        };

        Light m_light;

        GPUBuffer<Light> m_buffer;
        ConstantBufferView m_view;

        DescriptorHeapSegmentManager *m_manager;

        void SetBuffer(
            const DirectX::XMFLOAT3 &direction,
            const DirectX::XMFLOAT3 &color,
            std::unique_ptr<D3D12_DESCRIPTOR_RANGE> matrix_range
        );
    };
}


#endif //DIRECTIONALLIGHT_H
