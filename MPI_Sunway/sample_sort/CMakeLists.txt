cmake_minimum_required(VERSION 3.16)
project(sample_sort C)

set(CMAKE_C_STANDARD 99)



project(sample_sort C)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(MSMPI_INC "C:/Program Files (x86)/Microsoft SDKs/MPI/Include/")
set(MSMPI_LIB64 "C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64/")

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MSMPI_INC})

add_executable(sample_sort sample_sort.c)
#add_executable(MPI_TEST  test_fft.cpp)

target_link_libraries(matrix_multiply ${MSMPI_LIB64}/msmpi.lib)