cmake_minimum_required(VERSION 3.29)
project(aqua-engine CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC
        src/directx/wrapper/Device.cpp
        src/directx/wrapper/Factory.cpp
        src/directx/wrapper/Command.cpp
        src/directx/wrapper/Fence.cpp
        src/directx/wrapper/SwapChain.cpp
        src/directx/wrapper/DescriptorHeap.cpp
        src/directx/descriptor_heap/DescriptorHeapSegment.cpp
        src/directx/descriptor_heap/DescriptorHeapSegmentManager.cpp
        src/directx/descriptor_heap/GlobalDescriptorHeap.cpp
        src/directx/descriptor_heap/ShaderGlobalDescriptorHeap.cpp
        src/directx/descriptor_heap/GlobalDescriptorHeapManager.cpp
        src/directx/buffer/Buffer.cpp
        src/directx/buffer/BufferView.cpp
        src/directx/buffer/RenderTargetView.cpp
        src/directx/BackBuffers.cpp
        src/directx/wrapper/Barrier.cpp
        src/directx/buffer/GPUBuffer.cpp
        src/directx/buffer/ConstantBufferView.cpp
        src/directx/buffer/DepthStencilView.cpp
        src/directx/buffer/ShaderResourceView.cpp
        src/directx/mesh/Polygon.cpp
        src/directx/mesh/Triangle.cpp
        src/directx/ShaderObject.cpp
        src/directx/RootSignature.cpp
        src/directx/PipelineState.cpp
        src/directx/Display.cpp
        src/directx/mesh/Rectangle.cpp
        src/directx/TextureManager.cpp
        src/directx/mesh/RectangleTexture.cpp
        src/directx/mesh/Mesh.cpp
        src/directx/Camera.cpp
        src/fbx/FbxManager.cpp
        src/directx/mesh/fbx/FBXModel.cpp
        src/directx/light/DirectionLight.cpp
        src/directx/mesh/SkyBox.cpp
        src/directx/RenderTarget.cpp
        src/directx/light/PointLight.cpp
        src/directx/TransformMatrix.cpp
        src/directx/light/SpotLight.cpp
        src/directx/ComputePipeline.cpp
        src/directx/buffer/UnorderedAccessView.cpp
)

set(INCLUDE
        include/directx/wrapper/Device.h
        include/directx/wrapper/Factory.h
        include/directx/Util.h
        include/directx/wrapper/Command.h
        include/directx/wrapper/Fence.h
        include/directx/wrapper/SwapChain.h
        include/directx/wrapper/DescriptorHeap.h
        include/directx/descriptor_heap/DescriptorHeapSegment.h
        include/directx/descriptor_heap/DescriptorHeapSegmentManager.h
        include/directx/descriptor_heap/GlobalDescriptorHeap.h
        include/directx/descriptor_heap/ShaderGlobalDescriptorHeap.h
        include/directx/descriptor_heap/GlobalDescriptorHeapManager.h
        include/directx/descriptor_heap/HeapID.h
        include/directx/buffer/Buffer.h
        include/directx/buffer/BufferView.h
        include/directx/buffer/RenderTargetView.h
        include/directx/BackBuffers.h
        include/directx/wrapper/Barrier.h
        include/directx/buffer/GPUBuffer.h
        include/directx/buffer/ConstantBufferView.h
        include/directx/buffer/DepthStencilView.h
        include/directx/buffer/ShaderResourceView.h
        include/directx/mesh/Polygon.h
        include/directx/mesh/Triangle.h
        include/directx/ShaderObject.h
        include/directx/RootSignature.h
        include/directx/PipelineState.h
        include/directx/Display.h
        include/directx/mesh/Rectangle.h
        include/directx/TextureManager.h
        include/directx/mesh/RectangleTexture.h
        include/directx/mesh/Mesh.h
        include/directx/Camera.h
        include/fbx/FbxManager.h
        include/directx/mesh/fbx/FBXModel.h
        include/directx/light/DirectionLight.h
        include/directx/mesh/SkyBox.h
        include/directx/RenderTarget.h
        include/directx/light/PointLight.h
        include/directx/TransformMatrix.h
        include/directx/light/SpotLight.h
        include/directx/ComputePipeline.h
        include/directx/buffer/UnorderedAccessView.h
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")

message(STATUS "FBXSDK_DIR: $ENV{FBXSDK_DIR}")

find_package(directxtex CONFIG REQUIRED)

add_library(aqua-engine STATIC ${SRC} ${INCLUDE})
target_include_directories(aqua-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(aqua-engine PUBLIC $ENV{FBXSDK_DIR}/include)
target_link_libraries(aqua-engine PUBLIC d3d12.lib)
target_link_libraries(aqua-engine PUBLIC dxgi.lib)
target_link_libraries(aqua-engine PUBLIC d3dcompiler.lib)
target_link_libraries(aqua-engine PUBLIC Microsoft::DirectXTex)

if (LOCAL_BUILD)
    target_link_libraries(aqua-engine PUBLIC $ENV{FBXSDK_DIR}/lib/x64/debug/libfbxsdk-md.lib)
    target_link_libraries(aqua-engine PUBLIC $ENV{FBXSDK_DIR}/lib/x64/debug/libxml2-md.lib)
    target_link_libraries(aqua-engine PUBLIC $ENV{FBXSDK_DIR}/lib/x64/debug/zlib-md.lib)
    set_property(TARGET aqua-engine PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
else ()
    target_link_libraries(aqua-engine PUBLIC $ENV{FBXSDK_DIR}/lib/x64/release/libfbxsdk-md.lib)
    target_link_libraries(aqua-engine PUBLIC $ENV{FBXSDK_DIR}/lib/x64/release/libxml2-md.lib)
    target_link_libraries(aqua-engine PUBLIC $ENV{FBXSDK_DIR}/lib/x64/release/zlib-md.lib)
    set_property(TARGET aqua-engine PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif ()