#ifndef AQUA_SHADERRESOURCEVIEW_H
#define AQUA_SHADERRESOURCEVIEW_H


#include "BufferView.h"

namespace AquaEngine
{
    class ShaderResourceView : public BufferView
    {
    public:
        void Create(Buffer *buffer) override;
    };
}

#endif //AQUA_SHADERRESOURCEVIEW_H
