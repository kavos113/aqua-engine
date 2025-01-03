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
        test/engine/directx/mesh/Polygon.cpp
)

set(LOCAL_TEST_SRC
        test/engine/directx/TextureManager.cpp
        test/engine/directx/mesh/PolygonTexture.cpp
        test/engine/directx/mesh/FBX.cpp
)

if (LOCAL_BUILD)
    file(COPY
            test/engine/directx/ps.hlsl
            test/engine/directx/vs.hlsl
            test/engine/directx/isu.png
            test/engine/directx/isu.fbx
            test/engine/directx/icon.png
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    )
else()
    file(COPY
        test/engine/directx/ps.hlsl
        test/engine/directx/vs.hlsl
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

if (LOCAL_BUILD)
    add_executable(aqua-test ${LOCAL_TEST_SRC} ${TEST_SRC})
else ()
    add_executable(aqua-test ${TEST_SRC})
endif ()
add_dependencies(aqua-test aqua-engine)
target_link_libraries(aqua-test aqua-engine gtest_main)

include(GoogleTest)

gtest_discover_tests(aqua-test)


function(create_sample name files)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sample/${name})
    set(exe_name sample-${name})

    set(src_files ${ARGV})
    list(REMOVE_AT src_files 0)
    foreach (src_file IN LISTS src_files)
        if (src_file MATCHES ".*\\.(cpp|c|cc|cxx|h|hpp)$")
            list(APPEND src_list ${src_file})
        endif ()
        if (src_file MATCHES ".*\\.(hlsl|hlsli)$")
            file(COPY ${src_file} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        endif ()
        if (LOCAL_BUILD)
            if (src_file MATCHES ".*\\.(fbx|png|hdr)$")
                file(COPY ${src_file} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
            endif ()
        endif ()
    endforeach ()

    add_executable(${exe_name} ${src_list})
    add_dependencies(${exe_name} aqua-engine)
    target_include_directories(${exe_name} PUBLIC aqua-engine/include)
    target_link_libraries(${exe_name} aqua-engine)

    message(STATUS "Successfully created sample: ${name}")
endfunction()

create_sample(display sample/display/main.cpp)

create_sample(triangle
        sample/triangle/main.cpp
        sample/triangle/vs.hlsl
        sample/triangle/ps.hlsl
)

create_sample(texture
        sample/texture/main.cpp
        sample/texture/Graphics.cpp
        sample/texture/Graphics.h
        sample/texture/vs.hlsl
        sample/texture/ps.hlsl
        sample/texture/type.hlsli
        resources/models/icon.png
)

create_sample(fbx
        sample/fbx/main.cpp
        sample/fbx/Graphics.cpp
        sample/fbx/Graphics.h
        sample/fbx/vs.hlsl
        sample/fbx/ps.hlsl
        sample/fbx/type.hlsli
        resources/models/isu.fbx
        resources/models/isu.png
        resources/models/ninja.fbx
        resources/models/ninja.png
)

create_sample(light
        sample/light/main.cpp
        sample/light/Graphics.cpp
        sample/light/Graphics.h
        sample/light/BaseWindow.h
        sample/light/MainWindow.cpp
        sample/light/MainWindow.h
        sample/light/shader.hlsl
        resources/models/isu.fbx
        resources/models/isu.png
        resources/models/ninja.fbx
        resources/models/ninja.png
)

create_sample(cubemap
        sample/cubemap/main.cpp
        aqua-engine/src/directx/mesh/skybox.hlsl
        aqua-engine/src/directx/mesh/cubemap.hlsl
        resources/hdrs/sample1.hdr
)

create_sample(collision
        sample/collision/main.cpp
        sample/collision/Graphics.cpp
        sample/collision/Graphics.h
        sample/collision/BaseWindow.h
        sample/collision/MainWindow.cpp
        sample/collision/MainWindow.h
        sample/collision/shader.hlsl
        resources/models/isu.fbx
        resources/models/isu.png
        resources/models/ninja.fbx
        resources/models/ninja.png
)

create_sample(background
        sample/background/main.cpp
        sample/background/Graphics.cpp
        sample/background/Graphics.h
        sample/background/BaseWindow.h
        sample/background/MainWindow.cpp
        sample/background/MainWindow.h
        sample/background/shader.hlsl
        aqua-engine/src/directx/mesh/skybox.hlsl
        aqua-engine/src/directx/mesh/cubemap.hlsl
        resources/hdrs/sample1.hdr
        resources/models/isu.fbx
        resources/models/isu.png
        resources/models/ninja.fbx
        resources/models/ninja.png
)