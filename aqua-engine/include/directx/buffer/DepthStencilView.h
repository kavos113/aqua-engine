#ifndef AQUA_DEPTHSTENCILVIEW_H
#define AQUA_DEPTHSTENCILVIEW_H


#include "BufferView.h"

namespace AquaEngine
{
    class DepthStencilView final : public BufferView
    {
    public:
        void Create(Buffer &buffer) override;
        void Create(const Buffer &buffer, const D3D12_DEPTH_STENCIL_VIEW_DESC &dsvDesc) const;
    };
}


#endif //AQUA_DEPTHSTENCILVIEW_H
