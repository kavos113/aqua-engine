#ifndef AQUA_SHADEROBJECT_H
#define AQUA_SHADEROBJECT_H

#include <d3d12.h>

namespace AquaEngine
{
    class ShaderObject
    {
    public:
        ShaderObject();
        ~ShaderObject();

        HRESULT Load(const wchar_t* filename, const char* entryPoint, const char* target);
        HRESULT CompileFromMemory(const char* source, size_t sourceSize, const char* entryPoint, const char* target);

        [[nodiscard]] D3D12_SHADER_BYTECODE Bytecode() const;
    private:
        ID3D10Blob* m_blob;
    };
}


#endif //AQUA_SHADEROBJECT_H
