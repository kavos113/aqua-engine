#ifndef RECTANGLE_H
#define RECTANGLE_H
#include "Polygon.h"


class Rectangle final : public Polygon
{
public:
    Rectangle(DirectX::XMFLOAT3 topleft, DirectX::XMFLOAT3 topright, DirectX::XMFLOAT3 bottomleft, DirectX::XMFLOAT3 bottomright)
    {
        m_vertices.resize(4);
        m_vertices[0] = topleft;
        m_vertices[1] = topright;
        m_vertices[2] = bottomleft;
        m_vertices[3] = bottomright;

        m_indices = { 0, 1, 2,  2, 1, 3 };
    }

    void Create() override;

private:
    void CreateVertexBuffer();
    void CreateIndexBuffer();
};



#endif //RECTANGLE_H
