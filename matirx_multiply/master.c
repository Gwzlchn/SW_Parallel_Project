#include<stdlib.h>
#include<stdio.h>
#include<athread.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#define J 64
#define I 64
#define K 64
double a[J][K],b[K][I], master[J][I],slave[J][I];



extern SLAVE_FUN(func)();

static inline unsigned long rpcc(){
    unsigned long cycle;
    asm("rtc %0" : "=r"(cycle):);
    return cycle;
}

int main(){
    int i,j,k;
    double checksum,checksum2;
    unsigned long st,ed;
    //init 
    printf("BEGIN INIT\n");
    fflush(NULL);
    for(j=0;j<J;j++){
        for(i=0;i<I;i++){
            a[j][i] = (j==i)? i:0;
            b[j][i] = (j==i)? i:0;
        }
    }
    //master
    st = rpcc();
    for(j=0;j<J;j++){
        for(i=0;i<I;i++){
            master[j][i] = 0;
            for(k=0;k<K;k++)
                master[j][i] += a[j][k] * b[k][i];
        }
    }
    ed = rpcc();
    printf("the host counter = %ld\n",ed - st);

    //slave
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

    //check
    checksum = 0;checksum2=0;
    for(j=0;j<J;j++){
        for(i=0;i<I;i++){
       // printf("j=%d,i=%d,slave[j][i]=%f,master[j][i]=%f,checksum=%f,checksum2=%f\n",j,i,slave[j][i],master[j][i],checksum,checksum2);
            checksum +=  master[j][i];
            checksum2 += slave[j][i];
        }
    }
    printf("the master sum is %f\n the slave sum is %f\n",checksum,checksum2);
    fflush(NULL);
    athread_halt();
    printf("END\n");fflush(NULL);
    return 0;
}
