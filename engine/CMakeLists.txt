cmake_minimum_required(VERSION 3.29)
project(aqua-engine CXX)

set(CMAKE_CXX_STANDARD 20)

set(SRC
        src/directx/wrapper/Device.cpp
        src/directx/wrapper/Factory.cpp
)

set(INCLUDE
        include/directx/wrapper/Device.h
        include/directx/wrapper/Factory.h
        include/directx/Util.h
)

add_library(aqua-engine STATIC ${SRC} ${INCLUDE})