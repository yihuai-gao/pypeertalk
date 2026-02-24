cmake_minimum_required(VERSION 3.10)
project(pypeertalk)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)
# find_package(fmt REQUIRED)

add_subdirectory(third_party/libusbmuxd)
include_directories(third_party/libusbmuxd/include)

set(SOURCES
    pypeertalk/core/src/peertalk_pybind.cpp
    pypeertalk/core/src/peertalk_client.cpp
)

pybind11_add_module(pypeertalk_core ${SOURCES})
add_compile_options(-pthread)

target_include_directories(pypeertalk_core PRIVATE third_party/libusbmuxd/include)
target_include_directories(pypeertalk_core PRIVATE pypeertalk/core/include)


target_link_libraries(pypeertalk_core PRIVATE usbmuxd)