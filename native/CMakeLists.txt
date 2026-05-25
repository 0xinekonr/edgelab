cmake_minimum_required(VERSION 3.20)

project(edgelab_native VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_library(edgelab_core
    src/telemetry_formatter.cpp
)

target_include_directories(edgelab_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(device_simulator
    apps/device-simulator/main.cpp
)

target_link_libraries(device_simulator
    PRIVATE
        edgelab_core
)

enable_testing()

add_executable(telemetry_formatter_test
    tests/telemetry_formatter_test.cpp
)

target_link_libraries(telemetry_formatter_test
    PRIVATE
        edgelab_core
)

add_test(
    NAME telemetry_formatter_test
    COMMAND telemetry_formatter_test
)
