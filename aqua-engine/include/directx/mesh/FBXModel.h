#ifndef FBXMODEL_H
#define FBXMODEL_H
#include <fbxsdk.h>

#include <utility>

#include "Mesh.h"
#include "directx/buffer/ShaderResourceView.h"

namespace AquaEngine {

    class FBXModel : public Mesh<FBXModel>
    {
    public:
        FBXModel(
            DescriptorHeapSegmentManager& manager,
            std::string path
        )
            : Mesh(manager)
            , m_scene(nullptr)
            , m_path(std::move(path))
        {

        }

        void Create() override;
        void Render(Command& command) const override;

        void SetTexture(const std::string& texturePath, const D3D12_DESCRIPTOR_RANGE &texture_range, Command &command, DescriptorHeapSegmentManager &manager);

    private:
        struct Vertex
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT2 uv;
            DirectX::XMFLOAT3 normal;
        };

        std::vector<Vertex> m_vertices{};
        std::vector<unsigned short> m_indices{};

        GPUBuffer<Vertex> m_vertexBuffer;
        GPUBuffer<unsigned short> m_indexBuffer;

        Buffer m_texture;
        ShaderResourceView m_textureSrv;

        FbxScene* m_scene;

        const std::string m_path;

        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void LoadFBX();

        void LoadContent(FbxNode* node);
        void LoadMesh(FbxNode* node);

        void LoadVertices(FbxMesh* mesh);
        void LoadIndices(FbxMesh* mesh);
    };

} // AquaEngine

#endif //FBXMODEL_H
