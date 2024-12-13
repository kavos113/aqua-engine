cmake_minimum_required(VERSION 3.29)
project(aqua-engine CXX)

set(CMAKE_CXX_STANDARD 20)

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
)

add_library(aqua-engine STATIC ${SRC} ${INCLUDE})
target_include_directories(aqua-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(aqua-engine INTERFACE $ENV{Windows_SDK_Lib}/d3d12.lib)
target_link_libraries(aqua-engine INTERFACE $ENV{Windows_SDK_Lib}/dxgi.lib)