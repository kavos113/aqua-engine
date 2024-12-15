#include "directx/mesh/Polygon.h"

namespace AquaEngine
{
    void Polygon::Draw(Command &command) const
    {
        Mesh::Draw(command);

        command.List()->DrawIndexedInstanced(m_indices.size(), 1, 0, 0, 0);
    }

}
