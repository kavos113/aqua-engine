#ifndef AQUA_RENDERTARGETVIEW_H
#define AQUA_RENDERTARGETVIEW_H


#include "BufferView.h"
#include "Buffer.h"

namespace AquaEngine
{
    class RenderTargetView
    {
    public:
        void Create(Buffer &buffer) const;
        void Create(ID3D12Resource* resource) const;
        void Create(Buffer &buffer, const D3D12_RENDER_TARGET_VIEW_DESC &rtvDesc) const;

    private:
        BufferView m_view;
    };

}



#endif //AQUA_RENDERTARGETVIEW_H
