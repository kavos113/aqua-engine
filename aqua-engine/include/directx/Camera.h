#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>
#include <unordered_map>

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
            DirectX::XMFLOAT3 up
        );

        void Render(Command &command, const std::string &manager_name);

        void AddManager(
            const std::string &manager_name,
            std::unique_ptr<D3D12_DESCRIPTOR_RANGE> matrix_range
        );

        explicit Camera(RECT wr)
            : m_view()
            , m_projection()
            , wr(wr)
        {
        }

    private:
        struct CameraMatrix
        {
            DirectX::XMMATRIX view;
            DirectX::XMMATRIX projection;
            DirectX::XMFLOAT3 eye;
        };

        void SetMatrixBuffer(
            DirectX::XMFLOAT3 eye,
            DirectX::XMFLOAT3 focus,
            DirectX::XMFLOAT3 up
        );

        DirectX::XMMATRIX m_view;
        DirectX::XMMATRIX m_projection;

        GPUBuffer<CameraMatrix> m_matrixBuffer;
        std::unordered_map<std::string, ConstantBufferView> m_matrixCBV{};

        RECT wr;

    };

} // AquaEngine

#endif //CAMERA_H
