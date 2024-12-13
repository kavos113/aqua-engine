#ifndef AQUA_BUFFERVIEW_H
#define AQUA_BUFFERVIEW_H

#include <d3d12.h>
#include <memory>

#include "directx/descriptor_heap/DescriptorHeapSegment.h"
#include "Buffer.h"

class BufferView
{
public:
    virtual void Create(Buffer *buffer) = 0;
    
    void SetDescriptorHeapSegment(std::shared_ptr<DescriptorHeapSegment> descriptorHeapSegment, int offset)
    {
        m_DescriptorHeapSegment = descriptorHeapSegment;
        this->m_offset = offset;
    }
    
    void SetGraphicsRootDescriptorTable(Command *command) const
    {
        m_DescriptorHeapSegment->SetGraphicsRootDescriptorTable(command, m_offset);
    }
    
protected:
    std::shared_ptr<DescriptorHeapSegment> m_DescriptorHeapSegment;
    int m_offset;
};


#endif //AQUA_BUFFERVIEW_H
