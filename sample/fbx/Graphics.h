#ifndef APPLICATION_H
#define APPLICATION_H

#include <windows.h>

#include "../../aqua-engine/include/AquaEngine.h"

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
    void Render() const;
private:
    HWND hwnd;
    RECT rc;

    std::unique_ptr<AquaEngine::Command> command{};
    std::unique_ptr<AquaEngine::Display> display{};
    std::unique_ptr<AquaEngine::FBXModel> model{};
    AquaEngine::RootSignature rootSignature;
    AquaEngine::PipelineState pipelineState;
    AquaEngine::Camera camera;

    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
};



#endif //APPLICATION_H
