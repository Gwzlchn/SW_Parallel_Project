//
// Created by Zelin on 20/05/06.
//

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define ull unsigned long long

void fill_double_arr(double* arr,ull length){
    ull i=0;
    for(;i<length;i++){
        arr[i] = (double)(i);
    }
    return;
}




struct timespec  st_clk,ed_clk;
clock_t sum_clk;
volatile double a = 1.0;
volatile double b = 2.0;
int main(){

    volatile double a = 1.0;
    volatile double b = 2.0;
    volatile double c = a+b;
    double a_array[100000];
    double b_array[100000];
    fill_double_arr(a_array,100000);
    fill_double_arr(b_array,100000);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &st_clk);
    int i=0;
    for(i=0;i<500;i++){
        clock_t inner_st = clock();
        c += a_array[i*100] + b_array[i];
        clock_t inner_ed  = clock();

        sum_clk += (inner_ed - inner_st);

    }


    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ed_clk);

    printf("sum overhead clock: %ld\n",sum_clk);
    double t_ns = (double)(ed_clk.tv_sec - st_clk.tv_sec) * 1.0e9 +
                  (double)(ed_clk.tv_nsec - st_clk.tv_nsec);
    printf("outer overhead nano seconds: %lf\n",t_ns);

    printf("%lf",c);
    return 0;
}

