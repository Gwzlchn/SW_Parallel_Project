#include<stdlib.h>
#include<stdio.h>
#include<athread.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#define J 64
#define I 1000
double a[J][I],b[J][I],h[J][I],s[J][I];
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
            b[j][i] = (i+j+1.5);
        }
    }
    st = rpcc();
    for(j=0;j<J;j++){
        for(i=0;i<I;i++){
            h[j][i] = (a[j][i])/b[j][i];
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

    for(j=0;j<J;j++){
        for(i=0;i<I;i++){
            checksum2+= s[j][i];
            checksum+= h[j][i];
        }
    }
    printf("the master sum is %f\n
        the slave sum is %f\n",checksum,checksum2);

    athread_halt();
    return 0;
}
