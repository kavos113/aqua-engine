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
            DirectX::XMFLOAT3 direction,
            DirectX::XMFLOAT3 color,
            DescriptorHeapSegmentManager &model_heap,
            const D3D12_DESCRIPTOR_RANGE &matrix_range
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
            DirectX::XMFLOAT3 direction,
            DirectX::XMFLOAT3 color,
            const D3D12_DESCRIPTOR_RANGE &matrix_range
        );
    };
}


#endif //DIRECTIONALLIGHT_H
