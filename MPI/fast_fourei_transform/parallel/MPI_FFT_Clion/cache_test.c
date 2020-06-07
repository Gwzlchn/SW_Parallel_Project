//
// Created by Zelin on 20/05/06.
//


#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"


#define  ull unsigned long long
#define KB(x) ((ull) (x) << 10)
#define len 18

/* This solution only works when N      */
/* evenly divides into RAND_MAX.        */
/* So, the results are not equiprobably */
ull randomRange(ull min, ull max)
{
    srand((unsigned)time(NULL));
    ull random =
            (((ull) rand() <<  0) & 0x00000000FFFFFFFFull) |
            (((ull) rand() << 32) & 0xFFFFFFFF00000000ull);
    return ( random % max);
}


void fill_char_arr(char* arr,ull length){
    ull i=0;
    for(;i<length;i++){
       arr[i] = (char)(i*i)%255;
    }
    return;
}
int main()
{
    //需要测试的数组大小
    ull sizes_KB[len];
    int i=0;
    for(i=0;i<len;i++){
        sizes_KB[i] = 1 << i;
    }



    for (i=0;i<len;i++)
    {
        ull size = sizes_KB[i];
        char* memory_arr = (char*)malloc(KB(size)*sizeof(char));


        fill_char_arr(memory_arr,KB(size));



        ull dummy = 0;//用这个变量避免编译器对下面的循环进行优化

        //在内存块上进行大量的随机访问，并计时
        clock_t begin = clock();
        ull j=0;
        for (j = 0; j < (1<<25); j++){dummy += memory_arr[randomRange(0,KB(size)-1)];}
        clock_t end = clock();

        double elapsed_secs = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("%lld KB %lf decs, dummy: %lld\n",size,elapsed_secs,dummy);

    }
    return 0;
}