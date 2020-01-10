#include<stdlib.h>
#include<stdio.h>
#include<athread.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#define J 64
#define I 1000
double a[J][I],b[J][I],master[J][I],slave[J][I];
double check[J];
unsigned long counter[J];

extern SLAVE_FUN(func)();

static inline unsigned long rpcc(){
    unsigned long cycle;
    asm("rtc %0" : "=r"(cycle):);
    return cycle;
}

int main(){
    int i,j;
    double checksum,checksum2;
    unsigned long st,ed;
    printf("BEGIN INIT\n");

    fflush(NULL);
    for(j=0;j<J;j++){
        for(i=0;i<I;i++){
            a[j][i] = (i+j+0.5);
            b[j][i] = (i+j+1.0);
        }
    }
    st = rpcc();
    for(j=0;j<J;j++){
        for(i=0;i<I;i++){
            master[j][i] = (a[j][i])* b[j][i];
        }
    }
    ed = rpcc();
    printf("the host counter = %ld\n",ed - st);

    checksum = 0;checksum2=0;
    athread_init();
    st = rpcc();
    athread_spawn(func,0);
    athread_join();
    ed = rpcc();
    printf("the slave counter = %ld\n",ed - st);
    printf("END JOIN\n");
    fflush(NULL);

    printf("END JOIN\n");
    fflush(NULL);
    for(j=0;j<J;j++){
        for(i=0;i<I;i++){
       // printf("j=%d,i=%d,slave[j][i]=%f,master[j][i]=%f,checksum=%f,checksum2=%f\n",j,i,slave[j][i],master[j][i],checksum,checksum2);
            checksum2 =checksum2 +  slave[j][i];
            checksum= checksum +  master[j][i];
        }
    }
    printf("the master sum is %f\n the slave sum is %f\n",checksum,checksum2);
    fflush(NULL);
    athread_halt();
    printf("END\n");fflush(NULL);
    return 0;
}
