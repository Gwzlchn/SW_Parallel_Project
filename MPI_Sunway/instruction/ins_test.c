
#include <stdio.h>
#include <complex.h>
#include <stdlib.h>
#include <mpi.h>
//#include "swlu.h"
//#include "mpi.h"
#define GET_CYCLE(x) asm volatile ("rtc %0": "=r" (x));
#define ull unsigned long long
#define ul unsigned long
#define N 1024



void fill_int_mat(int* arr[],ull length){
    ull i=0;
    for(;i<length;i++){
        for(ull j=0;j<length;j++){
            arr[i][j] = i*j;
        }
    }
    return;
}

void fill_int_arr(int* arr,ull length){
    ull i=0;
    for(;i<length;i++){

            arr[i] = i+1;

    }
    return;
}

enum bool{false,true};



// 根节点打印堆上的指针占用空间
void print_root_memory_used(ull bytes, int my_rank) {
    if (my_rank != 0) return;
   // ull bytes = _msize(ptr);
    ull KB = 1024;
    ull MB = 1024 * 1024;
    ull GB = 1024 * 1024 * 1024;
    if (bytes <= KB) {
        printf("pointer size is %lld Bytes \n", bytes);
    }
    else if (bytes > KB && bytes <= (MB)) {
        printf("pointer size is %lld KBytes \n", bytes / KB);
    }
    else if (bytes > MB && bytes <= (GB)) {
        printf("pointer size is %lld MBytes \n", bytes / MB);
    }
    else if (bytes > GB) {
        printf("pointer size is %lld GBytes \n", bytes / GB);
    }
    return;



}


int main(int argc, char** argv) {
    MPI_Init(NULL,NULL);
    int my_rank=0,proc_size=0;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_size);


    ul start_cycle=0,end_cycle=0;
    double a=1,b=1;
    ul overhead = 0;
    int times = 32;

    // 加法指令开销
    for(int i=0;i<times;i++){
        asm volatile ("rtc %0": "=r" (start_cycle));
        volatile double c = a+b;
        asm volatile ("rtc %0": "=r" (end_cycle));
        overhead += end_cycle - start_cycle;
    }
    if(my_rank==0){
        printf("%ld\n",overhead/times);
    }



    // 乘法指令开销
    for(int i=0;i<times;i++){
        asm volatile ("rtc %0": "=r" (start_cycle));
        volatile double c = a*b;
        asm volatile ("rtc %0": "=r" (end_cycle));
        overhead += end_cycle - start_cycle;
    }
    if(my_rank==0){
        printf("%ld\n",overhead/times);
    }


    // 访存指令单位开销
    //初始化矩阵，在堆上申请内存
    int** matrix = (int**)malloc(sizeof(int*)*N);
    for(int i=0;i<N;i++){
        matrix[i] = (int*)malloc(sizeof(int)*N);
    }
    fill_int_mat(matrix,N);
    for(int i=0;i<times;i++){
        int random_int = rand()%N;
        int* current_row = matrix[random_int];
        asm volatile ("rtc %0": "=r" (start_cycle));
        volatile double c = current_row[random_int];
        asm volatile ("rtc %0": "=r" (end_cycle));
        overhead += end_cycle - start_cycle;
    }

    if(my_rank==0){
        printf("%ld\n",overhead/times);
    }



    // 通信指令单位开销
    for(int i=1;i<=32;i++){
        ull cur_size = 1<<i;
        int* array = (int*)malloc(sizeof(int)*cur_size);
        int* remote_array = (int*)malloc(sizeof(int)*cur_size);

        if(array==0 || remote_array==0){
            printf("not enough\n");
            return 0;
        }
        fill_int_arr(array,cur_size);
        ul start_cycle=0,end_cycle=0;
        asm volatile ("rtc %0": "=r" (start_cycle));
        if(my_rank==0)
            MPI_Send(array,cur_size,MPI_INT,1,0,MPI_COMM_WORLD);
        if(my_rank==1)
            MPI_Recv(remote_array,cur_size,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        asm volatile ("rtc %0": "=r" (end_cycle));

        ul overhead = end_cycle - start_cycle;

        if(my_rank==0){
            printf("%ld\t\t\t\t",overhead);
        }
        print_root_memory_used(sizeof(int)*cur_size,my_rank);
        free(array);
        free(remote_array);
        MPI_Barrier(MPI_COMM_WORLD);
    }




    return 0;

}
