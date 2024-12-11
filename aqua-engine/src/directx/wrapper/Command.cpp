#ifndef UNICODE
#define UNICODE
#endif

#include "directx/wrapper/Command.h"
#include "directx/Util.h"
#include "directx/wrapper/Device.h"

Command::Command()
{
    CreateCommandAllocator();
    CreateCommandQueue();
    CreateCommandList();
}

Command::~Command()
{
    SafeRelease(&m_commandQueue);
    SafeRelease(&m_commandAllocator);
    SafeRelease(&m_commandList);
}

HRESULT Command::Execute()
{
    HRESULT hr = m_commandList->Close();
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to close command list.\n");
        return hr;
    }
    
    ID3D12CommandList* commandLists[] = { m_commandList };
    m_commandQueue->ExecuteCommandLists(1, commandLists);
    
    hr = m_commandQueue->Signal(m_fence.Get(), m_fence.IncrementValue());
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to signal fence.\n");
        return hr;
    }
    
    hr = m_fence.Wait();
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to wait for fence.\n");
        return hr;
    }
    
    hr = m_commandAllocator->Reset();
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to reset command allocator.\n");
        return hr;
    }
    
    hr = m_commandList->Reset(m_commandAllocator, nullptr);
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to reset command list.\n");
        return hr;
    }
    
    return S_OK;
}

HRESULT Command::CreateCommandAllocator()
{
    HRESULT hr = Device::Get()->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&m_commandAllocator)
    );
    if (FAILED(hr))
    {
        MessageBox(
            nullptr,
            L"Failed to create command allocator.",
            L"Error",
            MB_OK
        );
        return hr;
    }
    
    return S_OK;
}

HRESULT Command::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC desc = {};
    
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0; // only single gpu
    
    HRESULT hr = Device::Get()->CreateCommandQueue(
        &desc,
        IID_PPV_ARGS(&m_commandQueue)
    );
    if (FAILED(hr))
    {
        MessageBox(
            nullptr,
            L"Failed to create command queue.",
            L"Error",
            MB_OK
        );
        return hr;
    }
    
    return S_OK;
}

HRESULT Command::CreateCommandList()
{
    HRESULT hr = Device::Get()->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_commandAllocator,
        nullptr,
        IID_PPV_ARGS(&m_commandList)
    );
    if (FAILED(hr))
    {
        MessageBox(
            nullptr,
            L"Failed to create command list.",
            L"Error",
            MB_OK
        );
        return hr;
    }
    
    return S_OK;
}