#ifndef AQUA_CONSTANTBUFFERVIEW_H
#define AQUA_CONSTANTBUFFERVIEW_H

#include "BufferView.h"

namespace AquaEngine
{
    class ConstantBufferView : public BufferView
    {
    public:
        void Create(Buffer *buffer) override;
    };
}

#endif //AQUA_CONSTANTBUFFERVIEW_H