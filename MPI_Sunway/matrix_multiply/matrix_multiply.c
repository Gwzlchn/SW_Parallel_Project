
#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<time.h>
#include <math.h>
#include <malloc.h>


#define GET_CYCLE(x) asm volatile ("rtc %0": "=r" (x));
//#define GET_CYCLE(x) x=1;
typedef enum bool{false,true} bool;
typedef unsigned short MatType;
typedef unsigned long long ull;
typedef unsigned long ul;
#define MPI_COMM_TYPE MPI_UNSIGNED_SHORT
//对row行col列矩阵初始化
void init_matrix_random(MatType* matrix, ull rows, ull cols)
{
    srand((unsigned)time(NULL));
    int i = 0;
    for (; i < rows * cols; i++) {
        matrix[i] = (MatType)rand() / RAND_MAX;
    }
}

void matrix_multiply(MatType* A, MatType* B, MatType* mat_res,
                     ull A_rows, ull A_cols, ull B_cols)
{
    ull i = 0,j=0,k=0;
    for (i=0; i < A_rows; i++) {
        for (j = 0; j < B_cols; j++) {
            MatType res = 0;
            for (k = 0; k < A_cols; k++) {
                res += A[i * A_cols + k] * B[k * B_cols + j];
            }
            mat_res[i * B_cols + j] = res;
        }
    }
}

bool isMatrixEqual(MatType* A, MatType* B, ull rows, ull cols)
{
    const float EPS = 1e-6;
    ull i=0,j=0;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            if (fabs(A[i * cols + j] - B[i * cols + j]) > EPS) {
                printf("The id is : i = %lld,\tj=%lld\n",i,j);
                printf("The gap is : %lf\n",fabs(A[i * cols + j] - B[i * cols + j]));
                return false;
            }
        }
    }

    return true;
}

// 根节点打印堆上的指针占用空间
void print_root_memory_used(char* ptr,ull size,int my_rank){
    if(my_rank!=0) return;
    ull bytes = size;
    ull KB = 1024;
    ull MB = 1024*1024;
    ull GB = 1024*1024*1024;
    printf("pointer %s ",ptr);
    if(bytes<=KB){
        printf("size is %lld Bytes \n",bytes);
    }else if(bytes > KB && bytes <= (MB)){
        printf("size is %lld KBytes \n",bytes/KB);
    }else if(bytes > MB && bytes <= (GB)){
        printf("size is %lld MBytes \n",bytes/MB);
    }else if(bytes > GB ){
        printf("size is %lld GBytes \n",bytes/GB);
    }
    return;

}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Error! Three arguments NUM_PROC and N are needed!\n");
        return 0;
    }
    int NUM_PROC = atoi(argv[1]);
    //int ELEM_PER_PROC = atoi(argv[2]);

    ull N = atoll(argv[2]); //方阵的维度
    ull ROWS_PER_PROC = N / NUM_PROC;//每个处理器要处理的行数


    int my_rank, num_procs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    if (num_procs != NUM_PROC || N%NUM_PROC!=0 ) {
        printf( " the number of processors doesnot match\n");
        return 0;
    }


    MatType* A, * B, * C, * C_seq;
    MatType* proc_row_A, * proc_row_C;//把A C划分成行向量
    ull size_of_whole_mat = sizeof(MatType) * N * N;
    ull size_of_proc_sub_mat = sizeof(MatType) * N * ROWS_PER_PROC;
    B = (MatType*)malloc(size_of_whole_mat);
    init_matrix_random(B, N, N);
    proc_row_A = (MatType*)malloc(size_of_proc_sub_mat);
    proc_row_C = (MatType*)malloc(size_of_proc_sub_mat);


    print_root_memory_used("B",size_of_whole_mat,my_rank);
    print_root_memory_used("proc_row_C",size_of_proc_sub_mat,my_rank);
    print_root_memory_used("proc_row_A",size_of_proc_sub_mat,my_rank);
    if (my_rank == 0) {
        A = (MatType*)malloc(size_of_whole_mat);
        C = (MatType*)malloc(size_of_whole_mat);


        print_root_memory_used("A",size_of_whole_mat,my_rank);
        print_root_memory_used("C",size_of_whole_mat,my_rank);


        init_matrix_random(A, N, N);
        printf("MOLLOC DONE\n");



    }
    MPI_Barrier(MPI_COMM_WORLD);

    //First Step : Scatter
    double board_time_st = MPI_Wtime();
    ul board_cycle_st=0,board_cycle_ed = 0;
    GET_CYCLE(board_cycle_st)
    MPI_Scatter(A, (ROWS_PER_PROC * N), MPI_COMM_TYPE, \
		proc_row_A, (ROWS_PER_PROC * N), MPI_COMM_TYPE, \
		0, MPI_COMM_WORLD);

    MPI_Bcast(B, (N * N), MPI_COMM_TYPE, 0, MPI_COMM_WORLD);
    GET_CYCLE(board_cycle_ed)
    double board_time_ed = MPI_Wtime();
    if(my_rank==0){
        printf("Scatter DONE\n");
    }

    // Second Step: Calculate
    double mul_time_st = MPI_Wtime();
    ul mul_cycle_st=0,mul_cycle_ed=0;
    GET_CYCLE(mul_cycle_st)
    matrix_multiply(proc_row_A, B, proc_row_C, \
		ROWS_PER_PROC, N, N);
	GET_CYCLE(mul_cycle_ed)
    double mul_time_ed = MPI_Wtime();
    if(my_rank==0){
        printf("Cal DONE\n");
    }
    // Third Step: Gather
    double gather_time_st = MPI_Wtime();
    ul gather_cycle_st=0,gather_cycle_ed=0;
    GET_CYCLE(gather_cycle_st)
    MPI_Gather(proc_row_C, (ROWS_PER_PROC * N), MPI_COMM_TYPE, \
		C, (ROWS_PER_PROC * N), MPI_COMM_TYPE,\
		0, MPI_COMM_WORLD);
    GET_CYCLE(gather_cycle_ed)
    double gather_time_ed = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);
    if(my_rank==0){
        printf("Gather DONE\n");
    }
    if (my_rank == 0) {
        printf("board and scatter time %f\n", board_time_ed - board_time_st);
        printf("multiply time %f\n", mul_time_ed - mul_time_st);
        printf("gather time %f\n", gather_time_ed - gather_time_st);

        printf("totally cost %f\n", gather_time_ed - board_time_st);


        printf("board and scatter cycle %ld\n", board_cycle_ed - board_cycle_st);
        printf("multiply cycle %ld\n", mul_cycle_ed - mul_cycle_st);
        printf("gather cycle %ld\n", gather_cycle_ed - gather_cycle_st);

        ul total_cycle = gather_cycle_ed - board_cycle_st;
        printf("totally cycle %ld\n", total_cycle);
        //printf("total cycle time: %lf\n",total_cycle/CPU_)

        //sequential
        ul seq_cycle_st=0,seq_cycle_ed =0;
        double seq_time_st = MPI_Wtime();
        GET_CYCLE(seq_cycle_st)
        C_seq = (MatType*)malloc(size_of_whole_mat);
        matrix_multiply(A, B, C_seq, N, N, N);
        GET_CYCLE(seq_cycle_ed)
        double seq_time_ed = MPI_Wtime();
        printf("Sequential time%f\n", seq_time_ed - seq_time_st);
        printf("Sequential cycle%ld\n", seq_cycle_ed - seq_cycle_st);
        if (isMatrixEqual(C, C_seq, N, N)) {
            printf("[P_0] Congratulations! The two results are equal.\n");
        }
        else {
            printf("[P_0] Bad news! The two results do not match.\n");
        }

        free(C);
        free(A);
        free(C_seq);
    }


    free(B);
    free(proc_row_A);
    free(proc_row_C);

    MPI_Finalize();


    return 0;
}
