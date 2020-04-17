

struct Complex
{
	double _real;
	double _imag;

	Complex();
	Complex(long real);
	Complex(double real, double image);

	bool operator==(const Complex& c);
	Complex& operator=(const Complex& c);
	Complex& operator=(const long& c);

	Complex operator+(const Complex& c);
	Complex& operator+=(const Complex& c);
	Complex operator-(const Complex& c);
	Complex& operator-=(const Complex& c);
	Complex operator*(const Complex& c);
	Complex& operator*=(const Complex& c);
	Complex& operator*=(const long& c);
	Complex operator/(const Complex& c);
	Complex& operator/=(const Complex& c);
	Complex& operator/=(const long& c);
};


inline Complex::Complex()
{
	_real = 0;
	_imag = 0;
	return;
}

Complex::Complex(long real)
	:_real(real),
	_imag(0)
{}

Complex::Complex(double real , double image)  //构造函数
	:_real(real)
	, _imag(image)
{}


Complex Complex::operator+(const Complex& c)
{
	Complex tmp;
	tmp._real = _real + c._real;
	tmp._imag = _imag + c._imag;
	return tmp;
}

Complex Complex::operator-(const Complex& c)
{
	Complex tmp;
	tmp._real = _real - c._real;
	tmp._imag = _imag - c._imag;
	return tmp;
}

Complex Complex::operator*(const Complex& c)
{
	Complex tmp;
	tmp._real = _real * c._real - _imag * c._imag;
	tmp._imag = _real * c._imag + _imag * c._real;
	return tmp;
}

Complex Complex::operator/(const Complex& c)
{
	Complex tmp;
	double t = c._real * c._real + c._imag * c._imag;
	tmp._real = (_real * c._real - _imag * (-c._imag)) / t;
	tmp._imag = (_real * (-c._imag) + _imag * c._real) / t;
	return tmp;

}

Complex& Complex::operator+=(const Complex& c)
{
	_real += c._real;
	_imag += c._imag;
	return *this;
}

Complex& Complex::operator-=(const Complex& c)
{
	_real -= c._real;
	_imag -= c._imag;
	return *this;
}

Complex& Complex::operator*=(const Complex& c)
{
	Complex tmp(*this);  //拷贝构造函数
	_real = tmp._real * c._real - _imag * c._imag;
	_imag = tmp._real * c._imag + tmp._imag * c._real;
	return *this;
}

Complex& Complex::operator*=(const long& c)
{
	_real *= c;
	_imag *= c;
	return *this;
}

Complex& Complex::operator/=(const Complex& c)
{
	Complex tmp(*this);
	double t = c._real * c._real + c._imag * c._imag;
	_real = (tmp._real * c._real - tmp._imag * (-c._imag)) / t;
	_imag = (tmp._real * (-c._imag) + tmp._imag * c._real) / t;
	return *this;
}

Complex& Complex::operator/=(const long& c)
{
	
	_real /= c; 
	_imag /= c;
	return *this;
}


bool Complex::operator==(const Complex& c)
{
	return (_real == c._real) &&
		(_imag == c._imag);
}




Complex& Complex::operator=(const Complex& c)
{
	if (this != &c)
	{
		_real = c._real;
		_imag = c._imag;
	}
	return *this;
}

inline Complex& Complex::operator=(const long& c)
{
	this->_real = c;
	this->_imag = 0;
	return *this;
}


double creal(const Complex& com) {
	return com._real;
}

double cimag(const Complex& com) {
	return com._imag;
}


void output_complex_arr(Complex* cd_arr, int len, char* file_name) {
	FILE* out_ptr;
	out_ptr = fopen(file_name, "w");
	fprintf(out_ptr, "%d\n", len);
	int i = 0;
	for (i = 0; i < len; i++) {
		fprintf(out_ptr, "(%.5lf + %.5lfi)\n", creal(cd_arr[i]), cimag(cd_arr[i]));
	}
	fprintf(out_ptr, "\n");
	fclose(out_ptr);

}

void print_complex_console(Complex* cd_arr, int len) {

	printf("%d\n", len);
	int i = 0;
	for (i = 0; i < len; i++) {
		printf("(%.5lf + %.5lfi)\n", creal(cd_arr[i]), cimag(cd_arr[i]));
	}
	printf("END ARRAY\n");
}
