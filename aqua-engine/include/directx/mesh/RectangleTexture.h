#ifndef RECTANGLETEXTURE_H
#define RECTANGLETEXTURE_H

#include <array>
#include <DirectXMath.h>
#include <string>
#include <vector>

#include "Mesh.h"
#include "directx/TextureManager.h"
#include "directx/buffer/GPUBuffer.h"

namespace AquaEngine
{
    class RectangleTexture final : public Mesh<RectangleTexture>
    {
    public:
        RectangleTexture(
            DirectX::XMFLOAT3 topleft,
            DirectX::XMFLOAT3 topright,
            DirectX::XMFLOAT3 bottomleft,
            DirectX::XMFLOAT3 bottomright,
            const std::string &texturePath,
            Command &command
        )
            : m_vertices({
                {topleft, {0.0f, 0.0f}},
                {topright, {1.0f, 0.0f}},
                {bottomleft, {0.0f, 1.0f}},
                {bottomright, {1.0f, 1.0f}}
            })
            , m_indices({0, 1, 2, 2, 1, 3})
            , m_texture(Buffer(TextureManager::LoadTextureFromFile(texturePath, command)))
            , m_srv(ShaderResourceView())
            , m_vertexBuffer(GPUBuffer<Vertex>())
            , m_indexBuffer(GPUBuffer<unsigned short>())
        {

        }

        void Create() override;
        void Render(Command &command) override;
        void CreateShaderResourceView(std::unique_ptr<D3D12_DESCRIPTOR_RANGE> texture_range, DescriptorHeapSegmentManager &manager);

    private:
        struct Vertex
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT2 uv;
        };

        std::array<Vertex, 4> m_vertices;
        std::array<unsigned short, 6> m_indices;

        Buffer m_texture;
        ShaderResourceView m_srv;

        GPUBuffer<Vertex> m_vertexBuffer;
        GPUBuffer<unsigned short> m_indexBuffer;

        void CreateVertexBuffer() override;
        void CreateIndexBuffer() override;
    };
}



#endif //RECTANGLETEXTURE_H
