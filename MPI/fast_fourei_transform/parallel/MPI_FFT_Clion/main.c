//#include <mpi.h>
#include <stdio.h>
//#include "common.h"
#include <complex.h>
#include "time.h"
#define GET_MASTER_CYCLE(x) asm("rtc %0": "=r" (x) : );

#define ull unsigned long long
#define N 102400

double a_array[N];

void fill_double_arr(double* arr,ull length){
    ull i=0;
    for(;i<length;i++){
        arr[i] = (double)(i*i);
    }
    return;
}


inline void rpcc_(unsigned long *counter)
{
    unsigned long rpcc;
    asm("rtc %0": "=r" (rpcc) : );
    *counter=rpcc;
}




int main(int argc, char** argv) {
    volatile  double a = 1.0;
    volatile double b = 2.0;

    unsigned long st_cycle=0 ,ed_cycle=0;
    fill_double_arr(a_array,N);
    clock_t st_clk=0,ed_clk=0;
    int i=0;


    printf("BEGIN add four numbers\n");
    //for(i=0;i<4;i++){
        a = a_array[i*16];
        b = a_array[i*32];

        st_clk = clock();
        asm volatile ("rtc %0": "=r" (st_cycle) : );
        volatile double c = a+b;
        volatile double d = c+b;
        volatile double e = d+c;
        volatile double f = e+d;
        asm volatile("rtc %0": "=r" (ed_cycle) : );
        ed_clk = clock();
        //printf("start cycle %ld,\tend cycle %ld\t\t",st_cycle,ed_cycle);
        unsigned long overhead = ed_cycle-st_cycle;
        printf("overhead:%ld\n",overhead);
        double k = a+b+c+d+e+f;
        printf("current k:%lf\n",k);

        unsigned long overhead_clock = ed_clk - st_clk;
        printf("overhead clock: %ld\n",overhead_clock);
        st_cycle = ed_cycle = 0;
    //}
    printf("END ADD four numbers\n\n");





    printf("\nBEGIN add two numbers\n");
    for(i=0;i<4;i++){
        a = a_array[i*16];
        b = a_array[i*32];


        asm("rtc %0": "=r" (st_cycle) : );
        volatile  double c = a+b;
        asm("rtc %0": "=r" (ed_cycle) : );


        //printf("start cycle %ld,\tend cycle %ld\t\t",st_cycle,ed_cycle);
        unsigned long overhead = ed_cycle-st_cycle;
        printf("overhead:%ld\n",overhead);

        printf("current c:%lf\n",c);


        st_cycle = ed_cycle = 0;
    }
    printf("END add two number\n\n");



    printf("BEGIN MULTIPLY two numbers\n");
    a = b = 0;
    for(i=0;i<4;i++){
        a = a_array[i*32];
        b = a_array[i*64];

        asm("rtc %0": "=r" (st_cycle) : );
        volatile double  c = a*b;
        asm("rtc %0": "=r" (ed_cycle) : );

        //printf("start cycle %ld,\tend cycle %ld\t\t",st_cycle,ed_cycle);
        unsigned long overhead = ed_cycle-st_cycle;
        printf("overhead:%ld\n",overhead);
        printf("current c:%lf\n",c);
        st_cycle = ed_cycle = 0;
    }


    printf("END MULTIPLY two numbers\n\n");

    double complex a_com = 1 + 2*I;
    double complex b_com = 2 + 4*I;
    double complex c_com = 0;
    printf("BEGIN Complex REGISTER\n");
    for(i=0;i<4;i++){
        a_com = a_array[i*16];
        b_com = a_array[i*32];

        rpcc_(&st_cycle);
        double complex c_com = a_com*b_com;
        rpcc_(&ed_cycle);

        //printf("start cycle %ld,\tend cycle %ld\t\t",st_cycle,ed_cycle);
        unsigned long overhead = ed_cycle-st_cycle;
        printf("overhead:%ld\n",overhead);
        printf("current c real part:%lf\n",creal(c_com));
        st_cycle = ed_cycle = 0;
    }
    printf("END Complex REGISTER\n\n");

    printf("BEGIN Memory\n");
    for(i=4;i>0;i--){


        rpcc_(&st_cycle);
        double c = a_array[i*1024] + a_array[i*2048];
        rpcc_(&ed_cycle);

        printf("start cycle %ld,\tend cycle %ld\t\t",st_cycle,ed_cycle);
        unsigned long overhead = ed_cycle-st_cycle;
        printf("overhead:%ld\n",overhead);
        printf("current c  :%lf\n",c);
        st_cycle = ed_cycle = 0;
    }
    printf("END Memory\n\n");

    GET_MASTER_CYCLE(st_cycle)
    GET_MASTER_CYCLE(ed_cycle)
    printf("start cycle %ld,\tend cycle %ld\t\t\n",st_cycle,ed_cycle);



    return 0;

}
