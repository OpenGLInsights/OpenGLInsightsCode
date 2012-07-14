/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <stdio.h>

#include "matstack.h"

static const float pi = 3.14159265f;
static const float toRad = pi/180.0f;

MatrixStack::MatrixStack(const mat44& m)
{
	mat44::operator=(m);
}
mat44& MatrixStack::operator=(const mat44& m)
{
	mat44::operator=(m);
	return *this;
}
void MatrixStack::push()
{
	push(*this);
}
void MatrixStack::push(const mat44& m)
{
	if (mats.size() > 64)
		printf("Error: MatrixStack stack overflow\n");
	else
	{
		mats.push(*this);
		*this = m;
	}
}
void MatrixStack::pop()
{
	if (mats.size() == 0)
		printf("Error: MatrixStack stack underflow\n");
	else
	{
		*this = mats.top();
		mats.pop();
	}
}
void MatrixStack::translate(vec3f v)
{
	*this *= mat44::translate(v);
}
void MatrixStack::translate(float x, float y, float z)
{
	*this *= mat44::translate(x, y, z);
}
void MatrixStack::rotate(float a, vec3f v)
{
	*this *= mat44::rotate(a * toRad, v);
}
void MatrixStack::rotate(float a, float x, float y, float z)
{
	*this *= mat44::rotate(a * toRad, vec3f(x, y, z));
}
void MatrixStack::scale(vec3f v)
{
	*this *= mat44::scale(v);
}
void MatrixStack::scale(float x, float y, float z)
{
	*this *= mat44::scale(x, y, z);
}
void MatrixStack::clear()
{
	*this = mat44::identity();
}
mat33 MatrixStack::normalMatrix()
{
	return mat33(inverse().transpose());
}
