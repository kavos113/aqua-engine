#include "../../../include/directx/light/SpotLight.h"

#include "directx/Util.h"

namespace AquaEngine {
    void SpotLight::Init(
        const DirectX::XMFLOAT3 position,
        const DirectX::XMFLOAT3 color,
        const float range,
        const DirectX::XMFLOAT3 direction,
        const float angle,
        DescriptorHeapSegmentManager &model_heap,
        std::unique_ptr<D3D12_DESCRIPTOR_RANGE> matrix_range
    )
    {
        m_manager = &model_heap;
        SetBuffer(position, color, range, direction, angle, std::move(matrix_range));
    }

    void SpotLight::Render(Command &command) const
    {
        m_view.SetGraphicsRootDescriptorTable(&command);
        m_matrix.SetToCommand(command);
    }

    void SpotLight::SetBuffer(
        const DirectX::XMFLOAT3 &position,
        const DirectX::XMFLOAT3 &color,
        float range,
        const DirectX::XMFLOAT3 &direction,
        float angle,
        std::unique_ptr<D3D12_DESCRIPTOR_RANGE> light_range
    )
    {
        m_light.position = position;
        m_light.color = color;
        m_light.range = range;
        m_light.direction = direction;
        m_light.angle = angle;
        m_light.padding = 0.0f;

        m_buffer.Create(BUFFER_DEFAULT(AlignmentSize(sizeof(Light), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)));
        m_buffer.GetMappedBuffer()->position = m_light.position;
        m_buffer.GetMappedBuffer()->color = m_light.color;
        m_buffer.GetMappedBuffer()->range = m_light.range;
        m_buffer.GetMappedBuffer()->direction = m_light.direction;
        m_buffer.GetMappedBuffer()->angle = m_light.angle;

        auto segment = std::make_shared<DescriptorHeapSegment>(m_manager->Allocate(1));
        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_SHADER_VISIBILITY_ALL,
            std::move(light_range),
            1
        );

        m_view.SetDescriptorHeapSegment(segment, 0);
        m_view.Create(m_buffer.GetBuffer());
    }
} // AquaEngine