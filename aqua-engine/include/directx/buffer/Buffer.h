#ifndef AQUA_BUFFER_H
#define AQUA_BUFFER_H

#include <d3d12.h>

namespace AquaEngine
{
    class Buffer
    {
    public:
        Buffer();
        Buffer(ID3D12Resource* buffer);
        virtual ~Buffer();

        virtual HRESULT Create(
            D3D12_HEAP_PROPERTIES heapProperties,
            D3D12_HEAP_FLAGS heapFlags,
            D3D12_RESOURCE_DESC resourceDesc,
            D3D12_RESOURCE_STATES resourceState,
            D3D12_CLEAR_VALUE* clearValue);

        [[nodiscard]] ID3D12Resource* GetBuffer() const;

        struct HeapProperties
        {
            static D3D12_HEAP_PROPERTIES Default()
            {
                return D3D12_HEAP_PROPERTIES {
                    .Type = D3D12_HEAP_TYPE_DEFAULT,
                    .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
                    .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
                    .CreationNodeMask = 1,
                    .VisibleNodeMask = 1
                };
            }

            static D3D12_HEAP_PROPERTIES Upload()
            {
                return D3D12_HEAP_PROPERTIES {
                    .Type = D3D12_HEAP_TYPE_UPLOAD,
                    .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
                    .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
                    .CreationNodeMask = 1,
                    .VisibleNodeMask = 1
                };
            }
        };

        struct ResourceDesc
        {
            static D3D12_RESOURCE_DESC Buffer(UINT64 size)
            {
                return D3D12_RESOURCE_DESC {
                    .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
                    .Alignment = 0,
                    .Width = size,
                    .Height = 1,
                    .DepthOrArraySize = 1,
                    .MipLevels = 1,
                    .Format = DXGI_FORMAT_UNKNOWN,
                    .SampleDesc = {
                        .Count = 1,
                        .Quality = 0
                            },
                    .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
                    .Flags = D3D12_RESOURCE_FLAG_NONE
                };
            }

            static D3D12_RESOURCE_DESC DepthStencil(UINT width, UINT height)
            {
                return D3D12_RESOURCE_DESC {
                    .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
                    .Alignment = 0,
                    .Width = width,
                    .Height = height,
                    .DepthOrArraySize = 1,
                    .MipLevels = 1,
                    .Format = DXGI_FORMAT_R32_TYPELESS,
                    .SampleDesc = {
                        .Count = 1,
                        .Quality = 0
                            },
                    .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
                    .Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
                };
            }
        };

    protected:
        ID3D12Resource* m_Buffer;
    };

    #define BUFFER_DEFAULT(size) Buffer::HeapProperties::Upload(), D3D12_HEAP_FLAG_NONE, Buffer::ResourceDesc::Buffer(size), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr

}
#endif //AQUA_BUFFER_H
