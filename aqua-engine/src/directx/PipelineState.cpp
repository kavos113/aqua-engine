#include "directx/PipelineState.h"
#include "directx/Util.h"
#include "directx/wrapper/Device.h"

namespace AquaEngine
{
    PipelineState::PipelineState()
        : m_pipelineState(nullptr)
        , m_psoDesc()
    {

    }

    PipelineState::~PipelineState()
    {
        SafeRelease(&m_pipelineState);
    }

    HRESULT PipelineState::Create()
    {
        m_psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

        m_psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
        m_psoDesc.BlendState.IndependentBlendEnable = FALSE;
        m_psoDesc.BlendState.RenderTarget[0] = {
            .BlendEnable = FALSE,
            .LogicOpEnable = FALSE,
            .RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
        };

        m_psoDesc.RasterizerState = {
            .FillMode = D3D12_FILL_MODE_SOLID,
            .CullMode = D3D12_CULL_MODE_BACK,
            .FrontCounterClockwise = FALSE,
            .DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
            .DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
            .SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
            .DepthClipEnable = TRUE,
            .MultisampleEnable = FALSE,
            .AntialiasedLineEnable = FALSE,
            .ForcedSampleCount = 0,
            .ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
        };

        m_psoDesc.DepthStencilState = {
            .DepthEnable = TRUE,
            .DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
            .DepthFunc = D3D12_COMPARISON_FUNC_LESS,
            .StencilEnable = FALSE,
        };

        m_psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

        m_psoDesc.NumRenderTargets = 1;
        m_psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

        m_psoDesc.SampleDesc = {
            .Count = 1,
            .Quality = 0
        };

        m_psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        m_psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

        HRESULT hr = Device::Get()->CreateGraphicsPipelineState(
            &m_psoDesc,
            IID_PPV_ARGS(&m_pipelineState)
        );
        if (FAILED(hr))
        {
            OutputDebugStringW(L"Failed to create pipeline state\n");
            return hr;
        }

        return 0;
    }

    void PipelineState::SetInputLayout(D3D12_INPUT_ELEMENT_DESC *inputLayout, int numElements)
    {
        m_psoDesc.InputLayout.pInputElementDescs = inputLayout;
        m_psoDesc.InputLayout.NumElements = numElements;
    }

    void PipelineState::SetRootSignature(RootSignature *rootSignature)
    {
        m_psoDesc.pRootSignature = rootSignature->GetRootSignature();
    }

    void PipelineState::SetVertexShader(ShaderObject *vs)
    {
        m_psoDesc.VS = vs->Bytecode();
    }

    void PipelineState::SetPixelShader(ShaderObject *ps)
    {
        m_psoDesc.PS = ps->Bytecode();
    }

}
