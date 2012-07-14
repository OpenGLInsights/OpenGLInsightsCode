/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include "matrix.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


mat33::mat33()
{
}
mat33::mat33(const mat44& o)
{
	m[0] = o.m[0]; m[1] = o.m[1]; m[2] = o.m[2];
	m[3] = o.m[4]; m[4] = o.m[5]; m[5] = o.m[6];
	m[6] = o.m[8]; m[7] = o.m[9]; m[8] = o.m[10];
}
mat44::mat44()
{
}
mat44::mat44(const mat44& o)
{
	memcpy(&m, &o.m, sizeof(m));
}
mat44::mat44(const float o[16])
{
	memcpy(&m, o, sizeof(m));
}
mat44::mat44(const float o[4][4])
{
	memcpy(&m, o, sizeof(m));
}
mat44::mat44(float m11, float m12, float m13, float m14,
	float m21, float m22, float m23, float m24,
	float m31, float m32, float m33, float m34,
	float m41, float m42, float m43, float m44)
{
	c1.r1 = m11; c2.r1 = m12; c3.r1 = m13; c4.r1 = m14;
	c1.r2 = m21; c2.r2 = m22; c3.r2 = m23; c4.r2 = m24;
	c1.r3 = m31; c2.r3 = m32; c3.r3 = m33; c4.r3 = m34;
	c1.r4 = m41; c2.r4 = m42; c3.r4 = m43; c4.r4 = m44;
}
mat44& mat44::operator=(const mat44& o)
{
	memcpy(m, o.m, sizeof(m));
	return *this;
}
float& mat44::operator[](int i)
{
	return m[i];
}
float& mat44::operator()(int i, int j)
{
	return d[i][j];
}
mat44 mat44::operator*(float d) const
{
	mat44 r(*this);
	r *= d;
	return r;
}
void mat44::operator*=(float d)
{
	m[ 0] *= d; m[ 4] *= d; m[ 8] *= d; m[12] *= d;
	m[ 1] *= d; m[ 5] *= d; m[ 9] *= d; m[13] *= d;
	m[ 2] *= d; m[ 6] *= d; m[10] *= d; m[14] *= d;
	m[ 3] *= d; m[ 7] *= d; m[11] *= d; m[15] *= d;
}
vec3f mat44::operator*(const vec3f& v) const
{
	return vec3f(
		m[ 0] * v.x + m[ 4] * v.y + m[ 8] * v.z,
		m[ 1] * v.x + m[ 5] * v.y + m[ 9] * v.z,
		m[ 2] * v.x + m[ 6] * v.y + m[10] * v.z
		);
}
vec4f mat44::operator*(const vec4f& v) const
{
	return vec4f(
		m[ 0] * v.x + m[ 4] * v.y + m[ 8] * v.z + m[12] * v.w,
		m[ 1] * v.x + m[ 5] * v.y + m[ 9] * v.z + m[13] * v.w,
		m[ 2] * v.x + m[ 6] * v.y + m[10] * v.z + m[14] * v.w,
		m[ 3] * v.x + m[ 7] * v.y + m[11] * v.z + m[15] * v.w
		);
}
mat44 mat44::operator*(const mat44& o) const
{
	mat44 r;
	r.m[ 0] = m[ 0]*o.m[ 0] + m[ 4]*o.m[ 1] + m[ 8]*o.m[ 2] + m[12]*o.m[ 3];
	r.m[ 4] = m[ 0]*o.m[ 4] + m[ 4]*o.m[ 5] + m[ 8]*o.m[ 6] + m[12]*o.m[ 7];
	r.m[ 8] = m[ 0]*o.m[ 8] + m[ 4]*o.m[ 9] + m[ 8]*o.m[10] + m[12]*o.m[11];
	r.m[12] = m[ 0]*o.m[12] + m[ 4]*o.m[13] + m[ 8]*o.m[14] + m[12]*o.m[15];
	
	r.m[ 1] = m[ 1]*o.m[ 0] + m[ 5]*o.m[ 1] + m[ 9]*o.m[ 2] + m[13]*o.m[ 3];
	r.m[ 5] = m[ 1]*o.m[ 4] + m[ 5]*o.m[ 5] + m[ 9]*o.m[ 6] + m[13]*o.m[ 7];
	r.m[ 9] = m[ 1]*o.m[ 8] + m[ 5]*o.m[ 9] + m[ 9]*o.m[10] + m[13]*o.m[11];
	r.m[13] = m[ 1]*o.m[12] + m[ 5]*o.m[13] + m[ 9]*o.m[14] + m[13]*o.m[15];
	
	r.m[ 2] = m[ 2]*o.m[ 0] + m[ 6]*o.m[ 1] + m[10]*o.m[ 2] + m[14]*o.m[ 3];
	r.m[ 6] = m[ 2]*o.m[ 4] + m[ 6]*o.m[ 5] + m[10]*o.m[ 6] + m[14]*o.m[ 7];
	r.m[10] = m[ 2]*o.m[ 8] + m[ 6]*o.m[ 9] + m[10]*o.m[10] + m[14]*o.m[11];
	r.m[14] = m[ 2]*o.m[12] + m[ 6]*o.m[13] + m[10]*o.m[14] + m[14]*o.m[15];
	
	r.m[ 3] = m[ 3]*o.m[ 0] + m[ 7]*o.m[ 1] + m[11]*o.m[ 2] + m[15]*o.m[ 3];
	r.m[ 7] = m[ 3]*o.m[ 4] + m[ 7]*o.m[ 5] + m[11]*o.m[ 6] + m[15]*o.m[ 7];
	r.m[11] = m[ 3]*o.m[ 8] + m[ 7]*o.m[ 9] + m[11]*o.m[10] + m[15]*o.m[11];
	r.m[15] = m[ 3]*o.m[12] + m[ 7]*o.m[13] + m[11]*o.m[14] + m[15]*o.m[15];
	return r;
}
void mat44::operator*=(const mat44& o)
{
	*this = *this * o;
}
mat44 mat44::operator+(const mat44& o) const
{
	mat44 r;
	r.m[ 0] = m[ 0]+o.m[ 0]; r.m[ 4] = m[ 4]+o.m[ 4]; r.m[ 8] = m[ 8]+o.m[ 8]; r.m[12] = m[12]+o.m[12];
	r.m[ 1] = m[ 1]+o.m[ 1]; r.m[ 5] = m[ 5]+o.m[ 5]; r.m[ 9] = m[ 9]+o.m[ 9]; r.m[13] = m[13]+o.m[13];
	r.m[ 2] = m[ 2]+o.m[ 2]; r.m[ 6] = m[ 6]+o.m[ 6]; r.m[10] = m[10]+o.m[10]; r.m[14] = m[14]+o.m[14];
	r.m[ 3] = m[ 3]+o.m[ 3]; r.m[ 7] = m[ 7]+o.m[ 7]; r.m[11] = m[11]+o.m[11]; r.m[15] = m[15]+o.m[15];
	return r;
}
void mat44::operator+=(const mat44& o)
{
	*this = *this + o;
}
mat44 mat44::operator-(const mat44& o) const
{
	mat44 r;
	r.m[ 0] = m[ 0]-o.m[ 0]; r.m[ 4] = m[ 4]-o.m[ 4]; r.m[ 8] = m[ 8]-o.m[ 8]; r.m[12] = m[12]-o.m[12];
	r.m[ 1] = m[ 1]-o.m[ 1]; r.m[ 5] = m[ 5]-o.m[ 5]; r.m[ 9] = m[ 9]-o.m[ 9]; r.m[13] = m[13]-o.m[13];
	r.m[ 2] = m[ 2]-o.m[ 2]; r.m[ 6] = m[ 6]-o.m[ 6]; r.m[10] = m[10]-o.m[10]; r.m[14] = m[14]-o.m[14];
	r.m[ 3] = m[ 3]-o.m[ 3]; r.m[ 7] = m[ 7]-o.m[ 7]; r.m[11] = m[11]-o.m[11]; r.m[15] = m[15]-o.m[15];
	return r;
}
void mat44::operator-=(const mat44& o)
{
	*this = *this - o;
}
float mat44::cofactor(int i, int j) const
{
	float sub[9];
	int p = 0;
	for (int x = 0; x < 4; ++x)
		if (x != i)
			for (int y = 0; y < 4; ++y)
				if (y != j)
					sub[p++] = d[x][y];
	float sd =
		sub[0]*(sub[4]*sub[8]-sub[7]*sub[5]) +
		sub[3]*(sub[2]*sub[7]-sub[1]*sub[8]) +
		sub[6]*(sub[1]*sub[5]-sub[2]*sub[4]);
	if ((i+j)%2 == 0)
		return sd;
	else
		return -sd;
}
float mat44::det() const
{
	float r =
		d[0][0] * cofactor(0, 0) +
		d[0][1] * cofactor(0, 1) +
		d[0][2] * cofactor(0, 2) +
		d[0][3] * cofactor(0, 3);
	return r;
}
mat44 mat44::transpose() const
{
	mat44 r;
	for (int x = 0; x < 4; ++x)
		for (int y = 0; y < 4; ++y)
			r.d[x][y] = d[y][x];
	return r;
}
mat44 mat44::inverse() const
{
	mat44 r;
	float rdet = 1.0f / det();
	for (int x = 0; x < 4; ++x)
		for (int y = 0; y < 4; ++y)
			r.d[x][y] = rdet * cofactor(y, x);
	return r;
}
void mat44::print()
{
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			printf("%.2f ", d[x][y]);
		}
		printf("\n");
	}
}
mat44 mat44::zero()
{
	mat44 r;
	r.m[ 0] = 0.0f; r.m[ 4] = 0.0f; r.m[ 8] = 0.0f; r.m[12] = 0.0f;
	r.m[ 1] = 0.0f; r.m[ 5] = 0.0f; r.m[ 9] = 0.0f; r.m[13] = 0.0f;
	r.m[ 2] = 0.0f; r.m[ 6] = 0.0f; r.m[10] = 0.0f; r.m[14] = 0.0f;
	r.m[ 3] = 0.0f; r.m[ 7] = 0.0f; r.m[11] = 0.0f; r.m[15] = 0.0f;
	return r;
}
mat44 mat44::filled(float v)
{
	mat44 r;
	r.m[ 0] = v; r.m[ 4] = v; r.m[ 8] = v; r.m[12] = v;
	r.m[ 1] = v; r.m[ 5] = v; r.m[ 9] = v; r.m[13] = v;
	r.m[ 2] = v; r.m[ 6] = v; r.m[10] = v; r.m[14] = v;
	r.m[ 3] = v; r.m[ 7] = v; r.m[11] = v; r.m[15] = v;
	return r;
}
mat44 mat44::identity()
{
	mat44 r;
	r.m[ 0] = 1.0f; r.m[ 4] = 0.0f; r.m[ 8] = 0.0f; r.m[12] = 0.0f;
	r.m[ 1] = 0.0f; r.m[ 5] = 1.0f; r.m[ 9] = 0.0f; r.m[13] = 0.0f;
	r.m[ 2] = 0.0f; r.m[ 6] = 0.0f; r.m[10] = 1.0f; r.m[14] = 0.0f;
	r.m[ 3] = 0.0f; r.m[ 7] = 0.0f; r.m[11] = 0.0f; r.m[15] = 1.0f;
	return r;
}
mat44 mat44::identity(float v)
{
	mat44 r;
	r.m[ 0] =    v; r.m[ 4] = 0.0f; r.m[ 8] = 0.0f; r.m[12] = 0.0f;
	r.m[ 1] = 0.0f; r.m[ 5] =    v; r.m[ 9] = 0.0f; r.m[13] = 0.0f;
	r.m[ 2] = 0.0f; r.m[ 6] = 0.0f; r.m[10] =    v; r.m[14] = 0.0f;
	r.m[ 3] = 0.0f; r.m[ 7] = 0.0f; r.m[11] = 0.0f; r.m[15] =    v;
	return r;
}

mat44 mat44::rotate(float yaw, float pitch, float roll)
{
	return
		mat44::rotate(roll, vec3f(0, 0, 1)) *
		mat44::rotate(pitch, vec3f(0, 1, 0)) *
		mat44::rotate(yaw, vec3f(1, 0, 0));

	//http://en.wikipedia.org/wiki/Rotation_matrix#Basic_rotations
	/*
	working:
	cy		0		sy
	sxsy	cx		-sxcy
	-sycx	sx		cxcy


	cycz			-cysz			sy
	sxsycz+cxsz		-sxsysz+cxcz	-sxcy
	-sycxcz+sxsz	sycxsz+sxcz		cxcy
	*/
	
	/*float cx = cos(yaw);
	float cy = cos(pitch);
	float cz = cos(roll);
	float sx = sin(yaw);
	float sy = sin(pitch);
	float sz = sin(roll);
	
	mat44 r;
	r.m[ 0] = cy*cz;
	r.m[ 4] = -cy*sz;
	r.m[ 8] = sy;
	r.m[ 1] = sx*sy*cz+cx*sz;
	r.m[ 5] = -sx*sy*sz+cx*cz;
	r.m[ 9] = -sx*cy;
	r.m[ 2] = -sy*cx*cz+sx*sz;
	r.m[ 6] = sy*cx*sz+sx*cz;
	r.m[10] = cx*cy;
	                                                r.m[12] = 0.0f;
	                                                r.m[13] = 0.0f;
	                                                r.m[14] = 0.0f;
	r.m[ 3] = 0.0f; r.m[ 7] = 0.0f; r.m[11] = 0.0f; r.m[15] = 1.0f;
	return r;*/
	/*
	//http://mathworld.wolfram.com/EulerAngles.html
	float cx = cos(x);
	float cy = cos(y);
	float cz = cos(z);
	float sx = sin(x);
	float sy = sin(y);
	float sz = sin(z);
	mat44 r;
	r.m[ 0] = cz*cx - cy*sx*sz;
	r.m[ 4] = cz*sx + cy*cx*sz;
	r.m[ 8] = sz*sy;
	r.m[ 1] = -sz*cx - cy*sx*cz;
	r.m[ 5] = -sz*sx + cy*cx*cz;;
	r.m[ 9] = cz*sy;
	r.m[ 2] = sy*sx;
	r.m[ 6] = -sy*cx;
	r.m[10] = cy;
	                                                r.m[12] = 0.0f;
	                                                r.m[13] = 0.0f;
	                                                r.m[14] = 0.0f;
	r.m[ 3] = 0.0f; r.m[ 7] = 0.0f; r.m[11] = 0.0f; r.m[15] = 1.0f;
	return r;
	*/
}

mat44 mat44::rotate(const vec3f& euler)
{
	return rotate(euler.x, euler.y, euler.z);
}

mat44 mat44::rotate(float a, vec3f v)
{
	float c = cos(a);
	float s = sin(a);
	v.normalize();
	mat44 r;
	//http://www.euclideanspace.com/maths/algebra/matrix/orthogonal/rotation/index.htm
	r.m[ 0] = 1.0f+(1.0f-c)*(v.x*v.x-1.0f);
	r.m[ 4] = -v.z*s+(1.0f-c)*v.x*v.y;
	r.m[ 8] = v.y*s+(1.0f-c)*v.x*v.z;
	r.m[ 1] = v.z*s+(1.0f-c)*v.x*v.y;
	r.m[ 5] = 1.0f+(1.0f-c)*(v.y*v.y-1.0f);
	r.m[ 9] = -v.x*s+(1.0f-c)*v.y*v.z;
	r.m[ 2] = -v.y*s+(1.0f-c)*v.x*v.z;
	r.m[ 6] = v.x*s+(1.0f-c)*v.y*v.z;
	r.m[10] = 1.0f+(1.0f-c)*(v.z*v.z-1.0f);
	/*
	http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_given_an_axis_and_an_angle
	r.m[ 0] = v.x*v.x+(1.0f-v.x*v.x)*c;
	r.m[ 4] = v.x*v.y*(1.0f-c)-v.z*s;
	r.m[ 8] = v.x*v.z*(1.0f-c)+v.y*s;
	r.m[ 1] = v.x*v.y*(1.0f-c)+v.z*s;
	r.m[ 5] = v.y*v.y+(1.0f-v.y*v.y)*c;
	r.m[ 9] = v.y*v.z*(1.0f-c)-v.x*s;
	r.m[ 2] = v.x*v.z*(1.0f-c)-v.z*s;
	r.m[ 6] = v.y*v.z*(1.0f-c)+v.x*s;
	r.m[10] = v.z*v.z+(1.0-v.z*v.z)*c;
	*/
	                                                r.m[12] = 0.0f;
	                                                r.m[13] = 0.0f;
	                                                r.m[14] = 0.0f;
	r.m[ 3] = 0.0f; r.m[ 7] = 0.0f; r.m[11] = 0.0f; r.m[15] = 1.0f;
	return r;
}

mat44 mat44::translate(float x, float y, float z)
{
	mat44 r;
	r.m[ 0] = 1.0f; r.m[ 4] = 0.0f; r.m[ 8] = 0.0f; r.m[12] =    x;
	r.m[ 1] = 0.0f; r.m[ 5] = 1.0f; r.m[ 9] = 0.0f; r.m[13] =    y;
	r.m[ 2] = 0.0f; r.m[ 6] = 0.0f; r.m[10] = 1.0f; r.m[14] =    z;
	r.m[ 3] = 0.0f; r.m[ 7] = 0.0f; r.m[11] = 0.0f; r.m[15] = 1.0f;
	return r;
}

mat44 mat44::translate(const vec3f& pos)
{
	return translate(pos.x, pos.y, pos.z);
}


mat44 mat44::scale(float x, float y, float z)
{
	mat44 r;
	r.m[ 0] =    x; r.m[ 4] = 0.0f; r.m[ 8] = 0.0f; r.m[12] = 0.0f;
	r.m[ 1] = 0.0f; r.m[ 5] =    y; r.m[ 9] = 0.0f; r.m[13] = 0.0f;
	r.m[ 2] = 0.0f; r.m[ 6] = 0.0f; r.m[10] =    z; r.m[14] = 0.0f;
	r.m[ 3] = 0.0f; r.m[ 7] = 0.0f; r.m[11] = 0.0f; r.m[15] = 1.0f;
	return r;
}

mat44 mat44::scale(const vec3f& pos)
{
	return scale(pos.x, pos.y, pos.z);
}
