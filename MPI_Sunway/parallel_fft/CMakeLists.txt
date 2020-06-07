cmake_minimum_required(VERSION 3.15)
project(parallel_fft C)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 99)
#set(CMAKE_CXX_STANDARD 11)
set(MSMPI_INC "C:/Program Files (x86)/Microsoft SDKs/MPI/Include/")
set(MSMPI_LIB64 "C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64/")

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MSMPI_INC})

add_executable(parallel_fft parallel_fft.c)
#add_executable(MPI_TEST  test_fft.cpp)

target_link_libraries(parallel_fft ${MSMPI_LIB64}/msmpi.lib)