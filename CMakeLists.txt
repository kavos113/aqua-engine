cmake_minimum_required(VERSION 3.29)

project(aqua)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(aqua-engine)

# Test
set(TEST_SRC
        test/engine/directx/wrapper/Factory.cpp
        test/engine/directx/wrapper/Device.cpp
        test/engine/directx/wrapper/Command.cpp
        test/engine/directx/wrapper/Fence.cpp
        test/engine/directx/wrapper/SwapChain.cpp
)

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