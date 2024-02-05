cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(TEXAT_GARFIELD_GAS_SIMULATION)
if(NOT TARGET Garfield::Garfield)
  find_package(Garfield REQUIRED)
endif()

# ---Define executables----------------------------------------------------------
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
link_directories(${ROOT_LIBRARY_DIR})

add_executable(texat_gas_simulation texat_gas_simulation.cc WireSet.h)
target_link_libraries(texat_gas_simulation Garfield::Garfield ${ROOT_LIBRARIES})

add_executable(draw_field draw_field.cc WireSet.h)
target_link_libraries(draw_field Garfield::Garfield ${ROOT_LIBRARIES})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/make_condor.py DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build/)
