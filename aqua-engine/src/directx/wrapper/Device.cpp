#include "directx/wrapper/Device.h"

namespace AquaEngine
{
    Microsoft::WRL::ComPtr<ID3D12Device> Device::m_device = nullptr;
    std::vector<IDXGIAdapter*> Device::m_adaptors;
}
