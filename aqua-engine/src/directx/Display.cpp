#include "directx/Display.h"

Display::Display(HWND hwnd, RECT rc, Command &command)
    : m_backBuffers(hwnd, rc, command)
    , m_command(&command)
    , wr(rc)
{
    CreateViewport();
}

void Display::BeginRender()
{
    m_backBuffers.BeginRender();
}

void Display::EndRender()
{
    m_backBuffers.EndRender();
}

void Display::Present()
{
    m_backBuffers.Present();
}

void Display::CreateViewport()
{
    m_viewport.Width = static_cast<float>(wr.right - wr.left);
    m_viewport.Height = static_cast<float>(wr.bottom - wr.top);
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    
    m_scissorRect.left = 0;
    m_scissorRect.top = 0;
    m_scissorRect.right = m_scissorRect.left + (wr.right - wr.left);
    m_scissorRect.bottom = m_scissorRect.top + (wr.bottom - wr.top);
}

void Display::SetViewports()
{
    m_command->List()->RSSetViewports(1, &m_viewport);
    m_command->List()->RSSetScissorRects(1, &m_scissorRect);
}