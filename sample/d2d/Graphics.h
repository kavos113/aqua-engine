#ifndef APPLICATION_H
#define APPLICATION_H

#include "AquaEngine.h"

#include <windows.h>
#include <d3d11.h>
#include <d3d11on12.h>
#include <d2d1_3.h>
#include <dwrite_3.h>

#define TIMER_MODEL1 1
#define TIMER_MODEL2 2

#define WM_AQUA_LOADING 0x0401

class Graphics
{
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

    void OnPress();

    struct Progress
    {
        float progress;
        const wchar_t *text;
    };

private:
    HWND hwnd;
    RECT rc;

    struct Weight
    {
        float weight;
    };

    std::unique_ptr<AquaEngine::Command> command{};
    std::unique_ptr<AquaEngine::Display> display{};
    std::unique_ptr<AquaEngine::FBXModel> model{};
    std::unique_ptr<AquaEngine::FBXModel> model2{};
    std::unique_ptr<AquaEngine::SkyBox> skybox{};
    std::shared_ptr<AquaEngine::Camera> camera{};
    AquaEngine::RootSignature rootSignature;
    AquaEngine::PipelineState pipelineState;
    AquaEngine::DirectionLight directionLight;

    AquaEngine::RenderTarget model_rt;
    AquaEngine::RootSignature rt_rootSignature;
    AquaEngine::PipelineState rt_pipelineState;
    AquaEngine::RenderTarget cubemap_rt;

    AquaEngine::GPUBuffer<Weight> weightBuffer;
    AquaEngine::ConstantBufferView weightCBV;

    float weight = 0.0f;
    bool isChangingScene = false;

    Microsoft::WRL::ComPtr<ID3D11On12Device> d3d11On12Device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d11DeviceContext;
    Microsoft::WRL::ComPtr<ID2D1Factory3> d2dFactory;
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    Microsoft::WRL::ComPtr<ID2D1Device2> d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext2> d2dDeviceContext;
    Microsoft::WRL::ComPtr<IDWriteFactory> dwriteFactory;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> d2dBrush;
    Microsoft::WRL::ComPtr<IDWriteTextFormat> dwriteTextFormat;
    std::vector<Microsoft::WRL::ComPtr<ID3D11Resource>> wrappedBackBuffers;
    std::vector<Microsoft::WRL::ComPtr<ID2D1Bitmap1>> d2dRenderTargets;

    void InitD2D();
    void RenderD2D();
};


#endif //APPLICATION_H
