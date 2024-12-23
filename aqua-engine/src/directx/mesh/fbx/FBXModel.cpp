#include "directx/mesh/fbx/FBXModel.h"

#include "directx/wrapper/Barrier.h"
#include "directx/wrapper/Device.h"
#include "fbx/FBXManager.h"

#include <ranges>

#include "directx/mesh/Mesh.h"

namespace
{
    FbxAMatrix GetGlobalPosition(FbxNode* node, const FbxTime& time, FbxPose* pose = nullptr, FbxAMatrix* parent_matrix = nullptr)
    {
        // if not specified pose, return global position at the current time
        if (!pose)
        {
            return node->EvaluateGlobalTransform(time);
        }

        int node_index = pose->Find(node);
        if (node_index < 0)
        {
            return node->EvaluateGlobalTransform(time);
        }

        if (pose->IsBindPose() || !pose->IsLocalMatrix(node_index))
        {
            FbxAMatrix global_position;
            FbxMatrix matrix = pose->GetMatrix(node_index);

            //std::copy(&matrix[0][0], &matrix[0][0] + sizeof(matrix.mData), &global_position[0][0]);
            memcpy(global_position, matrix, sizeof(matrix.mData));
            return global_position;
        }

        FbxAMatrix parent_global_position;

        if (parent_matrix)
        {
            parent_global_position = *parent_matrix;
        }
        else
        {
            if (FbxNode* parent = node->GetParent())
            {
                parent_global_position = GetGlobalPosition(parent, time, pose);
            }
        }

        FbxAMatrix local_position;
        FbxMatrix matrix = pose->GetMatrix(node_index);

        memcpy(local_position, matrix, sizeof(matrix.mData));

        local_position = parent_global_position * local_position;

        return local_position;
    }
}

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
        OutputDebugString("[FBX] Load FBX file.\n");
        CreateVertexBuffer();
        OutputDebugString("[FBX] Create vertex buffer.\n");
        CreateIndexBuffer();
        OutputDebugString("[FBX] Create index buffer.\n");
        CreateMaterialBuffer();
        OutputDebugString("[FBX] Create material buffer.\n");
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

    void FBXModel::CreateMaterialBuffer()
    {
        m_materialBuffer.Create(BUFFER_DEFAULT(AlignmentSize(sizeof(Material) * m_materials.size(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)));
        std::ranges::copy(m_materials, m_materialBuffer.GetMappedBuffer());
        m_materialBuffer.Unmap();
    }

    void FBXModel::CreateMaterialBufferView(const D3D12_DESCRIPTOR_RANGE &material_range)
    {
        auto segment = std::make_shared<DescriptorHeapSegment>(m_manager->Allocate(1));
        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_SHADER_VISIBILITY_ALL,
            &material_range,
            1
        );

        CreateMaterialBufferView(segment, 0);
    }

    void FBXModel::CreateMaterialBufferView(const std::shared_ptr<DescriptorHeapSegment> &segment, int offset)
    {
        m_materialCBV.SetDescriptorHeapSegment(segment, offset);
        m_materialCBV.Create(m_materialBuffer);
    }

    void FBXModel::LoadFBX()
    {
        int result = FBXManager::ReadFile(m_path.c_str(), &m_scene);
        if (result != 0)
        {
            OutputDebugString("Failed to load FBX file.\n");
            return;
        }

        m_scene->FillAnimStackNameArray(m_animStackNameArray);

        // pose
        int pose_count = m_scene->GetPoseCount();
        for (int i = 0; i < pose_count; ++i)
        {
            m_poseArray.Add(m_scene->GetPose(i));
        }

        m_frameTime.SetTime(0, 0, 0, 1, 0, m_scene->GetGlobalSettings().GetTimeMode());

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
            LoadMaterial(node);
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

    // all same only
    void FBXModel::LoadMaterial(FbxNode *node)
    {
        int material_count = node->GetMaterialCount();

        FbxPropertyT<FbxDouble3> fbx_double3;
        FbxPropertyT<FbxDouble> fbx_double;

        m_materials.resize(material_count);

        for (int i = 0; i < material_count; ++i)
        {
            FbxSurfaceMaterial* material = node->GetMaterial(i);

            // phong model
            if (material->GetClassId().Is(FbxSurfacePhong::ClassId))
            {
                auto phong_material = reinterpret_cast<FbxSurfacePhong *>(material);

                fbx_double3 = phong_material->Ambient;
                m_materials[i].ambient = DirectX::XMFLOAT3(fbx_double3.Get()[0], fbx_double3.Get()[1], fbx_double3.Get()[2]);

                fbx_double3 = phong_material->Diffuse;
                m_materials[i].diffuse = DirectX::XMFLOAT3(fbx_double3.Get()[0], fbx_double3.Get()[1], fbx_double3.Get()[2]);

                fbx_double3 = phong_material->Specular;
                m_materials[i].specular = DirectX::XMFLOAT3(fbx_double3.Get()[0], fbx_double3.Get()[1], fbx_double3.Get()[2]);

                fbx_double3 = phong_material->Emissive;
                m_materials[i].emissive = DirectX::XMFLOAT3(fbx_double3.Get()[0], fbx_double3.Get()[1], fbx_double3.Get()[2]);

                fbx_double = phong_material->TransparencyFactor;
                m_materials[i].opacity = 1.0f - fbx_double.Get();

                fbx_double = phong_material->Shininess;
                m_materials[i].shininess = fbx_double.Get();

                fbx_double = phong_material->ReflectionFactor;
                m_materials[i].reflectivity = fbx_double.Get();
            }
            else if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
            {
                auto lambert_material = reinterpret_cast<FbxSurfaceLambert *>(material);

                fbx_double3 = lambert_material->Ambient;
                m_materials[i].ambient = DirectX::XMFLOAT3(fbx_double3.Get()[0], fbx_double3.Get()[1], fbx_double3.Get()[2]);

                fbx_double3 = lambert_material->Diffuse;
                m_materials[i].diffuse = DirectX::XMFLOAT3(fbx_double3.Get()[0], fbx_double3.Get()[1], fbx_double3.Get()[2]);

                fbx_double3 = lambert_material->Emissive;
                m_materials[i].emissive = DirectX::XMFLOAT3(fbx_double3.Get()[0], fbx_double3.Get()[1], fbx_double3.Get()[2]);

                fbx_double = lambert_material->TransparencyFactor;
                m_materials[i].opacity = 1.0f - fbx_double.Get();
            }
        }
    }

    void FBXModel::SetTexture(const D3D12_DESCRIPTOR_RANGE &texture_range)
    {
        auto segment = std::make_shared<DescriptorHeapSegment>(m_manager->Allocate(1));

        m_textureSRV.SetDescriptorHeapSegment(segment, 0);
        m_textureSRV.Create(m_texture);

        segment->SetRootParameter(
            D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
            D3D12_SHADER_VISIBILITY_ALL,
            &texture_range,
            1
        );
    }

    void FBXModel::SetTexture(
        const std::shared_ptr<DescriptorHeapSegment> &segment,
        const int offset
    )
    {
        m_textureSRV.SetDescriptorHeapSegment(segment, offset);
        m_textureSRV.Create(m_texture);
    }

    HRESULT FBXModel::SetCurrentAnimStack(int index)
    {
        int anim_stack_count = m_animStackNameArray.GetCount();
        if (index < 0 || index >= anim_stack_count)
        {
            OutputDebugString("[FBX] invalid anim stack index.\n");
            return E_INVALIDARG;
        }

        FbxAnimStack* current_anim_stack = m_scene->FindMember<FbxAnimStack>(m_animStackNameArray[index]->Buffer());
        if (!current_anim_stack)
        {
            OutputDebugString("[FBX] failed to find anim stack.\n");
            return E_FAIL;
        }

        m_currentAnimLayer = current_anim_stack->GetMember<FbxAnimLayer>();
        m_scene->SetCurrentAnimationStack(current_anim_stack);

        FbxTakeInfo* current_take_info = m_scene->GetTakeInfo(*(m_animStackNameArray[index]));
        if (current_take_info)
        {
            m_startTime = current_take_info->mLocalTimeSpan.GetStart();
            m_stopTime = current_take_info->mLocalTimeSpan.GetStop();
        }
        else
        {
            FbxTimeSpan time_span;
            m_scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(time_span);
            m_startTime = time_span.GetStart();
            m_stopTime = time_span.GetStop();
        }

        m_status = Status::MUST_BE_REFRESHED;

        return S_OK;
    }

    HRESULT FBXModel::SetCurrentPoseIndex(int index)
    {
        m_poseIndex = index;
        m_status = Status::MUST_BE_REFRESHED;
        return S_OK;
    }

    HRESULT FBXModel::SetSelectedNode(FbxNode *node)
    {
        m_selectedNode = node;
        m_status = Status::MUST_BE_REFRESHED;
    }


    void FBXModel::LoadAnimation(FbxNode *node)
    {
    }

    void FBXModel::Render(Command &command)
    {
        FbxPose* pose = nullptr;
        if (m_poseIndex >= 0)
        {
            pose = m_scene->GetPose(m_poseIndex);
        }

        if (m_selectedNode)
        {
            UpdateNode(
                m_selectedNode,
                m_currentTime,
                FbxAMatrix(),
                m_currentAnimLayer,
                pose
            );
        }

        Mesh::Render(command);

        if (m_texture.IsActive())
        {
            m_textureSRV.SetGraphicsRootDescriptorTable(&command);
        }

        if (m_materialBuffer.IsActive())
        {
            m_materialCBV.SetGraphicsRootDescriptorTable(&command);
        }

        command.List()->DrawIndexedInstanced(m_indices.size(), 1, 0, 0, 0);
    }

    void FBXModel::UpdateNode(
        FbxNode *node,
        const FbxTime &time,
        const FbxAMatrix &parentGlobalPosition,
        FbxAnimLayer *animLayer,
        FbxPose *pose = nullptr
    )
    {

    }
} // AquaEngine