#ifndef AQUA_DEPTHSTENCILVIEW_H
#define AQUA_DEPTHSTENCILVIEW_H


#include "BufferView.h"

class DepthStencilView : public BufferView
{
public:
    void Create(Buffer *buffer) override;
};


#endif //AQUA_DEPTHSTENCILVIEW_H
