

//IMPORTANT HEADERS AND MICROS
//------------------------------------------------------------------------------------------------------------------------
#include"stdc++.h"
#include"Complex.h"
using namespace std;
const long double PI = acos(-1);

using cd = Complex;

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

vector<int> multiply_polys_seq_fft(const vector<int>& poly_a,const vector<int>& poly_b) {
	vector<cd> poly_a_cd(poly_a.begin(),poly_a.end()), poly_b_cd(poly_b.begin(),poly_b.end());

	int a_len = poly_a.size();
	int b_len = poly_b.size();
	int n = 1;
	while (n < a_len + b_len) n <<= 1;
	poly_a_cd.resize(n,0);
	poly_b_cd.resize(n,0);



	//finding FFT of equations A and B separately
	fft(poly_a_cd, false);
	fft(poly_b_cd, false);
	

	//combing the FFT of A and B
	vector<cd> F_C(n);
	for (int i = 0; i < n; i++)
		F_C[i] = poly_a_cd[i] * poly_b_cd[i];

	//finding the inverse DFT and dividing it by 1/n
	fft(F_C, true);

	vector<int> res;
	
	for (auto& d : F_C) {
		res.push_back(int(d._real + 0.5));
	}

	return res;
}


int main()
{
	
	vector<int> a = {1,2,3,4,5,6,7,8};
	vector<int> b = { 1,2,3,4,5,6,7,8 };

	vector<int> seq_fft_res = multiply_polys_seq_fft(a, b);

	for (auto& seq : seq_fft_res) {
		cout << seq << " ";
	}




	

	

	

	return 0;
}