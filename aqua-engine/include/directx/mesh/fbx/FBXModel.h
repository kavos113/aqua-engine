#ifndef FBXMODEL_H
#define FBXMODEL_H
#include <fbxsdk.h>

#include <utility>

#include "directx/TextureManager.h"
#include "directx/buffer/ShaderResourceView.h"
#include "directx/mesh/Mesh.h"

namespace AquaEngine {

    // vertex(control point), index(polygon), material, texture(uv), animation, pose
    class FBXModel : public Mesh<FBXModel>
    {
    public:
        enum class Status
        {
            MUST_BE_REFRESHED,
            REFRESHED
        };

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
        void Render(Command& command) override;

        void SetTexture(
            const D3D12_DESCRIPTOR_RANGE &texture_range
        );

        // if use shared segment
        void SetTexture(
            const std::shared_ptr<DescriptorHeapSegment>& segment,
            int offset = 0
            );

        void CreateMaterialBufferView(const D3D12_DESCRIPTOR_RANGE &material_range);
        void CreateMaterialBufferView(
            const std::shared_ptr<DescriptorHeapSegment>& segment,
            int offset = 0
        );

        HRESULT SetCurrentAnimStack(int index);
        HRESULT SetCurrentPoseIndex(int index);
        HRESULT SetSelectedNode(FbxNode* node);

    private:
        struct Vertex
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT2 uv;
            DirectX::XMFLOAT3 normal;
        };

        struct Material
        {
            DirectX::XMFLOAT3 ambient;
            DirectX::XMFLOAT3 diffuse;
            DirectX::XMFLOAT3 specular;
            DirectX::XMFLOAT3 emissive;
            float opacity;
            float shininess;
            float reflectivity;
        };

        std::vector<Vertex> m_vertices{};
        std::vector<unsigned short> m_indices{};

        GPUBuffer<Vertex> m_vertexBuffer;
        GPUBuffer<unsigned short> m_indexBuffer;

        std::vector<Material> m_materials{};
        GPUBuffer<Material> m_materialBuffer;
        ConstantBufferView m_materialCBV;

        Buffer m_texture;
        ShaderResourceView m_textureSRV;

        FbxScene* m_scene;
        FbxNode* m_selectedNode;
        FbxAnimLayer* m_currentAnimLayer;

        FbxArray<FbxString*> m_animStackNameArray;
        FbxArray<FbxPose*> m_poseArray;

        FbxTime m_frameTime, m_startTime, m_stopTime, m_currentTime;

        Status m_status;

        int m_poseIndex;

        const std::string m_path;

        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void CreateMaterialBuffer();
        void LoadFBX();

        void LoadContent(FbxNode* node);
        void LoadMesh(FbxNode* node);
        void LoadMaterial(FbxNode *node);
        void LoadAnimation(FbxNode* node);

        void LoadVertices(FbxMesh* mesh);
        void LoadIndices(FbxMesh* mesh);

        void UpdateNode(
            FbxNode* node,
            const FbxTime &time,
            const FbxAMatrix &parentGlobalPosition,
            FbxAnimLayer* animLayer,
            FbxPose* pose
        );
    };

} // AquaEngine

#endif //FBXMODEL_H
