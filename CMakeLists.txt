cmake_minimum_required(VERSION 3.29)

project(aqua)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "CMAKE CURRENT SOURCE DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

if (NOT MSVC)
    message(FATAL_ERROR "This project is only supported on Windows")
endif ()

add_subdirectory(aqua-engine)

# Test
set(TEST_SRC
        test/engine/directx/wrapper/Factory.cpp
        test/engine/directx/wrapper/Device.cpp
        test/engine/directx/wrapper/Command.cpp
        test/engine/directx/wrapper/Fence.cpp
        test/engine/directx/wrapper/SwapChain.cpp
        test/engine/directx/wrapper/DescriptorHeap.cpp
        test/engine/directx/descriptor_heap/GlobalDescriptorHeap.cpp
        test/engine/directx/wrapper/Barrier.cpp
        test/engine/directx/buffer/Buffer.cpp
        test/engine/directx/buffer/GPUBuffer.cpp
        test/engine/directx/buffer/BufferView.cpp
        test/engine/directx/BackBuffers.cpp
        test/engine/directx/Display.cpp
        test/engine/directx/ShaderObject.cpp
        test/engine/directx/RootSignature.cpp
        test/engine/directx/PipelineState.cpp
)

file(COPY test/engine/directx/ps.hlsl test/engine/directx/vs.hlsl DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(aqua-test ${TEST_SRC})
add_dependencies(aqua-test aqua-engine)
target_link_libraries(aqua-test aqua-engine gtest_main)

include(GoogleTest)

gtest_discover_tests(aqua-test)

# sample
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sample/display)
add_executable(sample-display WIN32 sample/display/main.cpp)
add_dependencies(sample-display aqua-engine)
target_include_directories(sample-display PUBLIC aqua-engine/include)
target_link_libraries(sample-display aqua-engine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sample/triangle)
add_executable(sample-triangle WIN32 sample/triangle/main.cpp)
add_dependencies(sample-triangle aqua-engine)
target_include_directories(sample-triangle PUBLIC aqua-engine/include)
target_link_libraries(sample-triangle aqua-engine)
file(COPY sample/triangle/vs.hlsl sample/triangle/ps.hlsl DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message(STATUS "copy vs.hlsl and ps.hlsl to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sample/texture)
add_executable(sample-texture WIN32 sample/texture/Graphics.cpp sample/texture/Graphics.h
        sample/texture/main.cpp)
add_dependencies(sample-texture aqua-engine)
target_include_directories(sample-texture PUBLIC aqua-engine/include)
target_link_libraries(sample-texture aqua-engine)
file(COPY sample/texture/vs.hlsl sample/texture/ps.hlsl sample/texture/type.hlsli sample/texture/icon.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message(STATUS "copy vs.hlsl and ps.hlsl to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sample/fbx)
add_executable(sample-fbx WIN32 sample/fbx/main.cpp sample/fbx/Graphics.cpp sample/fbx/Graphics.h)
add_dependencies(sample-fbx aqua-engine)
target_include_directories(sample-fbx PUBLIC aqua-engine/include)
target_link_libraries(sample-fbx aqua-engine)
file(COPY sample/fbx/vs.hlsl sample/fbx/ps.hlsl sample/fbx/isu.fbx sample/fbx/isu.png sample/fbx/ninja.fbx sample/fbx/ninja.png sample/fbx/type.hlsli DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
if (LOCAL_BUILD) # fbx and png: local only
    file(COPY sample/fbx/isu.fbx sample/fbx/isu.png sample/fbx/ninja.fbx sample/fbx/ninja.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    message(STATUS "copy fbx files.")
endif ()
message(STATUS "copy vs.hlsl and ps.hlsl to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")