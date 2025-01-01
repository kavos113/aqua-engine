#include "../../include/directx/TextureManager.h"

#include <tchar.h>
#include <DirectXTex.h>

#include "directx/Util.h"
#include "directx/buffer/GPUBuffer.h"
#include "directx/wrapper/Barrier.h"
#include "directx/wrapper/Device.h"

namespace AquaEngine
{
    std::map<std::string, Microsoft::WRL::ComPtr<ID3D12Resource>> TextureManager::m_resourceTable;

    Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::LoadTextureFromFile(const std::string &filename, Command &command)
    {
        if (m_resourceTable.contains(filename))
        {
            return m_resourceTable[filename];
        }
        DirectX::TexMetadata metadata = {};
        DirectX::ScratchImage scratchImage = {};

        HRESULT hr = LoadFromWICFile(
            GetWideString(filename).c_str(),
            DirectX::WIC_FLAGS_NONE,
            &metadata,
            scratchImage
        );
        if (FAILED(hr))
        {
            switch (hr)
            {
            case HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND):
                OutputDebugString(_T("file not found: "));
                OutputDebugString(filename.c_str());
                OutputDebugString("\n");
                break;

            case E_NOINTERFACE:
                OutputDebugString(_T("NOINTERFACE: probably forgot to run CoInitializeEx\n"));
                break;

            default:
                OutputDebugString(_T("Failed to load texture from file: "));
                OutputDebugString(filename.c_str());
                OutputDebugString("\n");
            }

            return {};
        }

        const DirectX::Image *image = scratchImage.GetImage(0, 0, 0);

        GPUBuffer<uint8_t> upload_buffer;
        hr = upload_buffer.Create(BUFFER_DEFAULT(AlignmentSize(image->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * image->height));
        if (FAILED(hr))
        {
            OutputDebugString(_T("Failed to create upload buffer\n"));
            return {};
        }

        Microsoft::WRL::ComPtr<ID3D12Resource> texture = nullptr;
        D3D12_RESOURCE_DESC desc = {
            .Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension),
            .Alignment = 0,
            .Width = metadata.width,
            .Height = static_cast<UINT>(metadata.height),
            .DepthOrArraySize = static_cast<UINT16>(metadata.arraySize),
            .MipLevels = static_cast<UINT16>(metadata.mipLevels),
            .Format = metadata.format,
            .SampleDesc = {
                .Count = 1,
                .Quality = 0
            },
            .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
            .Flags = D3D12_RESOURCE_FLAG_NONE
        };
        auto prop = Buffer::HeapProperties::Default();
        hr = Device::Get()->CreateCommittedResource(
            &prop,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&texture)
        );
        if (FAILED(hr))
        {
            OutputDebugString(_T("Failed to create texture buffer\n"));
            return {};
        }

        uint8_t* src_addr = image->pixels;
        size_t row_pitch = AlignmentSize(image->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
        uint8_t* dst_addr = upload_buffer.GetMappedBuffer();

        for (size_t i = 0; i < image->height; ++i)
        {
            std::copy_n(src_addr, row_pitch, dst_addr);

            src_addr += image->rowPitch;
            dst_addr += row_pitch;
        }

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
        UINT num_rows;
        UINT64 row_size;
        UINT64 total_size;

        D3D12_RESOURCE_DESC texture_desc = texture->GetDesc();
        Device::Get()->GetCopyableFootprints(
            &texture_desc,
            0,
            1,
            0,
            &footprint,
            &num_rows,
            &row_size,
            &total_size
            );

        D3D12_TEXTURE_COPY_LOCATION src = {
            .pResource = upload_buffer.GetBuffer().Get(),
            .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
            .PlacedFootprint = footprint
        };

        D3D12_TEXTURE_COPY_LOCATION dst = {
            .pResource = texture.Get(),
            .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
            .SubresourceIndex = 0
        };

        command.List()->CopyTextureRegion(
            &dst,
            0,
            0,
            0,
            &src,
            nullptr
        );

        Barrier::Transition(
            &command,
            texture.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );

        hr = command.Execute();
        if (FAILED(hr))
        {
            OutputDebugString(_T("Failed to execute command\n"));
            return {};
        }

        m_resourceTable[filename] = texture;

        return texture;
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::LoadTextureFromHDRFile(const std::string &filename, Command &command)
    {
        if (m_resourceTable.contains(filename))
        {
            return m_resourceTable[filename];
        }
        DirectX::TexMetadata metadata = {};
        DirectX::ScratchImage scratchImage = {};

        HRESULT hr = LoadFromHDRFile(
            GetWideString(filename).c_str(),
            &metadata,
            scratchImage
        );
        if (FAILED(hr))
        {
            if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            {
                OutputDebugString(_T("file not found: "));
                OutputDebugString(filename.c_str());
                OutputDebugString("\n");
                system("dir");
            } else
            {
                OutputDebugString(_T("Failed to load texture from file: "));
                OutputDebugString(filename.c_str());
                OutputDebugString("\n");
            }
            return {};
        }

        const DirectX::Image *image = scratchImage.GetImage(0, 0, 0);

        GPUBuffer<uint8_t> upload_buffer;
        hr = upload_buffer.Create(
            BUFFER_DEFAULT(AlignmentSize(image->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * image->height)
        );
        if (FAILED(hr))
        {
            OutputDebugString(_T("Failed to create upload buffer\n"));
            return {};
        }

        Microsoft::WRL::ComPtr<ID3D12Resource> texture = nullptr;
        D3D12_RESOURCE_DESC desc = {
            .Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension),
            .Alignment = 0,
            .Width = metadata.width,
            .Height = static_cast<UINT>(metadata.height),
            .DepthOrArraySize = static_cast<UINT16>(metadata.arraySize),
            .MipLevels = static_cast<UINT16>(metadata.mipLevels),
            .Format = metadata.format,
            .SampleDesc = {
                .Count = 1,
                .Quality = 0
            },
            .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
            .Flags = D3D12_RESOURCE_FLAG_NONE
        };
        auto prop = Buffer::HeapProperties::Default();
        hr = Device::Get()->CreateCommittedResource(
            &prop,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&texture)
        );
        if (FAILED(hr))
        {
            OutputDebugString(_T("Failed to create texture buffer\n"));
            return {};
        }

        uint8_t *src_addr = image->pixels;
        size_t row_pitch = AlignmentSize(image->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
        uint8_t *dst_addr = upload_buffer.GetMappedBuffer();

        for (size_t i = 0; i < image->height; ++i)
        {
            std::copy_n(src_addr, row_pitch, dst_addr);

            src_addr += image->rowPitch;
            dst_addr += row_pitch;
        }

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
        UINT num_rows;
        UINT64 row_size;
        UINT64 total_size;

        D3D12_RESOURCE_DESC texture_desc = texture->GetDesc();
        Device::Get()->GetCopyableFootprints(
            &texture_desc,
            0,
            1,
            0,
            &footprint,
            &num_rows,
            &row_size,
            &total_size
        );

        D3D12_TEXTURE_COPY_LOCATION src = {
            .pResource = upload_buffer.GetBuffer().Get(),
            .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
            .PlacedFootprint = footprint
        };

        D3D12_TEXTURE_COPY_LOCATION dst = {
            .pResource = texture.Get(),
            .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
            .SubresourceIndex = 0
        };

        command.List()->CopyTextureRegion(
            &dst,
            0,
            0,
            0,
            &src,
            nullptr
        );

        Barrier::Transition(
            &command,
            texture.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );

        hr = command.Execute();
        if (FAILED(hr))
        {
            OutputDebugString(_T("Failed to execute command\n"));
            return {};
        }

        m_resourceTable[filename] = texture;

        return texture;
    }

}
