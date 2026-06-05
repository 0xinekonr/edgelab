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
    src/virtual_device.cpp
    src/temperature_profile.cpp
    src/random_temperature_profile.cpp
)

target_include_directories(edgelab_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(device_simulator
    apps/device-simulator/main.cpp
)

add_library(device_simulator_support
        apps/device-simulator/simulation_config.cpp
)

target_include_directories(device_simulator_support
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/apps/device-simulator
)

target_link_libraries(device_simulator
    PRIVATE
        edgelab_core
        device_simulator_support
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

add_executable(virtual_device_test
    tests/virtual_device_test.cpp
)

target_link_libraries(virtual_device_test
    PRIVATE
        edgelab_core
)

add_test(
    NAME virtual_device_test
    COMMAND virtual_device_test
)

add_executable(temperature_profile_test
    tests/temperature_profile_test.cpp
)

target_link_libraries(temperature_profile_test
    PRIVATE
        edgelab_core
)

add_test(
    NAME temperature_profile_test
    COMMAND temperature_profile_test
)

add_executable(random_temperature_profile_test
    tests/random_temperature_profile_test.cpp
)

target_link_libraries(random_temperature_profile_test
    PRIVATE
        edgelab_core
)

add_test(
    NAME random_temperature_profile_test
    COMMAND random_temperature_profile_test
)

add_executable(simulation_config_test
    tests/simulation_config_test.cpp
)

target_link_libraries(simulation_config_test
     PRIVATE
        device_simulator_support
)

add_test(
        NAME simulation_config_test
        COMMAND simulation_config_test
)