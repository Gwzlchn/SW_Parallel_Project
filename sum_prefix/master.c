#include <stdlib.h>
#include <stdio.h>
#include <athread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define J 512 //64*8
int a[J], master[J], slave[J];

extern SLAVE_FUN(func)();

static inline unsigned long rpcc()
{
    unsigned long cycle;
    asm("rtc %0"
        : "=r"(cycle)
        :);
    return cycle;
}

void serial_prefix_sum(int *a,int length){
    
}

int main()
{
    int i, j, k;
    double checksum, checksum2;
    unsigned long st, ed;
    //init
    printf("BEGIN INIT\n");
    fflush(NULL);
    for (j = 0; j < J; j++)
    {
        
        master[j] = j ;
        slave[j] = j; 
    }
    //master
    st = rpcc();
    bubble_sort(master,J);
    ed = rpcc();
    printf("the host counter = %ld\n", ed - st);
    
    
    //slave
    athread_init();
    st = rpcc();
    athread_spawn(func, 0);
    athread_join();
    ed = rpcc();
    printf("the slave counter = %ld\n", ed - st);
    printf("END JOIN\n");
    fflush(NULL);

    //check

    for(j=0;j<16;j++){
        printf("master[%d] = %d\t master[%d] = %d\n",j,master[j], J-j-1,master[J-j-1]);
        printf("slave[%d] = %d\t slave[%d] = %d\n",j,slave[j], J-j-1,slave[J-j-1]);
    }
   
    fflush(NULL);
    athread_halt();
    printf("END\n");
    fflush(NULL);
    return 0;
}
