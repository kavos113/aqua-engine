#include "../../include/directx/TextureManager.h"

#include <tchar.h>
#include <DirectXTex.h>

#include "directx/Util.h"
#include "directx/buffer/GPUBuffer.h"
#include "directx/wrapper/Barrier.h"
#include "directx/wrapper/Device.h"

Buffer TextureManager::LoadTextureFromFile(const std::string &filename, Command &command)
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
        OutputDebugString(_T("Failed to load texture from file: "));
        OutputDebugString(filename.c_str());
        OutputDebugString("\n");
        return {};
    }

    const DirectX::Image *image = scratchImage.GetImage(0, 0, 0);

    GPUBuffer<uint8_t> upload_buffer;
    hr = upload_buffer.Create(BUFFER_DEFAULT(AlignmentSize(image->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT)));
    if (FAILED(hr))
    {
        OutputDebugString(_T("Failed to create upload buffer\n"));
        return {};
    }

    Buffer texture_buffer;
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
    hr = texture_buffer.Create(
        Buffer::HeapProperties::Default(),
        D3D12_HEAP_FLAG_NONE,
        desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr
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

    D3D12_TEXTURE_COPY_LOCATION src = {
        .pResource = upload_buffer.GetBuffer(),
        .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT
    };

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
    UINT num_rows;
    UINT64 row_size;
    UINT64 upload_size;
    D3D12_RESOURCE_DESC texture_desc = texture_buffer.GetBuffer()->GetDesc();
    Device::Get()->GetCopyableFootprints(
        &texture_desc,
        0,
        1,
        0,
        &footprint,
        &num_rows,
        &row_size,
        &upload_size
    );

    src.PlacedFootprint = footprint;
    src.PlacedFootprint.Offset = 0;
    src.PlacedFootprint.Footprint.Width = static_cast<UINT>(metadata.width);
    src.PlacedFootprint.Footprint.Height = static_cast<UINT>(metadata.height);
    src.PlacedFootprint.Footprint.Depth = static_cast<UINT>(metadata.depth);
    src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(AlignmentSize(image->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));
    src.PlacedFootprint.Footprint.Format = image->format;

    D3D12_TEXTURE_COPY_LOCATION dst = {
        .pResource = texture_buffer.GetBuffer(),
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
        texture_buffer.GetBuffer(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );

    hr = command.Execute();
    if (FAILED(hr))
    {
        OutputDebugString(_T("Failed to execute command\n"));
        return {};
    }

    m_resourceTable[filename] = texture_buffer;

    return texture_buffer;
}
