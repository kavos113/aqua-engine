#include "directx/light/PointLight.h"

#include "directx/Util.h"

namespace AquaEngine
{
    void PointLight::Init(
        const DirectX::XMFLOAT3 position,
        const DirectX::XMFLOAT3 color,
        const float range,
        DescriptorHeapSegmentManager &model_heap,
        std::unique_ptr<D3D12_DESCRIPTOR_RANGE> matrix_range
    )
    {
        m_manager = &model_heap;
        SetBuffer(position, color, range, std::move(matrix_range));
    }

    void PointLight::Render(Command &command) const
    {
        m_view.SetGraphicsRootDescriptorTable(&command);
        m_matrix.SetToCommand(command);
    }

    void PointLight::SetBuffer(
        const DirectX::XMFLOAT3 &position,
        const DirectX::XMFLOAT3 &color,
        const float range,
        std::unique_ptr<D3D12_DESCRIPTOR_RANGE> light_range
    )
    {
        m_light.position = position;
        m_light.color = color;
        m_light.range = range;
        m_light.padding = 0.0f;

        m_buffer.Create(BUFFER_DEFAULT(AlignmentSize(sizeof(Light), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)));
        m_buffer.GetMappedBuffer()->position = m_light.position;
        m_buffer.GetMappedBuffer()->color = m_light.color;
        m_buffer.GetMappedBuffer()->range = m_light.range;

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
