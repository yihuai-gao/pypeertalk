cmake_minimum_required(VERSION 3.10)
project(robotmq)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

execute_process(
    COMMAND ${Python_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_path('purelib'))"
    OUTPUT_VARIABLE PYTHON_SITE_PACKAGES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_package(pybind11 REQUIRED PATHS ${PYTHON_SITE_PACKAGES}/pybind11/share/cmake/pybind11)

find_package(Threads REQUIRED)

add_subdirectory(third_party/libusbmuxd)
include_directories(third_party/libusbmuxd/include)

# Set output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SOURCES
    pypeertalk/core/src/peertalk_pybind.cpp
    pypeertalk/core/src/peertalk_client.cpp
)

pybind11_add_module(pypeertalk_core ${SOURCES})
add_compile_options(-pthread)

target_include_directories(pypeertalk_core PRIVATE third_party/libusbmuxd/include)
target_include_directories(pypeertalk_core PRIVATE pypeertalk/core/include)


set_target_properties(pypeertalk_core PROPERTIES 
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/pypeertalk/core"
)

target_link_libraries(pypeertalk_core PRIVATE usbmuxd)