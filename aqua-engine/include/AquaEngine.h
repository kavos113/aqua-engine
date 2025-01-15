#ifndef AQUA_AQUAENGINE_H
#define AQUA_AQUAENGINE_H

#include "directx/descriptor_heap/GlobalDescriptorHeapManager.h"
#include "directx/wrapper/Device.h"
#include "directx/wrapper/Factory.h"
#include "directx/wrapper/Command.h"
#include "directx/Display.h"
#include "directx/BackBuffers.h"
#include "directx/PipelineState.h"
#include "directx/RootSignature.h"
#include "directx/ShaderObject.h"
#include "directx/Camera.h"
#include "directx/RenderTarget.h"
#include "directx/light/DirectionLight.h"
#include "directx/light/PointLight.h"
#include "directx/light/SpotLight.h"
#include "directx/ComputePipeline.h"

#include "directx/mesh/Triangle.h"
#include "directx/mesh/Rectangle.h"
#include "directx/mesh/RectangleTexture.h"
#include "directx/mesh/fbx/FBXModel.h"
#include "directx/mesh/SkyBox.h"

#include "directx/buffer/ConstantBufferView.h"
#include "directx/buffer/DepthStencilView.h"
#include "directx/buffer/RenderTargetView.h"
#include "directx/buffer/ShaderResourceView.h"
#include "directx/buffer/UnorderedAccessView.h"
#include "directx/buffer/GPUBuffer.h"
#include "directx/buffer/Buffer.h"

#include "fbx/FBXManager.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#endif //AQUA_AQUAENGINE_H
