/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <stdio.h>

#include "quaternion.h"

const float quatTolerance = 0.00001f;

const float pi = 3.14159265f;

Quat::Quat()
{
}
Quat::Quat(float nx, float ny, float nz, float nw)
{
	x = nx;
	y = ny;
	z = nz;
	w = nw;
}
Quat::Quat(float angle, vec3f vec)
{
	vec.normalize();
	angle *= 0.5f;
	float sina = sinf(angle);
	w = cosf(angle);
	x = vec.x * sina;
	y = vec.y * sina;
	z = vec.z * sina;
}
Quat::Quat(mat33 m)
{
	//using: http://planning.cs.uiuc.edu/node153.html
	// uu0 uv3 uw6
	// vu1 vv4 vw7
	// wu2 wv5 ww8
	float r = sqrt(1.0f + m.m[0] + m.m[4] + m.m[8]);
	if (r == 0.0f)
	{
		w = 1.0f;
		x = y = z = 0.0f;
	}
	else
	{
		w = r * 0.5f;
		x = (m.m[5] - m.m[7]) / (2.0f * r);
		y = (m.m[6] - m.m[2]) / (2.0f * r);
		z = (m.m[1] - m.m[3]) / (2.0f * r);
	}
}
Quat::Quat(mat44 m)
{
	//using: http://planning.cs.uiuc.edu/node153.html
	// 0  4  8  12
	// 1  5  9  13
	// 2  6  10 14
	// 3  7  11 15
	float r = sqrt(1.0f + m.m[0] + m.m[5] + m.m[10]);
	if (r == 0.0f)
	{
		w = 1.0f;
		x = y = z = 0.0f;
	}
	else
	{
		w = r * 0.5f;
		x = (m.m[6] - m.m[9]) / (2.0f * r);
		y = (m.m[8] - m.m[2]) / (2.0f * r);
		z = (m.m[1] - m.m[4]) / (2.0f * r);
	}
	
	//WTF RETARD WIKI. mixing w and x... honestly
	//http://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#From_an_orthogonal_matrix_to_a_quaternion
}
Quat Quat::operator+(const Quat& A) const
{
	return Quat(x+A.x, y+A.y, z+A.z, w+A.w);
}
Quat Quat::operator-(const Quat& A) const
{
	return Quat(x-A.x, y-A.y, z-A.z, w-A.w);
}
Quat Quat::operator*(float d) const
{
	return Quat(x*d, y*d, z*d, w*d);
}
Quat Quat::operator*(const Quat& A) const
{
	return Quat(
		x*A.w + w*A.x + y*A.z - z*A.y,
		w*A.y - x*A.z + y*A.w + z*A.x,
		w*A.z + x*A.y - y*A.x + z*A.w,
		w*A.w - x*A.x - y*A.y - z*A.z);
}
vec3f Quat::operator*(const vec3f& v) const
{
	float m[9];
	m[0] = w*w+x*x-y*y-z*z; m[1] = 2*x*y-2*w*z;     m[2] = 2*x*z + 2*w*y;
	m[3] = 2*x*y + 2*w*z;   m[4] = w*w-x*x+y*y-z*z; m[5] = 2*y*z - 2*w*x;
	m[6] = 2*x*z - 2*w*y;   m[7] = 2*y*z + 2*w*x;   m[8] = w*w-x*x-y*y+z*z;
	vec3f r;
	r.x = m[0] * v.x + m[1] * v.y + m[2] * v.z;
	r.y = m[3] * v.x + m[4] * v.y + m[5] * v.z;
	r.z = m[6] * v.x + m[7] * v.y + m[8] * v.z;
	return r;
}
Quat Quat::operator/(float d) const
{
	return Quat(x/d, y/d, z/d, w/d);
}
Quat Quat::operator/(const Quat& A) const
{
	float qq =
		A.x * A.x + 
		A.y * A.y + 
		A.z * A.z + 
		A.w * A.w;

	return *this * Quat(
		-A.x/qq,
		-A.y/qq,
		-A.z/qq,
		A.w/qq
		);
}
void Quat::operator+=(const Quat& A)
{
	x += A.x;
	y += A.y;
	z += A.z;
	w += A.w;
}
void Quat::operator-=(const Quat& A)
{
	x -= A.x;
	y -= A.y;
	z -= A.z;
	w -= A.w;
}
void Quat::operator*=(float d)
{
	x *= d;
	y *= d;
	z *= d;
	w *= d;
}
void Quat::operator*=(const Quat& A)
{
	float tx = x*A.w + w*A.x + y*A.z - z*A.y;
	float ty = w*A.y - x*A.z + y*A.w + z*A.x;
	float tz = w*A.z + x*A.y - y*A.x + z*A.w;
	w = w*A.w - x*A.x - y*A.y - z*A.z;
	x = tx;
	y = ty;
	z = tz;
}
void Quat::operator/=(float d)
{
	x /= d;
	y /= d;
	z /= d;
	w /= d;
}
float Quat::operator[](int index)
{
	if (index == 0)
	{
		return x;
	}
	else if (index == 1)
	{
		return y;
	}
	else if (index == 2)
	{
		return z;
	}
	else if (index == 3)
	{
		return w;
	}
	return 0.0f;
}
bool Quat::operator==(const Quat& A)
{
	return x == A.x &&
		y == A.y &&
		z == A.z &&
		w == A.w;
}
bool Quat::operator!=(const Quat& A)
{
	return x != A.x ||
		y != A.y ||
		z != A.z ||
		w != A.w;
}
void Quat::angleAxis(float& a, vec3f& v) const
{
	float wsign = w>0.0f?1.0f:-1.0f; //get shortest direction
	float ww = sqrt(1.0f - w*w) * wsign;
	if (ww == 0.0f)
		v = vec3f(0, 1, 0);
	else
		v = vec3f(x / ww, y / ww, z / ww);
	a = 2.0f * acos(w*wsign);
}
float Quat::getAngle(void) const
{
	return 2.0f * acos(fabs(w));
}
vec3f Quat::getAxis(void) const
{
	float ww = sqrt(1.0f - w*w);
	if (ww == 0.0f)
		return vec3f(0, 1, 0);
	else
		return vec3f(x / ww, y / ww, z / ww);
}
Quat Quat::inverse(void) const
{
	float qq =
		x * x + 
		y * y + 
		z * z + 
		w * w;
	return Quat(-x, -y, -z, w) / qq;
}
float Quat::dot(const Quat& A) const
{
	return
		x * A.x +
		y * A.y +
		z * A.z +
		w * A.w;
}
vec3f Quat::euler(void) const
{
	//awwwwesome: http://www.3dgametechnology.com/wp/converting-quaternion-to-euler-angle/
	float sqx = x * x;
	float sqy = y * y;
	float sqz = z * z;
	float sqw = w * w;

	float unit = sqx + sqy + sqz + sqw;
	float test = (x * w - y * z);
	
	if (test > 0.4999999f * unit)
	{
		return vec3f(pi / 2.0f, 2.0f*atan2(y, w), 0.0f);
	}
	else if (test < -0.4999999f * unit)
	{
		return vec3f(-pi / 2.0f, 2.0f*atan2(y, w), 0.0f);
	}
    else
    {
		return vec3f(
			asin(2.0f*(x*w-y*z)),
			atan2(2.0f*(x*z+y*w), 1.0f - 2.0f*(sqx+sqy)),
			atan2(2.0f*(x*y+z*w), 1.0f - 2.0f*(sqx+sqz))
			);
	}
	
	//stupid fail coordinate systems

	//nope: http://forums.create.msdn.com/forums/t/4574.aspx
	//atan2(2.0f*(x*w-y*z), 1.0f - 2.0f*(x*x+z*z)),
	//atan2(2.0f*(y*w-x*z), 1.0f - 2.0f*(y*y+z*z)),
	//asin(2.0f*(x*y+z*w))
	
	//full of crap: http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
	//atan2(2.0f*(y*w-x*z), 1.0f - 2.0f*(y*y+z*z)),
	//asin(2.0f*(x*y + z*w)),
	//atan2(2.0f*(x*w-y*z), 1.0f - 2.0f*(x*x+z*z))
		
	//full of crap: http://en.wikipedia.org/wiki/Euler_angles
	//atan2(2.0f*(w*x+y*z), 1.0f - 2.0f*(x*x+y*y)),
	//asin(2.0f*(w*y - z*x)),
	//atan2(2.0f*(w*z+x*y), 1.0f - 2.0f*(y*y+z*z))
}
float Quat::sqsize(void) const
{
	return x * x + 
		y * y + 
		z * z + 
		w * w;
}
float Quat::size(void) const
{
	return sqrt(x * x + 
		y * y + 
		z * z + 
		w * w);
}
Quat Quat::unit(void) const
{
	float size = sqrt(
		x * x + 
		y * y + 
		z * z + 
		w * w
		);
	return Quat(x/size, y/size, z/size, w/size);
}
void Quat::normalize(void)
{
	float size = sqrt(
		x * x + 
		y * y + 
		z * z + 
		w * w
		);
	if (w < 0)
		size = -size;
	x /= size;
	y /= size;
	z /= size;
	w /= size;
}
mat44 Quat::getMatrix()
{
	//from: http://gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation
	float x2 = x * x;
	float y2 = y * y;
	float z2 = z * z;
	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float wx = w * x;
	float wy = w * y;
	float wz = w * z;
	return mat44( 1.0f - 2.0f * (y2 + z2), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
		2.0f * (xy + wz), 1.0f - 2.0f * (x2 + z2), 2.0f * (yz - wx), 0.0f,
		2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (x2 + y2), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}
Quat Quat::slerp(Quat q, float d) const
{
	float cosHalfTheta = dot(q);
	if (fabs(cosHalfTheta) >= 1.0f)
		return *this;
	if (cosHalfTheta < 0.0)
	{
		q = -q;
		cosHalfTheta = -cosHalfTheta;
	}
	float halfTheta = acos(cosHalfTheta);
	float sinHalfTheta = sqrt(1.0f - cosHalfTheta * cosHalfTheta);
	if (fabs(sinHalfTheta) < 0.0001)
		return (*this + q) * 0.5;
	float a = sin((1.0f - d) * halfTheta) / sinHalfTheta;
	float b = sin(d * halfTheta) / sinHalfTheta;
	return (*this * a + q * b).unit(); //wtf shouldn't have to normalize
}
Quat Quat::lerp(Quat q, float d) const
{
	return *this + (q - *this) * d;
}
Quat Quat::nlerp(Quat q, float d) const
{
	return *this; //NOT IMPLEMENTED
}
Quat Quat::identity()
{
	Quat r;
	r.x = 0.0f;
	r.y = 0.0f;
	r.z = 0.0f;
	r.w = 1.0f;
	return r;
}
Quat operator-(const Quat& q) //negate
{
	Quat r;
	r.w = -q.w; //FIXME: should w be kept positive?
	r.x = -q.x;
	r.y = -q.y;
	r.z = -q.z;
	return r;
}
Quat Quat::fromEuler(float pitch, float yaw, float roll)
{
	Quat r;

	pitch *= 0.5f;
	yaw *= 0.5f;
	roll *= 0.5f;
	float c2 = cos(pitch);
	float s2 = sin(pitch);
	float c1 = cos(yaw);
	float s1 = sin(yaw);
	float c3 = cos(roll);
	float s3 = sin(roll);
	
	r.w = c1*c2*c3 - s1*s2*s3;
	r.x = s1*s2*c3 + c1*c2*s3;
	r.y = s1*c2*c3 + c1*s2*s3;
	r.z = c1*s2*c3 - s1*c2*s3;
	return r;
	/*
	    Quaternion quaternion;
    float num9 = roll * 0.5f;

    float num6 = (float) Math.Sin((double) num9);
    float num5 = (float) Math.Cos((double) num9);
    float num8 = pitch * 0.5f;
    float num4 = (float) Math.Sin((double) num8);

    float num3 = (float) Math.Cos((double) num8);
    float num7 = yaw * 0.5f;
    float num2 = (float) Math.Sin((double) num7);
    float num = (float) Math.Cos((double) num7);

    quaternion.X = ((num * num4) * num5) + ((num2 * num3) * num6);
    quaternion.Y = ((num2 * num3) * num5) - ((num * num4) * num6);
    quaternion.Z = ((num * num3) * num6) - ((num2 * num4) * num5);
    quaternion.W = ((num * num3) * num5) + ((num2 * num4) * num6);

    return quaternion;
    */
}
Quat Quat::fromEuler(const vec2f& angles)
{
	return fromEuler(angles.x, angles.y, 0.0f);
}
Quat Quat::fromEuler(const vec3f& angles)
{
	return fromEuler(angles.x, angles.y, angles.z);
}
Quat Quat::fromTo(const vec3f& from, const vec3f& to)
{
	//from http://www.euclideanspace.com/maths/algebra/vectors/angleBetween/index.htm
	Quat r;
	float d = from.dot(to);
	vec3f axis = from.cross(to);
	float qw = sqrt(from.sizesq()*to.sizesq()) + d;
	if (qw < 0.0001f)
	{
		//vectors are 180 degrees apart
		r.w = 0.0f; r.x = -from.z; r.y = from.y; r.z = from.x;
	}
	else
	{
		r.w = qw; r.x = axis.x; r.y = axis.y; r.z = axis.z;
	}
	r.normalize();
	return r;
}
Quat Quat::dirUp(const vec3f& dir, const vec3f& up)
{
	Quat ret;
	vec3f f = dir.unit();
	vec3f u = up.unit();
	vec3f s = f.cross(u).unit();
	u = s.cross(f);
	
	mat33 m;
	m.m[0] = s.x;
	m.m[1] = s.y;
	m.m[2] = s.z;
	m.m[3] = u.x;
	m.m[4] = u.y;
	m.m[5] = u.z;
	m.m[6] = -f.x;
	m.m[7] = -f.y;
	m.m[8] = -f.z;
	
	float r = sqrt(1.0f + m.m[0] + m.m[4] + m.m[8]);
	if (r == 0.0f)
	{
		ret.w = 1.0f;
		ret.x = ret.y = ret.z = 0.0f;
	}
	else
	{
		ret.w = r * 0.5f;
		ret.x = (m.m[5] - m.m[7]) / (2.0f * r);
		ret.y = (m.m[6] - m.m[2]) / (2.0f * r);
		ret.z = (m.m[1] - m.m[3]) / (2.0f * r);
	}
	return ret;
}


