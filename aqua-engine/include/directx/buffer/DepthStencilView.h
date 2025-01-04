#ifndef AQUA_DEPTHSTENCILVIEW_H
#define AQUA_DEPTHSTENCILVIEW_H


#include "IBufferView.h"

namespace AquaEngine
{
    class DepthStencilView final : public IBufferView
    {
    public:
        void Create(Buffer &buffer) override;
        void Create(Buffer &buffer, const D3D12_DEPTH_STENCIL_VIEW_DESC &dsvDesc) const;
    };
}


#endif //AQUA_DEPTHSTENCILVIEW_H
