#ifndef AQUA_RENDERTARGETVIEW_H
#define AQUA_RENDERTARGETVIEW_H


#include "BufferView.h"
#include "Buffer.h"

class RenderTargetView : public BufferView
{
public:
    void Create(Buffer *buffer) override;
    void Create(ID3D12Resource* resource);
};


#endif //AQUA_RENDERTARGETVIEW_H
