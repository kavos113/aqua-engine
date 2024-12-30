#include <tchar.h>
#include "directx/RootSignature.h"
#include "directx/Util.h"
#include "directx/wrapper/Device.h"

namespace AquaEngine
{
    RootSignature::RootSignature()
        : m_rootSignature(nullptr)
        , m_manager(nullptr)
    {

    }

    RootSignature::~RootSignature() = default;

    HRESULT RootSignature::Create()
    {
        D3D12_ROOT_SIGNATURE_DESC desc = {};

        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        std::vector<D3D12_ROOT_PARAMETER> rootParameters;
        if (m_manager)
        {
            rootParameters = m_manager->GetRootParameters();

            desc.NumParameters = static_cast<UINT>(rootParameters.size());
            desc.pParameters = rootParameters.data();
        }

        if (m_samplers.empty())
        {
            desc.NumStaticSamplers = 0;
        }
        else
        {
            desc.NumStaticSamplers = static_cast<UINT>(m_samplers.size());
            desc.pStaticSamplers = m_samplers.data();
        }

        ID3DBlob* signature = nullptr;
        ID3DBlob* error = nullptr;

        HRESULT hr = D3D12SerializeRootSignature(
            &desc,
            D3D_ROOT_SIGNATURE_VERSION_1_0,
            &signature,
            &error
        );
        if (FAILED(hr))
        {
            OutputDebugString(_T("Failed to serialize root signature.\n"));

            if (error == nullptr)
            {
                return hr;
            }

            std::string errStr;
            errStr.resize(error->GetBufferSize());
            std::copy_n(reinterpret_cast<char*>(error->GetBufferPointer()), error->GetBufferSize(), errStr.begin());

            OutputDebugStringA(errStr.c_str());
            return hr;
        }

        for (const auto &root_parameter: rootParameters)
        {
            if (root_parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
            {
                delete root_parameter.DescriptorTable.pDescriptorRanges;
            }
        }

        hr = Device::Get()->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&m_rootSignature)
        );
        if (FAILED(hr))
        {
            OutputDebugString(_T("Failed to create root signature.\n"));
            return hr;
        }

        SafeRelease(&signature);

        return S_OK;
    }

    void RootSignature::SetDescriptorHeapSegmentManager(DescriptorHeapSegmentManager *manager)
    {
        m_manager = manager;
    }

}
