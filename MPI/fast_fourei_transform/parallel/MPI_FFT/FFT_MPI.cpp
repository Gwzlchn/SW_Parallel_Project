

#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<mpi.h>


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


void Rader(cd* y, int len) 
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


void fft(cd* a, int n, bool invert) 
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
    Rader(poly_a_cd, len);
    Rader(poly_b_cd, len);
    fft(poly_a_cd, len, false);
    fft(poly_b_cd, len, false);


    //combing the FFT of A and B
    cd* F_C = (cd*)malloc(sizeof(cd) * len);
    int i;
    for (i = 0; i < len; i++)
        F_C[i] = poly_a_cd[i] * poly_b_cd[i];

    //finding the inverse DFT and dividing it by 1/n
    Rader(F_C, len);
    fft(F_C, len, true);

    int* res = (int*)malloc(sizeof(int) * len);

    for (i = 0; i < len; i++) {
        res[i] = ((int)(creal(F_C[i]) + 0.5));
    }

    *c_len = len;
    return res;
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


int main() {


    int a[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int b[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int a_len = sizeof(a) / sizeof(a[0]);
    int b_len = sizeof(b) / sizeof(b[0]);
    int c_len = 0;
    int* seq_fft_res = multiply_polys_seq_fft(a, b, a_len, b_len, &c_len);

    int i = 0;
    for (i = 0; i < c_len; i++) {
        printf("%d  ", seq_fft_res[i]);
    }


    return 0;
}