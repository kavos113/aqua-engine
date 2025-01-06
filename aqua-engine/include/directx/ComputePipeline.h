#ifndef COMPUTEPIPELINE_H
#define COMPUTEPIPELINE_H

#include <d3d12.h>

#include "RootSignature.h"
#include "ShaderObject.h"
#include "wrapper/Command.h"

namespace AquaEngine
{
    class ComputePipeline
    {
    public:
        ComputePipeline();

        ~ComputePipeline();

        HRESULT Create();

        void SetRootSignature(const RootSignature *rootSignature);

        void SetComputeShader(const ShaderObject *cs);

        void SetToCommand(Command &command) const
        {
            command.List()->SetPipelineState(m_pipelineState.Get());
        }

    private:
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
        D3D12_COMPUTE_PIPELINE_STATE_DESC m_psoDesc;
    };
} // AquaEngine

#endif //COMPUTEPIPELINE_H
