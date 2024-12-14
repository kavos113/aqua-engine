#ifndef AQUA_DISPLAY_H
#define AQUA_DISPLAY_H

#include <d3d12.h>

#include "directx/BackBuffers.h"

class Display
{
    Display(HWND hwnd, RECT rc, Command& command);
    
    void BeginRender();
    void EndRender();
    void Present();
    
    void SetViewports();
private:
    BackBuffers m_backBuffers;
    Command* m_command;
    
    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
    RECT wr;
    
    void CreateViewport();
};


#endif //AQUA_DISPLAY_H
