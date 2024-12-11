cmake_minimum_required(VERSION 3.29)
project(aqua-engine CXX)

set(CMAKE_CXX_STANDARD 20)

set(SRC
        src/directx/wrapper/Device.cpp
        src/directx/wrapper/Factory.cpp
        src/directx/wrapper/Command.cpp
        src/directx/wrapper/Fence.cpp
        src/directx/wrapper/SwapChain.cpp
)

set(INCLUDE
        include/directx/wrapper/Device.h
        include/directx/wrapper/Factory.h
        include/directx/Util.h
        include/directx/wrapper/Command.h
        include/directx/wrapper/Fence.h
        include/directx/wrapper/SwapChain.h
)

add_library(aqua-engine STATIC ${SRC} ${INCLUDE})
target_include_directories(aqua-engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(aqua-engine INTERFACE $ENV{Windows_SDK_Lib}/d3d12.lib)
target_link_libraries(aqua-engine INTERFACE $ENV{Windows_SDK_Lib}/dxgi.lib)