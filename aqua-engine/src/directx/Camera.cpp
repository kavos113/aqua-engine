#include "../../include/directx/Camera.h"

#include "directx/Util.h"
#include "directx/descriptor_heap/GlobalDescriptorHeapManager.h"

namespace AquaEngine {
    void Camera::Init(
        DirectX::XMFLOAT3 eye,
        DirectX::XMFLOAT3 focus,
        DirectX::XMFLOAT3 up
    )
    {
        SetMatrixBuffer(eye, focus, up);

        m_focusDistance = DirectX::XMVector3Length(DirectX::XMVectorSubtract(XMLoadFloat3(&focus), XMLoadFloat3(&eye))).m128_f32[0];
    }

    void Camera::Render(Command &command, const std::string &manager_name)
    {
        m_matrixCBV[manager_name].SetGraphicsRootDescriptorTable(&command);
    }

    void Camera::SetMatrixBuffer(
        DirectX::XMFLOAT3 eye,
        DirectX::XMFLOAT3 focus,
        DirectX::XMFLOAT3 up
    )
    {
        m_eye = eye;
        m_focus = focus;
        m_up = up;

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
        m_matrixBuffer.GetMappedBuffer()->eye = m_eye;
    }

    void Camera::UpdateMatrixBuffer()
    {
        m_view = DirectX::XMMatrixLookAtLH(
            XMLoadFloat3(&m_eye),
            XMLoadFloat3(&m_focus),
            XMLoadFloat3(&m_up)
        );
        m_matrixBuffer.GetMappedBuffer()->view = m_view;
        m_matrixBuffer.GetMappedBuffer()->eye = m_eye;
    }

    void Camera::AddManager(const std::string &manager_name, std::unique_ptr<D3D12_DESCRIPTOR_RANGE> matrix_range)
    {
        auto &manager = GlobalDescriptorHeapManager::GetShaderHeapManager(
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            manager_name
        );

        auto segment = std::make_shared<DescriptorHeapSegment>(manager.Allocate(1));
        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_SHADER_VISIBILITY_ALL,
            std::move(matrix_range),
            1
        );

        m_matrixCBV[manager_name].SetDescriptorHeapSegment(segment, 0);
        m_matrixCBV[manager_name].Create(m_matrixBuffer);
    }
} // AquaEngine