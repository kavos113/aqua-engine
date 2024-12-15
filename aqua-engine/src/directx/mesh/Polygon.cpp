#include "directx/mesh/Polygon.h"

namespace AquaEngine
{
    void Polygon::Draw(Command &command) const
    {
        Mesh::Draw(command);

        command.List()->DrawIndexedInstanced(m_indices.size(), 1, 0, 0, 0);
    }

    std::vector<D3D12_INPUT_ELEMENT_DESC> Mesh<Polygon>::m_inputElementDescs = {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        }
    };

}
