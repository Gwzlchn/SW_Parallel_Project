#include<stdio.h>
#include<math.h>
#include<string.h>
#include<slave.h>


#define J 128
#define SIZE  (J/64)

__thread_local volatile int get_reply,put_reply,my_id;
__thread_local volatile unsigned long st,ed;
__thread_local long slave_local[16];
__thread_local long slave_local_last = 0;

extern  long slave[J];


static inline void rtc(unsigned long *counter){
    unsigned long rtc;
    asm volatile("rcsr %0,4":"=r"(rtc));
    *counter =(unsigned long ) rtc;
}

void func(){
    
    my_id = athread_get_id(-1);
    int idx = my_id*SIZE;
    get_reply = 0;
    put_reply = 0;
    athread_get(PE_MODE, &slave[idx], &slave_local[0],  SIZE * 8, &get_reply, 0, 0, 0); 
    while(get_reply!=2)
        ;
    //rtc(&ed_get);

    //first time
    int i;
    for(i=1;i<SIZE;i++){
        slave_local[i] += slave_local[i-1];
    }

    athread_put(PE_MODE, &slave_local[0], &slave[idx], SIZE * 8, &put_reply, 0, 0);
    while(put_reply!=1)
        ;
    //rtc(&ed_put);
    athread_syn(ARRAY_SCOPE, 0xFFFF);
    if(my_id == 0) {
        printf("id=0 first time done");
         return;
    }
       

    //second time
    
    athread_get(PE_MODE, &slave[idx-1], &slave_local_last,   8, &get_reply, 0, 0, 0);
    slave[idx + SIZE - 1] += slave_local_last;
    athread_put(PE_MODE, &slave_local_last, &slave[idx + SIZE - 1],  8, &put_reply, 0, 0);
     while(put_reply!=1)
        ;
    //rtc(&ed_put);
    athread_syn(ARRAY_SCOPE, 0x7F7F);

    if(my_id == 1) {
        printf("id=1 second time done");
    }

    //third time
    for(i=0;i<SIZE-1;i++){
        slave_local[i] += slave_local_last;
    }
    athread_put(PE_MODE, &slave_local[0], &slave[idx], (SIZE -1)* 8, &put_reply, 0, 0);
    while(put_reply!=1)
        ;
    //rtc(&ed_put);
    athread_syn(ARRAY_SCOPE, 0x7F7F);

    if(my_id==1){
        printf("%ld",slave_local_last);
    }
    return;

    
}



















//register communication
// void func(){
//     my_id = athread_get_id(-1);

//     get_reply = 0;
//     put_reply = 0;

// 	  long ic0,ic1;
// 	  long tc0, tc1;
//     unsigned long st_get=0,ed_get=0,st_com=0,ed_com=0,st_put=0,ed_put=0;
    
//     rtc(&st_get);
// 	  penv_slave_float_ipc_count(&ic0,&tc0);
  

//     athread_get(PE_MODE, &a[my_id][0], &a_slave[0], \
//         K * 8, &get_reply, 0, 0, 0); 
        
//     athread_get(PE_MODE, &b[0][0], &b_slave[0][0], \
//         K * I * 8, &get_reply, 0, 0, 0); 
//     while(get_reply!=2);
//       rtc(&ed_get);
      
      
//        rtc(&st_com);
//     int i,k;
//     for(i=0;i<I;i++){
//         c_slave[i] = 0;
//         for(k=0;k<K;k++)
//             c_slave[i] += a_slave[i] * b_slave[k][i];
//     }
//     rtc(&ed_com);

//     rtc(&st_put);
//     athread_put(PE_MODE, &c_slave[0], &slave[my_id][0], \
//         I * 8, &put_reply, 0, 0);
//     while(put_reply!=1);
//       rtc(&ed_put);
      
//     penv_slave_float_ipc_count(&ic1,&tc1);
  
// 	  if(my_id==0) {
//     printf("ThreadID= %d, rate= %f (%ld/%ld)\n " , my_id, ((double)(ic1-ic0)/(double)(tc1-tc0)/8.0),ic1-ic0,tc1-tc0);
//     printf("count get = %ld compute = %ld put = %ld\ntotal=%ld\n",ed_get-st_get,ed_com-st_com,ed_put-st_put,ed_get-st_get+ed_com-st_com+ed_put-st_put);
//     }
// }
