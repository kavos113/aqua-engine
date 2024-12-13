#ifndef AQUA_RENDERTARGETVIEW_H
#define AQUA_RENDERTARGETVIEW_H


#include "BufferView.h"
#include "Buffer.h"

class RenderTargetView : public BufferView
{
public:
    void Create(Buffer *buffer) override;
};


#endif //AQUA_RENDERTARGETVIEW_H
