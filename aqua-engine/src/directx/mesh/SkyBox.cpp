#include "../../../include/directx/mesh/SkyBox.h"

#include <DirectXTex.h>
#include <ranges>

#include "directx/descriptor_heap/GlobalDescriptorHeapManager.h"

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
            GlobalDescriptorHeapManager::CreateShaderManager("cube_map", 6, D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
        );

        CreateVertexBuffer();
        CreateIndexBuffer();

        CreateCubeMapBuffer();
        CreateHDRIShaderResourceView();
        CreateMatrixBuffer({0.0f, 0.0f, 0.0f});
        CreateHDRIPipelineState();
    }

    void SkyBox::Render(Command &command)
    {
        Mesh::Render(command);

        m_hdriSrv.SetGraphicsRootDescriptorTable(&command);
        command.List()->DrawIndexedInstanced(m_indices.size(), 1, 0, 0, 0);
    }

    void SkyBox::CreateHDRIShaderResourceView()
    {
        auto segment = std::make_shared<DescriptorHeapSegment>(m_hdriManager->Allocate(1));

        m_hdriSrv.SetDescriptorHeapSegment(segment, 0);
        m_hdriSrv.Create(m_hdriBuffer);

        D3D12_DESCRIPTOR_RANGE range = {
            .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
            .NumDescriptors = 1,
            .BaseShaderRegister = 0,
            .RegisterSpace = 0,
            .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
        };
        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_SHADER_VISIBILITY_PIXEL,
            &range,
            1
        );
    }

    void SkyBox::CreateCubeMapBuffer()
    {
        D3D12_RESOURCE_DESC resourceDesc = Buffer::ResourceDesc::DepthStencil(cubeSize, cubeSize);
        resourceDesc.DepthOrArraySize = 6;

        m_cubeMapBuffer.Create(
            Buffer::HeapProperties::Default(),
            D3D12_HEAP_FLAG_NONE,
            resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr
        );

        auto &rtv_manager = GlobalDescriptorHeapManager::GetCPUHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        auto rtv_segment = std::make_shared<DescriptorHeapSegment>(rtv_manager.Allocate(6));

        for (int i = 0; i < 6; ++i)
        {
            m_cubeMapRtv[i].SetDescriptorHeapSegment(rtv_segment, i);
            m_cubeMapRtv[i].Create(m_cubeMapBuffer);
        }

        auto segment = std::make_shared<DescriptorHeapSegment>(m_cubeMapManager->Allocate(6));
        for (int i = 0; i < 6; ++i)
        {
            m_cubeMapSrv[i].SetDescriptorHeapSegment(segment, i);
            m_cubeMapSrv[i].Create(m_cubeMapBuffer);
        }
        D3D12_DESCRIPTOR_RANGE range = {
            .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
            .NumDescriptors = 1,
            .BaseShaderRegister = 0,
            .RegisterSpace = 0,
            .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
        };
        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_SRV,
            D3D12_SHADER_VISIBILITY_PIXEL,
            &range,
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
        D3D12_DESCRIPTOR_RANGE range = {
            .RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
            .NumDescriptors = 1,
            .BaseShaderRegister = 0,
            .RegisterSpace = 0,
            .OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
        };
        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_CBV,
            D3D12_SHADER_VISIBILITY_VERTEX,
            &range,
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
        vs.Load(L"skybox.hlsl", "vs", "vs_5_0");
        ps.Load(L"skybox.hlsl", "ps", "ps_5_0");

        m_hdriPipelineState.SetRootSignature(&m_hdriRootSignature);
        m_hdriPipelineState.SetVertexShader(&vs);
        m_hdriPipelineState.SetPixelShader(&ps);
        m_hdriPipelineState.SetInputLayout(m_inputElementDescs.data(), m_inputElementDescs.size());
        m_hdriPipelineState.SetDepthEnable(false);
        m_hdriPipelineState.SetCullMode(D3D12_CULL_MODE_NONE);
        hr = m_hdriPipelineState.Create();
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create pipeline state\n");
            assert(false);
        }
    }

    void SkyBox::ConvertHDRIToCubeMap(Command &command)
    {
        Mesh::Render(command);

        for (int i = 0; i < 6; ++i)
        {
            auto rtvHandle = m_cubeMapRtv[i].GetCPUHandle();
            command.List()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

            m_matrixCBV[i].SetGraphicsRootDescriptorTable(&command);
            command.List()->DrawIndexedInstanced(m_indices.size(), 1, 0, 0, 0);
        }

        HRESULT hr = command.Execute();
        if (FAILED(hr))
        {
            OutputDebugString("Failed to execute command\n");
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
