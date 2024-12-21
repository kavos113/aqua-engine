#include "../../../include/directx/mesh/FBXModel.h"

#include "directx/TextureManager.h"
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
        },
        {
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
        {
            "NORMAL",
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

        if (m_texture.IsActive())
        {
            m_textureSrv.SetGraphicsRootDescriptorTable(&command);
        }

        command.List()->DrawIndexedInstanced(m_indices.size(), 1, 0, 0, 0);
    }

    void FBXModel::CreateVertexBuffer()
    {
        m_vertexBuffer.Create(BUFFER_DEFAULT(sizeof(Vertex) * m_vertices.size()));
        std::ranges::copy(m_vertices, m_vertexBuffer.GetMappedBuffer());
        m_vertexBuffer.Unmap();

        m_vertexBufferView.BufferLocation = m_vertexBuffer.GetBuffer()->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(Vertex);
        m_vertexBufferView.SizeInBytes = sizeof(Vertex) * m_vertices.size();
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

        if (FbxNode* root = m_scene->GetRootNode())
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
            m_vertices[i].position = DirectX::XMFLOAT3(control_points[i][0], control_points[i][1], control_points[i][2]);
        }
    }

    void FBXModel::LoadIndices(FbxMesh *mesh)
    {
        int polygon_count = mesh->GetPolygonCount();
        m_indices.resize(polygon_count * 3);

        int uv_count = mesh->GetElementUVCount();
        int normal_count = mesh->GetElementNormalCount();

        for (int i = 0; i < polygon_count; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                m_indices[i * 3 + j] = mesh->GetPolygonVertex(i, j);

                // uv
                for (int k = 0; k < uv_count; ++k)
                {
                    const FbxGeometryElementUV *uv = mesh->GetElementUV(k);

                    switch (uv->GetMappingMode())
                    {
                    case FbxGeometryElement::eByControlPoint:
                        {
                            switch (uv->GetReferenceMode())
                            {
                            case FbxGeometryElement::eDirect:
                            {
                                FbxVector2 uv_value = uv->GetDirectArray().GetAt(m_indices[i * 3 + j]);
                                m_vertices[m_indices[i * 3 + j]].uv = DirectX::XMFLOAT2(uv_value[0], uv_value[1]);
                            }
                                break;
                            case FbxGeometryElement::eIndexToDirect:
                            {
                                int id = uv->GetIndexArray().GetAt(m_indices[i * 3 + j]);
                                FbxVector2 uv_value = uv->GetDirectArray().GetAt(id);
                                m_vertices[m_indices[i * 3 + j]].uv = DirectX::XMFLOAT2(uv_value[0], uv_value[1]);
                            }
                                break;
                            default:
                                break;
                            }
                        }
                        break;

                    case FbxGeometryElement::eByPolygonVertex:
                        {
                            switch (uv->GetReferenceMode())
                            {
                            case FbxGeometryElement::eDirect:
                            case FbxGeometryElement::eIndexToDirect:
                                {
                                    FbxVector2 uv_value = uv->GetDirectArray().GetAt(mesh->GetTextureUVIndex(i, j));
                                    m_vertices[m_indices[i * 3 + j]].uv = DirectX::XMFLOAT2(uv_value[0], uv_value[1]);
                                }
                                break;
                            default:
                                break;
                            }
                        }

                    default:
                        break;
                    }
                }

                // normal
                for (int k = 0; k < normal_count; ++k)
                {
                    const FbxGeometryElementNormal *normal = mesh->GetElementNormal(k);

                    switch (normal->GetMappingMode())
                    {
                    case FbxGeometryElement::eByControlPoint:
                        {
                            switch (normal->GetReferenceMode())
                            {
                            case FbxGeometryElement::eDirect:
                                {
                                    FbxVector4 normal_value = normal->GetDirectArray().GetAt(m_indices[i * 3 + j]);
                                    m_vertices[m_indices[i * 3 + j]].normal = DirectX::XMFLOAT3(normal_value[0], normal_value[1], normal_value[2]);
                                }
                                break;
                            case FbxGeometryElement::eIndexToDirect:
                                {
                                    int id = normal->GetIndexArray().GetAt(m_indices[i * 3 + j]);
                                    FbxVector4 normal_value = normal->GetDirectArray().GetAt(id);
                                    m_vertices[m_indices[i * 3 + j]].normal = DirectX::XMFLOAT3(normal_value[0], normal_value[1], normal_value[2]);
                                }
                                break;
                            default:
                                break;
                            }
                        }
                        break;

                    case FbxGeometryElement::eByPolygonVertex:
                        {
                            switch (normal->GetReferenceMode())
                            {
                            case FbxGeometryElement::eDirect:
                            case FbxGeometryElement::eIndexToDirect:
                                {
                                    FbxVector4 normal_value = normal->GetDirectArray().GetAt(mesh->GetTextureUVIndex(i, j));
                                    m_vertices[m_indices[i * 3 + j]].normal = DirectX::XMFLOAT3(normal_value[0], normal_value[1], normal_value[2]);
                                }
                                break;
                            default:
                                break;
                            }
                        }

                    default:
                        break;
                    }
                }
            }

        }


    }

    void FBXModel::SetTexture(
        const std::string &texturePath,
        const D3D12_DESCRIPTOR_RANGE &texture_range,
        Command &command,
        DescriptorHeapSegmentManager &manager)
    {
        Buffer b = Buffer(TextureManager::LoadTextureFromFile(texturePath, command));
        m_texture = std::move(b);

        auto segment = std::make_shared<DescriptorHeapSegment>(manager.Allocate(1));

        m_textureSrv.SetDescriptorHeapSegment(segment, 0);
        m_textureSrv.Create(m_texture);

        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_SHADER_VISIBILITY_PIXEL,
            &texture_range,
            1
        );
    }
} // AquaEngine