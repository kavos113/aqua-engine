#ifndef APPLICATION_H
#define APPLICATION_H

#include <windows.h>

#include "AquaEngine.h"

#define TIMER_MODEL1 1
#define TIMER_MODEL2 2

class Graphics {
public:
    Graphics(HWND hwnd, RECT rc);

    ~Graphics()
    {
        AquaEngine::FBXManager::Shutdown();

        AquaEngine::GlobalDescriptorHeapManager::Shutdown();
        AquaEngine::Device::Shutdown();
        AquaEngine::Factory::Shutdown();
    }

    void SetUp();
    void Render();
    void Timer(int id) const;
    void MoveCamera(float dx, float dy);
private:
    HWND hwnd;
    RECT rc;

    std::unique_ptr<AquaEngine::Command> command{};
    std::unique_ptr<AquaEngine::Display> display{};
    std::unique_ptr<AquaEngine::FBXModel> model{};
    std::unique_ptr<AquaEngine::FBXModel> model2{};
    AquaEngine::RootSignature rootSignature;
    AquaEngine::PipelineState pipelineState;
    AquaEngine::Camera camera;
    AquaEngine::DirectionLight directionLight;
};



#endif //APPLICATION_H
