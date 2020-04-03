
#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<time.h>
#include<iostream>


//#define ARRAY_SIZE 1048576

#define  ll long long

void array_sum_prefix_seq(ll* arr, ll len) {
	for (ll i = 1; i < len; i++) {
		arr[i] = arr[i - 1] + arr[i];
	}
	return;
}

ll get_correct_sum_fix_last(ll len) {
	ll temp = len / 2;
	return (len - 1) * temp;
}

void array_add_const(ll* arr, ll len, ll to_add_const) {
	for (ll i = 1; i < len; i++) {
		arr[i] += to_add_const;
	}
	return;
}

void init_array(ll* arr, ll len,bool zero=false) {
	for (ll i = 0; i < len; i++) {
		arr[i] = (!zero) ? i : 0;
	}
	return;
}

void array_print(ll* arr, ll len) {
	for (int i = 0; i < len; i++) {
		if (i % 8 == 0)printf("\n");
		printf("%lld  ", arr[i]);
	}
	printf("\n");
		
}

int main(int argc, char* argv[])
{
	if (argc != 3) {
		std::cerr << "Error! Three arguments NUM_PROC and N are needed!" << std::endl;
		return -1;
	}
	int NUM_PROC = atoi(argv[1]);

	
	ll N = atol(argv[2]); //向量的长度
	ll SIZE_PER_PROC = N / NUM_PROC;//每个处理器要处理的子向量长度


	int my_rank, num_procs;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	if ((num_procs != NUM_PROC || N%NUM_PROC!=0)  && my_rank==0) {
		std::cerr << " the number of processors doesnot match";
		return -1;
	}

	//每个进程相同大小的数组切片
	ll* chunk_per_proc = (ll*)malloc(sizeof(ll) * SIZE_PER_PROC);
	ll* array;
	ll* last_elem_array;
	
	

	if (my_rank == 0) {
		printf("-------------------BEGIN---------------\n");
		printf("TOTAL LENGTH: \t\t %lld\n", N);
		printf("LENGTH PER PROCESSOR: \t%lld\n%", SIZE_PER_PROC);
		array = (ll*)malloc(sizeof(ll) * N);
		

		init_array(array, N);

		int last_size = NUM_PROC + 1;
		last_elem_array = (ll*)malloc(sizeof(ll) * last_size);
		init_array(last_elem_array, last_size, true);
	

	}
	MPI_Barrier(MPI_COMM_WORLD);

	//Scatter
	double board_time_st = MPI_Wtime();

	MPI_Scatter(array, SIZE_PER_PROC, MPI_LONG_LONG, \
		chunk_per_proc, SIZE_PER_PROC, MPI_LONG_LONG, \
		0, MPI_COMM_WORLD);

	double board_time_ed = MPI_Wtime();

	//Calculate First Step
	double chunk_first_time_st = MPI_Wtime();
	array_sum_prefix_seq(chunk_per_proc, SIZE_PER_PROC);
	double chunk_first_time_ed = MPI_Wtime();
	//if (my_rank == num_procs - 1)
		//array_print(chunk_per_proc, SIZE_PER_PROC);
	MPI_Barrier(MPI_COMM_WORLD);

	//Calculate Second Step
	//Gather each last element,calculate prefix sum, then scatter out
	double gather_time_st = MPI_Wtime();
	ll* chunk_last_elem = chunk_per_proc + SIZE_PER_PROC - 1;
	MPI_Gather(chunk_last_elem  , 1, MPI_LONG_LONG, \
		(last_elem_array+1), 1, MPI_LONG_LONG, \
		0, MPI_COMM_WORLD);

	if (my_rank == 0) {
		//array_print(last_elem_array, NUM_PROC);
		array_sum_prefix_seq(last_elem_array, NUM_PROC);
		//array_print(last_elem_array, NUM_PROC);
	}
	MPI_Scatter((last_elem_array + 1), 1, MPI_LONG_LONG, \
		chunk_last_elem, 1, MPI_LONG_LONG, \
		0, MPI_COMM_WORLD);

	double gather_time_ed = MPI_Wtime();
	MPI_Barrier(MPI_COMM_WORLD);

	//Calcalatie Third Step
	//Get the prev array last value ,then add to chunk
	double addconst_time_st = MPI_Wtime();
	ll prev_chunk_last = 0;
	MPI_Scatter(last_elem_array , 1, MPI_LONG_LONG, \
		&prev_chunk_last, 1, MPI_LONG_LONG, \
		0, MPI_COMM_WORLD);
	array_add_const(chunk_per_proc, SIZE_PER_PROC, prev_chunk_last);

	MPI_Gather(chunk_per_proc, SIZE_PER_PROC, MPI_LONG_LONG, \
		array, SIZE_PER_PROC, MPI_LONG_LONG, \
		0, MPI_COMM_WORLD);
	double addconst_time_ed = MPI_Wtime();
	MPI_Barrier(MPI_COMM_WORLD);

	if (my_rank == 0) {
		printf("broard and scatter time %f\n", board_time_ed - board_time_st);
		printf("First step time %f\n", chunk_first_time_ed - chunk_first_time_st);
		printf("gather last elem then scatter time %f\n", gather_time_ed - gather_time_st);
		printf("gather prev last vlaue then add to chunk time %f\n ", addconst_time_ed - addconst_time_st);
		printf("totally cost %f\n", addconst_time_ed - board_time_st);


		printf("Last Element is %lld\n", array[N - 1]);
		ll correct = get_correct_sum_fix_last(N);
		printf("CORRECT LAST Element is %lld\n", correct);
		
		if (array[N - 1] == correct) {
			std::cout << "[P_0] Congradulations! The two results are equal." << std::endl;
		}
		else {
			std::cout << "[P_0] Bad news! The two results do not match." << std::endl;

		}

		double seq_time_st = MPI_Wtime();
		init_array(array, N);
		array_sum_prefix_seq(array, N);
		double seq_time_ed = MPI_Wtime();
		printf("Sequetial time%f\n", seq_time_ed - seq_time_st);

		free(array);
		printf("-------------------END---------------\n");
	}

	
	free(chunk_per_proc);
	MPI_Finalize();
	
		

	return 0;
}
