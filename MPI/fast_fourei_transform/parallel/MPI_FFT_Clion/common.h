//
// Created by Zelin on 20/05/06.
//

#ifndef MPI_FFT_CLION_COMMON_H
#define MPI_FFT_CLION_COMMON_H

// 主核拍数获取代码
inline void rpcc_(unsigned long *counter)
{
    unsigned long rpcc;
    asm("rtc %0": "=r" (rpcc) : );
    *counter=rpcc;
}

// 从核拍数获取代码
//void rtc_(unsigned long *counter)
//{
//    unsigned long rpcc;
//    asm volatile("rcsr %0, 4":"=r"(rpcc));
//    *counter=rpcc;
//}


#endif //MPI_FFT_CLION_COMMON_H
