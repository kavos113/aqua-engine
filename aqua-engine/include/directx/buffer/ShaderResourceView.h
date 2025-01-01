#ifndef AQUA_SHADERRESOURCEVIEW_H
#define AQUA_SHADERRESOURCEVIEW_H


#include "BufferView.h"

namespace AquaEngine
{
    class ShaderResourceView : public BufferView
    {
    public:
        void Create(Buffer &buffer) override;

        void Create(Buffer &buffer, const D3D12_SHADER_RESOURCE_VIEW_DESC &srvDesc) const;
    };
}

#endif //AQUA_SHADERRESOURCEVIEW_H
