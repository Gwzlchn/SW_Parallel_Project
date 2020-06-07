/**********************************
 * Lab Exam 2                     *
 * <Your name goes here>          *
 *                                *
 * This program performs parallel *
 * sample sort.                   *
 **********************************/

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define MAX_VAL 10000000
#define DEBUG 0
#define DEBUG2 0

#define ul unsigned long
#define ull unsigned long long
#define MPI_COMM_TYPE MPI_INT
#define GET_CYCLE(x) asm volatile ("rtc %0": "=r" (x));

// 计数变量
ull root_node_add_times = 0;
ull root_node_multiply_times = 0;
ull root_node_mem_times = 0;



int compare(const void * x, const void * y)
{
    int a = (int) *(int *)x;
    int b = (int) *(int *)y;
    if( a < b )
        return -1;
    else if ( a > b )
        return 1;
    else
        return 0;
}


int main(int argc, char ** argv)
{
  MPI_Init(&argc, &argv);
    int my_rank, proc_size; //The my_rank and number of processors.
    double start, end;

    //The first command line argument is the number of elements.
    int numElements = atoi(argv[1]);

    //Obtain the my_rank and number of processors.
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_size);

    //Generate random data in each process.
    srand(my_rank * 10000000);
    int workToDo = numElements / proc_size;
    if(my_rank == 0 && numElements % proc_size > 0)
      workToDo += numElements % proc_size;

    int * rdata = (int *) malloc(sizeof(int)*workToDo);

    for(int i = 0 ; i < workToDo; i++)
      rdata[i] = ((rand())<<1) % MAX_VAL;

    
    if(DEBUG2)
    {
      printf("The values of the array in process my_rank %d are:\n", my_rank);
      for(int i = 0; i < workToDo; i++)
        printf("%d ", rdata[i]);
      printf("\n");
    }



    // The sample sort algorithm is based upon the algorithm at the following
    // website:
    // http://parallelcomp.uw.hu/ch09lev1sec5.html
    
    //Sort data locally
    ul sort_split_cycle_st=0,sort_split_cycle_ed = 0;
    GET_CYCLE(sort_split_cycle_st);
    qsort(rdata, workToDo, sizeof(int), compare);

    //Find splitters.
    int * splitters = (int *) malloc(sizeof(int)*(proc_size - 1));
    for(int i = workToDo/(proc_size), j = 0; j < proc_size - 1; i += workToDo / (proc_size), j++ ){
        splitters[j] = rdata[i];
        root_node_mem_times+=2;
    }

    GET_CYCLE(sort_split_cycle_ed);

    if(DEBUG)
    {
      printf("The splitters in process my_rank %d are:\n", my_rank);
      for(int i = 0; i < proc_size - 1; i++)
        printf("%d ", splitters[i]);
      printf("\n");
    }

    int * allSplitters;
    if(my_rank == 0){
        allSplitters = (int *) malloc(sizeof(int) * proc_size * (proc_size - 1));
    }

    ul gather_splitter_cycle_st=0,gather_splitter_cycle_ed=0;
    GET_CYCLE(gather_splitter_cycle_st);
    MPI_Gather(splitters, proc_size - 1, MPI_COMM_TYPE, allSplitters, \
    proc_size - 1, MPI_COMM_TYPE, 0,\
               MPI_COMM_WORLD);
    GET_CYCLE(gather_splitter_cycle_ed);

    ul root_splitter_sort_cycle_st=0,root_splitter_sort_cycle_ed=0;
    if(my_rank == 0)
    {
      GET_CYCLE(root_splitter_sort_cycle_st)
      qsort(allSplitters, proc_size * (proc_size - 1), sizeof(int), compare);
      for(int i = proc_size - 1, j = 0; i < proc_size * (proc_size - 1); i+=proc_size, j++){
          splitters[j] = allSplitters[i];
          root_node_mem_times+=2;
      }

       GET_CYCLE(root_splitter_sort_cycle_ed)
      if(DEBUG)
      {
        printf("The global splitters in process my_rank %d are:\n", my_rank);
        for(int i = 0; i < proc_size - 1; i++)
          printf("%d ", splitters[i]);
        printf("\n");
      }

    }

    //Send splitters to each process
    ul cast_splitters_cycle_st=0,cast_splitters_cycle_ed=0;
    GET_CYCLE(cast_splitters_cycle_st);
    MPI_Bcast(splitters, proc_size, MPI_COMM_TYPE, 0, MPI_COMM_WORLD );
    GET_CYCLE(cast_splitters_cycle_ed);

    //Use the splitters to define buckets and place data in appropriate
    //buckets.
    int * sendCounts = (int *) malloc(sizeof(int)*(proc_size));
    int * sendDisplacements = (int *) malloc(sizeof(int)*(proc_size));
    int * recvCounts = (int *) malloc(sizeof(int)*(proc_size));
    int * recvDisplacements = (int *) malloc(sizeof(int)*(proc_size));


    // 将每个处理器中的元素按 splitters 分配到桶
    ul inner_proc_dispatch_cycle_st=0,inner_proc_dispatch_cycle_ed=0;
    GET_CYCLE(inner_proc_dispatch_cycle_st);
    int hi, lo, midpoint = 0;
    int used= 0;
    for(int i = 0; i < proc_size - 1; i++)
    {
      hi = workToDo-1;
      lo = 0;
      midpoint = 0;
      int key = splitters[i];
      root_node_mem_times++;
      while(hi >= lo)
      {
        midpoint = (hi+lo)/2;
        root_node_mem_times++;
        if(rdata[midpoint] == key)
          break;
        else if(rdata[midpoint] < key)
          lo = midpoint+1;
        else
          hi = midpoint-1;
      }
      if(rdata[midpoint] < key)
      {
        while(midpoint < workToDo && rdata[midpoint] < key){
            midpoint++;
            root_node_mem_times++;
        }

      }
      else if(rdata[midpoint] >= key)
      {
        while(midpoint > 0 && rdata[midpoint-1] >= key){
            midpoint--;
            root_node_mem_times++;
        }

      }

      if(i == 0)
      {
        sendCounts[i] = midpoint;
        sendDisplacements[i] = 0;
        used = midpoint;

        root_node_mem_times+=2;
      }
      else
      {
        sendCounts[i] = midpoint - used;
        used += sendCounts[i];
        sendDisplacements[i] = sendDisplacements[i-1] + sendCounts[i-1];

          root_node_mem_times+=4;
      }
    }
    sendCounts[proc_size - 1] = workToDo - used;
    sendDisplacements[proc_size - 1] = sendDisplacements[proc_size - 2] + sendCounts[proc_size - 2];
    root_node_mem_times+=4;
    GET_CYCLE(inner_proc_dispatch_cycle_ed);

    if(DEBUG)
    {
      printf("The send counts in process my_rank %d are:\n", my_rank);
      for(int i = 0; i < proc_size; i++)
        printf("%d ", sendCounts[i]);
      printf("\n");
      printf("The send displacements in process my_rank %d are:\n", my_rank);
      for(int i = 0; i < proc_size; i++)
        printf("%d ", sendDisplacements[i]);
      printf("\n");
    }
    
    //Perform all to all communication to send the amount of data for each 
    //processor to receive.
    MPI_Alltoall(sendCounts, 1, MPI_COMM_TYPE, recvCounts, 1, MPI_COMM_TYPE, MPI_COMM_WORLD);

    //Compute receive displacements
    recvDisplacements[0] = 0;
    root_node_mem_times++;
    for(int i = 1; i < proc_size; i++){
        recvDisplacements[i] = recvCounts[i-1] + recvDisplacements[i-1];
        root_node_mem_times+=3;
    }


    if(DEBUG)
    {
      printf("The receive counts in process my_rank %d are:\n", my_rank);
      for(int i = 0; i < proc_size; i++)
        printf("%d ", recvCounts[i]);
      printf("\n");
      printf("The receive displacements in process my_rank %d are:\n", my_rank);
      for(int i = 0; i < proc_size; i++)
        printf("%d ", recvDisplacements[i]);
      printf("\n");
    }

    int recvBufSize = 0;
    for(int i = 0; i < proc_size; i++){
        recvBufSize += recvCounts[i];
        root_node_mem_times++;
    }

    int * recvBuf = (int *) malloc(sizeof(int)*recvBufSize);
    ul all_to_all_comm_cycle_st=0,all_to_all_comm_cycle_ed=0;
    GET_CYCLE(all_to_all_comm_cycle_st);
    MPI_Alltoallv(rdata, sendCounts, sendDisplacements, MPI_COMM_TYPE,
                  recvBuf, recvCounts, recvDisplacements, MPI_COMM_TYPE,
                         MPI_COMM_WORLD);
    GET_CYCLE(all_to_all_comm_cycle_ed);

    ul inner_proc_sort_cycle_st=0,inner_proc_sort_cycle_ed=0;
    GET_CYCLE(inner_proc_sort_cycle_st);
    qsort(recvBuf, recvBufSize, sizeof(int), compare);
    GET_CYCLE(inner_proc_sort_cycle_ed);


    int sendSize = recvBufSize;
    int * p0RecvCounts, * p0RecvDisplacements, * result;
    if(my_rank == 0)
    {
      p0RecvCounts = (int *) malloc(sizeof(int) * proc_size);
      p0RecvDisplacements = (int *) malloc(sizeof(int) * proc_size);
      result = (int *) malloc(sizeof(int)*numElements);
    }

    //Gather sizes in process my_rank 0.
    MPI_Gather(&sendSize, 1, MPI_COMM_TYPE, p0RecvCounts, 1, MPI_COMM_TYPE, 0, 
      MPI_COMM_WORLD);

    if(my_rank == 0)
    {
      p0RecvDisplacements[0] = 0;
      for(int i = 1; i < proc_size; i++){
          p0RecvDisplacements[i] = p0RecvDisplacements[i-1] + p0RecvCounts[i-1];
          root_node_mem_times+=3;
      }

    }

    //Gather the data in process my_rank 0.
    ul gather_all_proc_to_root_cycle_st=0,gather_all_proc_to_root_cycle_ed=0;
    GET_CYCLE(gather_all_proc_to_root_cycle_st);
    MPI_Gatherv(recvBuf, sendSize, MPI_COMM_TYPE,
                result, p0RecvCounts, p0RecvDisplacements,
                       MPI_COMM_TYPE, 0, MPI_COMM_WORLD);
    GET_CYCLE(gather_all_proc_to_root_cycle_ed);


    if(my_rank == 0)
    {


      //Print the data.
      if(DEBUG){
          if( numElements > 200)
          {
              printf("The first 100 elements of the sorted array are:\n");
              for(int i = 0; i < 100; i++)
                  printf("%d\n", result[i]);

              printf("The last 100 elements of the sorted array are:\n");
              for(int i = numElements-100; i < numElements; i++)
                  printf("%d\n", result[i]);
          }
          else
          {
              printf("The sorted elements of the array are:\n");
              for(int i = 0; i < numElements; i++)
                  printf("%d\n", result[i]);
          }
      }
      printf("\n-----------------BEGIN TIMER----------------\n\n");
      printf("sort data locally and find splitters cycle\t %ld\n",sort_split_cycle_ed-sort_split_cycle_st);
      printf("gather all splitters cycle\t\t\t %ld\n",gather_splitter_cycle_ed-gather_splitter_cycle_st);
      printf("root splitters sort cycle\t\t\t %ld\n",root_splitter_sort_cycle_ed-root_splitter_sort_cycle_st);
      printf("cast all splitters cycle\t\t\t %ld\n",cast_splitters_cycle_ed-cast_splitters_cycle_st);
      printf("inner proc dispatch cycle\t\t\t %ld\n",inner_proc_dispatch_cycle_ed-inner_proc_dispatch_cycle_st);
      printf("all to all commute cycle\t\t\t %ld\n",all_to_all_comm_cycle_ed-all_to_all_comm_cycle_st);
      printf("inner proc sort cycle\t\t\t\t %ld\n",inner_proc_sort_cycle_ed-inner_proc_sort_cycle_st);
      printf("gather all proc to root cycle\t\t\t %ld\n\n",gather_all_proc_to_root_cycle_ed-gather_all_proc_to_root_cycle_st);

      printf("total cycle cost\t\t\t\t %ld\n\n",gather_all_proc_to_root_cycle_ed - sort_split_cycle_st);
      printf("-----------------END   TIMER----------------\n\n");

    printf("--------------------BEGIN COUNT-----------------------------\n\n");
    printf("root node add times \t\t\t\t %lld\n",root_node_add_times);
    printf("root node multiply times \t\t\t\t %lld\n",root_node_multiply_times);
    printf("root node mem times \t\t\t %lld\n",root_node_mem_times);
    printf("\n\n--------------------END COUNT-----------------------------\n\n");

      free((void *) p0RecvCounts);
      free((void *) p0RecvDisplacements);
      free((void *) result);
    }

    free((void *) rdata);
    free((void *) sendCounts);
    free((void *) sendDisplacements);
    free((void *) recvCounts);
    free((void *) recvDisplacements);
    free((void *) recvBuf);
    free((void *) splitters);
  MPI_Finalize();
}

