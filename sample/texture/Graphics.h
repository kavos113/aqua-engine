#ifndef APPLICATION_H
#define APPLICATION_H

#include <windows.h>

#include "AquaEngine.h"

class Graphics {
public:
    Graphics(HWND hwnd, RECT rc);

    ~Graphics()
    {
        AquaEngine::GlobalDescriptorHeapManager::Shutdown();
        AquaEngine::Device::Shutdown();
        AquaEngine::Factory::Shutdown();
    }

    void SetUp();
    void Render();
private:
    HWND hwnd;
    RECT rc;

    std::unique_ptr<AquaEngine::Command> command{};
    std::unique_ptr<AquaEngine::Display> display{};
    std::unique_ptr<AquaEngine::RectangleTexture> rectangle{};
    AquaEngine::RootSignature rootSignature;
    AquaEngine::PipelineState pipelineState;
    AquaEngine::Camera camera;
};



#endif //APPLICATION_H
