#include "directx/mesh/fbx/FBXModel.h"

#include "directx/wrapper/Barrier.h"
#include "directx/wrapper/Device.h"
#include "fbx/FBXManager.h"

#include <ranges>

#include "directx/mesh/Mesh.h"

using DirectX::operator-;

namespace
{
    FbxAMatrix GetGlobalPosition(FbxNode* node, const FbxTime& time, FbxPose* pose = nullptr, const FbxAMatrix* parent_matrix = nullptr)
    {
        // if not specified pose, return global position at the current time
        if (!pose)
        {
            return node->EvaluateGlobalTransform(time);
        }

        const int node_index = pose->Find(node);
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

    FbxAMatrix GetGeometryOffset(const FbxNode* node)
    {
        const FbxVector4 translation = node->GetGeometricTranslation(FbxNode::eSourcePivot);
        const FbxVector4 rotation = node->GetGeometricRotation(FbxNode::eSourcePivot);
        const FbxVector4 scaling = node->GetGeometricScaling(FbxNode::eSourcePivot);

        return FbxAMatrix(translation, rotation, scaling);
    }

    void FbxMatrixScale(FbxAMatrix& m, double s)
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                m[i][j] *= s;
            }
        }
    }

    void FbxMatrixAdd(FbxAMatrix& dst, FbxAMatrix& src)
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                dst[i][j] += src[i][j];
            }
        }
    }

    void FbxMatrixAddDiagonal(FbxAMatrix& m, double d)
    {
        for (int i = 0; i < 4; ++i)
        {
            m[i][i] += d;
        }
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
        const auto segment = std::make_shared<DescriptorHeapSegment>(m_manager->Allocate(1));
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
        const int result = FBXManager::ReadFile(m_path.c_str(), &m_scene);
        if (result != 0)
        {
            OutputDebugString("Failed to load FBX file.\n");
            return;
        }

        m_scene->FillAnimStackNameArray(m_animStackNameArray);
        for (int i = 0; i < m_animStackNameArray.GetCount(); ++i)
        {
            m_animStackLayers[m_animStackNameArray[i]->Buffer()] = i;
        }

        // pose
        const int pose_count = m_scene->GetPoseCount();
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
        const FbxNodeAttribute::EType type = node->GetNodeAttribute()->GetAttributeType();

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

    void FBXModel::LoadVertices(const FbxMesh *mesh)
    {
        const int vertex_count = mesh->GetControlPointsCount();
        m_vertices.resize(vertex_count);

        FbxVector4* control_points = mesh->GetControlPoints();

        for (int i = 0; i < vertex_count; ++i)
        {
            m_vertices[i].position = DirectX::XMFLOAT3(control_points[i][0], control_points[i][1], control_points[i][2]);
        }
    }

    void FBXModel::LoadIndices(FbxMesh *mesh)
    {
        const int polygon_count = mesh->GetPolygonCount();
        m_indices.resize(polygon_count * 3);

        const int uv_count = mesh->GetElementUVCount();
        const int normal_count = mesh->GetElementNormalCount();

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
                                const int id = uv->GetIndexArray().GetAt(m_indices[i * 3 + j]);
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
                                    const int id = normal->GetIndexArray().GetAt(m_indices[i * 3 + j]);
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
    void FBXModel::LoadMaterial(const FbxNode *node)
    {
        const int material_count = node->GetMaterialCount();

        FbxPropertyT<FbxDouble3> fbx_double3;
        FbxPropertyT<FbxDouble> fbx_double;

        m_materials.resize(material_count);

        for (int i = 0; i < material_count; ++i)
        {
            FbxSurfaceMaterial* material = node->GetMaterial(i);

            // phong model
            if (material->GetClassId().Is(FbxSurfacePhong::ClassId))
            {
                const auto phong_material = reinterpret_cast<FbxSurfacePhong *>(material);

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
                const auto lambert_material = reinterpret_cast<FbxSurfaceLambert *>(material);

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
        const auto segment = std::make_shared<DescriptorHeapSegment>(m_manager->Allocate(1));

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

    HRESULT FBXModel::PlayAnimation(const std::string &name, AnimationMode mode)
    {
        m_animationMode = mode;
        HRESULT hr = SetCurrentAnimStack(name);
        if (FAILED(hr))
        {
            OutputDebugString("[FBX] failed to find anim stack.\n");
        }
        return hr;
    }

    HRESULT FBXModel::SetCurrentAnimStack(const std::string &name)
    {
        HRESULT hr = SetCurrentAnimStack(m_animStackLayers[name]);
        if (FAILED(hr))
        {
            OutputDebugString("[FBX] failed to find anim stack.\n");
        }
        return hr;
    }

    HRESULT FBXModel::SetCurrentAnimStack(int index)
    {
        const int anim_stack_count = m_animStackNameArray.GetCount();
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

        const FbxTakeInfo* current_take_info = m_scene->GetTakeInfo(*(m_animStackNameArray[index]));
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
                m_currentAnimLayer,
                pose
            );
        }
        else
        {
            UpdateNode(
                m_scene->GetRootNode(),
                m_currentTime,
                m_currentAnimLayer,
                pose
            );
        }

        // auto b = m_vertexBuffer.GetMappedBuffer();
        // std::println("FrameTime: {}", m_currentTime.GetMilliSeconds());
        // // for (int i = 0; i < m_vertices.size(); ++i)
        // // {
        // //     std::println("{}, {}, {}", b[i].position.x, b[i].position.y, b[i].position.z);
        // // }
        // std::println("{}, {}, {}", b[0].position.x, b[0].position.y, b[0].position.z);
        // std::println("--------------------------------");

        m_status = Status::MUST_BE_REFRESHED;

        return S_OK;
    }

    void FBXModel::SetCurrentPoseIndex(int index)
    {
        m_poseIndex = index;
        m_status = Status::MUST_BE_REFRESHED;
    }

    void FBXModel::SetSelectedNode(FbxNode *node)
    {
        m_selectedNode = node;
        m_status = Status::MUST_BE_REFRESHED;
    }

    void FBXModel::Render(Command &command)
    {
        Mesh::Render(command);

        if (m_texture.IsActive())
        {
            m_textureSRV.SetGraphicsRootDescriptorTable(&command);
        }

        if (m_materialBuffer.IsActive())
        {
            m_materialCBV.SetGraphicsRootDescriptorTable(&command);
        }

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
                m_currentAnimLayer,
                pose
            );
        }
        else
        {
            UpdateNode(
                m_scene->GetRootNode(),
                m_currentTime,
                m_currentAnimLayer,
                pose
            );
        }

        command.List()->DrawIndexedInstanced(m_indices.size(), 1, 0, 0, 0);
    }

    void FBXModel::Timer()
    {
        if (m_stopTime > m_startTime)
        {
            m_status = Status::MUST_BE_REFRESHED;
            m_currentTime += m_frameTime;

            if (m_currentTime > m_stopTime)
            {
                if (m_animationMode == AnimationMode::LOOP)
                {
                    m_currentTime = m_startTime; // ループ再生
                }
                else
                {
                    m_stopTime = m_startTime;
                    m_currentTime = m_startTime;
                }
            }
        }
        else
        {
            m_status = Status::REFRESHED;
        }


        // auto b = m_vertexBuffer.GetMappedBuffer();
        // std::println("FrameTime: {}", m_currentTime.GetMilliSeconds());
        // // for (int i = 0; i < m_vertices.size(); ++i)
        // // {
        // //     std::println("{}, {}, {}", b[i].position.x, b[i].position.y, b[i].position.z);
        // // }
        // std::println("{}, {}, {}", b[0].position.x, b[0].position.y, b[0].position.z);
        // std::println("--------------------------------");
    }

    void FBXModel::UpdateNode(
        FbxNode *node,
        FbxTime &time,
        FbxAnimLayer *animLayer,
        FbxPose *pose = nullptr
    )
    {
        const FbxAMatrix global_position = GetGlobalPosition(node, time, pose);

        if (const FbxNodeAttribute* node_attribute = node->GetNodeAttribute())
        {
            const FbxAMatrix geometry_offset = GetGeometryOffset(node);
            FbxAMatrix global_offset_position = global_position * geometry_offset;

            if (node_attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
            {
                FbxMesh* mesh = node->GetMesh();
                const int vertex_count = mesh->GetControlPointsCount();

                const bool has_vertex_cache = mesh->GetDeformerCount(FbxDeformer::eVertexCache) > 0 &&
                    dynamic_cast<FbxVertexCacheDeformer*>(mesh->GetDeformer(0, FbxDeformer::eVertexCache))->Active.Get();
                const bool has_shape = mesh->GetShapeCount() > 0;
                const bool has_skin = mesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
                const bool has_deformation = has_vertex_cache || has_shape || has_skin;

                if (has_deformation)
                {
                    if (has_vertex_cache)
                    {
                        ReadVertexCache(mesh, time);
                    }
                    else
                    {
                        if (has_shape)
                        {
                            ComputeShapeDeformation(mesh, time, animLayer);
                        }

                        const int skin_count = mesh->GetDeformerCount(FbxDeformer::eSkin);
                        int cluster_count = 0;
                        for (int i = 0; i < skin_count; ++i)
                        {
                            cluster_count += reinterpret_cast<FbxSkin*>(mesh->GetDeformer(i, FbxDeformer::eSkin))->GetClusterCount();
                        }

                        if (cluster_count > 0)
                        {
                            ComputeSkinDeformation(mesh, time, global_offset_position, pose);
                        }
                    }
                }
            }
        }

        for (int i = 0; i < node->GetChildCount(); ++i)
        {
            UpdateNode(node->GetChild(i), time, animLayer, pose);
        }
    }

    void FBXModel::ReadVertexCache(const FbxMesh *mesh, const FbxTime &time)
    {
        const auto* deformer = dynamic_cast<FbxVertexCacheDeformer*>(mesh->GetDeformer(0, FbxDeformer::eVertexCache));

        if (deformer->Type.Get() != FbxVertexCacheDeformer::eVertexCache)
        {
            return;
        }

        FbxCache* cache = deformer->GetCache();
        const int channel_index = cache->GetChannelIndex(deformer->Channel.Get());
        const unsigned int vertex_count = mesh->GetControlPointsCount();

        unsigned int length = 0;
        cache->Read(nullptr, length, FBXSDK_TIME_ZERO, channel_index);
        if (length != vertex_count * 3)
        {
            return;
        }

        float* buf;
        const bool res = cache->Read(&buf, length, time, channel_index);
        if (res)
        {
            unsigned int index = 0;

            while (index < vertex_count)
            {
                m_vertexBuffer.GetMappedBuffer()[index].position = DirectX::XMFLOAT3(buf[index * 3 + 0], buf[index * 3 + 1], buf[index * 3 + 2]);
                index++;
            }
        }
    }

    void FBXModel::ComputeShapeDeformation(FbxMesh *mesh, const FbxTime &time, FbxAnimLayer *animLayer)
    {
        const int vertex_count = mesh->GetControlPointsCount();
        const int blend_shape_count = mesh->GetDeformerCount(FbxDeformer::eBlendShape);

        for (int i = 0; i < blend_shape_count; ++i)
        {
            auto* blend_shape = reinterpret_cast<FbxBlendShape *>(mesh->GetDeformer(i, FbxDeformer::eBlendShape));

            const int blend_shape_channel_count = blend_shape->GetBlendShapeChannelCount();
            for (int j = 0; j < blend_shape_channel_count; ++j)
            {
                FbxBlendShapeChannel* channel = blend_shape->GetBlendShapeChannel(j);

                if (!channel)
                {
                    continue;
                }

                FbxAnimCurve* curve = mesh->GetShapeChannel(i, j, animLayer);
                if (!curve)
                {
                    continue;
                }

                double weight = curve->Evaluate(time);

                const int shape_count = channel->GetTargetShapeCount();
                const double* full_weights = channel->GetTargetShapeFullWeights();

                // firstとsecondの間の補間を計算する
                int first_index = -1;
                int second_index = -1;
                for (int k = 0; k < shape_count; ++k)
                {
                    if (weight > 0 && weight <= full_weights[0])
                    {
                        second_index = 0;
                        break;
                    }

                    if (weight > full_weights[k] && weight <= full_weights[k + 1])
                    {
                        first_index = k;
                        second_index = k + 1;
                        break;
                    }
                }

                const FbxShape* first_shape = nullptr;
                const FbxShape* second_shape = nullptr;
                if (first_index >= 0)
                {
                    first_shape = channel->GetTargetShape(first_index);
                }
                if (second_index >= 0)
                {
                    second_shape = channel->GetTargetShape(second_index);
                }

                if (first_index == -1 && second_shape != nullptr)
                {
                    const double second_weight = full_weights[0];
                    weight = weight / second_weight * 100;

                    for (int k = 0; k < vertex_count; ++k)
                    {
                        const DirectX::XMFLOAT3 current = m_vertices[k].position;
                        FbxVector4 second_fbx_vertex = second_shape->GetControlPoints()[k];

                        const float x = current.x + (second_fbx_vertex[0] - current.x) * weight / 100;
                        const float y = current.y + (second_fbx_vertex[1] - current.y) * weight / 100;
                        const float z = current.z + (second_fbx_vertex[2] - current.z) * weight / 100;

                        m_vertexBuffer.GetMappedBuffer()[k].position = DirectX::XMFLOAT3(x, y, z);
                    }
                }
                else if (first_index >= 0 && second_index >= 0)
                {
                    const double first_weight = full_weights[first_index];
                    const double second_weight = full_weights[second_index];
                    weight = (weight - first_weight) / (second_weight - first_weight) * 100;

                    for (int k = 0; k < vertex_count; ++k)
                    {
                        const DirectX::XMFLOAT3 current = m_vertices[k].position;
                        FbxVector4 first_fbx_vertex = first_shape->GetControlPoints()[k];
                        FbxVector4 second_fbx_vertex = second_shape->GetControlPoints()[k];

                        const float x = current.x + (second_fbx_vertex[0] - first_fbx_vertex[0]) * weight / 100;
                        const float y = current.y + (second_fbx_vertex[1] - first_fbx_vertex[1]) * weight / 100;
                        const float z = current.z + (second_fbx_vertex[2] - first_fbx_vertex[2]) * weight / 100;

                        m_vertexBuffer.GetMappedBuffer()[k].position = DirectX::XMFLOAT3(x, y, z);
                    }
                }
            }
        }
    }

    void FBXModel::ComputeSkinDeformation(
        const FbxMesh *mesh, FbxTime &time, FbxAMatrix &global_position, FbxPose *pose)
    {
        const auto skin_deformer = reinterpret_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin));
        const FbxSkin::EType type = skin_deformer->GetSkinningType();

        switch (type)
        {
        case FbxSkin::eLinear:
        case FbxSkin::eRigid:
            ComputeLinearDeformation(mesh, time, global_position, pose);
            break;

        case FbxSkin::eDualQuaternion:
            ComputeDualQuaternionDeformation(mesh, time, global_position, pose);
            break;

        case FbxSkin::eBlend:
            int vertex_count = mesh->GetControlPointsCount();

            std::vector<FbxVector4> linear;
            std::vector<FbxVector4> dual_quaternion;

            ComputeLinearDeformation(mesh, time, global_position, pose, &linear);
            ComputeDualQuaternionDeformation(mesh, time, global_position, pose, &dual_quaternion);

            int blend_weights_count = skin_deformer->GetControlPointIndicesCount();
            for (int i = 0; i < blend_weights_count; ++i)
            {
                double weight = skin_deformer->GetControlPointBlendWeights()[i];
                FbxVector4 new_fbx = dual_quaternion[i] * weight + linear[i] * (1.0 - weight);
                m_vertexBuffer.GetMappedBuffer()[i].position = DirectX::XMFLOAT3(new_fbx[0], new_fbx[1], new_fbx[2]);
            }

        }
    }

    void FBXModel::ComputeLinearDeformation(
        const FbxMesh *mesh,
        const FbxTime &time,
        const FbxAMatrix &global_position,
        FbxPose *pose,
        std::vector<FbxVector4> *dst
    )
    {
        const FbxCluster::ELinkMode cluster_mode =
            reinterpret_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin))
                ->GetCluster(0)
                ->GetLinkMode();

        const int vertex_count = mesh->GetControlPointsCount();

        std::vector<FbxAMatrix> cluster_deformation(vertex_count);
        for (auto && deformation : cluster_deformation)
        {
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    deformation[i][j] = 0.0;
                }
            }
        }

        std::vector<double> cluster_weights(vertex_count);

        if (cluster_mode == FbxCluster::eAdditive)
        {
            for (int i = 0; i < vertex_count; ++i)
            {
                cluster_deformation[i].SetIdentity();
            }
        }

        std::vector<int> counts(vertex_count);
        std::vector<double> weights(vertex_count, 0);

        const int skin_count = mesh->GetDeformerCount(FbxDeformer::eSkin);
        for (int i = 0; i < skin_count; ++i)
        {
            auto* skin_deformer = reinterpret_cast<FbxSkin*>(mesh->GetDeformer(i, FbxDeformer::eSkin));

            const int cluster_count = skin_deformer->GetClusterCount();
            for (int j = 0; j < cluster_count; ++j)
            {
                FbxCluster* cluster = skin_deformer->GetCluster(j);

                if (!cluster->GetLink())
                {
                    continue;
                }

                FbxAMatrix vertex_trasnsform_matrix;
                ComputeClusterDeformation(mesh, time, global_position, pose, cluster, vertex_trasnsform_matrix);


                const int index_count = cluster->GetControlPointIndicesCount();
                for (int k = 0; k < index_count; ++k)
                {
                    const int index = cluster->GetControlPointIndices()[k];
                    counts[index] += 1;

                    if (index >= vertex_count)
                    {
                        continue;
                    }

                    const double weight = cluster->GetControlPointWeights()[k];
                    weights[index] += weight;

                    if (weight == 0.0)
                    {
                        continue;
                    }

                    FbxAMatrix influence = vertex_trasnsform_matrix;
                    FbxMatrixScale(influence, weight);

                    if (cluster_mode == FbxCluster::eAdditive)
                    {
                        FbxMatrixAddDiagonal(influence, 1.0 - weight);
                        cluster_deformation[index] = influence * cluster_deformation[index];
                        cluster_weights[index] = 1.0;
                    }
                    else
                    {
                        FbxMatrixAdd(cluster_deformation[index], influence);
                        cluster_weights[index] += weight;
                    }
                }
            }
        }

        if (dst != nullptr)
        {
            dst->resize(vertex_count);
        }

        for (int i = 0; i < vertex_count; ++i)
        {
            const DirectX::XMFLOAT3 current = m_vertices[i].position;
            FbxVector4 current_fbx = FbxVector4(current.x, current.y, current.z, 1.0);
            FbxVector4 new_fbx = current_fbx;
            const double weight = cluster_weights[i];

            if (weight == 0.0)
            {
                continue;
            }

            new_fbx = cluster_deformation[i].MultT(current_fbx);
            if (cluster_mode == FbxCluster::eNormalize)
            {
                new_fbx /= weight;
            }
            else if (cluster_mode == FbxCluster::eTotalOne)
            {
                current_fbx *= 1.0 - weight;
                new_fbx += current_fbx;
            }

            if (dst != nullptr)
            {
                dst->at(i) = new_fbx;
            }
            else
            {
                m_vertexBuffer.GetMappedBuffer()[i].position = DirectX::XMFLOAT3(new_fbx[0], new_fbx[1], new_fbx[2]);
            }

        }
    }

    void FBXModel::ComputeDualQuaternionDeformation(
        const FbxMesh *mesh,
        const FbxTime &time,
        const FbxAMatrix &global_position,
        FbxPose *pose,
        std::vector<FbxVector4> *dst
    )
    {
        const FbxCluster::ELinkMode cluster_mode =
            reinterpret_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin))
                ->GetCluster(0)
                ->GetLinkMode();

        const int vertex_count = mesh->GetControlPointsCount();
        const int skin_count = mesh->GetDeformerCount(FbxDeformer::eSkin);

        std::vector<FbxDualQuaternion> cluster_deformation(vertex_count);
        std::vector<double> cluster_weights(vertex_count);

        for (int i = 0; i < skin_count; ++i)
        {
            FbxSkin* skin_deformer = reinterpret_cast<FbxSkin*>(mesh->GetDeformer(i, FbxDeformer::eSkin));

            const int cluster_count = skin_deformer->GetClusterCount();
            for (int j = 0; j < cluster_count; ++j)
            {
                FbxCluster* cluster = skin_deformer->GetCluster(j);

                if (!cluster->GetLink())
                {
                    continue;
                }

                FbxAMatrix vertex_transform_matrix;
                ComputeClusterDeformation(mesh, time, global_position, pose, cluster, vertex_transform_matrix);

                FbxQuaternion q = vertex_transform_matrix.GetQ();
                FbxVector4 t = vertex_transform_matrix.GetT();
                FbxDualQuaternion dq(q, t);

                const int index_count = cluster->GetControlPointIndicesCount();
                for (int k = 0; k < index_count; ++k)
                {
                    const int index = cluster->GetControlPointIndices()[k];

                    if (index >= vertex_count)
                    {
                        continue;
                    }

                    const double weight = cluster->GetControlPointWeights()[k];

                    if (weight == 0.0)
                    {
                        continue;
                    }

                    FbxDualQuaternion influence = dq * weight;

                    if (cluster_mode == FbxCluster::eAdditive)
                    {
                        cluster_deformation[index] = influence;
                        cluster_weights[index] = index;
                    }
                    else
                    {
                        if (j == 0)
                        {
                            cluster_deformation[index] = influence;
                        }
                        else
                        {
                            const double sign = cluster_deformation[index].GetFirstQuaternion().DotProduct(dq.GetFirstQuaternion());
                            if (sign >= 0.0)
                            {
                                cluster_deformation[index] = cluster_deformation[index] + influence;
                            }
                            else
                            {
                                cluster_deformation[index] = cluster_deformation[index] - influence;
                            }
                        }

                        cluster_weights[index] += weight;
                    }
                }
            }
        }

        if (dst != nullptr)
        {
            dst->resize(vertex_count);
        }

        for (int i = 0; i < vertex_count; ++i)
        {
            const DirectX::XMFLOAT3 current = m_vertices[i].position;
            FbxVector4 current_fbx = FbxVector4(current.x, current.y, current.z, 1.0);
            FbxVector4 new_fbx = current_fbx;
            const double weight = cluster_weights[i];

            if (weight == 0.0)
            {
                continue;
            }

            cluster_deformation[i].Normalize();
            new_fbx = cluster_deformation[i].Deform(new_fbx);

            if (cluster_mode == FbxCluster::eNormalize)
            {
                new_fbx /= weight;
            }
            else if (cluster_mode == FbxCluster::eTotalOne)
            {
                current_fbx *= 1.0 - weight;
                new_fbx += current_fbx;
            }


            if (dst != nullptr)
            {
                dst->at(i) = new_fbx;
            }
            else
            {
                m_vertexBuffer.GetMappedBuffer()[i].position = DirectX::XMFLOAT3(new_fbx[0], new_fbx[1], new_fbx[2]);
            }
        }
    }

    void FBXModel::ComputeClusterDeformation(
        const FbxMesh *mesh,
        const FbxTime &time,
        const FbxAMatrix &global_position,
        FbxPose *pose,
        FbxCluster *cluster,
        FbxAMatrix &vertex_transform_matrix
    )
    {
        const FbxCluster::ELinkMode cluster_mode = cluster->GetLinkMode();

        if (cluster_mode == FbxCluster::eAdditive && cluster->GetAssociateModel())
        {
            FbxAMatrix associate_global_init_position;
            cluster->GetTransformAssociateModelMatrix(associate_global_init_position);
            const FbxAMatrix associate_geometry_offset = GetGeometryOffset(cluster->GetAssociateModel());
            associate_global_init_position *= associate_geometry_offset;
            const FbxAMatrix associate_global_position = GetGlobalPosition(cluster->GetAssociateModel(), time, pose);

            FbxAMatrix model_global_init_position;
            cluster->GetTransformMatrix(model_global_init_position);
            const FbxAMatrix model_geometry_offset = GetGeometryOffset(mesh->GetNode());
            model_global_init_position *= model_geometry_offset;

            FbxAMatrix cluster_global_init_position;
            cluster->GetTransformLinkMatrix(cluster_global_init_position);
            const FbxAMatrix cluster_geometry_offset = GetGeometryOffset(cluster->GetLink());
            cluster_global_init_position *= cluster_geometry_offset;
            const FbxAMatrix cluster_global_position = cluster_global_init_position;

            // M^{-1} * A * AC^{-1} * CC * C^{-1} * M
            vertex_transform_matrix
                = model_global_init_position.Inverse()
                * associate_global_init_position
                * associate_global_position.Inverse()
                * cluster_global_position
                * cluster_global_init_position
                * model_global_init_position;
        }
        else
        {
            FbxAMatrix model_global_init_position;
            cluster->GetTransformMatrix(model_global_init_position);
            const FbxAMatrix model_global_position = global_position;
            const FbxAMatrix model_geometry_offset = GetGeometryOffset(mesh->GetNode());
            model_global_init_position *= model_geometry_offset;

            FbxAMatrix cluster_global_init_position;
            cluster->GetTransformLinkMatrix(cluster_global_init_position);
            const FbxAMatrix cluster_global_position = GetGlobalPosition(cluster->GetLink(), time, pose);
            const FbxAMatrix cluster_relative_init_position = cluster_global_init_position.Inverse() * model_global_init_position;
            const FbxAMatrix cluster_relative_position_inverse = model_global_position.Inverse() * cluster_global_position;

            vertex_transform_matrix = cluster_relative_position_inverse * cluster_relative_init_position;
        }
    }
} // AquaEngine