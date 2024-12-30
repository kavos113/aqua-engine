#include "../../../include/directx/mesh/SkyBox.h"

#include <d3d12.h>
#include <DirectXTex.h>
#include <ranges>

#include "directx/descriptor_heap/GlobalDescriptorHeapManager.h"
#include "directx/wrapper/Barrier.h"
#include "directx/wrapper/Device.h"

using DirectX::operator+;

namespace AquaEngine
{
    std::vector<D3D12_INPUT_ELEMENT_DESC> Mesh<SkyBox>::m_inputElementDescs = {
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

    void SkyBox::Create()
    {
        m_hdriManager = std::make_unique<DescriptorHeapSegmentManager>(
            GlobalDescriptorHeapManager::CreateShaderManager("hdri", 10, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
        );
        m_cubeMapManager = std::make_unique<DescriptorHeapSegmentManager>(
            GlobalDescriptorHeapManager::CreateShaderManager("cube_map", 6, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
        );

        CreateVertexBuffer();
        CreateIndexBuffer();

        CreateMatrixBuffer({0.0f, 0.0f, 0.0f});
        CreateCubeMapBuffer();
        CreateHDRIShaderResourceView();
        CreateHDRIPipelineState();
    }

    void SkyBox::Render(Command &command)
    {

    }

    void SkyBox::CreateHDRIShaderResourceView()
    {
        auto segment = std::make_shared<DescriptorHeapSegment>(m_hdriManager->Allocate(1));

        m_hdriSrv.SetDescriptorHeapSegment(segment, 0);
        m_hdriSrv.Create(m_hdriBuffer);

        auto range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
            D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
            1,
            0,
            0,
            D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
        );
        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_SHADER_VISIBILITY_PIXEL,
            std::move(range),
            1
        );
    }

    void SkyBox::CreateCubeMapBuffer()
    {
        D3D12_RESOURCE_DESC resourceDesc = Buffer::ResourceDesc::DepthStencil(cubeSize, cubeSize);
        resourceDesc.DepthOrArraySize = 6;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        resourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

        m_cubeMapBuffer.Create(
            Buffer::HeapProperties::Default(),
            D3D12_HEAP_FLAG_NONE,
            resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr
        );

        auto &rtv_manager = GlobalDescriptorHeapManager::GetCPUHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        auto rtv_segment = std::make_shared<DescriptorHeapSegment>(rtv_manager.Allocate(6));


        D3D12_RENDER_TARGET_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
        desc.Texture2DArray.MipSlice = 0;
        desc.Texture2DArray.ArraySize = 1;
        for (int i = 0; i < 6; ++i)
        {
            m_cubeMapRtv[i].SetDescriptorHeapSegment(rtv_segment, i);
            desc.Texture2DArray.FirstArraySlice = i;
            m_cubeMapRtv[i].Create(m_cubeMapBuffer, desc);
        }

        auto segment = std::make_shared<DescriptorHeapSegment>(m_cubeMapManager->Allocate(1));
        m_cubeMapSrv.SetDescriptorHeapSegment(segment, 0);
        m_cubeMapSrv.Create(
            m_cubeMapBuffer,
            {
                .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
                .ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE,
                .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                .TextureCube = {
                    .MostDetailedMip = 0,
                    .MipLevels = 1,
                    .ResourceMinLODClamp = 0.0f
                }
            }
        );
        auto range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
            D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
            1,
            0,
            0,
            D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
        );
        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_SHADER_VISIBILITY_ALL,
            std::move(range),
            1
        );
    }

    void SkyBox::CreateMatrixBuffer(const DirectX::XMFLOAT3 &eye)
    {
        DirectX::XMVECTOR eyeVec = XMLoadFloat3(&eye);
        m_viewMatrices = {
            DirectX::XMMatrixLookAtLH(
                eyeVec,
                eyeVec + DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
                DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
            ),
            DirectX::XMMatrixLookAtLH(
                eyeVec,
                eyeVec + DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f),
                DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
            ),
            DirectX::XMMatrixLookAtLH(
                eyeVec,
                eyeVec + DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
                DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f)
            ),
            DirectX::XMMatrixLookAtLH(
                eyeVec,
                eyeVec + DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f),
                DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)
            ),
            DirectX::XMMatrixLookAtLH(
                eyeVec,
                eyeVec + DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
                DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
            ),
            DirectX::XMMatrixLookAtLH(
                eyeVec,
                eyeVec + DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f),
                DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
            )
        };

        for (int i = 0; i < 6; ++i)
        {
            m_matrixBuffer[i].Create(
                BUFFER_DEFAULT(AlignmentSize(sizeof(Matrix), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT))
            );
            m_matrixBuffer[i].GetMappedBuffer()->view = m_viewMatrices[i];
        }

        auto segment = std::make_shared<DescriptorHeapSegment>(m_hdriManager->Allocate(6));
        auto range = std::make_unique<D3D12_DESCRIPTOR_RANGE>(
            D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
            1,
            0,
            0,
            D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
        );
        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_SHADER_VISIBILITY_ALL,
            std::move(range),
            1
        );

        for (int i = 0; i < 6; ++i)
        {
            m_matrixCBV[i].SetDescriptorHeapSegment(segment, i);
            m_matrixCBV[i].Create(m_matrixBuffer[i]);
        }
    }

    void SkyBox::CreateHDRIPipelineState()
    {
        m_hdriRootSignature.AddStaticSampler(RootSignature::DefaultStaticSampler());
        m_hdriRootSignature.SetDescriptorHeapSegmentManager(m_hdriManager.get());
        HRESULT hr = m_hdriRootSignature.Create();
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create root signature\n");
            assert(false);
        }

        ShaderObject vs, ps;
        vs.Load(L"cubemap.hlsl", "vs", "vs_5_0");
        ps.Load(L"cubemap.hlsl", "ps", "ps_5_0");

        m_hdriPipelineState.SetRootSignature(&m_hdriRootSignature);
        m_hdriPipelineState.SetVertexShader(&vs);
        m_hdriPipelineState.SetPixelShader(&ps);
        m_hdriPipelineState.SetInputLayout(m_inputElementDescs.data(), m_inputElementDescs.size());
        m_hdriPipelineState.SetDepthEnable(false);
        m_hdriPipelineState.SetCullMode(D3D12_CULL_MODE_NONE);
        m_hdriPipelineState.SetRTVFormat(DXGI_FORMAT_R32G32B32A32_FLOAT);
        hr = m_hdriPipelineState.Create();
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create pipeline state\n");
            assert(false);
        }
    }

    void SkyBox::ConvertHDRIToCubeMap(Command &command)
    {
        m_hdriRootSignature.SetToCommand(command);
        m_hdriPipelineState.SetToCommand(command);
        Mesh::Render(command);

        Barrier::Transition(
            &command,
            m_cubeMapBuffer.GetBuffer().Get(),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );

        for (int i = 0; i < 6; ++i)
        {
            auto rtvHandle = m_cubeMapRtv[i].GetCPUHandle();
            command.List()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

            D3D12_VIEWPORT viewport = {
                .TopLeftX = 0.0f,
                .TopLeftY = 0.0f,
                .Width = static_cast<float>(cubeSize),
                .Height = static_cast<float>(cubeSize),
                .MinDepth = 0.0f,
                .MaxDepth = 1.0f
            };
            command.List()->RSSetViewports(1, &viewport);

            D3D12_RECT scissor = {
                .left = 0,
                .top = 0,
                .right = cubeSize,
                .bottom = cubeSize
            };
            command.List()->RSSetScissorRects(1, &scissor);

            m_matrixCBV[i].SetGraphicsRootDescriptorTable(&command);
            m_hdriSrv.SetGraphicsRootDescriptorTable(&command);
            command.List()->DrawIndexedInstanced(m_indices.size(), 1, 0, 0, 0);
        }

        HRESULT hr = command.Execute();
        if (FAILED(hr))
        {
            OutputDebugString("Failed to execute command\n");
            assert(false);
        }
    }

    void SkyBox::SaveDDS(Command &command)
    {
        DirectX::ScratchImage image;
        HRESULT hr = CaptureTexture(
            command.Queue().Get(),
            m_cubeMapBuffer.GetBuffer().Get(),
            true,
            image
        );
        if (FAILED(hr))
        {
            OutputDebugString("Failed to capture texture\n");
            assert(false);
        }

        hr = SaveToDDSFile(
            image.GetImages(),
            image.GetImageCount(),
            image.GetMetadata(),
            DirectX::DDS_FLAGS_NONE,
            L"skybox.dds"
        );
        if (FAILED(hr))
        {
            OutputDebugString("Failed to save to DDS file\n");
            assert(false);
        }
        OutputDebugString("Saved to DDS File.\n");
    }

    void SkyBox::CreateCubeMapPipelineState()
    {
        m_cubeMapRootSignature.AddStaticSampler(RootSignature::DefaultStaticSampler());
        m_cubeMapRootSignature.SetDescriptorHeapSegmentManager(m_cubeMapManager.get());
        HRESULT hr = m_cubeMapRootSignature.Create();
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create root signature\n");
            assert(false);
        }

        ShaderObject vs, ps;
        vs.Load(L"skybox.hlsl", "vs", "vs_5_0");
        ps.Load(L"skybox.hlsl", "ps", "ps_5_0");

        m_cubeMapPipelineState.SetRootSignature(&m_cubeMapRootSignature);
        m_cubeMapPipelineState.SetVertexShader(&vs);
        m_cubeMapPipelineState.SetPixelShader(&ps);
        m_cubeMapPipelineState.SetInputLayout(m_inputElementDescs.data(), m_inputElementDescs.size());
        m_cubeMapPipelineState.SetDepthEnable(false);
        m_cubeMapPipelineState.SetCullMode(D3D12_CULL_MODE_FRONT);
        hr = m_cubeMapPipelineState.Create();
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create pipeline state\n");
            assert(false);
        }
    }

    void SkyBox::CreateVertexBuffer()
    {
        m_vertexBuffer.Create(BUFFER_DEFAULT(sizeof(Vertex) * 8));
        std::ranges::copy(m_vertices, m_vertexBuffer.GetMappedBuffer());
        m_vertexBuffer.Unmap();

        m_vertexBufferView.BufferLocation = m_vertexBuffer.GetBuffer()->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(Vertex);
        m_vertexBufferView.SizeInBytes = sizeof(Vertex) * 8;
    }

    void SkyBox::CreateIndexBuffer()
    {
        m_indexBuffer.Create(BUFFER_DEFAULT(sizeof(unsigned short) * 36));
        std::ranges::copy(m_indices, m_indexBuffer.GetMappedBuffer());
        m_indexBuffer.Unmap();

        m_indexBufferView.BufferLocation = m_indexBuffer.GetBuffer()->GetGPUVirtualAddress();
        m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
        m_indexBufferView.SizeInBytes = sizeof(unsigned short) * 36;
    }
} // AquaEngine
