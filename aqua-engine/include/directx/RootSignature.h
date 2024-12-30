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

        void SetToCommand(Command &command) const
        {
            command.List()->SetGraphicsRootSignature(m_rootSignature.Get());
        }

        [[nodiscard]] Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const
        {
            return m_rootSignature;
        }

        static D3D12_STATIC_SAMPLER_DESC DefaultStaticSampler()
        {
            return D3D12_STATIC_SAMPLER_DESC{
                .Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                .AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                .ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
                .MinLOD = 0.0f,
                .MaxLOD = D3D12_FLOAT32_MAX,
                .ShaderRegister = 0,
                .RegisterSpace = 0,
                .ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL
            };
        }

    private:
        Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

        std::vector<D3D12_STATIC_SAMPLER_DESC> m_samplers;

        DescriptorHeapSegmentManager* m_manager;
    };
}


#endif //AQUA_ROOTSIGNATURE_H
