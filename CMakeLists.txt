# If subdirectory don't find packages
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
    project(tof_cpp_examples CXX)
    find_package(tof REQUIRED)
endif()

add_executable(stream_pcie stream_pcie.cpp)
target_link_libraries(stream_pcie PRIVATE tof::tof)
