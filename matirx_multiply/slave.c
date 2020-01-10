#include<stdio.h>
#include<math.h>
#include<string.h>
#include<slave.h>


#define J 64
#define I 64
#define K 64

__thread_local volatile int get_reply,put_reply,my_id;
__thread_local volatile unsigned long st,ed;
__thread_local double a_slave[I],b_slave[K][I],c_slave[I];

extern  double a[J][K],b[K][I], master[J][I],slave[J][I];
extern unsigned long counter[64];

static inline unsigned long rtc(){
    unsigned long rtc;
    asm volatile("rcsr %0,4":"=r"(rtc));
    return rtc;
}

void func(){
    my_id = athread_get_id(-1);
    //printf("current thread id = %d",my_id);
    get_reply = 0;
    put_reply = 0;
    athread_get(PE_MODE, &a[my_id][0], &a_slave[0], \
        K * 8, &get_reply, 0, 0, 0); 
        
    athread_get(PE_MODE, &b[0][0], &b_slave[0][0], \
        K * I * 8, &get_reply, 0, 0, 0); 
    while(get_reply!=2);
    
    //printf("%f\t%f\n",a_slave[999],b_slave[999]);
    fflush(NULL);
    int i,k;
    for(i=0;i<I;i++){
        c_slave[i] = 0;
        for(k=0;k<K;k++)
            c_slave[i] += a_slave[i] * b_slave[k][i];
    }
    //printf("myid = %d done\n",my_id);
    athread_put(PE_MODE, &c_slave[0], &slave[my_id][0], \
        I * 8, &put_reply, 0, 0);
    while(put_reply!=1);

    
}
