
cmake_minimum_required(VERSION 3.16)
project(MPI_Sunway C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(instruction)
add_subdirectory(matrix_multiply)
add_subdirectory(parallel_fft)
add_subdirectory(prefix_sum)
add_subdirectory(sample_sort)
add_subdirectory(scatter_test)


set(CMAKE_BUILD_TYPE Debug)


set(MSMPI_INC "C:/Program Files (x86)/Microsoft SDKs/MPI/Include/")
set(MSMPI_LIB64 "C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64/")

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MSMPI_INC})




