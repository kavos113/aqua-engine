#include "../../../include/directx/mesh/FBXModel.h"

#include "fbx/FBXManager.h"

namespace AquaEngine {

    std::vector<D3D12_INPUT_ELEMENT_DESC> Mesh<FBXModel>::m_inputElementDescs = {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        }
    };

    void FBXModel::Create()
    {
        LoadFBX();
        CreateVertexBuffer();
        CreateIndexBuffer();
    }

    void FBXModel::Render(Command &command) const
    {
        Mesh::Render(command);

        command.List()->DrawIndexedInstanced(m_indices.size(), 1, 0, 0, 0);
    }

    void FBXModel::CreateVertexBuffer()
    {
        m_vertexBuffer.Create(BUFFER_DEFAULT(sizeof(DirectX::XMFLOAT3) * m_vertices.size()));
        std::ranges::copy(m_vertices, m_vertexBuffer.GetMappedBuffer());
        m_vertexBuffer.Unmap();

        m_vertexBufferView.BufferLocation = m_vertexBuffer.GetBuffer()->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(DirectX::XMFLOAT3);
        m_vertexBufferView.SizeInBytes = sizeof(DirectX::XMFLOAT3) * m_vertices.size();
    }

    void FBXModel::CreateIndexBuffer()
    {
        m_indexBuffer.Create(BUFFER_DEFAULT(sizeof(unsigned short) * m_indices.size()));
        std::ranges::copy(m_indices, m_indexBuffer.GetMappedBuffer());
        m_indexBuffer.Unmap();

        m_indexBufferView.BufferLocation = m_indexBuffer.GetBuffer()->GetGPUVirtualAddress();
        m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
        m_indexBufferView.SizeInBytes = sizeof(unsigned short) * m_indices.size();
    }

    void FBXModel::LoadFBX()
    {
        int result = FBXManager::ReadFile(m_path.c_str(), &m_scene);
        if (result != 0)
        {
            OutputDebugString("Failed to load FBX file.\n");
            return;
        }

        FbxNode* root = m_scene->GetRootNode();
        if (root)
        {
            for (int i = 0; i < root->GetChildCount(); ++i)
            {
                LoadContent(root->GetChild(i));
            }
        }
    }

    void FBXModel::LoadContent(FbxNode *node)
    {
        FbxNodeAttribute::EType type = node->GetNodeAttribute()->GetAttributeType();

        if (type == FbxNodeAttribute::eMesh)
        {
            LoadMesh(node);
        }

        for (int i = 0; i < node->GetChildCount(); ++i)
        {
            LoadContent(node->GetChild(i));
        }
    }

    void FBXModel::LoadMesh(FbxNode *node)
    {
        FbxMesh* mesh = node->GetMesh();

        LoadVertices(mesh);
        LoadIndices(mesh);
    }

    void FBXModel::LoadVertices(FbxMesh *mesh)
    {
        int vertex_count = mesh->GetControlPointsCount();
        m_vertices.resize(vertex_count);

        FbxVector4* control_points = mesh->GetControlPoints();

        for (int i = 0; i < vertex_count; ++i)
        {
            m_vertices[i] = DirectX::XMFLOAT3(control_points[i][0] * 10, control_points[i][1] * 10, control_points[i][2] * 10);
        }
    }

    void FBXModel::LoadIndices(FbxMesh *mesh)
    {
        int polygon_count = mesh->GetPolygonCount();
        m_indices.resize(polygon_count * 3);

        for (int i = 0; i < polygon_count; ++i)
        {
            m_indices[i * 3 + 0] = mesh->GetPolygonVertex(i, 0);
            m_indices[i * 3 + 1] = mesh->GetPolygonVertex(i, 1);
            m_indices[i * 3 + 2] = mesh->GetPolygonVertex(i, 2);
        }
    }
} // AquaEngine