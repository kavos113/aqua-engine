#ifndef AQUA_SHADERRESOURCEVIEW_H
#define AQUA_SHADERRESOURCEVIEW_H


#include "IBufferView.h"

namespace AquaEngine
{
    class ShaderResourceView : public IBufferView
    {
    public:
        void Create(Buffer &buffer) override;

        void Create(Buffer &buffer, const D3D12_SHADER_RESOURCE_VIEW_DESC &srvDesc) const;
    };
}

#endif //AQUA_SHADERRESOURCEVIEW_H
