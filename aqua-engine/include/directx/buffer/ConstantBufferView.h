#ifndef AQUA_CONSTANTBUFFERVIEW_H
#define AQUA_CONSTANTBUFFERVIEW_H

#include "BufferView.h"

namespace AquaEngine
{
    class ConstantBufferView
    {
    public:
        void Create(Buffer &buffer) const;

    private:
        BufferView m_view;
    };
}

#endif //AQUA_CONSTANTBUFFERVIEW_H