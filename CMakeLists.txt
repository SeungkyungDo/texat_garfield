cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(TEXAT_GARFIELD_GAS_SIMULATION)
if(NOT TARGET Garfield::Garfield)
  find_package(Garfield REQUIRED)
endif()

# ---Define executables----------------------------------------------------------
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
link_directories(${ROOT_LIBRARY_DIR})

message(STATUS ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(texat_gas_simulation.exe texat_gas_simulation.cc DetectorConfiguration.h)
target_link_libraries(texat_gas_simulation.exe Garfield::Garfield ${ROOT_LIBRARIES})

add_executable(draw_field.exe draw_field.cc DetectorConfiguration.h)
target_link_libraries(draw_field.exe Garfield::Garfield ${ROOT_LIBRARIES})
