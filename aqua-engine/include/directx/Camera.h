#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>

#include "buffer/ConstantBufferView.h"
#include "buffer/GPUBuffer.h"
#include "descriptor_heap/DescriptorHeapSegment.h"
#include "descriptor_heap/DescriptorHeapSegmentManager.h"

namespace AquaEngine {

    class Camera {
    public:
        void Init(
            DirectX::XMFLOAT3 eye,
            DirectX::XMFLOAT3 focus,
            DirectX::XMFLOAT3 up,
            DescriptorHeapSegmentManager &model_heap,
            const
            D3D12_DESCRIPTOR_RANGE &matrix_range
        );
        void Init(
            DirectX::XMFLOAT3 eye,
            DirectX::XMFLOAT3 focus,
            DirectX::XMFLOAT3 up,
            DescriptorHeapSegmentManager &model_heap,
            unsigned int matrix_shader_register
        );
        void Render(Command &command) const;

        explicit Camera(RECT wr)
            : m_view()
            , m_projection()
            , m_manager(nullptr)
            , wr(wr)
        {
        }

    private:
        struct CameraMatrix
        {
            DirectX::XMMATRIX view;
            DirectX::XMMATRIX projection;
        };

        void SetMatrixBuffer(
            DirectX::XMFLOAT3 eye,
            DirectX::XMFLOAT3 focus,
            DirectX::XMFLOAT3 up,
            const D3D12_DESCRIPTOR_RANGE &matrix_range
        );

        DirectX::XMMATRIX m_view;
        DirectX::XMMATRIX m_projection;

        GPUBuffer<CameraMatrix> m_matrixBuffer;
        ConstantBufferView m_matrixCBV;

        DescriptorHeapSegmentManager* m_manager;

        RECT wr;

    };

} // AquaEngine

#endif //CAMERA_H
