//NOTE: the 2*N should be in power of 2

//IMPORTANT HEADERS AND MICROS
//------------------------------------------------------------------------------------------------------------------------
#include"../stdc++.h"
using namespace std;

#define fast ios_base::sync_with_stdio(NULL),cin.tie(NULL),cout.tie(NULL);

const long double PI = acos(-1);

//PROGRAM
//-----------------------------------------------------------------------------------------------------------------------
using cd = complex<double>;

int reverse(int num, int lg_n) {
	int res = 0;
	for (int i = 0; i < lg_n; i++) {
		if (num & (1 << i))
			res |= 1 << (lg_n - 1 - i);
	}
	return res;
}

void fft(vector<cd>& a, bool invert) {
	int n = a.size();
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
		for (cd& x : a)
			x /= n;
	}
}

int main()
{
	
	vector<cd> a = {1,2,3,4,5,6,7,8};
	vector<cd> b = { 1,2,3,4,5,6,7,8 };


	//init with 0
	int a_len = a.size(); 
	int b_len = b.size();
	int n = 1;
	while (n < a_len + b_len) n *= 2;
	for (int i = a_len; i < n; i++)
		a.push_back(0);
	for (int i = b_len; i <n ; i++)
		b.push_back( 0);



	//finding FFT of equations A and B separately
	fft(a, false);
	fft(b, false);

	//combing the FFT of A and B
	vector<cd> F_C(n);
	for (int i = 0; i < n; i++)
		F_C[i] = a[i] * b[i];

	//finding the inverse DFT and dividing it by 1/n
	 fft(F_C, true);
	

	//output the real part of the result
	for (int i = 0; i < n; i++)
		cout << int((real(F_C[i]) + 0.5)) << ' ';

	return 0;
}