



class Complex
{
public:
	Complex(double real, double image);
	void Print();
	bool operator==(const Complex& c);
	Complex& operator++();   //前置++
	Complex operator++(int); //后置++
	Complex& operator--();   //前置--
	Complex operator--(int); //后置--
	bool operator>(const Complex& c);
	Complex& operator=(const Complex& c);

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
	double _real;
	double _image;
};




Complex::Complex(double real = 0.0, double image = 0.0)  //构造函数
	:_real(real)
	, _image(image)
{}


Complex Complex::operator+(const Complex& c)
{
	Complex tmp;
	tmp._real = _real + c._real;
	tmp._image = _image + c._image;
	return tmp;
}

Complex Complex::operator-(const Complex& c)
{
	Complex tmp;
	tmp._real = _real - c._real;
	tmp._image = _image - c._image;
	return tmp;
}

Complex Complex::operator*(const Complex& c)
{
	Complex tmp;
	tmp._real = _real * c._real - _image * c._image;
	tmp._image = _real * c._image + _image * c._real;
	return tmp;
}

Complex Complex::operator/(const Complex& c)
{
	Complex tmp;
	double t = c._real * c._real + c._image * c._image;
	tmp._real = (_real * c._real - _image * (-c._image)) / t;
	tmp._image = (_real * (-c._image) + _image * c._real) / t;
	return tmp;

}

Complex& Complex::operator+=(const Complex& c)
{
	_real += c._real;
	_image += c._image;
	return *this;
}

Complex& Complex::operator-=(const Complex& c)
{
	_real -= c._real;
	_image -= c._image;
	return *this;
}

Complex& Complex::operator*=(const Complex& c)
{
	Complex tmp(*this);  //拷贝构造函数
	_real = tmp._real * c._real - _image * c._image;
	_image = tmp._real * c._image + tmp._image * c._real;
	return *this;
}

Complex& Complex::operator*=(const long& c)
{
	_real *= c;
	_image *= c;
	return *this;
}

Complex& Complex::operator/=(const Complex& c)
{
	Complex tmp(*this);
	double t = c._real * c._real + c._image * c._image;
	_real = (tmp._real * c._real - tmp._image * (-c._image)) / t;
	_image = (tmp._real * (-c._image) + tmp._image * c._real) / t;
	return *this;
}

Complex& Complex::operator/=(const long& c)
{
	
	_real /= c; 
	_image /= c;
	return *this;
}


bool Complex::operator==(const Complex& c)
{
	return (_real == c._real) &&
		(_image == c._image);
}




Complex& Complex::operator=(const Complex& c)
{
	if (this != &c)
	{
		_real = c._real;
		_image = c._image;
	}
	return *this;
}


double real(const Complex& com) {
	return com._real;
}


