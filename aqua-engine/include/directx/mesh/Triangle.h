#ifndef AQUA_TRIANGLE_H
#define AQUA_TRIANGLE_H


#include "Polygon.h"

class Triangle final : public Polygon
{
public:
    Triangle(DirectX::XMFLOAT3 pos1, DirectX::XMFLOAT3 pos2, DirectX::XMFLOAT3 pos3)
    {
        m_vertices.resize(3);
        m_vertices[0] = pos1;
        m_vertices[1] = pos2;
        m_vertices[2] = pos3;
        
        m_indices = { 0, 1, 2 };
    }
    
    void Create() override;
    
private:
    void CreateVertexBuffer();
    void CreateIndexBuffer();
};


#endif //AQUA_TRIANGLE_H
