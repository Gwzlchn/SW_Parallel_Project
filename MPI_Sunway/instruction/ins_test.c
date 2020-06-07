
#include <stdio.h>
#include <complex.h>
//#include "swlu.h"
//#include "mpi.h"
#define GET_CYCLE(x) asm volatile ("rtc %0": "=r" (x));
#define ull unsigned long long
#define ul unsigned long
#define N 102400

int a_array[N];

void fill_int_arr(int* arr,ull length){
    ull i=0;
    for(;i<length;i++){
        arr[i] = (int)(i*i);
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


    fill_int_arr(a_array,N);

    ull i=0;
    ull times = 10;


    printf("BEGIN add  numbers\n");
    //MPI_Init(&argc,&argv);
    //swlu_prof_init();
    //swlu_prof_start();
    for(i=0;i<times;i++){
        a *= 1.1;
        b *= 1.1;

       ul st_cycle=0,ed_cycle=0;
        GET_CYCLE(st_cycle)
        volatile double c = a+b;

        GET_CYCLE(ed_cycle)

        //printf("start cycle %ld,\tend cycle %ld\t\t",st_cycle,ed_cycle);

        unsigned long overhead = ed_cycle-st_cycle;
        printf("add two number overhead:%ld\n",overhead);

        //printf("current k:%lf\n",k);

        //unsigned long overhead_clock = ed_clk - st_clk;
        //printf("overhead clock: %ld\n",overhead_clock);
        st_cycle = ed_cycle = 0;
    }


    printf("END ADD  numbers\n\n");

    //swlu_prof_stop();
    //swlu_prof_print();






    printf("BEGIN MULTIPLY two numbers\n");
    a = b = 0;
    for(i=0;i<times;i++){
        a = a_array[i*32];
        b = a_array[i*64];

        ul st_cycle = 0,ed_cycle=0;
        GET_CYCLE(st_cycle)
        volatile double  c = a*b;
        GET_CYCLE(ed_cycle)

        //printf("start cycle %ld,\tend cycle %ld\t\t",st_cycle,ed_cycle);
        unsigned long overhead = ed_cycle-st_cycle;
        printf("overhead:%ld\n",overhead);
        //printf("current c:%lf\n",c);
        volatile double k = c*2+a;

    }
    printf("END MULTIPLY two numbers\n\n");





    printf("BEGIN Memory\n");
    for(i=times;i>0;i--){

        ul st_cycle=0,ed_cycle=0;
        GET_CYCLE(st_cycle)
        volatile double c = a_array[i] + a_array[i*4096];
        GET_CYCLE(ed_cycle)

        //printf("start cycle %ld,\tend cycle %ld\t\t",st_cycle,ed_cycle);
        unsigned long overhead = ed_cycle-st_cycle;
        printf("overhead:%ld\n",overhead);
        //printf("current c  :%lf\n",c);
        st_cycle = ed_cycle = 0;
    }
    printf("END Memory\n\n");

    
    //MPI_Finalize();


    return 0;

}
