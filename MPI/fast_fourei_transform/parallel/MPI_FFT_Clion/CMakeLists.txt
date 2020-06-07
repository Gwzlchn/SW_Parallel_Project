cmake_minimum_required(VERSION 3.15)
project(MPI_FFT_Clion C)


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 99)
#set(CMAKE_CXX_STANDARD 11)
set(MSMPI_INC "C:/Program Files (x86)/Microsoft SDKs/MPI/Include/")
set(MSMPI_LIB64 "C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64/")

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MSMPI_INC})

add_executable(MPI_FFT_Clion main.c common.h mpi_snip.cpp cache_test.c main2.c)
#add_executable(MPI_TEST  test_fft.cpp)

target_link_libraries(MPI_FFT_Clion ${MSMPI_LIB64}/msmpi.lib)