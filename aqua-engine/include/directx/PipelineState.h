#ifndef AQUA_PIPELINESTATE_H
#define AQUA_PIPELINESTATE_H

#include <d3d12.h>
#include "RootSignature.h"
#include "ShaderObject.h"

namespace AquaEngine
{
    class PipelineState
    {
    public:
        PipelineState();
        ~PipelineState();

        HRESULT Create();

        void SetInputLayout(const D3D12_INPUT_ELEMENT_DESC *inputLayout, int numElements);

        void SetRootSignature(const RootSignature *rootSignature);

        void SetVertexShader(const ShaderObject *vs);

        void SetPixelShader(const ShaderObject *ps);

        void SetCullMode(D3D12_CULL_MODE cullMode);

        void SetDepthEnable(bool enable);

        void SetRTVFormat(DXGI_FORMAT format);

        void SetBlendState(const D3D12_BLEND_DESC &blendDesc);


        void SetToCommand(Command& command) const
        {
            command.List()->SetPipelineState(m_pipelineState.Get());
        }

    private:
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDesc;
    };
}


#endif //AQUA_PIPELINESTATE_H
