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

#define MAX_VAL 1000
#define DEBUG 1
#define DEBUG2 1

int compare(const void *, const void *);

int main(int argc, char ** argv)
{
  MPI_Init(&argc, &argv);
    int rank, size; //The rank and number of processors.
    double start, end;

    //The first command line argument is the number of elements.
    int numElements = atoi(argv[1]);

    //Obtain the rank and number of processors.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Generate random data in each process.
    srand(rank*10000000);
    int workToDo = numElements/size;
    if(rank == 0 && numElements%size > 0)
      workToDo += numElements%size;
      
    int * rdata = (int *) malloc(sizeof(int)*workToDo);

    for(int i = 0 ; i < workToDo; i++)
      rdata[i] = rand() % MAX_VAL;

    if(DEBUG2)
    {
      printf("The values of the array in process rank %d are:\n", rank);
      for(int i = 0; i < workToDo; i++)
        printf("%d ", rdata[i]);
      printf("\n");
    }

    start = MPI_Wtime();

    // The sample sort algorithm is based upon the algorithm at the following
    // website:
    // http://parallelcomp.uw.hu/ch09lev1sec5.html
    
    //Sort data locally
    qsort(rdata, workToDo, sizeof(int), compare);

    if(DEBUG2)
    {
      printf("The local sorted values of the array in process rank %d are:\n", rank);
      for(int i = 0; i < workToDo; i++)
        printf("%d ", rdata[i]);
      printf("\n");
    }

    //Find splitters.
    int * splitters = (int *) malloc(sizeof(int)*(size-1));

    for(int i = workToDo/(size), j = 0; j < size-1; i += workToDo/(size), j++ )
        splitters[j] = rdata[i];

    if(DEBUG)
    {
      printf("The splitters in process rank %d are:\n", rank);
      for(int i = 0; i < size-1; i++)
        printf("%d ", splitters[i]);
      printf("\n");
    }

    int * allSplitters;
    if(rank == 0)
      allSplitters = (int *) malloc(sizeof(int)*size*(size-1));

    MPI_Gather(splitters, size-1, MPI_INT, allSplitters, size-1, MPI_INT, 0,
        MPI_COMM_WORLD);

    if(rank == 0)
    {
      qsort(allSplitters, size*(size-1), sizeof(int), compare);
      for(int i = size-1, j = 0; i < size*(size-1); i+=size, j++)
        splitters[j] = allSplitters[i];
      if(DEBUG)
      {
        printf("The global splitters in process rank %d are:\n", rank);
        for(int i = 0; i < size-1; i++)
          printf("%d ", splitters[i]);
        printf("\n");
      }

    }

    //Send splitters to each process
    MPI_Bcast( splitters, size, MPI_INT, 0, MPI_COMM_WORLD );

    //Use the splitters to define buckets and place data in appropriate
    //buckets.
    int * sendCounts = (int *) malloc(sizeof(int)*(size));
    int * sendDisplacements = (int *) malloc(sizeof(int)*(size));
    int * recvCounts = (int *) malloc(sizeof(int)*(size));
    int * recvDisplacements = (int *) malloc(sizeof(int)*(size));

    int hi, lo, midpoint = 0;
    int used= 0;
    for(int i = 0; i < size-1; i++)
    {
      hi = workToDo-1;
      lo = 0;
      midpoint = 0;
      int key = splitters[i];
      while(hi >= lo)
      {
        midpoint = (hi+lo)/2;
        if(rdata[midpoint] == key)
          break;
        else if(rdata[midpoint] < key)
          lo = midpoint+1;
        else
          hi = midpoint-1;
      }
      if(rdata[midpoint] < key)
      {
        while(midpoint < workToDo && rdata[midpoint] < key)
          midpoint++;
      }
      else if(rdata[midpoint] >= key)
      {
        while(midpoint > 0 && rdata[midpoint-1] >= key)
          midpoint--;
      }

      if(i == 0)
      {
        sendCounts[i] = midpoint;
        sendDisplacements[i] = 0;
        used = midpoint;
      }
      else
      {
        sendCounts[i] = midpoint - used;
        used += sendCounts[i];
        sendDisplacements[i] = sendDisplacements[i-1] + sendCounts[i-1];
      }
    }
    sendCounts[size-1] = workToDo - used;
    sendDisplacements[size-1] = sendDisplacements[size-2] + sendCounts[size-2];

    if(DEBUG)
    {
      printf("The send counts in process rank %d are:\n", rank);
      for(int i = 0; i < size; i++)
        printf("%d ", sendCounts[i]);
      printf("\n");
      printf("The send displacements in process rank %d are:\n", rank);
      for(int i = 0; i < size; i++)
        printf("%d ", sendDisplacements[i]);
      printf("\n");
    }
    
    //Perform all to all communication to send the amount of data for each 
    //processor to receive.
    MPI_Alltoall(sendCounts, 1, MPI_INT, recvCounts, 1, MPI_INT, MPI_COMM_WORLD);

    //Compute receive displacements
    recvDisplacements[0] = 0;
    for(int i = 1; i < size; i++)
      recvDisplacements[i] = recvCounts[i-1] + recvDisplacements[i-1];

    if(DEBUG)
    {
      printf("The receive counts in process rank %d are:\n", rank);
      for(int i = 0; i < size; i++)
        printf("%d ", recvCounts[i]);
      printf("\n");
      printf("The receive displacements in process rank %d are:\n", rank);
      for(int i = 0; i < size; i++)
        printf("%d ", recvDisplacements[i]);
      printf("\n");
    }

    int recvBufSize = 0;
    for(int i = 0; i < size; i++)
      recvBufSize += recvCounts[i];
    int * recvBuf = (int *) malloc(sizeof(int)*recvBufSize);

    MPI_Alltoallv(rdata, sendCounts, sendDisplacements, MPI_INT,
                  recvBuf, recvCounts, recvDisplacements, MPI_INT,
                         MPI_COMM_WORLD);

    qsort(recvBuf, recvBufSize, sizeof(int), compare);

    int sendSize = recvBufSize;
    int * p0RecvCounts, * p0RecvDisplacements, * result;
    if(rank == 0)
    {
      p0RecvCounts = (int *) malloc(sizeof(int)*size);
      p0RecvDisplacements = (int *) malloc(sizeof(int)*size);
      result = (int *) malloc(sizeof(int)*numElements);
    }

    //Gather sizes in process rank 0.
    MPI_Gather(&sendSize, 1, MPI_INT, p0RecvCounts, 1, MPI_INT, 0, 
      MPI_COMM_WORLD);

    if(rank == 0)
    {
      p0RecvDisplacements[0] = 0;
      for(int i = 1; i < size; i++)
        p0RecvDisplacements[i] = p0RecvDisplacements[i-1] + p0RecvCounts[i-1];
    }

    //Gather the data in process rank 0.
    MPI_Gatherv(recvBuf, sendSize, MPI_INT,
                result, p0RecvCounts, p0RecvDisplacements,
                       MPI_INT, 0, MPI_COMM_WORLD);

    if(rank == 0)
    {
      end = MPI_Wtime();
      printf("The run time is %lf s\n", end-start);
      //Print the data.
      if(numElements > 200)
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
