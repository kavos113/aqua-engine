#ifndef AQUA_POLYGON_H
#define AQUA_POLYGON_H

#include <DirectXMath.h>
#include <vector>

#include "Mesh.h"
#include "directx/buffer/GPUBuffer.h"
#include "directx/wrapper/Command.h"

namespace AquaEngine
{
    class Polygon : public Mesh<Polygon>
    {
    public:
        Polygon(DescriptorHeapSegmentManager& manager)
            : Mesh(manager)
        {

        }
        ~Polygon() override = default;

        void Create() override = 0;

        void Render(Command& command) const override;

    protected:
        std::vector<DirectX::XMFLOAT3> m_vertices{};
        std::vector<unsigned short> m_indices{};

        GPUBuffer<DirectX::XMFLOAT3> m_vertexBuffer;
        GPUBuffer<unsigned short> m_indexBuffer;
    };
}


#endif //AQUA_POLYGON_H
