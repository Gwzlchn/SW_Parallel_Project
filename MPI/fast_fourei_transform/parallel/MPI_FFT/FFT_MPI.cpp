

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


int fft(cd* a, int n, bool invert) 
{

    int len;
    for (len = 2; len <= n; len <<= 1) {
        double ang = 2 * PI / len * (invert ? -1 : 1);
        cd wlen(cos(ang), sin(ang));
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
    return len;
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
    fft(poly_a_cd, len, false);
    fft(poly_b_cd, len, false);


    output_complex_arr(poly_a_cd, len, "test_out_a.txt");

    //combing the FFT of A and B
    cd* F_C = (cd*)malloc(sizeof(cd) * len);
    int i;
    for (i = 0; i < len; i++)
        F_C[i] = poly_a_cd[i] * poly_b_cd[i];

    //finding the inverse DFT and dividing it by 1/n
    array_bit_reverse(F_C, len);
    fft(F_C, len, true);

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


int fft_one_step(cd* a, int n,int step,bool invert)
{

    int len = step;
   // for (len = 2; len <= n; len <<= 1) {
        double ang = 2 * PI / len * (invert ? -1 : 1);
        cd wlen(cos(ang), sin(ang));
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
   // }

    if (invert) {
        int i;
        for (i = 0; i < n; i++) {
            a[i] /= n;
        }

    }

    return len;
}

void array_par_fft(int* array_a, int len) {
    cd* array_a_cd = (cd*)malloc(sizeof(cd) * len);
    int i = 0;
    for (i = 0; i < len; i++) {
        array_a_cd[i] = array_a[i];
    }
    array_bit_reverse(array_a_cd, len);

    MPI_Init(NULL, NULL);
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
   
    int lg_proc_size = log2_int(size);
    int lg_arr_len = log2_int(len);

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

    int PER_PROC_SIZE = len / size;
    if (len % size) {
        printf("the input length is wrong!\n");
        return;
    }

    cd* array_per_proc = (cd*)malloc(sizeof(cd) * len);
    MPI_Scatter(array_a_cd, PER_PROC_SIZE, MPI_OWN_COMPLEX_TYPE, \
        array_per_proc, PER_PROC_SIZE ,MPI_OWN_COMPLEX_TYPE, \
        0, MPI_COMM_WORLD);

    //fft_one_step(array_a_cd, PER_PROC_SIZE, false);


    int cur_fft_step = fft(array_per_proc, PER_PROC_SIZE, false);

    int step = 2;
    for (step = 2; step <= size; step<<=1) {
        int half_step = step >> 1;
        if (rank % step != 0) {
            MPI_Send(array_per_proc, PER_PROC_SIZE * half_step, MPI_OWN_COMPLEX_TYPE, \
                rank - half_step, 0, MPI_COMM_WORLD);

            break;
        }
        else {
            cd* cur_cd_address = array_per_proc + PER_PROC_SIZE * half_step;
            MPI_Recv(cur_cd_address, PER_PROC_SIZE * half_step, MPI_OWN_COMPLEX_TYPE, \
                rank + half_step, 0, MPI_COMM_WORLD,MPI_STATUSES_IGNORE);
            cur_fft_step *= 2; 
            fft_one_step(array_per_proc, PER_PROC_SIZE * step, cur_fft_step,false);
        }
    }





    if (rank == 0) {
        print_complex_console(array_per_proc, len);
    }

    



    //array_per_proc = (cd*)

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


int main(int argc, char** argv) {


    int a[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int b[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int a_len = sizeof(a) / sizeof(a[0]);
    int b_len = sizeof(b) / sizeof(b[0]);
    int c_len = 0;
    int* seq_fft_res = multiply_polys_seq_fft(a, b, a_len, b_len, &c_len);

    int i = 0;
    for (i = 0; i < c_len; i++) {
       // printf("%d  ", seq_fft_res[i]);
    }

    argv[1] = "array_in.txt";
    if (argv[1] == NULL) {
        
        return 0;
    }

    int par_size = 0;
    FILE* arr_file;
    arr_file = fopen(argv[1], "r");

    if (arr_file) {
        fscanf(arr_file, "%d", &par_size);
        int* par_arr = (int*)malloc(sizeof(int) * par_size);


        //i = 0;
        for (i = 0; i < par_size; i++) {
            fscanf(arr_file, "%d", &par_arr[i]);
        }

        array_par_fft(par_arr, par_size);
        
    }
    else {
        printf("NO INOUT FILE!\n");
    }
    






    return 0;
}