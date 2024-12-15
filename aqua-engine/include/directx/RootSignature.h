#ifndef AQUA_ROOTSIGNATURE_H
#define AQUA_ROOTSIGNATURE_H

#include <d3d12.h>

#include "directx/descriptor_heap/DescriptorHeapSegmentManager.h"

namespace AquaEngine
{
    class RootSignature
    {
    public:
        RootSignature();
        ~RootSignature();

        HRESULT Create();

        void SetDescriptorHeapSegmentManager(DescriptorHeapSegmentManager *manager);
        void AddStaticSampler(const D3D12_STATIC_SAMPLER_DESC& sampler)
        {
            m_samplers.push_back(sampler);
        }

        void SetToCommand(const Command& command) const
        {
            command.List()->SetGraphicsRootSignature(m_rootSignature);
        }

        [[nodiscard]] ID3D12RootSignature* GetRootSignature() const
        {
            return m_rootSignature;
        }
    private:
        ID3D12RootSignature *m_rootSignature;

        std::vector<D3D12_STATIC_SAMPLER_DESC> m_samplers;

        DescriptorHeapSegmentManager* m_manager;
    };
}


#endif //AQUA_ROOTSIGNATURE_H
