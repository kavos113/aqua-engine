#include "directx/wrapper/Factory.h"

namespace AquaEngine
{
    Microsoft::WRL::ComPtr<IDXGIFactory6> Factory::m_dxgiFactory = nullptr;

}
