#include <math.h>
#include <iostream>
#include "mpi.h"
#define PI 3.141592653589793
#define TWOPI 6.283185307179586

int power2(const int exponent)
{
    int result = 1;
    for (int i = 0; i < exponent; i++) result *= 2;
    return result;
}

void radix2fft(double* __restrict__ x, double* __restrict__ y, const int m) // x real data, y imaginary data, 2^m is signal length
{
    const int N = power2(m);

    // bit reversal
    double xtmp, ytmp;
    const unsigned char rev[256] ={0x0,0x80,0x40,0xc0,0x20,0xa0,0x60,0xe0,0x10,0x90,0x50,0xd0,0x30,0xb0,0x70,0xf0,
                        0x8,0x88,0x48,0xc8,0x28,0xa8,0x68,0xe8,0x18,0x98,0x58,0xd8,0x38,0xb8,0x78,0xf8,
                        0x4,0x84,0x44,0xc4,0x24,0xa4,0x64,0xe4,0x14,0x94,0x54,0xd4,0x34,0xb4,0x74,0xf4,
                        0xc,0x8c,0x4c,0xcc,0x2c,0xac,0x6c,0xec,0x1c,0x9c,0x5c,0xdc,0x3c,0xbc,0x7c,0xfc,
                        0x2,0x82,0x42,0xc2,0x22,0xa2,0x62,0xe2,0x12,0x92,0x52,0xd2,0x32,0xb2,0x72,0xf2,
                        0xa,0x8a,0x4a,0xca,0x2a,0xaa,0x6a,0xea,0x1a,0x9a,0x5a,0xda,0x3a,0xba,0x7a,0xfa,
                        0x6,0x86,0x46,0xc6,0x26,0xa6,0x66,0xe6,0x16,0x96,0x56,0xd6,0x36,0xb6,0x76,0xf6,
                        0xe,0x8e,0x4e,0xce,0x2e,0xae,0x6e,0xee,0x1e,0x9e,0x5e,0xde,0x3e,0xbe,0x7e,0xfe,
                        0x1,0x81,0x41,0xc1,0x21,0xa1,0x61,0xe1,0x11,0x91,0x51,0xd1,0x31,0xb1,0x71,0xf1,
                        0x9,0x89,0x49,0xc9,0x29,0xa9,0x69,0xe9,0x19,0x99,0x59,0xd9,0x39,0xb9,0x79,0xf9,
                        0x5,0x85,0x45,0xc5,0x25,0xa5,0x65,0xe5,0x15,0x95,0x55,0xd5,0x35,0xb5,0x75,0xf5,
                        0xd,0x8d,0x4d,0xcd,0x2d,0xad,0x6d,0xed,0x1d,0x9d,0x5d,0xdd,0x3d,0xbd,0x7d,0xfd,
                        0x3,0x83,0x43,0xc3,0x23,0xa3,0x63,0xe3,0x13,0x93,0x53,0xd3,0x33,0xb3,0x73,0xf3,
                        0xb,0x8b,0x4b,0xcb,0x2b,0xab,0x6b,0xeb,0x1b,0x9b,0x5b,0xdb,0x3b,0xbb,0x7b,0xfb,
                        0x7,0x87,0x47,0xc7,0x27,0xa7,0x67,0xe7,0x17,0x97,0x57,0xd7,0x37,0xb7,0x77,0xf7,
                        0xf,0x8f,0x4f,0xcf,0x2f,0xaf,0x6f,0xef,0x1f,0x9f,0x5f,0xdf,0x3f,0xbf,0x7f,0xff};
 
    unsigned int i, reversal;
    for (i = 0; i < N; i++)
    {
	reversal = (unsigned int)((rev[i&0xFF]<<24) | (rev[(i>>8)&0xFF]<<16) | (rev[(i>>16)&0xFF]<<8) | (rev[(i>>24)&0xFF]))>>(32-m);
	if (i < reversal)
	{
	    xtmp = x[i];
	    ytmp = y[i];
	    x[i] = x[reversal];
	    y[i] = y[reversal];
	    x[reversal] = xtmp;
	    y[reversal] = ytmp;
	}
    }

    // radix 2 algorithm
    int l, s, e, slimit, p_l, p_l1, leftIndex, rightIndex;
    double re, im;
    double* lookupSin = new double[N/2];
    double* lookupCos = new double[N/2];

    for (l = 0; l < m; l++) // level
    {
	slimit = power2(m-l-1);
	p_l = power2(l);
	p_l1 = p_l*2;

	for (e = 0; e < p_l; e++) // lookup tables
	{
	    lookupCos[e] = cos(-PI*e/p_l);
	    lookupSin[e] = -sqrt(1-lookupCos[e]*lookupCos[e]);
	    //sincos(PI*e/p_l, lookupSin+e, lookupCos+e);
	}

	for (s = 0; s < slimit; s++) // section
	{
	    for (e = 0; e < p_l; e++) // element
	    {
		leftIndex = p_l1*s + e;
		rightIndex = leftIndex + p_l;
		
		re = lookupCos[e]*x[rightIndex] - lookupSin[e]*y[rightIndex];
		im = lookupCos[e]*y[rightIndex] + lookupSin[e]*x[rightIndex];

		x[rightIndex] = x[leftIndex] - re;
		y[rightIndex] = y[leftIndex] - im;
		x[leftIndex] += re;
		y[leftIndex] += im;
	    }
	}
    }

    delete[] lookupSin;
    delete[] lookupCos;

} // end void radix2fft()


// parallel cooley-turkey algorithm
void radix2fft_p(double* __restrict__ x, double* __restrict__ y, const int p) // x real data, y imaginary data, 2^p is signal length
{
    // treat data of length N as m x M matrix with m*M = N
    int N = power2(p),
	m_p = p/2,
	m = power2(m_p),
	M_p = p - m_p,
	M = N/m;
    double* xtmp = new double[N];
    double* ytmp = new double[N];

    int j, J, n;
    double arg, fsin, fcos;

    int  numtasks, rank;

    MPI_Init(NULL, NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double* xlocal, * ylocal;
    int length[numtasks], numElements[numtasks], displacement[numtasks];

    if (rank == 0)
    {
	for (j = 0; j < m; j++)
	{
	    // transpose
	    for (J = 0; J < M; J++)
	    {
		xtmp[j*M + J] = x[J*m + j];
		ytmp[j*M + J] = y[J*m + j];
	    }
	}
    }

    // ffts per task
    int	remainder = m % numtasks,
	sum = 0;

    for (int i = 0; i < numtasks; i++)
    {
	length[i] = m/numtasks;
	if (i < remainder) length[i] += 1;
	numElements[i] = length[i]*M;
	displacement[i] = sum;
	sum += numElements[i];
    }

    xlocal = new double[numElements[rank]];
    ylocal = new double[numElements[rank]];

    std::cout<<"rank="<<rank<<" length="<<length[rank]<<" numelements="<<numElements[rank]<<std::endl;

    MPI_Scatterv(xtmp, numElements, displacement, MPI_DOUBLE, xlocal, numElements[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(ytmp, numElements, displacement, MPI_DOUBLE, ylocal, numElements[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (j = 0; j < length[rank]; j++)
    {
	// fft each group
	radix2fft(&xlocal[j*M], &ylocal[j*M], M_p);
    }

    MPI_Gatherv(xlocal, numElements[rank], MPI_DOUBLE, xtmp, numElements, displacement, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gatherv(ylocal, numElements[rank], MPI_DOUBLE, ytmp, numElements, displacement, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    delete[] xlocal;
    delete[] ylocal;

    if (rank == 0)
    {
	// multiply by exp(-2*pi*i*j*J/N)
	for (j = 0; j < m; j++)
	{
	    for (J = 0; J < M; J++)
	    {
		n = j*M + J;
		sincos(-TWOPI*j*J/N, &fsin, &fcos);
		x[n] = xtmp[n]*fcos - ytmp[n]*fsin;
		y[n] = ytmp[n]*fcos + xtmp[n]*fsin;
	    }
	}

	for (J = 0; J < M; J++)
	{
	    for (j = 0; j < m; j++)
	    {
		// transpose
		xtmp[J*m + j] = x[j*M + J];
		ytmp[J*m + j] = y[j*M + J];
	    }

	}
    }

    // ffts per task
    remainder = M % numtasks;
    sum = 0;

    for (int i = 0; i < numtasks; i++)
    {
	length[i] = M/numtasks;
	if (i < remainder) length[i] += 1;
	numElements[i] = length[i]*m;
	displacement[i] = sum;
	sum += numElements[i];
    }

    std::cout<<"rank="<<rank<<" length="<<length[rank]<<" numelements="<<numElements[rank]<<std::endl;

    xlocal = new double[numElements[rank]];
    ylocal = new double[numElements[rank]];

    MPI_Scatterv(xtmp, numElements, displacement, MPI_DOUBLE, xlocal, numElements[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatterv(ytmp, numElements, displacement, MPI_DOUBLE, ylocal, numElements[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (J = 0; J < length[rank]; J++)
    {
	// fft each group
	radix2fft(&xlocal[J*m], &ylocal[J*m], m_p);
    }

    MPI_Gatherv(xlocal, numElements[rank], MPI_DOUBLE, xtmp, numElements, displacement, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Gatherv(ylocal, numElements[rank], MPI_DOUBLE, ytmp, numElements, displacement, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    delete[] xlocal;
    delete[] ylocal;

    if (rank == 0)
    {
	// transpose
	for (J = 0; J < M; J++)
	{
	    for (j = 0; j < m; j++)
	    {
		x[j*M + J] = xtmp[J*m + j];
		y[j*M + J] = ytmp[J*m + j];
	    }
	}
    }

    if (rank == 0) std::cout<<x[100]<<" + i*"<<y[100]<<std::endl;

    MPI_Finalize();

    delete[] xtmp;
    delete[] ytmp;

} // end void radix2fft_p()


int main(int argc, char *argv[])
{
    int p = 22, // length of data = 2^p
	N = power2(p);
    double* x = new double[N];
    double* y = new double[N];

    //-----DATA-----//
    double w = 0.001;
    for (int n = 0; n < N; n++)
    {
	x[n] = cos(w*n);
	y[n] = sin(w*n);
    }

    radix2fft_p(x, y, p);

    delete[] x;
    delete[] y;

    return 0;
}