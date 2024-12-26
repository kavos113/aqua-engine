#ifndef FBXMODEL_H
#define FBXMODEL_H
#include <fbxsdk.h>
#include <unordered_map>

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

        enum class AnimationMode
        {
            LOOP,
            ONCE,
        };

        FBXModel(
            DescriptorHeapSegmentManager& manager,
            std::string model_path
        )
            : Mesh(manager)
            , m_status(Status::MUST_BE_REFRESHED)
            , m_animationMode(AnimationMode::ONCE)
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
            , m_status(Status::MUST_BE_REFRESHED)
            , m_animationMode(AnimationMode::ONCE)
            , m_path(std::move(model_path))
        {

        }

        void Create() override;
        void Render(Command& command) override;
        void Timer();

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

        HRESULT PlayAnimation(const std::string &name, AnimationMode mode = AnimationMode::ONCE);

        HRESULT SetCurrentAnimStack(int index);
        HRESULT SetCurrentAnimStack(const std::string &name);

        void SetCurrentPoseIndex(int index);

        void SetSelectedNode(FbxNode *node);

        [[nodiscard]] unsigned int GetFrameCount() const
        {
            return static_cast<unsigned int>(m_frameTime.GetMilliSeconds());
        }

        [[nodiscard]] std::vector<std::string> GetAnimStackNames() const
        {
            std::vector<std::string> names;
            for (int i = 0; i < m_animStackNameArray.GetCount(); ++i)
            {
                names.emplace_back(m_animStackNameArray[i]->Buffer());
            }
            return names;
        }

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

        FbxScene* m_scene{};
        FbxNode* m_selectedNode{};
        FbxAnimLayer* m_currentAnimLayer{};

        FbxArray<FbxPose*> m_poseArray{};
        FbxArray<FbxString*> m_animStackNameArray{};
        std::unordered_map<std::string, int> m_animStackLayers{};

        FbxTime m_frameTime, m_startTime, m_stopTime, m_currentTime;

        Status m_status;
        AnimationMode m_animationMode;

        int m_poseIndex = -1;

        const std::string m_path;

        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void CreateMaterialBuffer();
        void LoadFBX();

        void LoadContent(FbxNode* node);
        void LoadMesh(FbxNode* node);
        void LoadMaterial(const FbxNode *node);

        void LoadVertices(const FbxMesh* mesh);
        void LoadIndices(FbxMesh* mesh);

        void UpdateNode(
            FbxNode *node,
            FbxTime &time,
            FbxAnimLayer *animLayer,
            FbxPose *pose
        );
        void ReadVertexCache(const FbxMesh* mesh, const FbxTime &time);
        void ComputeShapeDeformation(FbxMesh* mesh, const FbxTime &time, FbxAnimLayer *animLayer);
        void ComputeSkinDeformation(const FbxMesh* mesh, FbxTime &time, FbxAMatrix& global_position, FbxPose* pose);
        void ComputeLinearDeformation(const FbxMesh* mesh, const FbxTime &time, const FbxAMatrix& global_position, FbxPose* pose, std::vector<FbxVector4> *dst = nullptr);
        void ComputeDualQuaternionDeformation(const FbxMesh* mesh, const FbxTime &time, const FbxAMatrix& global_position, FbxPose* pose, std::vector<FbxVector4> *dst = nullptr);

        static void ComputeClusterDeformation(const FbxMesh* mesh, const FbxTime &time, const FbxAMatrix& global_position, FbxPose* pose, FbxCluster* cluster, FbxAMatrix& vertex_transform_matrix);

    };

} // AquaEngine

#endif //FBXMODEL_H
