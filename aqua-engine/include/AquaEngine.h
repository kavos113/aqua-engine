#ifndef AQUA_AQUAENGINE_H
#define AQUA_AQUAENGINE_H

#include <d3d12.h>
#include <dxgi1_6.h>

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
#include "directx/MultiPassRenderer.h"
#include "directx/light/DirectionLight.h"
#include "directx/light/PointLight.h"
#include "directx/light/SpotLight.h"
#include "directx/ComputePipeline.h"

#include "directx/mesh/Triangle.h"
#include "directx/mesh/Rectangle.h"
#include "directx/mesh/RectangleTexture.h"
#include "directx/mesh/fbx/FBXModel.h"
#include "directx/mesh/SkyBox.h"

#include "fbx/FBXManager.h"

#endif //AQUA_AQUAENGINE_H
