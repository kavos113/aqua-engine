#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H
#include <map>

#include "buffer/ShaderResourceView.h"


namespace AquaEngine
{
    class TextureManager
    {
    public:
        static Microsoft::WRL::ComPtr<ID3D12Resource> LoadTextureFromFile(const std::string &filename, Command &command);

        static Microsoft::WRL::ComPtr<ID3D12Resource> LoadTextureFromHDRFile(const std::string &filename, Command &command);

    private:
        static std::map<std::string, Microsoft::WRL::ComPtr<ID3D12Resource>> m_resourceTable;
    };
}



#endif //TEXTUREMANAGER_H
