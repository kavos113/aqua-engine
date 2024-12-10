cmake_minimum_required(VERSION 3.29)
project(aqua CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(test)
add_subdirectory(engine)

target_link_libraries(test PRIVATE aqua-engine)