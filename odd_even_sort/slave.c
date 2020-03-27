#include <stdio.h>
#include <math.h>
#include <string.h>
#include <slave.h>

#define J 64

__thread_local volatile int get_reply, put_reply, my_id;
__thread_local volatile unsigned long st, ed;
__thread_local double slave_pair[2];

extern double slave[J];

void rtc(unsigned long *counter)
{
    unsigned long rtc;
    asm volatile("rcsr %0,4": "=r"(rtc));
    *counter = (unsigned long)rtc;
}



void swap(double *xp, double *yp)  
{  
    double temp = *xp;  
    *xp = *yp;  
    *yp = temp;  
}  



void func()
{
    my_id = athread_get_id(-1);

    //最后一个线程直接返回
    if (my_id == 63)
        return;

    int loop_cnt = J/ 2;
    get_reply = 0;
    put_reply = 0;

    long ic0, ic1;
    long tc0, tc1;
    unsigned long st_get = 0, ed_get = 0, st_com = 0, ed_com = 0, st_put = 0, ed_put = 0;

    int i=0;
    for (; i < loop_cnt; i++)
    {   
        //penv_slave_float_ipc_init ();
        //取数,计时
        if (my_id == 0 )
            rtc(&st_get);
        //penv_slave_float_ipc_count(&ic0, &tc0);

        athread_get(PE_MODE, &slave[my_id], &slave_pair[0],
                    2 * 8, &get_reply, 0, 0, 0);

        // athread_get(PE_MODE, &slave[my_id+1], &slave_pair[1], \
        //     8, &get_reply, 0, 0, 0);
        while (get_reply != 2)
            ;
        if (my_id == 0)
            rtc(&ed_get);
        //完成取数

        //计算，计时
        if (my_id == 0)
            rtc(&st_com);
        //even phase
        if(!(my_id &0x1)){
            if(slave_pair[0] > slave_pair[1])
                swap(&slave_pair[0] , &slave_pair[1]);
        }
       athread_syn(ARRAY_SCOPE, 0xFFFF);

        if(my_id & 0x1){
            if(slave_pair[0] > slave_pair[1])
                swap(&slave_pair[0] , &slave_pair[1]);
        }
        athread_syn(ARRAY_SCOPE, 0xFFFF);


        if (my_id == 0)
            rtc(&ed_com);
        //完成计算，计时

        if (my_id == 0)
            rtc(&st_put);
        athread_put(PE_MODE, &slave_pair[0], &slave[my_id],
                    2 * 8, &put_reply, 0, 0);
        while (put_reply != 1)
            ;
        if (my_id == 0)
            rtc(&ed_put);

        //penv_slave_float_ipc_count(&ic1, &tc1);

        if (my_id == 0)
        {
            printf("ThreadID= %d, rate= %f (%ld/%ld)\n ", my_id, ((double)(ic1 - ic0) / (double)(tc1 - tc0) / 8.0), ic1 - ic0, tc1 - tc0);
            printf("count get = %ld compute = %ld put = %ld\ntotal=%ld\n", ed_get - st_get, ed_com - st_com, ed_put - st_put, ed_get - st_get + ed_com - st_com + ed_put - st_put);
        }
    }
    return;
}
