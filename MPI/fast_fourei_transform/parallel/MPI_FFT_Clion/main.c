//#include <mpi.h>
#include <stdio.h>
//#include "common.h"
#include <complex.h>
#include "time.h"


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

enum bool{false,true};
enum bool copyFile( const char * srcFile , const char * destFile )
{
    FILE * src , * dst ;
    if( NULL == srcFile || NULL == destFile )
    {
        printf("copyFile src or dest file null !\n");
        return false;
    }
    src = fopen( srcFile , "rb" );
    dst = fopen( destFile, "wb" );
    if( NULL == src || NULL == dst )
    {
        printf("copyFile fopen failed !\n");
        return false;
    }
    // copy file
    char buf;
    while( fread( &buf , sizeof(char) , 1 , src ) != 0 )
    {
        // write to dst file !
        fwrite( &buf , sizeof(char) , 1 , dst );
    }
    // don't forget to close the files
    fclose( src );
    fclose( dst );
    return true;
}




int main(int argc, char** argv) {
    volatile  double a = 1.0;
    volatile double b = 2.0;
    int times = 16;
    unsigned long st_cycle=0 ,ed_cycle=0;
    fill_double_arr(a_array,N);

    int i=0;

    copyFile("/proc/cpuinfo","copy.txt");

    printf("BEGIN add four numbers\n");
    for(i=0;i<times;i++){
        a = a_array[i*16];
        b = a_array[i*32];


        asm volatile ("rtc %0": "=r" (st_cycle) );
        volatile double c = a+b;
        volatile double d = c+b;
        volatile double e = d+c;
        volatile double f = e+d;
        asm volatile("rtc %0": "=r" (ed_cycle) );

        //printf("start cycle %ld,\tend cycle %ld\t\t",st_cycle,ed_cycle);
        unsigned long overhead = ed_cycle-st_cycle;
        printf("overhead:%ld\n",overhead);
        volatile double k = a+b+c+d+e+f;
        //printf("current k:%lf\n",k);

        //unsigned long overhead_clock = ed_clk - st_clk;
        //printf("overhead clock: %ld\n",overhead_clock);
        st_cycle = ed_cycle = 0;
    }
    printf("END ADD four numbers\n\n");





    printf("\nBEGIN add two numbers\n");
    for(i=0;i<times;i++){
        a = a_array[i*16];
        b = a_array[i*32];


        asm volatile ("rtc %0": "=r" (st_cycle) : );
        volatile  double c = a+b;
        volatile double d = c+b;
        //volatile double e = d+c;
        //volatile double f = e+d;
        asm volatile ("rtc %0": "=r" (ed_cycle) : );


        //printf("start cycle %ld,\tend cycle %ld\t\t",st_cycle,ed_cycle);
        unsigned long overhead = ed_cycle-st_cycle;
        printf("overhead:%ld\n",overhead);

        //printf("current c:%lf\n",c);
        volatile double e = d+c;
        volatile double f = e+d;
        volatile double k = a+b+c+d+e+f;
        a_array[i] = k;

        st_cycle = ed_cycle = 0;
    }
    printf("END add two number\n\n");



    printf("BEGIN MULTIPLY two numbers\n");
    a = b = 0;
    for(i=0;i<times;i++){
        a = a_array[i*32];
        b = a_array[i*64];

        asm volatile ("rtc %0": "=r" (st_cycle) : );
        volatile double  c = a*b;
        asm volatile ("rtc %0": "=r" (ed_cycle) : );

        //printf("start cycle %ld,\tend cycle %ld\t\t",st_cycle,ed_cycle);
        unsigned long overhead = ed_cycle-st_cycle;
        printf("overhead:%ld\n",overhead);
        //printf("current c:%lf\n",c);
        volatile double k = c*2+a;
        a_array[i] = k;
        st_cycle = ed_cycle = 0;
    }


    printf("END MULTIPLY two numbers\n\n");

    double complex a_com = 1 + 2*I;
    double complex b_com = 2 + 4*I;
    printf("BEGIN Complex REGISTER\n");
    for(i=0;i<times;i++){
        a_com = a_array[i*16];
        b_com = a_array[i*32];

        asm volatile ("rtc %0": "=r" (st_cycle) : );
        double complex c_com = a_com*b_com;
        asm volatile ("rtc %0": "=r" (ed_cycle) : );

        //printf("start cycle %ld,\tend cycle %ld\t\t",st_cycle,ed_cycle);
        unsigned long overhead = ed_cycle-st_cycle;
        printf("overhead:%ld\n",overhead);
        //printf("current c real part:%lf\n",creal(c_com));
        volatile double k = creal(c_com)*2+a;
        a_array[i] = k;
        st_cycle = ed_cycle = 0;
    }
    printf("END Complex REGISTER\n\n");

    printf("BEGIN Memory\n");
    for(i=times;i>0;i--){


        asm volatile ("rtc %0": "=r" (st_cycle) : );
        volatile double c = a_array[i] + a_array[i*4096];
        asm volatile ("rtc %0": "=r" (ed_cycle) : );

        //printf("start cycle %ld,\tend cycle %ld\t\t",st_cycle,ed_cycle);
        unsigned long overhead = ed_cycle-st_cycle;
        printf("overhead:%ld\n",overhead);
        //printf("current c  :%lf\n",c);
        st_cycle = ed_cycle = 0;
    }
    printf("END Memory\n\n");

    asm volatile ("rtc %0": "=r" (st_cycle) : );
    asm volatile ("rtc %0": "=r" (ed_cycle) : );
    printf("start cycle %ld,\tend cycle %ld\t\t\n",st_cycle,ed_cycle);



    return 0;

}
