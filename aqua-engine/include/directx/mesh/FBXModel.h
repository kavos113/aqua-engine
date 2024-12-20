#ifndef FBXMODEL_H
#define FBXMODEL_H
#include <fbxsdk.h>

#include "Mesh.h"

namespace AquaEngine {

    class FBXModel : public Mesh<FBXModel>
    {
    public:
        FBXModel(
            DescriptorHeapSegmentManager& manager,
            const std::string& path
        )
            : Mesh(manager)
            , m_scene(nullptr)
            , m_path(path)
        {

        }

        void Create() override;
        void Render(Command& command) const override;

    private:
        struct Vertex
        {
            DirectX::XMFLOAT3 position;
            //DirectX::XMFLOAT2 uv;
            //DirectX::XMFLOAT3 normal;
        };

        std::vector<Vertex> m_vertices;
        std::vector<unsigned short> m_indices;

        GPUBuffer<Vertex> m_vertexBuffer;
        GPUBuffer<unsigned short> m_indexBuffer;

        FbxScene* m_scene;

        const std::string m_path;

        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void LoadFBX();

        void LoadContent(FbxNode* node);
        void LoadMesh(FbxNode* node);

        void LoadVertices(const FbxMesh* mesh);
        void LoadIndices(const FbxMesh* mesh);
    };

} // AquaEngine

#endif //FBXMODEL_H
