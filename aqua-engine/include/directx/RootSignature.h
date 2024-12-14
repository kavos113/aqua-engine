#ifndef AQUA_ROOTSIGNATURE_H
#define AQUA_ROOTSIGNATURE_H

#include <d3d12.h>

#include "directx/descriptor_heap/DescriptorHeapSegmentManager.h"

class RootSignature
{
public:
    RootSignature();
    ~RootSignature();
    
    HRESULT Create();
    
    void SetDescriptorHeapSegmentManager(DescriptorHeapSegmentManager *manager);
    
    void SetToCommand(Command& command) const
    {
        command.List()->SetGraphicsRootSignature(m_RootSignature);
    }
    
    [[nodiscard]] ID3D12RootSignature* GetRootSignature() const
    {
        return m_RootSignature;
    }
private:
    ID3D12RootSignature *m_RootSignature;
    
    DescriptorHeapSegmentManager* m_manager;
};


#endif //AQUA_ROOTSIGNATURE_H
