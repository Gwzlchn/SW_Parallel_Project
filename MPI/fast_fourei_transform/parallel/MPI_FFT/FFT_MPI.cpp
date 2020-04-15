

//IMPORTANT HEADERS AND MICROS
//------------------------------------------------------------------------------------------------------------------------
#include"stdc++.h"
#include"Complex.h"
using namespace std;
const long double PI = acos(-1);

using cd = Complex;

void Swap(cd* a, cd* b) {
	cd* temp = a;
	*b = *a;
	*a = *temp;
}

int reverse(int num, int lg_n) {
	int res = 0;
	for (int i = 0; i < lg_n; i++) {
		if (num & (1 << i))
			res |= 1 << (lg_n - 1 - i);
	}
	return res;
}


	

void fft(cd a[] ,int n, bool invert) 
{

	int lg_n = 0;
	while ((1 << lg_n) < n)
		lg_n++;

	for (int i = 0; i < n; i++) {
		if (i < reverse(i, lg_n))
			swap(a[i], a[reverse(i, lg_n)]);
	}

	for (int len = 2; len <= n; len <<= 1) {
		double ang = 2 * PI / len * (invert ? -1 : 1);
		cd wlen(cos(ang), sin(ang));
		for (int i = 0; i < n; i += len) {
			cd w(1);
			for (int j = 0; j < len / 2; j++) {
				cd u = a[i + j], v = a[i + j + len / 2] * w;
				a[i + j] = u + v;
				a[i + j + len / 2] = u - v;
				w *= wlen;
			}
		}
	}

	if (invert) {
		for (int i = 0; i < n; i++) {
			a[i] /= n;
		}

	}
}

int* multiply_polys_seq_fft(int poly_a[],const int poly_b[],int a_len,int b_len) 
{
	
	int len = 1;
	while (len < a_len + b_len) len <<= 1;

	cd* poly_a_cd = (cd*)malloc(sizeof(cd) * len);
	cd* poly_b_cd = (cd*)malloc(sizeof(cd) * len);

	for (int i = 0; i < a_len; i++) {
		poly_a_cd[i] = poly_a[i];
	}
	for (int i = 0; i < b_len; i++) {
		poly_b_cd[i] = poly_b[i];
	}
	for (int i = a_len; i < len; i++) {
		poly_a_cd[i] = 0;
	}
	for (int i = b_len; i < len; i++) {
		poly_b_cd[i] = 0;
	}


	//finding FFT of equations A and B separately
	fft(poly_a_cd, len,false);
	fft(poly_b_cd, len,false);
	

	//combing the FFT of A and B
	cd* F_C = (cd*)malloc(sizeof(cd) * len);
	for (int i = 0; i < len; i++)
		F_C[i] = poly_a_cd[i] * poly_b_cd[i];

	//finding the inverse DFT and dividing it by 1/n
	fft(F_C, len,true);

	int* res = (int*)malloc(sizeof(int)*len);
	
	for (int i = 0; i < len;i++) {
		res[i] = (int(F_C[i]._real + 0.5));
	}

	return res;
}


int main()
{
	
	int a[] = {1,2,3,4,5,6,7,8};
	int b[] = { 1,2,3,4,5,6,7,8 };
	int a_len = sizeof(a) / sizeof(a[0]);
	int b_len = sizeof(b) / sizeof(b[0]);

	int* seq_fft_res = multiply_polys_seq_fft(a, b,a_len,b_len);

	for (int i = 0; i < 16;i++) {
		cout << seq_fft_res[i] << " ";
	}




	

	

	

	return 0;
}