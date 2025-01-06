#include "../../include/directx/ComputePipeline.h"

#include "directx/wrapper/Device.h"

namespace AquaEngine
{
    ComputePipeline::ComputePipeline()
        : m_pipelineState(nullptr)
        , m_psoDesc()
    {
        m_psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
        m_psoDesc.NodeMask = 0;
    }

    ComputePipeline::~ComputePipeline() = default;

    HRESULT ComputePipeline::Create()
    {
        HRESULT hr = Device::Get()->CreateComputePipelineState(
            &m_psoDesc,
            IID_PPV_ARGS(&m_pipelineState)
        );
        if (FAILED(hr))
        {
            OutputDebugString("Failed to create compute pipeline state\n");
            return hr;
        }

        return S_OK;
    }

    void ComputePipeline::SetRootSignature(const RootSignature *rootSignature)
    {
        m_psoDesc.pRootSignature = rootSignature->GetRootSignature().Get();
    }

    void ComputePipeline::SetComputeShader(const ShaderObject *cs)
    {
        m_psoDesc.CS = cs->Bytecode();
    }
} // AquaEngine
