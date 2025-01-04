#include "../../include/directx/DirectionLight.h"

#include "directx/Util.h"

namespace AquaEngine
{
    void DirectionLight::Init(
        const DirectX::XMFLOAT3 &direction,
        const DirectX::XMFLOAT3 &color,
        DescriptorHeapSegmentManager &model_heap,
        std::unique_ptr<D3D12_DESCRIPTOR_RANGE> matrix_range
    )
    {
        m_manager = &model_heap;
        SetBuffer(direction, color, std::move(matrix_range));
    }

    void DirectionLight::Render(Command &command) const
    {
        m_view.SetGraphicsRootDescriptorTable(&command);
    }

    void DirectionLight::SetBuffer(
        const DirectX::XMFLOAT3 &direction,
        const DirectX::XMFLOAT3 &color,
        std::unique_ptr<D3D12_DESCRIPTOR_RANGE> matrix_range
    )
    {
        m_light.direction = direction;
        m_light.color = color;
        m_light.padding = 0.0f;

        m_buffer.Create(BUFFER_DEFAULT(AlignmentSize(sizeof(Light), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)));
        m_buffer.GetMappedBuffer()->direction = m_light.direction;
        m_buffer.GetMappedBuffer()->color = m_light.color;

        auto segment = std::make_shared<DescriptorHeapSegment>(m_manager->Allocate(1));
        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_SHADER_VISIBILITY_ALL,
            std::move(matrix_range),
            1
        );

        m_view.SetDescriptorHeapSegment(segment, 0);
        m_view.Create(m_buffer.GetBuffer());
    }
}
