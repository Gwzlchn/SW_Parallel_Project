
#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<time.h>

#define GET_CYCLE(x) asm volatile ("rtc %0": "=r" (x));


typedef enum bool{false,true} bool;
typedef unsigned long long ull;
typedef unsigned long ul;

// 计数变量
ull root_node_add_times = 0;
ull root_node_multiply_times = 0;
ull root_node_mem_times = 0;



void array_sum_prefix_seq(ull* arr, ull len) {
    ull i = 1;
	for (i = 1; i < len; i++) {
        root_node_add_times++;
        root_node_mem_times++;
		arr[i] = arr[i - 1] + arr[i];
	}
	return;
}

ull get_correct_sum_fix_last(ull len) {
	ull temp = len / 2;
	return (len - 1) * temp;
}

void array_add_const(ull* arr, ull len, ull to_add_const) {
    ull i=1;
	for (i = 1; i < len; i++) {
        root_node_add_times++;
        root_node_mem_times++;
		arr[i] += to_add_const;
	}
	return;
}

void init_array(ull* arr, ull len,bool zero) {
    ull i=0;
	for (i = 0; i < len; i++) {
		arr[i] = (!zero) ? i : 0;
	}
	return;
}

void array_print(ull* arr, ull len) {
    ull i=0;
	for (i = 0; i < len; i++) {
		if (i % 8 == 0)printf("\n");
		printf("%lld  ", arr[i]);
	}
	printf("\n");
		
}

void check_answer(ull* array,ull len)
{
    ull N = len;
    //printf("Last Element is %lld\n", array[N - 1]);
    ull correct = get_correct_sum_fix_last(N);
    //printf("CORRECT LAST Element is %lld\n", correct);

    if (array[N - 1] == correct) {
        printf("[P_0] Congradulations! The two results are equal.\n" );
    }
    else {
        printf("[P_0] Bad news! The two results do not match.\n");

    }
}

int main(int argc, char* argv[])
{
	if (argc != 3) {
		printf( "Error! Extra two arguments NUM_PROC and N are needed!\n");
		return -1;
	}
	int NUM_PROC = atoi(argv[1]);

	
	ull N = atol(argv[2]); //向量的长度
	ull SIZE_PER_PROC = N / NUM_PROC;//每个处理器要处理的子向量长度


	int my_rank, num_procs;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	if ((num_procs != NUM_PROC || N%NUM_PROC!=0)  && my_rank==0) {
		printf( " the number of processors doesnot match\n");
		return -1;
	}

	//每个进程相同大小的数组切片
	ull* chunk_per_proc = (ull*)malloc(sizeof(ull) * SIZE_PER_PROC);
	ull* array;
	ull* last_elem_array;
	
	

	if (my_rank == 0) {

		array = (ull*)malloc(sizeof(ull) * N);
		

		init_array(array, N,false);

		int last_size = NUM_PROC + 1;
		last_elem_array = (ull*)malloc(sizeof(ull) * last_size);
		init_array(last_elem_array, last_size, true);
	

	}
	MPI_Barrier(MPI_COMM_WORLD);

	//Scatter
	double scatter_time_st = MPI_Wtime();
    ul scatter_cycle_st=0,scatter_cycle_ed = 0;
    GET_CYCLE(scatter_cycle_st)
	MPI_Scatter(array, SIZE_PER_PROC, MPI_LONG_LONG, \
		chunk_per_proc, SIZE_PER_PROC, MPI_LONG_LONG, \
		0, MPI_COMM_WORLD);
    GET_CYCLE(scatter_cycle_ed)
	double scatter_time_ed = MPI_Wtime();



	//Calculate First Step
	double chunk_first_time_st = MPI_Wtime();
	ul chunk_first_cycle_st=0,chunk_first_cycle_ed=0;
	GET_CYCLE(chunk_first_cycle_st)
	array_sum_prefix_seq(chunk_per_proc, SIZE_PER_PROC);
	double chunk_first_time_ed = MPI_Wtime();
	GET_CYCLE(chunk_first_cycle_ed)
	MPI_Barrier(MPI_COMM_WORLD);

	//Calculate Second Step
	//Gather each last element,calculate prefix sum, then scatter out
	ul root_gather_cycle_st=0,root_gather_cycle_ed=0;
	double root_gather_time_st = MPI_Wtime();
	ull* chunk_last_elem = chunk_per_proc + SIZE_PER_PROC - 1;
	GET_CYCLE(root_gather_cycle_st)
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

    GET_CYCLE(root_gather_cycle_ed)
	double root_gather_time_ed = MPI_Wtime();
	MPI_Barrier(MPI_COMM_WORLD);

	//Calculate Third Step
	//Get the prev array last value ,then add to chunk
	ul add_const_cycle_st=0,add_const_cycle_ed=0;
	double add_const_time_st = MPI_Wtime();
	ull prev_chunk_last = 0;
	GET_CYCLE(add_const_cycle_st)
	MPI_Scatter(last_elem_array , 1, MPI_LONG_LONG, \
		&prev_chunk_last, 1, MPI_LONG_LONG, \
		0, MPI_COMM_WORLD);
	array_add_const(chunk_per_proc, SIZE_PER_PROC, prev_chunk_last);

	MPI_Gather(chunk_per_proc, SIZE_PER_PROC, MPI_LONG_LONG, \
		array, SIZE_PER_PROC, MPI_LONG_LONG, \
		0, MPI_COMM_WORLD);
	GET_CYCLE(add_const_cycle_ed)
	double add_const_time_ed = MPI_Wtime();
	MPI_Barrier(MPI_COMM_WORLD);

	if (my_rank == 0) {
        printf("-------------------BEGIN---------------\n\n");
        printf("TOTAL LENGTH: \t\t %lld\n", N);
        printf("LENGTH PER PROCESSOR: \t%lld\n\n", SIZE_PER_PROC);

        printf("-------------------BEGIN TIMER---------------\n");
		printf("scatter time\t\t\t\t\t\t\t %f\n", scatter_time_ed -scatter_time_st);
		printf("First step time\t\t\t\t\t\t\t %f\n", chunk_first_time_ed - chunk_first_time_st);
		printf("gather last elem then scatter time\t\t\t\t %f\n", root_gather_time_ed - root_gather_time_st);
		printf("gather prev last value then add a constant to chunk time\t %f\n", add_const_time_ed - add_const_time_st);
		printf("total cost time\t\t\t\t\t\t\t %f\n\n", add_const_time_ed - scatter_time_st);


        printf("scatter cycle\t\t\t\t\t\t\t %ld\n", scatter_cycle_ed -scatter_cycle_st);
        printf("First step cycle\t\t\t\t\t\t %ld\n", chunk_first_cycle_ed - chunk_first_cycle_st);
        printf("gather last elem then scatter cycle\t\t\t\t %ld\n", root_gather_cycle_ed - root_gather_cycle_st);
        printf("gather prev last value then add a constant to chunk cycle\t %ld\n", add_const_cycle_ed - add_const_cycle_st);
        printf("total cost cycle\t\t\t\t\t\t %ld\n\n", add_const_cycle_ed - scatter_cycle_st);

        printf("-------------------END TIMER---------------\n\n");

        printf("--------------------BEGIN COUNT-----------------------------\n\n");
        printf("root node add times \t\t\t\t %lld\n",root_node_add_times);
        printf("root node multiply times \t\t\t\t %lld\n",root_node_multiply_times);
        printf("root node mem times \t\t\t %lld\n",root_node_mem_times);
        printf("\n\n--------------------END COUNT-----------------------------\n\n");

        check_answer(array,N);
		// 串行计算时间
        init_array(array, N,false);
		ul seq_cycle_st=0,seq_cycle_ed=0;
		double seq_time_st = MPI_Wtime();
        GET_CYCLE(seq_cycle_st)
		array_sum_prefix_seq(array, N);
        GET_CYCLE(seq_cycle_ed)
		double seq_time_ed = MPI_Wtime();
		printf("Sequential time\t\t\t\t\t\t\t %f\n", seq_time_ed - seq_time_st);
        printf("Sequential cycle\t\t\t\t\t\t %ld\n", seq_cycle_ed - seq_cycle_st);
		free(array);
		printf("\n-------------------END---------------\n");
	}

	
	free(chunk_per_proc);
	MPI_Finalize();
	
		

	return 0;
}
