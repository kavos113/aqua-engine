#ifndef SKYBOX_H
#define SKYBOX_H

#include <array>

#include "Mesh.h"
#include "directx/Camera.h"
#include "directx/PipelineState.h"
#include "directx/TextureManager.h"
#include "directx/buffer/RenderTargetView.h"
#include "directx/buffer/ShaderResourceView.h"

namespace AquaEngine
{
    class SkyBox : public Mesh<SkyBox>
    {
    public:
        SkyBox(const std::string &hdriPath, Command &command, DescriptorHeapSegmentManager &manager)
            : Mesh(manager)
            , m_hdriPath(hdriPath)
            , m_hdriBuffer(TextureManager::LoadTextureFromHDRFile(hdriPath, command))
            , m_hdriSrv(ShaderResourceView())
            , m_vertices(
                {
                    {{-1.0f, -1.0f, -1.0f}},
                    {{-1.0f, 1.0f, -1.0f}},
                    {{1.0f, -1.0f, -1.0f}},
                    {{1.0f, 1.0f, -1.0f}},
                    {{-1.0f, -1.0f, 1.0f}},
                    {{-1.0f, 1.0f, 1.0f}},
                    {{1.0f, -1.0f, 1.0f}},
                    {{1.0f, 1.0f, 1.0f}}
                }
            )
            , m_indices(
                {
                    0,
                    1,
                    2,
                    1,
                    3,
                    2,
                    // Front face
                    4,
                    6,
                    5,
                    5,
                    6,
                    7,
                    // Back face
                    0,
                    2,
                    6,
                    0,
                    6,
                    4,
                    // Bottom face
                    1,
                    5,
                    3,
                    3,
                    5,
                    7,
                    // Top face
                    0,
                    4,
                    1,
                    1,
                    4,
                    5,
                    // Left face
                    2,
                    3,
                    6,
                    3,
                    7,
                    6 // Right face
                }
            )
        {
        }

        void Create() override;
        void Render(Command &command) override;

        void ConvertHDRIToCubeMap(Command &command);
        void SaveDDS(Command &command);

        void SetCamera(std::shared_ptr<Camera> camera)
        {
            m_camera = std::move(camera);
        }

    private:
        struct Vertex
        {
            DirectX::XMFLOAT3 position;
        };

        struct Matrix
        {
            DirectX::XMMATRIX view;
        };

        std::array<DirectX::XMMATRIX, 6> m_viewMatrices;
        std::array<GPUBuffer<Matrix>, 6> m_matrixBuffer;
        std::array<ConstantBufferView, 6> m_matrixCBV;

        std::string m_hdriPath;

        Buffer m_hdriBuffer;
        ShaderResourceView m_hdriSrv;

        std::array<Vertex, 8> m_vertices;
        std::array<unsigned short, 36> m_indices;

        GPUBuffer<Vertex> m_vertexBuffer;
        GPUBuffer<unsigned short> m_indexBuffer;

        std::unique_ptr<DescriptorHeapSegmentManager> m_hdriManager;
        RootSignature m_hdriRootSignature;
        PipelineState m_hdriPipelineState;

        Buffer m_cubeMapBuffer;
        ShaderResourceView m_cubeMapSrv;
        std::array<RenderTargetView, 6> m_cubeMapRtv;
        std::unique_ptr<DescriptorHeapSegmentManager> m_cubeMapManager;
        RootSignature m_cubeMapRootSignature;
        PipelineState m_cubeMapPipelineState;

        std::shared_ptr<Camera> m_camera;

        const unsigned short cubeSize = 512;

        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void CreateMatrixBuffer(const DirectX::XMFLOAT3 &eye);
        void CreateCubeMapBuffer();

        void CreateHDRIShaderResourceView();
        void CreateHDRIPipelineState();

        void CreateCubeMapPipelineState();
    };
} // AquaEngine

#endif //SKYBOX_H
