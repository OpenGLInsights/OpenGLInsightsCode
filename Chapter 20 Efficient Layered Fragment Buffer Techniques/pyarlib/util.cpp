/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>

#include "util.h"

const float pi = 3.14159265f;

#define WORDBITS (sizeof(int)*8)

unsigned int ones32(register unsigned int x)
{
	/* 32-bit recursive reduction using SWAR...
	but first step is mapping 2-bit values
	into sum of 2 1-bit values in sneaky way
	*/
	x -= ((x >> 1) & 0x55555555);
	x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
	x = (((x >> 4) + x) & 0x0f0f0f0f);
	x += (x >> 8);
	x += (x >> 16);
	return(x & 0x0000003f);
}

int ceilLog2(int x)
{
	register int y = (x & (x - 1));
	y |= -y;
	y >>= (WORDBITS - 1);
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
#ifdef	LOG0UNDEFINED
	return(ones32(x) - 1 - y);
#else
	return(ones32(x >> 1) - y);
#endif
}
int ilog2(int x)
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
#ifdef	LOG0UNDEFINED
	return(ones32(x) - 1);
#else
	return(ones32(x >> 1));
#endif
}
int ceilSqrt(const int& x)
{
	int i = (int)sqrt((float)x)-1;
	while (i * i < x)
		++i;
	return i;
}
int countBits(const int& x)
{
	int c = 0;
	for (int i = 0; i < 32; ++i)
		if ((x & (1 << i)) > 0)
			++c;
	return c;
}

int nextPowerOf2(int x)
{
	x--;
	x = (x >> 1) | x;
	x = (x >> 2) | x;
	x = (x >> 4) | x;
	x = (x >> 8) | x;
	x = (x >> 16) | x;
	x++;
	return x;
}

int ceil(const int& n, const int& d)
{
	return n / d + (n % d > 0 ? 1 : 0);
}

unsigned int binomial(unsigned int n, unsigned int k)
{
	//from: http://www.luschny.de/math/factorial/FastBinomialFunction.html

	assert(k <= n);
	if ((k == 0) || (k == n)) return 1;
	if (k > n / 2) { k = n - k; }
	int fi = 0, nk = n - k;
	int rootN = (int)sqrt((double)n);
	
	static int* primes = NULL;
	static int numPrimes = 0;
	static int currentN = 0;
	
	//if (currentN < n)
	//{
		//delete[] primes;
		currentN = n+1;
		primes = new int[currentN];
		int* primesSeive = new int[currentN];
		memset(primesSeive, 0, sizeof(int) * currentN);
		numPrimes = 0;
		for (int i = 2; i < currentN; ++i)
		{
			if (primesSeive[i] == 0)
			{
				primes[numPrimes++] = i;
				for (int m = i+i; m < currentN; m += i)
					++primesSeive[m];
			}
		}
		delete[] primesSeive;
	//}
	
	for(int i = 0; i < numPrimes; i++) 
	{
		int prime = primes[i];
		if (prime > nk)
		{
			primes[fi++] = prime;
			continue;
		}

		if (prime > (int)n / 2)
		{
			continue;
		}

		if (prime > rootN)
		{
			if ((n % prime) < (k % prime))
			{
				primes[fi++] = prime;
			}
			continue;
		}

		int N = n, K = k, p = 1, r = 0;

		while (N > 0)
		{
			r = ((N % prime) < (K % prime + r)) ? 1 : 0;
			if (r == 1)
			{
				p *= prime;
			}
			N /= prime;
			K /= prime;
		}
		if (p > 1) primes[fi++] = p;
	}

	unsigned int binom = 1;
	for(int i = 0; i < fi; i++) 
		binom *= primes[i];
	
	delete[] primes;
		
	return binom;
}

int fact(int i)
{
	//FIXME: much better algorithms out there
	printf("%i\n", i);
	int x = 1;
	while (i > 1)
		x *= i--;
	printf("%i\n", x);
	return x;
}

float normalpdf(float x, float sigma)
{
	return exp(-(x*x)/(2.0f*sigma*sigma)) / (sigma*sqrt(2.0f*pi));
}

float standardpdf(float x)
{
	return exp(-(x*x)/(2.0f)) / sqrt(2.0f*pi);
}

float standardcdf(float x)
{
	/*
	FROM: http://www.hobsala.com/codes/math.html
	
	This code implements a function that calculates the 
	standard normal CDF (x), using an approximation from 
	Abromowitz and Stegun Handbook of Mathematical Functions.
	http://www.math.sfu.ca/~cbm/aands/page_932.htm 
	*/
	const float b1 =  0.319381530;
	const float b2 = -0.356563782;
	const float b3 =  1.781477937;
	const float b4 = -1.821255978;
	const float b5 =  1.330274429;
	const float p  =  0.2316419;

	if (x >= 0.0)
	{
		float t = 1.0 / (1.0 + p*x);
		return (1.0 - standardpdf(x)*t* 
			(t*(t*(t*(t*b5 + b4) + b3) + b2) + b1));
	} 
	else
	{ 
		float t = 1.0 / ( 1.0 - p * x );
		return (standardpdf(x)*t* 
			(t*(t*(t*(t*b5 + b4) + b3) + b2) + b1));
	}
}

float gaussianKernal(int x, int n)
{
	const float sd = 3.0f;
	
	//find probability for range x to x+1, dividing sd standard deviations by n
	float x1 = (((float)x / (float)n) * 2.0f - 1.0f) * sd;
	float sampleDist = (sd + sd) / (float)n;
	float x2 = x1 + sampleDist;
	
	//subtract area under curve so standardpdf outside [-sd, sd] is < 0.
	//also scale returned samples so n samples sum to 1
	float total = standardcdf(sd) - standardcdf(-sd);
	float minp = standardpdf(sd);
	float sub = minp * (sd + sd);
	total -= sub;
	return ((standardcdf(x2) - standardcdf(x1)) - (minp * sampleDist)) / total;
}

std::string humanBytes(int i, bool use_binary_prefix)
{
	//http://en.wikipedia.org/wiki/Megabyte
	//binary
	static const char* prefix_iec[] = {
		"B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB"
		};
	//decimal
	static const char* prefix_si[] = {
		"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"
		};
	//since int is used, TB and greater isn't necessary
	
	std::stringstream ret;
	ret.setf(std::ios::fixed,std::ios::floatfield);
	ret << std::setprecision(2);
	if (use_binary_prefix)
	{
		int prefix = mymin(8, ilog2(i) / 10);
		float v = (float)i / (1 << (prefix*10));
		ret << v << prefix_iec[prefix];
	}
	else
	{
		int prefix = mymin(8, (int)(log10((float)i) / 3));
		float v = (float)i / pow(10.0f, prefix*3);
		ret << v << prefix_si[prefix];
	}
	return ret.str();
}

std::string intToString(int i)
{
	static std::stringstream stream;
	stream.clear();
	stream.str("");
	stream << i;
	return stream.str();
}

int stringToInt(const std::string& s)
{
	int i = -1;
	static std::stringstream stream;
	stream.clear();
	stream.str(s);
	stream >> i;
	return i;
}


