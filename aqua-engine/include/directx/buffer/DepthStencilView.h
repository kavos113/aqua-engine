#ifndef AQUA_DEPTHSTENCILVIEW_H
#define AQUA_DEPTHSTENCILVIEW_H


#include "BufferView.h"

namespace AquaEngine
{
    class DepthStencilView
    {
    public:
        void Create(Buffer &buffer) const;
        void Create(Buffer &buffer, const D3D12_DEPTH_STENCIL_VIEW_DESC &dsvDesc) const;

    private:
        BufferView m_view;
    };
}


#endif //AQUA_DEPTHSTENCILVIEW_H
