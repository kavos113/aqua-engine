#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H
#include <map>

#include "buffer/ShaderResourceView.h"


class TextureManager
{
public:
    static Buffer LoadTextureFromFile(const std::string &filename, Command &command);

private:
    static std::map<std::string, Buffer> m_resourceTable;
};



#endif //TEXTUREMANAGER_H
