

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
#define MPI_OWN_COMPLEX_TYPE MPI_C_DOUBLE_COMPLEX
typedef enum {
    false, true
} bool;
#endif // __GNUC__

const long double PI = M_PI;


void Swap(cd* a, cd* b) {
    cd temp = *b;
    *b = *a;
    *a = temp;
}

inline int log2_int(register int x) {
    register int ans = 0;
    while ((x >>= 1)) ++ans;
    return ans;
}


void print_complex_console(cd* cd_arr, int len) {

    printf("%d\n", len);
    int i = 0;
    for (i = 0; i < len; i++) {
        printf("(%.5lf + %.5lfi)\n", creal(cd_arr[i]), cimag(cd_arr[i]));
    }
    printf("\n");
}



void array_bit_reverse(cd* y, int len)
{

    int i, j;
    int k = 0;
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

//返回的是当前操作间距
int fft_seq(cd* a, int n, bool invert)
{

    int len;
    for (len = 2; len <= n; len <<= 1) {
        double ang = 2 * PI / len * (invert ? -1 : 1);
        cd wlen = cos(ang) +  sin(ang)*I;
        int i = 0;
        for (i = 0; i < n; i += len) {
            cd w = 1;
            int j = 0;
            for (j = 0; j < len / 2; j++) {
                cd u = a[i + j], v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }

    if (invert) {
        int i;
        for (i = 0; i < n; i++) {
            a[i] /= n;
        }

    }
    return len/2;
}

void init_poly_arr(const int* poly_a, const int* poly_b, int a_len, int b_len,
                   cd* poly_a_cd,cd* poly_b_cd,int len)
{
    int i;
    for (i = 0; i < a_len; i++) {
        poly_a_cd[i] = poly_a[i];
    }
    for (i = 0; i < b_len; i++) {
        poly_b_cd[i] = poly_b[i];
    }
    for (i = a_len; i < len; i++) {
        poly_a_cd[i] = 0;
    }
    for (i = b_len; i < len; i++) {
        poly_b_cd[i] = 0;
    }


}

int* multiply_polys_seq_fft(const int* poly_a, const int* poly_b, const int a_len, const int b_len, int* c_len)
{

    int len = 1;
    while (len < a_len + b_len) len <<= 1;

    cd* poly_a_cd =(cd*)malloc(sizeof(cd) * len);
    cd* poly_b_cd = (cd*)malloc(sizeof(cd) * len);
    init_poly_arr(poly_a, poly_b, a_len, b_len, poly_a_cd, poly_b_cd,len);

    //finding FFT of equations A and B separately
    array_bit_reverse(poly_a_cd, len);
    array_bit_reverse(poly_b_cd, len);
    fft_seq(poly_a_cd, len, false);
    fft_seq(poly_b_cd, len, false);


    //output_complex_arr(poly_a_cd, len, "test_out_a.txt");

    //combing the FFT of A and B
    cd* F_C = (cd*)malloc(sizeof(cd) * len);
    int i;
    for (i = 0; i < len; i++)
        F_C[i] = poly_a_cd[i] * poly_b_cd[i];

    //finding the inverse DFT and dividing it by 1/n
    array_bit_reverse(F_C, len);
    fft_seq(F_C, len, true);

    int* res = (int*)malloc(sizeof(int) * len);

    for (i = 0; i < len; i++) {
        res[i] = ((int)(creal(F_C[i]) + 0.5));
    }

    *c_len = len;
    free(poly_a_cd);
    free(poly_b_cd);
    free(F_C);
    return res;
}


void fft_one_step(cd* a, int n,int step,bool invert)
{

    //int len = step;
    double ang = 2 * PI / step * (invert ? -1 : 1);
    cd wlen = cos(ang) + sin(ang)*I;
    int i = 0;
    for (i = 0; i < n; i += step) {
        cd w = 1;
        int j = 0;
        for (j = 0; j < step / 2; j++) {
            cd u = a[i + j], v = a[i + j + step / 2] * w;
            a[i + j] = u + v;
            a[i + j + step / 2] = u - v;
            w *= wlen;
        }
    }


    if (invert) {
        int i;
        for (i = 0; i < n; i++) {
            a[i] /= n;
        }

    }

    return;
}

cd* init_par_array(int* array_a, int a_len, int final_len) {
    cd* array_a_cd = (cd*)malloc(sizeof(cd) * final_len);
    int i = 0;
    for (i = 0; i < a_len; i++) {
        array_a_cd[i] = array_a[i];
    }
    for (i = a_len; i < final_len; i++) {
        array_a_cd[i] = 0;
    }
    array_bit_reverse(array_a_cd, final_len);

    return array_a_cd;
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



void array_par_fft(int* array_a, int a_len) {

    int len = 1;
    while (len < a_len +1 ) len <<= 1;

    cd* array_a_cd = init_par_array(array_a, a_len,len);

    MPI_Init(NULL, NULL);
    int my_rank, proc_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_size);



    /* create a type for struct complex */
#ifdef _MSC_VER
    const int nitems = 2;
    int          blocklengths[2] = { 1,1 };
    MPI_Datatype types[2] = { MPI_DOUBLE, MPI_DOUBLE };
    MPI_Datatype MPI_OWN_COMPLEX_TYPE;
    MPI_Aint     offsets[2];
    offsets[0] = offsetof(cd, _real);
    offsets[1] = offsetof(cd, _imag);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_OWN_COMPLEX_TYPE);
    MPI_Type_commit(&MPI_OWN_COMPLEX_TYPE);

#endif // _MSC_VER
    /* create a type for struct complex  DONE*/

    int PER_PROC_SIZE = len / proc_size;
    if (len % proc_size) {
        printf("the input length is wrong!\n");
        return;
    }

    cd* array_per_proc = (cd*)malloc(sizeof(cd) * len);
    MPI_Scatter(array_a_cd, PER_PROC_SIZE, MPI_OWN_COMPLEX_TYPE, \
        array_per_proc, PER_PROC_SIZE ,MPI_OWN_COMPLEX_TYPE, \
        0, MPI_COMM_WORLD);


    int cur_fft_step = fft_seq(array_per_proc, PER_PROC_SIZE, false);

    if (my_rank == 0) {
        printf("%d\n", cur_fft_step);
    }


    unsigned bit_mask = 1;
    int done = 0;
    int partner;
    //每次当前处理器与相邻处理器的间距
    unsigned proc_step = 2;
    unsigned half_proc_step = proc_step >> 1;


    while (!done && bit_mask < proc_size) {
        partner = my_rank ^ bit_mask;
        // 接收数据，计算下一轮
        if (my_rank < partner) {
            //
            cd* cur_cd_address = array_per_proc + (PER_PROC_SIZE) * (half_proc_step);
            MPI_Recv(cur_cd_address, PER_PROC_SIZE * half_proc_step, MPI_OWN_COMPLEX_TYPE, \
                my_rank + half_proc_step, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
            cur_fft_step *= 2;
            fft_one_step(array_per_proc, PER_PROC_SIZE * proc_step, cur_fft_step, false);

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







    if (my_rank == 0) {
        print_complex_console(array_per_proc, len);
    }


    free(array_per_proc);

    MPI_Finalize();

    return;
}

int* multiply_polys_par_mpi_fft(int* poly_a, int* poly_b, int a_len, int b_len, int* c_len)
{

    int len = 1;
    while (len < a_len + b_len) len <<= 1;

    cd* poly_a_cd = (cd*)malloc(sizeof(cd) * len);
    cd* poly_b_cd = (cd*)malloc(sizeof(cd) * len);
    init_poly_arr(poly_a, poly_b, a_len, b_len, poly_a_cd, poly_b_cd, len);


    return NULL;
}

//读取文件行数，并将文件指针指回头部
int count_file_line(FILE* file_ptr) {
    int line_count = 0;
    char ch;
    while ((ch = fgetc(file_ptr)) != EOF) {
        if (ch == '\n')
            line_count++;
    }
    rewind(file_ptr);
    return line_count;
}

void init_integer_array_from_file(FILE* file_ptr, int* arr,int file_line_cnt) {
    int i = 0;
    for (i = 0; i < file_line_cnt; i++) {
        fscanf(file_ptr, "%d", &arr[i]);
    }
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
        return 0;
    }

    int a_len = count_file_line(arr_a_file_ptr);
    int b_len = count_file_line(arr_b_file_ptr);

    int* array_a = (int*)malloc(sizeof(int) * a_len);
    int* array_b = (int*)malloc(sizeof(int) * b_len);

    init_integer_array_from_file(arr_a_file_ptr, array_a, a_len);
    init_integer_array_from_file(arr_b_file_ptr, array_b, b_len);



    //Sequential
    int c_len = 0;
    int* seq_fft_res = multiply_polys_seq_fft(array_a, array_b, a_len, b_len, &c_len);

    int i = 0;
    for (i = 0; i < c_len; i++) {
        printf("%d  ", seq_fft_res[i]);
    }





    //Parallel
    array_par_fft(array_a,a_len);









    return 0;
}