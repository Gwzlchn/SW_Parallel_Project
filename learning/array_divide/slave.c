#include<stdio.h>
#include<math.h>
#include<string.h>
#include<slave.h>


#define J 64
#define I 1000

__thread_local volatile int get_reply,put_reply,my_id;
__thread_local volatile unsigned long st,ed;
__thread_local double a_slave[I],b_slave[I],c_slave[I];

extern double a[J][I],b[J][I],slave[J][I];
extern unsigned long counter[64];


void func(){
    my_id = athread_get_id(-1);
    //printf("current thread id = %d",my_id);
    get_reply = 0;
    put_reply = 0;
    athread_get(PE_MODE, &a[my_id][0], &a_slave[0], \
        I * 8, &get_reply, 0, 0, 0); 
        
    athread_get(PE_MODE, &b[my_id][0], &b_slave[0], \
        I * 8, &get_reply, 0, 0, 0); 
    while(get_reply!=2);
    
    printf("%f\t%f\n",a_slave[999],b_slave[999]);
    fflush(NULL);
    int i=0;
    for(;i<I;i++){
    
        c_slave[i] = a_slave[i] * b_slave[i];
    }
    //printf("myid = %d done\n",my_id);
    athread_put(PE_MODE, &c_slave[0], &slave[my_id][0], \
        I * 8, &put_reply, 0, 0);
    while(put_reply!=1);

    
}
