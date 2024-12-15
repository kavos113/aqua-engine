#include "directx/ShaderObject.h"
#include "directx/Util.h"

#include <d3dcompiler.h>
#include <string>
#include <tchar.h>

namespace AquaEngine
{
    ShaderObject::ShaderObject()
        : m_blob(nullptr)
    {

    }

    ShaderObject::~ShaderObject()
    {
        SafeRelease(&m_blob);
    }

    HRESULT ShaderObject::Load(
        const wchar_t *filename,
        const char *entryPoint,
        const char *target)
    {
        ID3D10Blob* errorBlob = nullptr;

        HRESULT hr = D3DCompileFromFile(
            filename,
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entryPoint,
            target,
            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
            0,
            &m_blob,
            &errorBlob
        );
        if (FAILED(hr))
        {
            OutputDebugString(_T("Failed to compile vertex shader\n"));
            HRESULT hr2 = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

            if (hr == hr2)
            {
                OutputDebugString(_T("File not found\n"));
                return hr;
            }

            std::string errStr;
            errStr.resize(errorBlob->GetBufferSize());
            std::copy_n(reinterpret_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize(), errStr.begin());

            OutputDebugStringA(errStr.c_str());
            return hr;
        }

        return S_OK;
    }

    HRESULT ShaderObject::CompileFromMemory(const char *source, size_t sourceSize, const char *entryPoint, const char *target)
    {
        return 0;
    }

    D3D12_SHADER_BYTECODE ShaderObject::Bytecode() const
    {
        return D3D12_SHADER_BYTECODE{
            .pShaderBytecode = m_blob->GetBufferPointer(),
            .BytecodeLength = m_blob->GetBufferSize()
        };
    }

}
