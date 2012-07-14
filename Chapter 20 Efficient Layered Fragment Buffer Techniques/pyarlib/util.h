/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef PYARLIB_UTIL_H
#define PYARLIB_UTIL_H

#include <algorithm>
#include <sstream>
#include <math.h>

#define UNIT_RAND (rand()/(float)RAND_MAX)

extern const float pi;

int ilog2(int x);
int ceilLog2(int x);
int ceilSqrt(const int& x);
int countBits(const int& x);
int nextPowerOf2(int x);
int ceil(const int& n, const int& d);
unsigned int binomial(unsigned int n, unsigned int k);
int fact(int i); //need optimizing: http://www.luschny.de/math/factorial/FastFactorialFunctions.htm
float normalpdf(float x, float sigma); // use x - mean if nonzero
float standardpdf(float x); //faster normalpdf(x, 1)
float standardcdf(float x);
float gaussianKernal(int x, int n);

std::string humanBytes(int i, bool use_binary_prefix = true);
std::string intToString(int i);
int stringToInt(const std::string& s);

template<class T> std::string toString(const T& v)
{
	std::stringstream s;
	s << v;
	return s.str();
}

//this should put an end to the flood of ambiguous overloads
#define CT(n) static_cast<T>(n)
template<class T> inline const T& myclamp(const T& x, const T& a, const T& b) {return (x<a)?a:((x>b)?b:x);}
template<class T> inline const T mysign(const T& x) {return (x > CT(0)) ? CT(1) : ((x<CT(0)) ? CT(-1) : CT(0));}
template<class T> inline const T& mymin(const T& a, const T& b) {return (a<b) ? a : b;}
template<class T> inline const T& mymax(const T& a, const T& b) {return (a>b) ? a : b;}
template<class T> inline const T myabs(const T& x) {return (x < CT(0)) ? -x : x;}
#undef CT

#endif
