#ifndef AQUA_CONSTANTBUFFERVIEW_H
#define AQUA_CONSTANTBUFFERVIEW_H

#include "IBufferView.h"

namespace AquaEngine
{
    class ConstantBufferView : public IBufferView
    {
    public:
        void Create(Buffer &buffer) override;
    };
}

#endif //AQUA_CONSTANTBUFFERVIEW_H