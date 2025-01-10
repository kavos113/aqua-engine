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
            , m_focusDistance(0)
            , wr(wr)
        {
        }

        void RotX(float angle)
        {
            Rotate(angle, 0.0f, 0.0f);
        }

        // 水平回転
        void RotY(float angle)
        {
            Rotate(0.0f, angle, 0.0f);
        }

        void RotZ(float angle)
        {
            Rotate(0.0f, 0.0f, angle);
        }

        void Rotate(float x = 0.0f, float y = 0.0f, float z = 0.0f)
        {
            DirectX::XMMATRIX rot = DirectX::XMMatrixRotationRollPitchYaw(x, y, z);
            DirectX::XMVECTOR focus_new = XMVector3TransformCoord(
                DirectX::XMVectorSubtract(XMLoadFloat3(&m_focus), XMLoadFloat3(&m_eye)),
                rot
            );
            m_focus = DirectX::XMFLOAT3(
                m_eye.x + DirectX::XMVectorGetX(focus_new),
                m_eye.y + DirectX::XMVectorGetY(focus_new),
                m_eye.z + DirectX::XMVectorGetZ(focus_new)
            );

            DirectX::XMVECTOR up_new = XMVector3TransformCoord(
                XMLoadFloat3(&m_up),
                rot
            );
            m_up = DirectX::XMFLOAT3(
                DirectX::XMVectorGetX(up_new),
                DirectX::XMVectorGetY(up_new),
                DirectX::XMVectorGetZ(up_new)
            );
            UpdateMatrixBuffer();
        }

        DirectX::XMMATRIX GetView() const
        {
            return m_view;
        }

        DirectX::XMMATRIX GetProjection() const
        {
            return m_projection;
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

        void UpdateMatrixBuffer();

        DirectX::XMMATRIX m_view;
        DirectX::XMMATRIX m_projection;

        DirectX::XMFLOAT3 m_eye{};
        DirectX::XMFLOAT3 m_focus{};
        DirectX::XMFLOAT3 m_up{};

        GPUBuffer<CameraMatrix> m_matrixBuffer;
        std::unordered_map<std::string, ConstantBufferView> m_matrixCBV{};

        float m_focusDistance;

        RECT wr;

    };

} // AquaEngine

#endif //CAMERA_H
