
#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<time.h>
#include<iostream>

#define  ll long long

//对row行col列矩阵初始化
void init_matrix_eye(double* matrix, ll rows, ll cols)
{
	for (ll i = 0; i < rows ; i++) {
		matrix[i * cols + i] = i;
	}
	return;
}

void init_matrix_random(double* matrix, ll rows, ll cols)
{
	srand((unsigned)time(NULL));
	for (int i = 0; i < rows * cols; i++) {
		matrix[i] = (double)rand() / RAND_MAX;
	}
}

void matrix_multiply(double* A, double* B, double* mat_res,
	ll A_rows, ll A_cols, ll B_cols)
{
	for (ll i = 0; i < A_rows; i++) {
		for (ll j = 0; j < B_cols; j++) {
			double res = 0;
			for (ll k = 0; k < A_cols; k++) {
				res += A[i * A_cols + k] * B[k * B_cols + j];
			}
			mat_res[i * B_cols + j] = res;
		}
	}
}

bool isMatrixEqual(double* A, double* B, ll rows, ll cols)
{
	const float EPS = 1e-6;
	for (ll i = 0; i < rows; i++) {
		for (ll j = 0; j < cols; j++) {
			if (fabs(A[i * cols + j] - B[i * cols + j]) > EPS) {
				std::cout << "The id is : i = " << i << " j = " << j << std::endl;
				std::cout << "The gap is : " << fabs(A[i * cols + j] - B[i * cols + j]) << std::endl;
				return false;
			}
		}
	}

	return true;
}

int main(int argc, char* argv[])
{
	if (argc != 3) {
		std::cout << "Error! Three arguments NUM_PROC and N are needed!" << std::endl;
		return 0;
	}
	int NUM_PROC = atoi(argv[1]);
	//int ELEM_PER_PROC = atoi(argv[2]);
	
	ll N = atoll(argv[2]); //方阵的维度
	int ROWS_PER_PROC = N / NUM_PROC;//每个处理器要处理的行数


	int my_rank, num_procs;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	if (num_procs != NUM_PROC || N%NUM_PROC!=0 ) {
		std::cerr << " the number of processors doesnot match";
		return 0;
	}


	double* A, * B, * C, * C_seq;
	double* proc_row_A, * proc_row_C;//把A C划分成行向量

	B = (double*)malloc(sizeof(double) * N * N);
	init_matrix_random(B, N, N);
	proc_row_A = (double*)malloc(sizeof(double) * N * ROWS_PER_PROC);
	proc_row_C = (double*)malloc(sizeof(double) * N * ROWS_PER_PROC);


	if (my_rank == 0) {
		A = (double*)malloc(sizeof(double) * N * N);
		C = (double*)malloc(sizeof(double) * N * N);
		C_seq = (double*)malloc(sizeof(double) * N * N);
		init_matrix_random(A, N, N);

	}
	MPI_Barrier(MPI_COMM_WORLD);

	//Scatter
	double board_time_st = MPI_Wtime();
	MPI_Scatter(A, ROWS_PER_PROC * N, MPI_DOUBLE, \
		proc_row_A, ROWS_PER_PROC * N, MPI_DOUBLE, \
		0, MPI_COMM_WORLD);

	MPI_Bcast(B, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	double board_time_ed = MPI_Wtime();

	double mul_time_st = MPI_Wtime();
	matrix_multiply(proc_row_A, B, proc_row_C, \
		ROWS_PER_PROC, N, N);
	double mul_time_ed = MPI_Wtime();

	double gather_time_st = MPI_Wtime();
	MPI_Gather(proc_row_C, (ROWS_PER_PROC * N), MPI_DOUBLE, \
		C, (ROWS_PER_PROC * N), MPI_DOUBLE,\
		0, MPI_COMM_WORLD);
	double gather_time_ed = MPI_Wtime();

	MPI_Barrier(MPI_COMM_WORLD);

	if (my_rank == 0) {
		printf("broard and scatter time %f\n", board_time_ed - board_time_st);
		printf("multiply time %f\n", mul_time_ed - mul_time_st);
		printf("gather time %f\n", gather_time_ed - gather_time_st);

		printf("totally cost %f\n", gather_time_ed - board_time_st);

		double seq_time_st = MPI_Wtime();
		matrix_multiply(A, B, C_seq, N, N, N);
		double seq_time_ed = MPI_Wtime();
		printf("Sequetial time%f\n", seq_time_ed - seq_time_st);
		if (isMatrixEqual(C, C_seq, N, N)) {
			std::cout << "[P_0] Congradulations! The two results are equal." << std::endl;
		}
		else {
			std::cout << "[P_0] Bad news! The two results do not match." << std::endl;
		}
	}


	MPI_Finalize();
	

	return 0;
}
