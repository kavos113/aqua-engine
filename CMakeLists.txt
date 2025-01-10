cmake_minimum_required(VERSION 3.29)

project(aqua)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "CMAKE CURRENT SOURCE DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

if (NOT MSVC)
    message(FATAL_ERROR "This project is only supported on Windows")
endif ()

add_subdirectory(aqua-engine)

set(EFK_RUNTIME_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/vendor/effekseer/install_msvc2022_x64)

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
            resources/models/isu.png
            resources/models/isu.fbx
            resources/models/icon.png
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
            if (src_file MATCHES ".*\\.(fbx|png|hdr|efkefc)$")
                file(COPY ${src_file} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
            endif ()
        endif ()
    endforeach ()

    add_executable(${exe_name} WIN32 ${src_list})
    add_dependencies(${exe_name} aqua-engine)
    target_include_directories(${exe_name} PUBLIC aqua-engine/include)
    target_link_libraries(${exe_name} aqua-engine)
    target_link_libraries(${exe_name} d2d1.lib)
    target_link_libraries(${exe_name} dwrite.lib)

    if (LOCAL_BUILD)
        target_include_directories(${exe_name} PUBLIC
                ${EFK_RUNTIME_ROOT}/include/Effekseer
                ${EFK_RUNTIME_ROOT}/include/EffekseerRendererDX12)
        target_link_libraries(${exe_name}
                ${EFK_RUNTIME_ROOT}/lib/Effekseerd.lib
                ${EFK_RUNTIME_ROOT}/lib/EffekseerRendererDX12d.lib
                ${EFK_RUNTIME_ROOT}/lib/LLGId.lib)
        set_property(TARGET ${exe_name} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
    else ()
        set_property(TARGET ${exe_name} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    endif ()

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

create_sample(nowloading
        sample/nowloading/main.cpp
        sample/nowloading/Graphics.cpp
        sample/nowloading/Graphics.h
        sample/nowloading/BaseWindow.h
        sample/nowloading/MainWindow.cpp
        sample/nowloading/MainWindow.h
        sample/nowloading/shader.hlsl
        resources/models/isu.fbx
        resources/models/isu.png
        resources/models/ninja.fbx
        resources/models/ninja.png
)

create_sample(point-light
        sample/point-light/main.cpp
        sample/point-light/Graphics.cpp
        sample/point-light/Graphics.h
        sample/point-light/BaseWindow.h
        sample/point-light/MainWindow.cpp
        sample/point-light/MainWindow.h
        sample/point-light/shader.hlsl
        resources/models/isu.fbx
        resources/models/isu.png
        resources/models/ninja.fbx
        resources/models/ninja.png
)

create_sample(spot-light
        sample/spot-light/main.cpp
        sample/spot-light/Graphics.cpp
        sample/spot-light/Graphics.h
        sample/spot-light/BaseWindow.h
        sample/spot-light/MainWindow.cpp
        sample/spot-light/MainWindow.h
        sample/spot-light/shader.hlsl
        resources/models/isu.fbx
        resources/models/isu.png
        resources/models/ninja.fbx
        resources/models/ninja.png
)

create_sample(blend
        sample/blend/main.cpp
        sample/blend/Graphics.cpp
        sample/blend/Graphics.h
        sample/blend/BaseWindow.h
        sample/blend/MainWindow.cpp
        sample/blend/MainWindow.h
        sample/blend/shader.hlsl
        sample/blend/rect.hlsl
        resources/models/isu.fbx
        resources/models/isu.png
        resources/models/ninja.fbx
        resources/models/ninja.png
)

create_sample(blur
        sample/blur/main.cpp
        sample/blur/Graphics.cpp
        sample/blur/Graphics.h
        sample/blur/BaseWindow.h
        sample/blur/MainWindow.h
        sample/blur/MainWindow.cpp
        sample/blur/shader.hlsl
        sample/blur/rt1.hlsl
        resources/models/isu.fbx
        resources/models/isu.png
        resources/models/ninja.fbx
        resources/models/ninja.png
)

if (LOCAL_BUILD)
    create_sample(effect
            sample/effect/main.cpp
            sample/effect/Graphics.cpp
            sample/effect/Graphics.h
            sample/effect/BaseWindow.h
            sample/effect/MainWindow.h
            sample/effect/MainWindow.cpp
            sample/effect/shader.hlsl
            resources/models/isu.fbx
            resources/models/isu.png
            resources/models/ninja.fbx
            resources/models/ninja.png
            vendor/effekseer/Examples/Resources/Laser01.efkefc
    )
endif ()