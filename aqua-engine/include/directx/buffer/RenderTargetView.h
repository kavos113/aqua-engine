#ifndef AQUA_RENDERTARGETVIEW_H
#define AQUA_RENDERTARGETVIEW_H


#include "IBufferView.h"
#include "Buffer.h"

namespace AquaEngine
{
    class RenderTargetView : public IBufferView
    {
    public:
        void Create(Buffer &buffer) override;
        void Create(ID3D12Resource* resource) const;
        void Create(Buffer &buffer, const D3D12_RENDER_TARGET_VIEW_DESC &rtvDesc) const;
    };

}



#endif //AQUA_RENDERTARGETVIEW_H
