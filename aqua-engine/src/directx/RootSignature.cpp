#include <tchar.h>
#include "directx/RootSignature.h"
#include "directx/Util.h"
#include "directx/wrapper/Device.h"

namespace AquaEngine
{
    RootSignature::RootSignature()
        : m_RootSignature(nullptr)
        , m_manager(nullptr)
    {

    }

    RootSignature::~RootSignature()
    {
        SafeRelease(&m_RootSignature);
    }

    HRESULT RootSignature::Create()
    {
        D3D12_ROOT_SIGNATURE_DESC desc = {};

        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        if (m_manager)
        {
            std::pair<D3D12_ROOT_PARAMETER*, size_t> rootParameters = m_manager->GetRootParameters();

            desc.NumParameters = static_cast<UINT>(rootParameters.second);
            desc.pParameters = rootParameters.first;
        }

        // TODO: static samplers
        desc.NumStaticSamplers = 0;

        ID3DBlob* signature;
        ID3DBlob* error;

        HRESULT hr = D3D12SerializeRootSignature(
            &desc,
            D3D_ROOT_SIGNATURE_VERSION_1_0,
            &signature,
            &error
        );
        if (FAILED(hr))
        {
            OutputDebugString(_T("Failed to serialize root signature.\n"));

            std::string errStr;
            errStr.resize(error->GetBufferSize());
            std::copy_n(reinterpret_cast<char*>(error->GetBufferPointer()), error->GetBufferSize(), errStr.begin());

            OutputDebugStringA(errStr.c_str());
            return hr;
        }

        hr = Device::Get()->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&m_RootSignature)
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
