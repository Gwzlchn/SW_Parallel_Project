
#include <stdio.h>
#include <complex.h>
#include <stdlib.h>
#include <mpi.h>
#include <stddef.h>
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

typedef struct  MPI_TEST_STRUCT{
    double a,b,c,d,e,f,g,h;
    double i,j,k,l,m,n,o,p;
}MPI_TEST_STRUCT;

/* create a type for struct complex */
MPI_Datatype create_MPI_type()
{
    const int nitems = 16;
    int          blocklengths[16] = { 1,1, 1,1, 1,1, 1,1, 1,1, 1,1, 1,1, 1,1 };
    MPI_Datatype types[16] = { MPI_DOUBLE, MPI_DOUBLE,MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE };
    MPI_Datatype MPI_OWN_STRUCT_TYPE;
    MPI_Aint     offsets[16];
    offsets[0] = offsetof(MPI_TEST_STRUCT, a);
    offsets[1] = offsetof(MPI_TEST_STRUCT, b);
    offsets[2] = offsetof(MPI_TEST_STRUCT, c);
    offsets[3] = offsetof(MPI_TEST_STRUCT, d);
    offsets[4] = offsetof(MPI_TEST_STRUCT, e);
    offsets[5] = offsetof(MPI_TEST_STRUCT, f);
    offsets[6] = offsetof(MPI_TEST_STRUCT, g);
    offsets[7] = offsetof(MPI_TEST_STRUCT, h);
    offsets[8] = offsetof(MPI_TEST_STRUCT, i);
    offsets[9] = offsetof(MPI_TEST_STRUCT, j);
    offsets[10] = offsetof(MPI_TEST_STRUCT, k);
    offsets[11] = offsetof(MPI_TEST_STRUCT, l);
    offsets[12] = offsetof(MPI_TEST_STRUCT, m);
    offsets[13] = offsetof(MPI_TEST_STRUCT, n);
    offsets[14] = offsetof(MPI_TEST_STRUCT, o);
    offsets[15] = offsetof(MPI_TEST_STRUCT, p);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_OWN_STRUCT_TYPE);
    MPI_Type_commit(&MPI_OWN_STRUCT_TYPE);

    return MPI_OWN_STRUCT_TYPE;
}


// 根节点打印堆上的指针占用空间
void print_root_memory_used(ull bytes, int my_rank) {
    if (my_rank != 0) return;
   // ull bytes = _msize(ptr);
    ull KB = 1024;
    ull MB = 1024 * 1024;
    ull GB = 1024 * 1024 * 1024;
    printf("root proc ");
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


    MPI_Datatype MPI_OWN_STRUCT_TYPE = create_MPI_type();

    // 根节点数据量
    int cur_root_len = atoi(argv[1]);
    int cur_root_size = sizeof(double complex)*cur_root_len;
    double complex* array ;
    int per_proc_size = cur_root_size/proc_size;
    int per_proc_len = cur_root_len/proc_size;

    double complex* per_proc_array = (double complex*)malloc(per_proc_size);
    array = (double complex*)malloc(cur_root_size);
    if(my_rank==0){

        if(array==0 || per_proc_array==0){
            printf("not enough\n");
            MPI_Abort(MPI_COMM_WORLD,99);
            //return 0;
        }
        if( per_proc_len%proc_size != 0){
            printf("not match\n");
            MPI_Abort(MPI_COMM_WORLD,22);
        }
    }


    ul start_cycle=0,end_cycle=0;

    MPI_Barrier(MPI_COMM_WORLD);

    asm volatile ("rtc %0": "=r" (start_cycle));
    MPI_Scatter(array,per_proc_len,MPI_C_DOUBLE_COMPLEX,\
                per_proc_array,per_proc_len,MPI_C_DOUBLE_COMPLEX,0,MPI_COMM_WORLD);
    asm volatile ("rtc %0": "=r" (end_cycle));


    MPI_Barrier(MPI_COMM_WORLD);

    ul overhead = end_cycle - start_cycle;

    if(my_rank==0){
        //输出消耗的周期
        printf("%ld\t\t\t\t",overhead);
        //free(array);
    }
    // 输出根节点调度的总数据量
    print_root_memory_used(cur_root_size,my_rank);

    //free(per_proc_array);
    MPI_Finalize();


    return 0;

}
