#ifndef FBXMODEL_H
#define FBXMODEL_H
#include <fbxsdk.h>

#include <utility>

#include "Mesh.h"
#include "directx/TextureManager.h"
#include "directx/buffer/ShaderResourceView.h"

namespace AquaEngine {

    class FBXModel : public Mesh<FBXModel>
    {
    public:
        FBXModel(
            DescriptorHeapSegmentManager& manager,
            std::string model_path
        )
            : Mesh(manager)
            , m_scene(nullptr)
            , m_path(std::move(model_path))
        {

        }

        explicit FBXModel(
            DescriptorHeapSegmentManager& manager,
            std::string model_path,
            const std::string &texturePath,
            Command &command
        )
            : Mesh(manager)
            , m_texture(Buffer(TextureManager::LoadTextureFromFile(texturePath, command)))
            , m_scene(nullptr)
            , m_path(std::move(model_path))
        {

        }

        void Create() override;
        void Render(Command& command) const override;

        void SetTexture(
            const D3D12_DESCRIPTOR_RANGE &texture_range,
            DescriptorHeapSegmentManager &manager
            );

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
