#ifndef AQUA_SHADERRESOURCEVIEW_H
#define AQUA_SHADERRESOURCEVIEW_H


#include "BufferView.h"

namespace AquaEngine
{
    class ShaderResourceView
    {
    public:
        void Create(Buffer &buffer) const;

        void Create(Buffer &buffer, const D3D12_SHADER_RESOURCE_VIEW_DESC &srvDesc) const;

    private:
        BufferView m_view;
    };
}

#endif //AQUA_SHADERRESOURCEVIEW_H
