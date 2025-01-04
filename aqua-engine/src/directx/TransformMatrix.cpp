#include "../../include/directx/TransformMatrix.h"

#include "../../include/directx/Util.h"

namespace AquaEngine
{
    void TransformMatrix::CreateMatrixBuffer(
        std::unique_ptr<D3D12_DESCRIPTOR_RANGE> matrix_range,
        DescriptorHeapSegmentManager &manager
    )
    {
        HRESULT hr = m_matrixBuffer.Create(
            BUFFER_DEFAULT(AlignmentSize(sizeof(TransformMatrix), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT))
        );
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create matrix buffer\n");
            return;
        }

        auto segment = std::make_shared<DescriptorHeapSegment>(manager.Allocate(1));

        m_matrixCBV.SetDescriptorHeapSegment(segment, 0);
        m_matrixCBV.Create(m_matrixBuffer.GetBuffer());

        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_SHADER_VISIBILITY_VERTEX,
            std::move(matrix_range),
            1
        );
    }

    void TransformMatrix::CreateMatrixBuffer(
        const std::shared_ptr<DescriptorHeapSegment> &segment,
        const int offset
    )
    {
        HRESULT hr = m_matrixBuffer.Create(
            BUFFER_DEFAULT(AlignmentSize(sizeof(TransformMatrix), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT))
        );
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create matrix buffer\n");
            return;
        }

        m_matrixCBV.SetDescriptorHeapSegment(segment, offset);
        m_matrixCBV.Create(m_matrixBuffer.GetBuffer());
    }

    void TransformMatrix::SetMatrixBuffer(const DirectX::XMMATRIX &matrix) const
    {
        m_matrixBuffer.GetMappedBuffer()->world = matrix;
    }

    void TransformMatrix::SetToCommand(Command &command) const
    {
        if (m_matrixBuffer.IsActive())
        {
            m_matrixCBV.SetGraphicsRootDescriptorTable(&command);
        }
    }
} // AquaEngine
