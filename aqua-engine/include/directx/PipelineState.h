#ifndef AQUA_PIPELINESTATE_H
#define AQUA_PIPELINESTATE_H

#include <d3d12.h>
#include <memory>
#include "RootSignature.h"
#include "ShaderObject.h"

class PipelineState
{
public:
    PipelineState();
    ~PipelineState();
    
    HRESULT Create();
    
    void SetInputLayout(D3D12_INPUT_ELEMENT_DESC *inputLayout, int numElements);
    void SetRootSignature(RootSignature *rootSignature);
    void SetVertexShader(ShaderObject* vs);
    void SetPixelShader(ShaderObject* ps);
    
    void SetToCommand(Command& command) const
    {
        command.List()->SetPipelineState(m_pipelineState);
    }
    
private:
    ID3D12PipelineState* m_pipelineState;
    
    D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDesc;
};


#endif //AQUA_PIPELINESTATE_H
