#ifndef AQUA_POLYGON_H
#define AQUA_POLYGON_H

#include <DirectXMath.h>
#include <vector>
#include "directx/buffer/GPUBuffer.h"
#include "directx/wrapper/Command.h"

class Polygon
{
public:
    virtual void Create() = 0;
    
    void Draw(Command& command) const;
    
    [[nodiscard]] D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
    [[nodiscard]] D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
    
protected:
    std::vector<DirectX::XMFLOAT3> m_vertices;
    std::vector<unsigned short> m_indices;
    
    GPUBuffer<DirectX::XMFLOAT3> m_vertexBuffer;
    GPUBuffer<unsigned short> m_indexBuffer;
    
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
};


#endif //AQUA_POLYGON_H
