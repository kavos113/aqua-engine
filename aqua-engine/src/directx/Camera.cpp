#include "../../include/directx/Camera.h"

#include "directx/Util.h"

namespace AquaEngine {
    void Camera::Init(
        DirectX::XMFLOAT3 eye,
        DirectX::XMFLOAT3 focus,
        DirectX::XMFLOAT3 up,
        DescriptorHeapSegmentManager &model_heap,
        const D3D12_DESCRIPTOR_RANGE &matrix_range
    )
    {
        m_manager = &model_heap;

        SetMatrixBuffer(eye, focus, up, matrix_range);
    }

    // matrix buffer's range type : CBV (register b)
    void Camera::Init(
        DirectX::XMFLOAT3 eye,
        DirectX::XMFLOAT3 focus,
        DirectX::XMFLOAT3 up,
        DescriptorHeapSegmentManager &model_heap,
        unsigned int matrix_shader_register
    )
    {
        m_manager = &model_heap;

        D3D12_DESCRIPTOR_RANGE matrix_range = {
            .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
            .NumDescriptors = 1,
            .BaseShaderRegister = matrix_shader_register,
            .RegisterSpace = 0,
            .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
        };

        SetMatrixBuffer(eye, focus, up, matrix_range);
    }

    void Camera::Render(Command &command) const
    {
        m_matrixCBV.SetGraphicsRootDescriptorTable(&command);
    }

    void Camera::SetMatrixBuffer(
        DirectX::XMFLOAT3 eye,
        DirectX::XMFLOAT3 focus,
        DirectX::XMFLOAT3 up,
        const D3D12_DESCRIPTOR_RANGE &matrix_range
    )
    {
        m_view = DirectX::XMMatrixLookAtLH(
            XMLoadFloat3(&eye),
            XMLoadFloat3(&focus),
            XMLoadFloat3(&up)
            );
        m_projection = DirectX::XMMatrixPerspectiveFovLH(
            DirectX::XM_PIDIV2,
            static_cast<float>(wr.right - wr.left) / static_cast<float>(wr.bottom - wr.top),
            1.0f,
            10.0f
            );

        m_matrixBuffer.Create(BUFFER_DEFAULT(AlignmentSize(sizeof(CameraMatrix), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)));
        m_matrixBuffer.GetMappedBuffer()->view = m_view;
        m_matrixBuffer.GetMappedBuffer()->projection = m_projection;

        auto segment = std::make_shared<DescriptorHeapSegment>(m_manager->Allocate(1));
        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_SHADER_VISIBILITY_ALL,
            &matrix_range,
            1
        );

        m_matrixCBV.SetDescriptorHeapSegment(segment, 0);
        m_matrixCBV.Create(m_matrixBuffer);
    }
} // AquaEngine