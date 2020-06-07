

#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<mpi.h>
#include <stddef.h>

#ifdef _MSC_VER
#include<corecrt_math_defines.h>
#include"Complex.h"
typedef Complex cd;
#endif // __MSVER_

#ifdef __GNUC__

#include<complex.h>

typedef double complex cd;

typedef enum {
    false, true
} bool;
#endif // __GNUC__

const long double PI = M_PI;
MPI_Datatype  MPI_OWN_COMPLEX_TYPE;
#define debug0 0
typedef unsigned long long IntCoefType;
typedef unsigned long LenType;


/* create a type for struct complex */
MPI_Datatype create_MPI_type()
{
    const int nitems = 2;
    int          blocklengths[2] = { 1,1 };
    MPI_Datatype types[2] = { MPI_DOUBLE, MPI_DOUBLE };
    MPI_Datatype MPI_OWN_COMPLEX_TYPE;
    MPI_Aint     offsets[2];
    offsets[0] = offsetof(cd, _real);
    offsets[1] = offsetof(cd, _imag);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_OWN_COMPLEX_TYPE);
    MPI_Type_commit(&MPI_OWN_COMPLEX_TYPE);

    return MPI_OWN_COMPLEX_TYPE;
}

void Swap(cd* a, cd* b) {
    cd temp = *b;
    *b = *a;
    *a = temp;
}



//返回 a b 之和最近的二次幂
inline LenType power_2_sum(LenType a_len, LenType b_len) {
    LenType len = 1;
    while (len < a_len + b_len) len <<= 1;
    return len;
}


//读取文件行数，并将文件指针指回头部
inline LenType count_file_line(FILE* file_ptr) {
    LenType line_count = 0;
    char ch;
    while ((ch = fgetc(file_ptr)) != EOF) {
        if (ch == '\n')
            line_count++;
    }
    rewind(file_ptr);
    return line_count;
}

inline void init_integer_array_from_file(FILE* file_ptr, IntCoefType* arr, LenType file_line_cnt) {
    LenType i = 0;
    for (i = 0; i < file_line_cnt; i++) {
        fscanf(file_ptr, "%lld", &arr[i]);
    }
}

void array_bit_reverse(cd* y, LenType len)
{
    LenType i, j;
    LenType k = 0;
    for (i = 1, j = len >> 1, k; i < len - 1; ++i) {
        if (i < j)
            Swap(&y[i], &y[j]);
        k = len >> 1;
        while (j >= k) {
            j -= k;
            k >>= 1;
        }
        if (j < k)
            j += k;
    }
}


cd* init_complex_array(const IntCoefType* array_a, LenType a_len, LenType final_len) {

    cd* array_a_cd = (cd*)malloc(sizeof(cd) * final_len);
    LenType i = 0;
    for (i = 0; i < a_len; i++) {
        array_a_cd[i] = array_a[i];
    }
    for (i = a_len; i < final_len; i++) {
        array_a_cd[i] = 0;
    }

    return array_a_cd;
}



IntCoefType* complex_array_round_to_int(cd* cd_array, LenType len) {
    IntCoefType* res = (IntCoefType*)malloc(sizeof(IntCoefType) * len);
    LenType i = 0;
    for (i = 0; i < len; i++) {
        res[i] = ((IntCoefType)(creal(cd_array[i]) + 0.5));
    }
    return res;
}


void fft_one_step(cd* a, LenType n, LenType step, bool invert)
{
    double ang = 2 * PI / step * (invert ? -1 : 1);
    cd wlen(cos(ang), sin(ang));
    LenType i = 0;
    for (i = 0; i < n; i += step) {
        cd w = 1;
        LenType j = 0;
        for (j = 0; j < step / 2; j++) {
            cd u = a[i + j], v = a[i + j + step / 2] * w;
            a[i + j] = u + v;
            a[i + j + step / 2] = u - v;
            w *= wlen;
        }
    }
    return;
}

//返回的是当前操作间距
int fft_seq(cd* a, LenType n, bool invert)
{

    LenType len;
    for (len = 2; len <= n; len <<= 1) {
        fft_one_step(a, n, len, invert);
    }

    
    return len/2;
}

inline void invert_fft_divide(cd* array, LenType n)
{
    LenType i;
    for (i = 0; i < n; i++) {
        array[i] /= n;
    }
 
}






IntCoefType* multiply_polys_seq_fft(const IntCoefType* poly_a, const IntCoefType* poly_b, \
                            const LenType a_len, const LenType b_len, LenType len)
{
   
   

    cd* poly_a_cd = init_complex_array(poly_a, a_len, len);
    cd* poly_b_cd = init_complex_array(poly_b, b_len, len);
   

    //finding FFT of equations A and B separately
    array_bit_reverse(poly_a_cd, len);
    array_bit_reverse(poly_b_cd, len);
    fft_seq(poly_a_cd, len, false);
    fft_seq(poly_b_cd, len, false);


    //combing the FFT of A and B
    cd* F_C = (cd*)malloc(sizeof(cd) * len);
    LenType i;
    for (i = 0; i < len; i++)
        F_C[i] = poly_a_cd[i] * poly_b_cd[i];
    if (debug0) {
        printf("\narray F_C before Sequential FFT\n");
        print_complex_console(F_C, len);
    }
   
    //finding the inverse DFT and dividing it by 1/n
    array_bit_reverse(F_C, len);
    fft_seq(F_C, len, true);
    if (debug0) {
        printf("\narray F_C after Sequential FFT\n");
        print_complex_console(F_C, len);
    }


    invert_fft_divide(F_C, len);



    
    IntCoefType* res = complex_array_round_to_int(F_C, len);
    

    //free(poly_a_cd);
    //free(poly_b_cd);
    //free(F_C);
    return res;
}





/*-----------------------------------------------------------------*/
/* Function:    Parallel FFT
 * Purpose:     Compute the FFT of ints stored on the processes
 *
 *
 * Notes:
 *    1.  Uses tree structured communication.
 *    2.  proc_size, the number of processes must be a power of 2.
 *    3.  The return value is valid only on process 0.
 *    4.  The pairing of the processes is done using bitwise
 *        exclusive or.  Here's a table showing the rule for
 *        for bitwise exclusive or
 *           X Y X^Y
 *           0 0  0
 *           0 1  1
 *           1 0  1
 *           1 1  0
 *        Here's a table showing the process pairing with 8
 *        processes (r = my_rank, other column heads are bitmask)
 *           r     001 010 100
 *           -     --- --- ---
 *           0 000 001 010 100
 *           1 001 000  x   x
 *           2 010 011 000  x
 *           3 011 010  x   x
 *           4 100 101 110 000
 *           5 101 100  x   x
 *           6 110 111 100  x
 *           7 111 110  x   x
 */
cd* array_par_fft
    (cd* a_complex_array, LenType a_complex_len,bool invert,\
        int my_rank,int proc_size) 
{

 
    LenType PER_PROC_SIZE = a_complex_len / proc_size;

    if (a_complex_len % proc_size) {
        printf("the input length is wrong!\n");
        return NULL;
    }

    cd* array_per_proc = (cd*)malloc(sizeof(cd) * a_complex_len);
    MPI_Scatter(a_complex_array, PER_PROC_SIZE, MPI_OWN_COMPLEX_TYPE, \
        array_per_proc, PER_PROC_SIZE ,MPI_OWN_COMPLEX_TYPE, \
        0, MPI_COMM_WORLD);


    LenType cur_fft_step = fft_seq(array_per_proc, PER_PROC_SIZE, invert);


    unsigned bit_mask = 1;
    int done = 0;
    int partner;
    //每次当前处理器与相邻处理器的间距
    LenType proc_step = 2;
    LenType half_proc_step = proc_step >> 1;


    while (!done && bit_mask < proc_size) {
        partner = my_rank ^ bit_mask;
        // 接收数据，计算下一轮
        if (my_rank < partner) {
            //
            cd* cur_cd_address = array_per_proc + size_t(PER_PROC_SIZE) * size_t(half_proc_step);
            MPI_Recv(cur_cd_address, PER_PROC_SIZE * half_proc_step, MPI_OWN_COMPLEX_TYPE, \
                my_rank + half_proc_step, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            cur_fft_step *= 2;
            fft_one_step(array_per_proc, PER_PROC_SIZE * proc_step, cur_fft_step, invert);

            bit_mask <<= 1;
            proc_step <<= 1;
            half_proc_step <<= 1;
        }
        //发送数据，之后退出
        else {
            MPI_Send(array_per_proc, PER_PROC_SIZE * half_proc_step, MPI_OWN_COMPLEX_TYPE, \
                my_rank - half_proc_step, 0, MPI_COMM_WORLD);
            done = 1;
        }
    }

    return array_per_proc;
}

IntCoefType* multiply_polys_par_mpi_fft
    (IntCoefType* poly_a, IntCoefType* poly_b, LenType a_len, LenType b_len, LenType c_len,\
    int my_rank,int proc_size) 
{

    
   
    LenType len = 1;
        while (len < a_len + b_len) len <<= 1;

        cd* array_a_cd = init_complex_array(poly_a, a_len, len);
        cd* array_b_cd = init_complex_array(poly_b, b_len, len);
        array_bit_reverse(array_a_cd, len);
        array_bit_reverse(array_b_cd, len);
        cd* poly_a_cd_fft = array_par_fft(array_a_cd, len, false,my_rank, proc_size);
        cd* poly_b_cd_fft = array_par_fft(array_b_cd, len, false,my_rank, proc_size);


        cd* poly_F_C_root = (cd*)malloc(sizeof(cd) * len);


       /* printf("\nCURRENT ARRAY A\n");
        print_complex_console(poly_a_cd_fft, len);
        printf("\nCURRENT ARRAY B\n");
        print_complex_console(poly_b_cd_fft, len);*/

       
        int i = 0;
        for (i = 0; i < len; i++) {
            poly_F_C_root[i] = poly_a_cd_fft[i] * poly_b_cd_fft[i];
        }
        
        array_bit_reverse(poly_F_C_root, len);
        cd* poly_F_C = array_par_fft(poly_F_C_root, len, true, my_rank, proc_size);
       

        invert_fft_divide(poly_F_C, len);

    
        if (my_rank == 0) {
            IntCoefType* res = complex_array_round_to_int(poly_F_C, len);
            return res;
    }



   
    return NULL;
}



int main(int argc, char** argv) {

    //第一个输入参数是A数组文件
    argv[1] = "array_a_in.txt";
    argv[2] = "array_b_in.txt";
    FILE* arr_a_file_ptr,*arr_b_file_ptr;
    arr_a_file_ptr = fopen(argv[1], "r");
    arr_b_file_ptr = fopen(argv[2], "r");

    if (!arr_a_file_ptr || !arr_b_file_ptr) {
        printf("NO INOUT FILE!\n");
        return -1;
    }

    LenType a_len = count_file_line(arr_a_file_ptr);
    LenType b_len = count_file_line(arr_b_file_ptr);
    LenType c_len = power_2_sum(a_len, b_len);
    IntCoefType* array_a = (IntCoefType*)malloc(sizeof(IntCoefType) * a_len);
    IntCoefType* array_b = (IntCoefType*)malloc(sizeof(IntCoefType) * b_len);

    init_integer_array_from_file(arr_a_file_ptr, array_a, a_len);
    init_integer_array_from_file(arr_b_file_ptr, array_b, b_len);



     MPI_Init(&argc, &argv);


    int my_rank, proc_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_size);

    /* create a type for struct complex */
#ifdef _MSC_VER
    MPI_OWN_COMPLEX_TYPE = create_MPI_type();
#endif // _MSC_VER

       
    //check Sequential in rank 0
    if (my_rank == 0) {
       
        IntCoefType* seq_fft_res = multiply_polys_seq_fft(array_a, array_b, a_len, b_len, c_len);

        printf("Sequential ANSWER\n");
        LenType i = 0;
        for (i = 0; i < c_len; i++) {
            if (i % 12 == 0)printf("\n");
            printf("%lld\t", seq_fft_res[i]);
            
        }
        printf("\nSequential ANSWER DONE\n");
        printf("\n\n\n");
    }
    



    //Parallel    
    IntCoefType* res = multiply_polys_par_mpi_fft(array_a, array_b, a_len, b_len, c_len,my_rank,proc_size);
    if (my_rank == 0) {

        printf("PARALLEL ANSWER\n");
        LenType i = 0;
        for (i = 0; i < c_len; i++) {
            if (i % 12 == 0)printf("\n");
            printf("%lld\t", res[i]);
            
        }
        printf("\nPARALLEL ANSWER DONE\n");
        printf("\n\n\n");
    }

    MPI_Finalize();




    return 0;
}