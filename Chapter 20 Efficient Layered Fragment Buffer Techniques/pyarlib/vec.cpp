/* Copyright 2011 Pyarelal Knowles, under GNU GPL (see LICENCE.txt) */
/* file generated on 21/02/12 with gencode.py */

//#alltypes=("f", "float"), ("i", "int"), ("d", "double")
#include "vec.h"

#ifdef _WIN32
#pragma warning (disable:4244) //loss of precision warnings
#endif

vec2f::vec2f()
{
}
vec2f::vec2f(float d)
{
	x = d;
	y = d;
}
vec2f::vec2f(float nx, float ny)
{
	x = nx;
	y = ny;
}
vec2f::vec2f(vec3f other)
{
	x = (float)other.x;
	y = (float)other.y;
}
vec2f::vec2f(vec4f other)
{
	x = (float)other.x;
	y = (float)other.y;
}
vec2f::vec2f(vec2i other)
{
	x = (float)other.x;
	y = (float)other.y;
}
vec2f::vec2f(vec3i other)
{
	x = (float)other.x;
	y = (float)other.y;
}
vec2f::vec2f(vec4i other)
{
	x = (float)other.x;
	y = (float)other.y;
}
vec2f::vec2f(vec2d other)
{
	x = (float)other.x;
	y = (float)other.y;
}
vec2f::vec2f(vec3d other)
{
	x = (float)other.x;
	y = (float)other.y;
}
vec2f::vec2f(vec4d other)
{
	x = (float)other.x;
	y = (float)other.y;
}
void vec2f::operator()(float d)
{
	x = d;
	y = d;
}
void vec2f::operator()(float nx, float ny)
{
	x = nx;
	y = ny;
}
vec2f vec2f::operator+(const vec2f& o) const
{
	return vec2f(x+o.x, y+o.y);
}
void vec2f::operator+=(const vec2f& o)
{
	x += o.x;
	y += o.y;
}
vec2f vec2f::operator+(float d) const
{
	return vec2f(x+d, y+d);
}
vec2f vec2f::operator+(double d) const
{
	return vec2f(x+d, y+d);
}
void vec2f::operator+=(float d)
{
	x += d;
	y += d;
}
void vec2f::operator+=(double d)
{
	x += d;
	y += d;
}
vec2f vec2f::operator-(const vec2f& o) const
{
	return vec2f(x-o.x, y-o.y);
}
void vec2f::operator-=(const vec2f& o)
{
	x -= o.x;
	y -= o.y;
}
vec2f vec2f::operator-(float d) const
{
	return vec2f(x-d, y-d);
}
vec2f vec2f::operator-(double d) const
{
	return vec2f(x-d, y-d);
}
void vec2f::operator-=(float d)
{
	x -= d;
	y -= d;
}
void vec2f::operator-=(double d)
{
	x -= d;
	y -= d;
}
vec2f vec2f::operator/(const vec2f& o) const
{
	return vec2f(x/o.x, y/o.y);
}
void vec2f::operator/=(const vec2f& o)
{
	x /= o.x;
	y /= o.y;
}
vec2f vec2f::operator/(float d) const
{
	return vec2f(x/d, y/d);
}
vec2f vec2f::operator/(double d) const
{
	return vec2f(x/d, y/d);
}
void vec2f::operator/=(float d)
{
	x /= d;
	y /= d;
}
void vec2f::operator/=(double d)
{
	x /= d;
	y /= d;
}
vec2f vec2f::operator*(const vec2f& o) const
{
	return vec2f(x*o.x, y*o.y);
}
void vec2f::operator*=(const vec2f& o)
{
	x *= o.x;
	y *= o.y;
}
vec2f vec2f::operator*(float d) const
{
	return vec2f(x*d, y*d);
}
vec2f vec2f::operator*(double d) const
{
	return vec2f(x*d, y*d);
}
void vec2f::operator*=(float d)
{
	x *= d;
	y *= d;
}
void vec2f::operator*=(double d)
{
	x *= d;
	y *= d;
}
bool vec2f::operator==(const vec2f& other) const
{
	return x==other.x && y==other.y;
}
bool vec2f::operator!=(const vec2f& other) const
{
	return x!=other.x || y!=other.y;
}
bool vec2f::operator>(const vec2f& other) const
{
	return size() > other.size();
}
bool vec2f::operator<(const vec2f& other) const
{
	return size() < other.size();
}
bool vec2f::operator>=(const vec2f& other) const
{
	return size() >= other.size();
}
bool vec2f::operator<=(const vec2f& other) const
{
	return size() <= other.size();
}
vec2f vec2f::operator%(const vec2f& other) const
{
	return vec2f(fmod(x,other.x), fmod(y,other.y));
}
vec2f vec2f::operator%(float d) const
{
	return vec2f(fmod(x,d), fmod(y,d));
}
void vec2f::operator%=(const vec2f& other)
{
	x = fmod(x, other.x);
	y = fmod(y, other.y);
}
void vec2f::operator%=(float d)
{
	x = fmod(x, d);
	y = fmod(y, d);
}
float vec2f::distsq(const vec2f& other) const
{
	float dx = other.x - x;
	float dy = other.y - y;
	return dx*dx + dy*dy;
}
float vec2f::dist(const vec2f& other) const
{
	float dx = (float)(other.x - x);
	float dy = (float)(other.y - y);
	return (float)sqrt(dx*dx + dy*dy);
}
float vec2f::dot(const vec2f& other) const
{
	return (float)(x*other.x + y*other.y);
}
float vec2f::sizesq() const
{
	return (float)x*x + (float)y*y;
}
float vec2f::size() const
{
	return (float)sqrt((float)x*x + (float)y*y);
}
float vec2f::cmax() const
{
	float r = x;
	r = r > y ? r : y;
	return r;
}
float vec2f::cmin() const
{
	float r = x;
	r = r < y ? r : y;
	return r;
}
vec2f vec2f::tolen(float len)
{
	return *this * (len / size());
}
void vec2f::normalize()
{
	*this /= size();
}
vec2f vec2f::unit() const
{
	return *this / size();
}
vec3f vec2f::toVec() const
{
	vec3f v;
	v.y = sin(x);
	float r = cos(x);
	v.x = -sin(y)*r;
	v.z = -cos(y)*r;
	return v;
}
vec2f vec2f::fromVec(const vec3f& v)
{
	vec2f r;
	float d = sqrt(v.x*v.x + v.z*v.z);
	r.x = atan2(v.y, d);
	r.y = atan2(-v.x, -v.z);
	return r;
}

//swivel operators
vec2f vec2f::yx() const
{
	return vec2f(y, x);
}

vec2f operator-(vec2f v)
{
	return vec2f(-v.x, -v.y);
}
vec2f operator/(float n, vec2f v)
{
	return vec2f(n/v.x, n/v.y);
}
vec2f operator/(double n, vec2f v)
{
	return vec2f(n/v.x, n/v.y);
}
vec2f vmin(vec2f a, vec2f b)
{
	vec2f ret;
	ret.y = a.y < b.y ? a.y : b.y;
	return ret;
}
vec2f vmax(vec2f a, vec2f b)
{
	vec2f ret;
	ret.y = a.y > b.y ? a.y : b.y;
	return ret;
}
vec2f vmin(vec2f a, float b)
{
	vec2f ret;
	ret.y = a.y < b ? a.y : b;
	return ret;
}
vec2f vmax(vec2f a, float b)
{
	vec2f ret;
	ret.y = a.y > b ? a.y : b;
	return ret;
}
vec2f vmin(float a, vec2f b)
{
	vec2f ret;
	ret.y = a < b.y ? a : b.y;
	return ret;
}
vec2f vmax(float a, vec2f b)
{
	vec2f ret;
	ret.y = a > b.y ? a : b.y;
	return ret;
}
vec3f::vec3f()
{
}
vec3f::vec3f(float d)
{
	x = d;
	y = d;
	z = d;
}
vec3f::vec3f(float nx, float ny, float nz)
{
	x = nx;
	y = ny;
	z = nz;
}
vec3f::vec3f(vec2f other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = 0;
}
vec3f::vec3f(vec4f other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = (float)other.z;
}
vec3f::vec3f(vec2i other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = 0;
}
vec3f::vec3f(vec3i other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = (float)other.z;
}
vec3f::vec3f(vec4i other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = (float)other.z;
}
vec3f::vec3f(vec2d other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = 0;
}
vec3f::vec3f(vec3d other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = (float)other.z;
}
vec3f::vec3f(vec4d other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = (float)other.z;
}
vec3f::vec3f(vec2d other, float nz)
{
	x = (float)other.x;
	y = (float)other.y;
	z = nz;
}
void vec3f::operator()(float d)
{
	x = d;
	y = d;
	z = d;
}
void vec3f::operator()(float nx, float ny, float nz)
{
	x = nx;
	y = ny;
	z = nz;
}
vec3f vec3f::operator+(const vec3f& o) const
{
	return vec3f(x+o.x, y+o.y, z+o.z);
}
void vec3f::operator+=(const vec3f& o)
{
	x += o.x;
	y += o.y;
	z += o.z;
}
vec3f vec3f::operator+(float d) const
{
	return vec3f(x+d, y+d, z+d);
}
vec3f vec3f::operator+(double d) const
{
	return vec3f(x+d, y+d, z+d);
}
void vec3f::operator+=(float d)
{
	x += d;
	y += d;
	z += d;
}
void vec3f::operator+=(double d)
{
	x += d;
	y += d;
	z += d;
}
vec3f vec3f::operator-(const vec3f& o) const
{
	return vec3f(x-o.x, y-o.y, z-o.z);
}
void vec3f::operator-=(const vec3f& o)
{
	x -= o.x;
	y -= o.y;
	z -= o.z;
}
vec3f vec3f::operator-(float d) const
{
	return vec3f(x-d, y-d, z-d);
}
vec3f vec3f::operator-(double d) const
{
	return vec3f(x-d, y-d, z-d);
}
void vec3f::operator-=(float d)
{
	x -= d;
	y -= d;
	z -= d;
}
void vec3f::operator-=(double d)
{
	x -= d;
	y -= d;
	z -= d;
}
vec3f vec3f::operator/(const vec3f& o) const
{
	return vec3f(x/o.x, y/o.y, z/o.z);
}
void vec3f::operator/=(const vec3f& o)
{
	x /= o.x;
	y /= o.y;
	z /= o.z;
}
vec3f vec3f::operator/(float d) const
{
	return vec3f(x/d, y/d, z/d);
}
vec3f vec3f::operator/(double d) const
{
	return vec3f(x/d, y/d, z/d);
}
void vec3f::operator/=(float d)
{
	x /= d;
	y /= d;
	z /= d;
}
void vec3f::operator/=(double d)
{
	x /= d;
	y /= d;
	z /= d;
}
vec3f vec3f::operator*(const vec3f& o) const
{
	return vec3f(x*o.x, y*o.y, z*o.z);
}
void vec3f::operator*=(const vec3f& o)
{
	x *= o.x;
	y *= o.y;
	z *= o.z;
}
vec3f vec3f::operator*(float d) const
{
	return vec3f(x*d, y*d, z*d);
}
vec3f vec3f::operator*(double d) const
{
	return vec3f(x*d, y*d, z*d);
}
void vec3f::operator*=(float d)
{
	x *= d;
	y *= d;
	z *= d;
}
void vec3f::operator*=(double d)
{
	x *= d;
	y *= d;
	z *= d;
}
bool vec3f::operator==(const vec3f& other) const
{
	return x==other.x && y==other.y && z==other.z;
}
bool vec3f::operator!=(const vec3f& other) const
{
	return x!=other.x || y!=other.y || z!=other.z;
}
bool vec3f::operator>(const vec3f& other) const
{
	return size() > other.size();
}
bool vec3f::operator<(const vec3f& other) const
{
	return size() < other.size();
}
bool vec3f::operator>=(const vec3f& other) const
{
	return size() >= other.size();
}
bool vec3f::operator<=(const vec3f& other) const
{
	return size() <= other.size();
}
vec3f vec3f::operator%(const vec3f& other) const
{
	return vec3f(fmod(x,other.x), fmod(y,other.y), fmod(z,other.z));
}
vec3f vec3f::operator%(float d) const
{
	return vec3f(fmod(x,d), fmod(y,d), fmod(z,d));
}
void vec3f::operator%=(const vec3f& other)
{
	x = fmod(x, other.x);
	y = fmod(y, other.y);
	z = fmod(z, other.z);
}
void vec3f::operator%=(float d)
{
	x = fmod(x, d);
	y = fmod(y, d);
	z = fmod(z, d);
}
float vec3f::distsq(const vec3f& other) const
{
	float dx = other.x - x;
	float dy = other.y - y;
	float dz = other.z - z;
	return dx*dx + dy*dy + dz*dz;
}
float vec3f::dist(const vec3f& other) const
{
	float dx = (float)(other.x - x);
	float dy = (float)(other.y - y);
	float dz = (float)(other.z - z);
	return (float)sqrt(dx*dx + dy*dy + dz*dz);
}
float vec3f::dot(const vec3f& other) const
{
	return (float)(x*other.x + y*other.y + z*other.z);
}
float vec3f::sizesq() const
{
	return (float)x*x + (float)y*y + (float)z*z;
}
float vec3f::size() const
{
	return (float)sqrt((float)x*x + (float)y*y + (float)z*z);
}
float vec3f::cmax() const
{
	float r = x;
	r = r > y ? r : y;
	r = r > z ? r : z;
	return r;
}
float vec3f::cmin() const
{
	float r = x;
	r = r < y ? r : y;
	r = r < z ? r : z;
	return r;
}
vec3f vec3f::tolen(float len)
{
	return *this * (len / size());
}
void vec3f::normalize()
{
	*this /= size();
}
vec3f vec3f::unit() const
{
	return *this / size();
}
vec3f vec3f::cross(const vec3f& other) const
{
	float a = y * other.z - z * other.y;
	float b = x * other.z - z * other.x;
	float c = x * other.y - y * other.x;
	return vec3f(a, -b, c);
}

//swivel operators
vec2f vec3f::xy() const
{
	return vec2f(x, y);
}
vec2f vec3f::xz() const
{
	return vec2f(x, z);
}
vec2f vec3f::yx() const
{
	return vec2f(y, x);
}
vec2f vec3f::yz() const
{
	return vec2f(y, z);
}
vec2f vec3f::zx() const
{
	return vec2f(z, x);
}
vec2f vec3f::zy() const
{
	return vec2f(z, y);
}
vec3f vec3f::xzy() const
{
	return vec3f(x, z, y);
}
vec3f vec3f::yxz() const
{
	return vec3f(y, x, z);
}
vec3f vec3f::yzx() const
{
	return vec3f(y, z, x);
}
vec3f vec3f::zxy() const
{
	return vec3f(z, x, y);
}
vec3f vec3f::zyx() const
{
	return vec3f(z, y, x);
}

vec3f operator-(vec3f v)
{
	return vec3f(-v.x, -v.y, -v.z);
}
vec3f operator/(float n, vec3f v)
{
	return vec3f(n/v.x, n/v.y, n/v.z);
}
vec3f operator/(double n, vec3f v)
{
	return vec3f(n/v.x, n/v.y, n/v.z);
}
vec3f vmin(vec3f a, vec3f b)
{
	vec3f ret;
	ret.y = a.y < b.y ? a.y : b.y;
	ret.z = a.z < b.z ? a.z : b.z;
	return ret;
}
vec3f vmax(vec3f a, vec3f b)
{
	vec3f ret;
	ret.y = a.y > b.y ? a.y : b.y;
	ret.z = a.z > b.z ? a.z : b.z;
	return ret;
}
vec3f vmin(vec3f a, float b)
{
	vec3f ret;
	ret.y = a.y < b ? a.y : b;
	ret.z = a.z < b ? a.z : b;
	return ret;
}
vec3f vmax(vec3f a, float b)
{
	vec3f ret;
	ret.y = a.y > b ? a.y : b;
	ret.z = a.z > b ? a.z : b;
	return ret;
}
vec3f vmin(float a, vec3f b)
{
	vec3f ret;
	ret.y = a < b.y ? a : b.y;
	ret.z = a < b.z ? a : b.z;
	return ret;
}
vec3f vmax(float a, vec3f b)
{
	vec3f ret;
	ret.y = a > b.y ? a : b.y;
	ret.z = a > b.z ? a : b.z;
	return ret;
}
vec4f::vec4f()
{
}
vec4f::vec4f(float d)
{
	x = d;
	y = d;
	z = d;
	w = d;
}
vec4f::vec4f(float nx, float ny, float nz, float nw)
{
	x = nx;
	y = ny;
	z = nz;
	w = nw;
}
vec4f::vec4f(vec2f other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = 0;
	w = 0;
}
vec4f::vec4f(vec3f other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = (float)other.z;
	w = 0;
}
vec4f::vec4f(vec2i other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = 0;
	w = 0;
}
vec4f::vec4f(vec3i other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = (float)other.z;
	w = 0;
}
vec4f::vec4f(vec4i other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = (float)other.z;
	w = (float)other.w;
}
vec4f::vec4f(vec2d other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = 0;
	w = 0;
}
vec4f::vec4f(vec3d other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = (float)other.z;
	w = 0;
}
vec4f::vec4f(vec4d other)
{
	x = (float)other.x;
	y = (float)other.y;
	z = (float)other.z;
	w = (float)other.w;
}
vec4f::vec4f(vec2d other, float nz, float nw)
{
	x = (float)other.x;
	y = (float)other.y;
	z = nz;
	w = nw;
}
vec4f::vec4f(vec3d other, float nw)
{
	x = (float)other.x;
	y = (float)other.y;
	z = (float)other.z;
	w = nw;
}
void vec4f::operator()(float d)
{
	x = d;
	y = d;
	z = d;
	w = d;
}
void vec4f::operator()(float nx, float ny, float nz, float nw)
{
	x = nx;
	y = ny;
	z = nz;
	w = nw;
}
vec4f vec4f::operator+(const vec4f& o) const
{
	return vec4f(x+o.x, y+o.y, z+o.z, w+o.w);
}
void vec4f::operator+=(const vec4f& o)
{
	x += o.x;
	y += o.y;
	z += o.z;
	w += o.w;
}
vec4f vec4f::operator+(float d) const
{
	return vec4f(x+d, y+d, z+d, w+d);
}
vec4f vec4f::operator+(double d) const
{
	return vec4f(x+d, y+d, z+d, w+d);
}
void vec4f::operator+=(float d)
{
	x += d;
	y += d;
	z += d;
	w += d;
}
void vec4f::operator+=(double d)
{
	x += d;
	y += d;
	z += d;
	w += d;
}
vec4f vec4f::operator-(const vec4f& o) const
{
	return vec4f(x-o.x, y-o.y, z-o.z, w-o.w);
}
void vec4f::operator-=(const vec4f& o)
{
	x -= o.x;
	y -= o.y;
	z -= o.z;
	w -= o.w;
}
vec4f vec4f::operator-(float d) const
{
	return vec4f(x-d, y-d, z-d, w-d);
}
vec4f vec4f::operator-(double d) const
{
	return vec4f(x-d, y-d, z-d, w-d);
}
void vec4f::operator-=(float d)
{
	x -= d;
	y -= d;
	z -= d;
	w -= d;
}
void vec4f::operator-=(double d)
{
	x -= d;
	y -= d;
	z -= d;
	w -= d;
}
vec4f vec4f::operator/(const vec4f& o) const
{
	return vec4f(x/o.x, y/o.y, z/o.z, w/o.w);
}
void vec4f::operator/=(const vec4f& o)
{
	x /= o.x;
	y /= o.y;
	z /= o.z;
	w /= o.w;
}
vec4f vec4f::operator/(float d) const
{
	return vec4f(x/d, y/d, z/d, w/d);
}
vec4f vec4f::operator/(double d) const
{
	return vec4f(x/d, y/d, z/d, w/d);
}
void vec4f::operator/=(float d)
{
	x /= d;
	y /= d;
	z /= d;
	w /= d;
}
void vec4f::operator/=(double d)
{
	x /= d;
	y /= d;
	z /= d;
	w /= d;
}
vec4f vec4f::operator*(const vec4f& o) const
{
	return vec4f(x*o.x, y*o.y, z*o.z, w*o.w);
}
void vec4f::operator*=(const vec4f& o)
{
	x *= o.x;
	y *= o.y;
	z *= o.z;
	w *= o.w;
}
vec4f vec4f::operator*(float d) const
{
	return vec4f(x*d, y*d, z*d, w*d);
}
vec4f vec4f::operator*(double d) const
{
	return vec4f(x*d, y*d, z*d, w*d);
}
void vec4f::operator*=(float d)
{
	x *= d;
	y *= d;
	z *= d;
	w *= d;
}
void vec4f::operator*=(double d)
{
	x *= d;
	y *= d;
	z *= d;
	w *= d;
}
bool vec4f::operator==(const vec4f& other) const
{
	return x==other.x && y==other.y && z==other.z && w==other.w;
}
bool vec4f::operator!=(const vec4f& other) const
{
	return x!=other.x || y!=other.y || z!=other.z || w!=other.w;
}
bool vec4f::operator>(const vec4f& other) const
{
	return size() > other.size();
}
bool vec4f::operator<(const vec4f& other) const
{
	return size() < other.size();
}
bool vec4f::operator>=(const vec4f& other) const
{
	return size() >= other.size();
}
bool vec4f::operator<=(const vec4f& other) const
{
	return size() <= other.size();
}
vec4f vec4f::operator%(const vec4f& other) const
{
	return vec4f(fmod(x,other.x), fmod(y,other.y), fmod(z,other.z), fmod(w,other.w));
}
vec4f vec4f::operator%(float d) const
{
	return vec4f(fmod(x,d), fmod(y,d), fmod(z,d), fmod(w,d));
}
void vec4f::operator%=(const vec4f& other)
{
	x = fmod(x, other.x);
	y = fmod(y, other.y);
	z = fmod(z, other.z);
	w = fmod(w, other.w);
}
void vec4f::operator%=(float d)
{
	x = fmod(x, d);
	y = fmod(y, d);
	z = fmod(z, d);
	w = fmod(w, d);
}
float vec4f::distsq(const vec4f& other) const
{
	float dx = other.x - x;
	float dy = other.y - y;
	float dz = other.z - z;
	float dw = other.w - w;
	return dx*dx + dy*dy + dz*dz + dw*dw;
}
float vec4f::dist(const vec4f& other) const
{
	float dx = (float)(other.x - x);
	float dy = (float)(other.y - y);
	float dz = (float)(other.z - z);
	float dw = (float)(other.w - w);
	return (float)sqrt(dx*dx + dy*dy + dz*dz + dw*dw);
}
float vec4f::dot(const vec4f& other) const
{
	return (float)(x*other.x + y*other.y + z*other.z + w*other.w);
}
float vec4f::sizesq() const
{
	return (float)x*x + (float)y*y + (float)z*z + (float)w*w;
}
float vec4f::size() const
{
	return (float)sqrt((float)x*x + (float)y*y + (float)z*z + (float)w*w);
}
float vec4f::cmax() const
{
	float r = x;
	r = r > y ? r : y;
	r = r > z ? r : z;
	r = r > w ? r : w;
	return r;
}
float vec4f::cmin() const
{
	float r = x;
	r = r < y ? r : y;
	r = r < z ? r : z;
	r = r < w ? r : w;
	return r;
}
vec4f vec4f::tolen(float len)
{
	return *this * (len / size());
}
void vec4f::normalize()
{
	*this /= size();
}
vec4f vec4f::unit() const
{
	return *this / size();
}

//swivel operators
vec2f vec4f::xy() const
{
	return vec2f(x, y);
}
vec2f vec4f::xz() const
{
	return vec2f(x, z);
}
vec2f vec4f::xw() const
{
	return vec2f(x, w);
}
vec2f vec4f::yx() const
{
	return vec2f(y, x);
}
vec2f vec4f::yz() const
{
	return vec2f(y, z);
}
vec2f vec4f::yw() const
{
	return vec2f(y, w);
}
vec2f vec4f::zx() const
{
	return vec2f(z, x);
}
vec2f vec4f::zy() const
{
	return vec2f(z, y);
}
vec2f vec4f::zw() const
{
	return vec2f(z, w);
}
vec2f vec4f::wx() const
{
	return vec2f(w, x);
}
vec2f vec4f::wy() const
{
	return vec2f(w, y);
}
vec2f vec4f::wz() const
{
	return vec2f(w, z);
}
vec3f vec4f::xyz() const
{
	return vec3f(x, y, z);
}
vec3f vec4f::xyw() const
{
	return vec3f(x, y, w);
}
vec3f vec4f::xzy() const
{
	return vec3f(x, z, y);
}
vec3f vec4f::xzw() const
{
	return vec3f(x, z, w);
}
vec3f vec4f::xwy() const
{
	return vec3f(x, w, y);
}
vec3f vec4f::xwz() const
{
	return vec3f(x, w, z);
}
vec3f vec4f::yxz() const
{
	return vec3f(y, x, z);
}
vec3f vec4f::yxw() const
{
	return vec3f(y, x, w);
}
vec3f vec4f::yzx() const
{
	return vec3f(y, z, x);
}
vec3f vec4f::yzw() const
{
	return vec3f(y, z, w);
}
vec3f vec4f::ywx() const
{
	return vec3f(y, w, x);
}
vec3f vec4f::ywz() const
{
	return vec3f(y, w, z);
}
vec3f vec4f::zxy() const
{
	return vec3f(z, x, y);
}
vec3f vec4f::zxw() const
{
	return vec3f(z, x, w);
}
vec3f vec4f::zyx() const
{
	return vec3f(z, y, x);
}
vec3f vec4f::zyw() const
{
	return vec3f(z, y, w);
}
vec3f vec4f::zwx() const
{
	return vec3f(z, w, x);
}
vec3f vec4f::zwy() const
{
	return vec3f(z, w, y);
}
vec3f vec4f::wxy() const
{
	return vec3f(w, x, y);
}
vec3f vec4f::wxz() const
{
	return vec3f(w, x, z);
}
vec3f vec4f::wyx() const
{
	return vec3f(w, y, x);
}
vec3f vec4f::wyz() const
{
	return vec3f(w, y, z);
}
vec3f vec4f::wzx() const
{
	return vec3f(w, z, x);
}
vec3f vec4f::wzy() const
{
	return vec3f(w, z, y);
}
vec4f vec4f::xywz() const
{
	return vec4f(x, y, w, z);
}
vec4f vec4f::xzyw() const
{
	return vec4f(x, z, y, w);
}
vec4f vec4f::xzwy() const
{
	return vec4f(x, z, w, y);
}
vec4f vec4f::xwyz() const
{
	return vec4f(x, w, y, z);
}
vec4f vec4f::xwzy() const
{
	return vec4f(x, w, z, y);
}
vec4f vec4f::yxzw() const
{
	return vec4f(y, x, z, w);
}
vec4f vec4f::yxwz() const
{
	return vec4f(y, x, w, z);
}
vec4f vec4f::yzxw() const
{
	return vec4f(y, z, x, w);
}
vec4f vec4f::yzwx() const
{
	return vec4f(y, z, w, x);
}
vec4f vec4f::ywxz() const
{
	return vec4f(y, w, x, z);
}
vec4f vec4f::ywzx() const
{
	return vec4f(y, w, z, x);
}
vec4f vec4f::zxyw() const
{
	return vec4f(z, x, y, w);
}
vec4f vec4f::zxwy() const
{
	return vec4f(z, x, w, y);
}
vec4f vec4f::zyxw() const
{
	return vec4f(z, y, x, w);
}
vec4f vec4f::zywx() const
{
	return vec4f(z, y, w, x);
}
vec4f vec4f::zwxy() const
{
	return vec4f(z, w, x, y);
}
vec4f vec4f::zwyx() const
{
	return vec4f(z, w, y, x);
}
vec4f vec4f::wxyz() const
{
	return vec4f(w, x, y, z);
}
vec4f vec4f::wxzy() const
{
	return vec4f(w, x, z, y);
}
vec4f vec4f::wyxz() const
{
	return vec4f(w, y, x, z);
}
vec4f vec4f::wyzx() const
{
	return vec4f(w, y, z, x);
}
vec4f vec4f::wzxy() const
{
	return vec4f(w, z, x, y);
}
vec4f vec4f::wzyx() const
{
	return vec4f(w, z, y, x);
}

vec4f operator-(vec4f v)
{
	return vec4f(-v.x, -v.y, -v.z, -v.w);
}
vec4f operator/(float n, vec4f v)
{
	return vec4f(n/v.x, n/v.y, n/v.z, n/v.w);
}
vec4f operator/(double n, vec4f v)
{
	return vec4f(n/v.x, n/v.y, n/v.z, n/v.w);
}
vec4f vmin(vec4f a, vec4f b)
{
	vec4f ret;
	ret.y = a.y < b.y ? a.y : b.y;
	ret.z = a.z < b.z ? a.z : b.z;
	ret.w = a.w < b.w ? a.w : b.w;
	return ret;
}
vec4f vmax(vec4f a, vec4f b)
{
	vec4f ret;
	ret.y = a.y > b.y ? a.y : b.y;
	ret.z = a.z > b.z ? a.z : b.z;
	ret.w = a.w > b.w ? a.w : b.w;
	return ret;
}
vec4f vmin(vec4f a, float b)
{
	vec4f ret;
	ret.y = a.y < b ? a.y : b;
	ret.z = a.z < b ? a.z : b;
	ret.w = a.w < b ? a.w : b;
	return ret;
}
vec4f vmax(vec4f a, float b)
{
	vec4f ret;
	ret.y = a.y > b ? a.y : b;
	ret.z = a.z > b ? a.z : b;
	ret.w = a.w > b ? a.w : b;
	return ret;
}
vec4f vmin(float a, vec4f b)
{
	vec4f ret;
	ret.y = a < b.y ? a : b.y;
	ret.z = a < b.z ? a : b.z;
	ret.w = a < b.w ? a : b.w;
	return ret;
}
vec4f vmax(float a, vec4f b)
{
	vec4f ret;
	ret.y = a > b.y ? a : b.y;
	ret.z = a > b.z ? a : b.z;
	ret.w = a > b.w ? a : b.w;
	return ret;
}
vec2i::vec2i()
{
}
vec2i::vec2i(int d)
{
	x = d;
	y = d;
}
vec2i::vec2i(int nx, int ny)
{
	x = nx;
	y = ny;
}
vec2i::vec2i(vec2f other)
{
	x = (int)other.x;
	y = (int)other.y;
}
vec2i::vec2i(vec3f other)
{
	x = (int)other.x;
	y = (int)other.y;
}
vec2i::vec2i(vec4f other)
{
	x = (int)other.x;
	y = (int)other.y;
}
vec2i::vec2i(vec3i other)
{
	x = (int)other.x;
	y = (int)other.y;
}
vec2i::vec2i(vec4i other)
{
	x = (int)other.x;
	y = (int)other.y;
}
vec2i::vec2i(vec2d other)
{
	x = (int)other.x;
	y = (int)other.y;
}
vec2i::vec2i(vec3d other)
{
	x = (int)other.x;
	y = (int)other.y;
}
vec2i::vec2i(vec4d other)
{
	x = (int)other.x;
	y = (int)other.y;
}
void vec2i::operator()(int d)
{
	x = d;
	y = d;
}
void vec2i::operator()(int nx, int ny)
{
	x = nx;
	y = ny;
}
vec2i vec2i::operator+(const vec2i& o) const
{
	return vec2i(x+o.x, y+o.y);
}
void vec2i::operator+=(const vec2i& o)
{
	x += o.x;
	y += o.y;
}
vec2i vec2i::operator+(int d) const
{
	return vec2i(x+d, y+d);
}
vec2i vec2i::operator+(float d) const
{
	return vec2i(x+d, y+d);
}
vec2i vec2i::operator+(double d) const
{
	return vec2i(x+d, y+d);
}
void vec2i::operator+=(int d)
{
	x += d;
	y += d;
}
void vec2i::operator+=(float d)
{
	x += d;
	y += d;
}
void vec2i::operator+=(double d)
{
	x += d;
	y += d;
}
vec2i vec2i::operator-(const vec2i& o) const
{
	return vec2i(x-o.x, y-o.y);
}
void vec2i::operator-=(const vec2i& o)
{
	x -= o.x;
	y -= o.y;
}
vec2i vec2i::operator-(int d) const
{
	return vec2i(x-d, y-d);
}
vec2i vec2i::operator-(float d) const
{
	return vec2i(x-d, y-d);
}
vec2i vec2i::operator-(double d) const
{
	return vec2i(x-d, y-d);
}
void vec2i::operator-=(int d)
{
	x -= d;
	y -= d;
}
void vec2i::operator-=(float d)
{
	x -= d;
	y -= d;
}
void vec2i::operator-=(double d)
{
	x -= d;
	y -= d;
}
vec2i vec2i::operator/(const vec2i& o) const
{
	return vec2i(x/o.x, y/o.y);
}
void vec2i::operator/=(const vec2i& o)
{
	x /= o.x;
	y /= o.y;
}
vec2i vec2i::operator/(int d) const
{
	return vec2i(x/d, y/d);
}
vec2i vec2i::operator/(float d) const
{
	return vec2i(x/d, y/d);
}
vec2i vec2i::operator/(double d) const
{
	return vec2i(x/d, y/d);
}
void vec2i::operator/=(int d)
{
	x /= d;
	y /= d;
}
void vec2i::operator/=(float d)
{
	x /= d;
	y /= d;
}
void vec2i::operator/=(double d)
{
	x /= d;
	y /= d;
}
vec2i vec2i::operator*(const vec2i& o) const
{
	return vec2i(x*o.x, y*o.y);
}
void vec2i::operator*=(const vec2i& o)
{
	x *= o.x;
	y *= o.y;
}
vec2i vec2i::operator*(int d) const
{
	return vec2i(x*d, y*d);
}
vec2i vec2i::operator*(float d) const
{
	return vec2i(x*d, y*d);
}
vec2i vec2i::operator*(double d) const
{
	return vec2i(x*d, y*d);
}
void vec2i::operator*=(int d)
{
	x *= d;
	y *= d;
}
void vec2i::operator*=(float d)
{
	x *= d;
	y *= d;
}
void vec2i::operator*=(double d)
{
	x *= d;
	y *= d;
}
bool vec2i::operator==(const vec2i& other) const
{
	return x==other.x && y==other.y;
}
bool vec2i::operator!=(const vec2i& other) const
{
	return x!=other.x || y!=other.y;
}
bool vec2i::operator>(const vec2i& other) const
{
	return size() > other.size();
}
bool vec2i::operator<(const vec2i& other) const
{
	return size() < other.size();
}
bool vec2i::operator>=(const vec2i& other) const
{
	return size() >= other.size();
}
bool vec2i::operator<=(const vec2i& other) const
{
	return size() <= other.size();
}
vec2i vec2i::operator%(const vec2i& other) const
{
	return vec2i(x%other.x, y%other.y);
}
vec2i vec2i::operator%(int d) const
{
	return vec2i(x%d, y%d);
}
void vec2i::operator%=(const vec2i& other)
{
	x %= other.x;
	y %= other.y;
}
void vec2i::operator%=(int d)
{
	x %= d;
	y %= d;
}
int vec2i::distsq(const vec2i& other) const
{
	int dx = other.x - x;
	int dy = other.y - y;
	return dx*dx + dy*dy;
}
int vec2i::dist(const vec2i& other) const
{
	float dx = (float)(other.x - x);
	float dy = (float)(other.y - y);
	return (int)sqrt(dx*dx + dy*dy);
}
int vec2i::dot(const vec2i& other) const
{
	return (int)(x*other.x + y*other.y);
}
int vec2i::sizesq() const
{
	return (float)x*x + (float)y*y;
}
int vec2i::size() const
{
	return (int)sqrt((float)x*x + (float)y*y);
}
int vec2i::cmax() const
{
	int r = x;
	r = r > y ? r : y;
	return r;
}
int vec2i::cmin() const
{
	int r = x;
	r = r < y ? r : y;
	return r;
}
vec2i vec2i::tolen(int len)
{
	return *this * (len / size());
}
void vec2i::normalize()
{
	*this /= size();
}
vec2i vec2i::unit() const
{
	return *this / size();
}

//swivel operators
vec2i vec2i::yx() const
{
	return vec2i(y, x);
}

vec2i operator-(vec2i v)
{
	return vec2i(-v.x, -v.y);
}
vec2i operator/(int n, vec2i v)
{
	return vec2i(n/v.x, n/v.y);
}
vec2i operator/(float n, vec2i v)
{
	return vec2i(n/v.x, n/v.y);
}
vec2i operator/(double n, vec2i v)
{
	return vec2i(n/v.x, n/v.y);
}
vec2i vmin(vec2i a, vec2i b)
{
	vec2i ret;
	ret.y = a.y < b.y ? a.y : b.y;
	return ret;
}
vec2i vmax(vec2i a, vec2i b)
{
	vec2i ret;
	ret.y = a.y > b.y ? a.y : b.y;
	return ret;
}
vec2i vmin(vec2i a, int b)
{
	vec2i ret;
	ret.y = a.y < b ? a.y : b;
	return ret;
}
vec2i vmax(vec2i a, int b)
{
	vec2i ret;
	ret.y = a.y > b ? a.y : b;
	return ret;
}
vec2i vmin(int a, vec2i b)
{
	vec2i ret;
	ret.y = a < b.y ? a : b.y;
	return ret;
}
vec2i vmax(int a, vec2i b)
{
	vec2i ret;
	ret.y = a > b.y ? a : b.y;
	return ret;
}
vec3i::vec3i()
{
}
vec3i::vec3i(int d)
{
	x = d;
	y = d;
	z = d;
}
vec3i::vec3i(int nx, int ny, int nz)
{
	x = nx;
	y = ny;
	z = nz;
}
vec3i::vec3i(vec2f other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = 0;
}
vec3i::vec3i(vec3f other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = (int)other.z;
}
vec3i::vec3i(vec4f other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = (int)other.z;
}
vec3i::vec3i(vec2i other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = 0;
}
vec3i::vec3i(vec4i other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = (int)other.z;
}
vec3i::vec3i(vec2d other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = 0;
}
vec3i::vec3i(vec3d other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = (int)other.z;
}
vec3i::vec3i(vec4d other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = (int)other.z;
}
vec3i::vec3i(vec2d other, int nz)
{
	x = (int)other.x;
	y = (int)other.y;
	z = nz;
}
void vec3i::operator()(int d)
{
	x = d;
	y = d;
	z = d;
}
void vec3i::operator()(int nx, int ny, int nz)
{
	x = nx;
	y = ny;
	z = nz;
}
vec3i vec3i::operator+(const vec3i& o) const
{
	return vec3i(x+o.x, y+o.y, z+o.z);
}
void vec3i::operator+=(const vec3i& o)
{
	x += o.x;
	y += o.y;
	z += o.z;
}
vec3i vec3i::operator+(int d) const
{
	return vec3i(x+d, y+d, z+d);
}
vec3i vec3i::operator+(float d) const
{
	return vec3i(x+d, y+d, z+d);
}
vec3i vec3i::operator+(double d) const
{
	return vec3i(x+d, y+d, z+d);
}
void vec3i::operator+=(int d)
{
	x += d;
	y += d;
	z += d;
}
void vec3i::operator+=(float d)
{
	x += d;
	y += d;
	z += d;
}
void vec3i::operator+=(double d)
{
	x += d;
	y += d;
	z += d;
}
vec3i vec3i::operator-(const vec3i& o) const
{
	return vec3i(x-o.x, y-o.y, z-o.z);
}
void vec3i::operator-=(const vec3i& o)
{
	x -= o.x;
	y -= o.y;
	z -= o.z;
}
vec3i vec3i::operator-(int d) const
{
	return vec3i(x-d, y-d, z-d);
}
vec3i vec3i::operator-(float d) const
{
	return vec3i(x-d, y-d, z-d);
}
vec3i vec3i::operator-(double d) const
{
	return vec3i(x-d, y-d, z-d);
}
void vec3i::operator-=(int d)
{
	x -= d;
	y -= d;
	z -= d;
}
void vec3i::operator-=(float d)
{
	x -= d;
	y -= d;
	z -= d;
}
void vec3i::operator-=(double d)
{
	x -= d;
	y -= d;
	z -= d;
}
vec3i vec3i::operator/(const vec3i& o) const
{
	return vec3i(x/o.x, y/o.y, z/o.z);
}
void vec3i::operator/=(const vec3i& o)
{
	x /= o.x;
	y /= o.y;
	z /= o.z;
}
vec3i vec3i::operator/(int d) const
{
	return vec3i(x/d, y/d, z/d);
}
vec3i vec3i::operator/(float d) const
{
	return vec3i(x/d, y/d, z/d);
}
vec3i vec3i::operator/(double d) const
{
	return vec3i(x/d, y/d, z/d);
}
void vec3i::operator/=(int d)
{
	x /= d;
	y /= d;
	z /= d;
}
void vec3i::operator/=(float d)
{
	x /= d;
	y /= d;
	z /= d;
}
void vec3i::operator/=(double d)
{
	x /= d;
	y /= d;
	z /= d;
}
vec3i vec3i::operator*(const vec3i& o) const
{
	return vec3i(x*o.x, y*o.y, z*o.z);
}
void vec3i::operator*=(const vec3i& o)
{
	x *= o.x;
	y *= o.y;
	z *= o.z;
}
vec3i vec3i::operator*(int d) const
{
	return vec3i(x*d, y*d, z*d);
}
vec3i vec3i::operator*(float d) const
{
	return vec3i(x*d, y*d, z*d);
}
vec3i vec3i::operator*(double d) const
{
	return vec3i(x*d, y*d, z*d);
}
void vec3i::operator*=(int d)
{
	x *= d;
	y *= d;
	z *= d;
}
void vec3i::operator*=(float d)
{
	x *= d;
	y *= d;
	z *= d;
}
void vec3i::operator*=(double d)
{
	x *= d;
	y *= d;
	z *= d;
}
bool vec3i::operator==(const vec3i& other) const
{
	return x==other.x && y==other.y && z==other.z;
}
bool vec3i::operator!=(const vec3i& other) const
{
	return x!=other.x || y!=other.y || z!=other.z;
}
bool vec3i::operator>(const vec3i& other) const
{
	return size() > other.size();
}
bool vec3i::operator<(const vec3i& other) const
{
	return size() < other.size();
}
bool vec3i::operator>=(const vec3i& other) const
{
	return size() >= other.size();
}
bool vec3i::operator<=(const vec3i& other) const
{
	return size() <= other.size();
}
vec3i vec3i::operator%(const vec3i& other) const
{
	return vec3i(x%other.x, y%other.y, z%other.z);
}
vec3i vec3i::operator%(int d) const
{
	return vec3i(x%d, y%d, z%d);
}
void vec3i::operator%=(const vec3i& other)
{
	x %= other.x;
	y %= other.y;
	z %= other.z;
}
void vec3i::operator%=(int d)
{
	x %= d;
	y %= d;
	z %= d;
}
int vec3i::distsq(const vec3i& other) const
{
	int dx = other.x - x;
	int dy = other.y - y;
	int dz = other.z - z;
	return dx*dx + dy*dy + dz*dz;
}
int vec3i::dist(const vec3i& other) const
{
	float dx = (float)(other.x - x);
	float dy = (float)(other.y - y);
	float dz = (float)(other.z - z);
	return (int)sqrt(dx*dx + dy*dy + dz*dz);
}
int vec3i::dot(const vec3i& other) const
{
	return (int)(x*other.x + y*other.y + z*other.z);
}
int vec3i::sizesq() const
{
	return (float)x*x + (float)y*y + (float)z*z;
}
int vec3i::size() const
{
	return (int)sqrt((float)x*x + (float)y*y + (float)z*z);
}
int vec3i::cmax() const
{
	int r = x;
	r = r > y ? r : y;
	r = r > z ? r : z;
	return r;
}
int vec3i::cmin() const
{
	int r = x;
	r = r < y ? r : y;
	r = r < z ? r : z;
	return r;
}
vec3i vec3i::tolen(int len)
{
	return *this * (len / size());
}
void vec3i::normalize()
{
	*this /= size();
}
vec3i vec3i::unit() const
{
	return *this / size();
}
vec3i vec3i::cross(const vec3i& other) const
{
	int a = y * other.z - z * other.y;
	int b = x * other.z - z * other.x;
	int c = x * other.y - y * other.x;
	return vec3i(a, -b, c);
}

//swivel operators
vec2i vec3i::xy() const
{
	return vec2i(x, y);
}
vec2i vec3i::xz() const
{
	return vec2i(x, z);
}
vec2i vec3i::yx() const
{
	return vec2i(y, x);
}
vec2i vec3i::yz() const
{
	return vec2i(y, z);
}
vec2i vec3i::zx() const
{
	return vec2i(z, x);
}
vec2i vec3i::zy() const
{
	return vec2i(z, y);
}
vec3i vec3i::xzy() const
{
	return vec3i(x, z, y);
}
vec3i vec3i::yxz() const
{
	return vec3i(y, x, z);
}
vec3i vec3i::yzx() const
{
	return vec3i(y, z, x);
}
vec3i vec3i::zxy() const
{
	return vec3i(z, x, y);
}
vec3i vec3i::zyx() const
{
	return vec3i(z, y, x);
}

vec3i operator-(vec3i v)
{
	return vec3i(-v.x, -v.y, -v.z);
}
vec3i operator/(int n, vec3i v)
{
	return vec3i(n/v.x, n/v.y, n/v.z);
}
vec3i operator/(float n, vec3i v)
{
	return vec3i(n/v.x, n/v.y, n/v.z);
}
vec3i operator/(double n, vec3i v)
{
	return vec3i(n/v.x, n/v.y, n/v.z);
}
vec3i vmin(vec3i a, vec3i b)
{
	vec3i ret;
	ret.y = a.y < b.y ? a.y : b.y;
	ret.z = a.z < b.z ? a.z : b.z;
	return ret;
}
vec3i vmax(vec3i a, vec3i b)
{
	vec3i ret;
	ret.y = a.y > b.y ? a.y : b.y;
	ret.z = a.z > b.z ? a.z : b.z;
	return ret;
}
vec3i vmin(vec3i a, int b)
{
	vec3i ret;
	ret.y = a.y < b ? a.y : b;
	ret.z = a.z < b ? a.z : b;
	return ret;
}
vec3i vmax(vec3i a, int b)
{
	vec3i ret;
	ret.y = a.y > b ? a.y : b;
	ret.z = a.z > b ? a.z : b;
	return ret;
}
vec3i vmin(int a, vec3i b)
{
	vec3i ret;
	ret.y = a < b.y ? a : b.y;
	ret.z = a < b.z ? a : b.z;
	return ret;
}
vec3i vmax(int a, vec3i b)
{
	vec3i ret;
	ret.y = a > b.y ? a : b.y;
	ret.z = a > b.z ? a : b.z;
	return ret;
}
vec4i::vec4i()
{
}
vec4i::vec4i(int d)
{
	x = d;
	y = d;
	z = d;
	w = d;
}
vec4i::vec4i(int nx, int ny, int nz, int nw)
{
	x = nx;
	y = ny;
	z = nz;
	w = nw;
}
vec4i::vec4i(vec2f other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = 0;
	w = 0;
}
vec4i::vec4i(vec3f other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = (int)other.z;
	w = 0;
}
vec4i::vec4i(vec4f other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = (int)other.z;
	w = (int)other.w;
}
vec4i::vec4i(vec2i other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = 0;
	w = 0;
}
vec4i::vec4i(vec3i other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = (int)other.z;
	w = 0;
}
vec4i::vec4i(vec2d other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = 0;
	w = 0;
}
vec4i::vec4i(vec3d other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = (int)other.z;
	w = 0;
}
vec4i::vec4i(vec4d other)
{
	x = (int)other.x;
	y = (int)other.y;
	z = (int)other.z;
	w = (int)other.w;
}
vec4i::vec4i(vec2d other, int nz, int nw)
{
	x = (int)other.x;
	y = (int)other.y;
	z = nz;
	w = nw;
}
vec4i::vec4i(vec3d other, int nw)
{
	x = (int)other.x;
	y = (int)other.y;
	z = (int)other.z;
	w = nw;
}
void vec4i::operator()(int d)
{
	x = d;
	y = d;
	z = d;
	w = d;
}
void vec4i::operator()(int nx, int ny, int nz, int nw)
{
	x = nx;
	y = ny;
	z = nz;
	w = nw;
}
vec4i vec4i::operator+(const vec4i& o) const
{
	return vec4i(x+o.x, y+o.y, z+o.z, w+o.w);
}
void vec4i::operator+=(const vec4i& o)
{
	x += o.x;
	y += o.y;
	z += o.z;
	w += o.w;
}
vec4i vec4i::operator+(int d) const
{
	return vec4i(x+d, y+d, z+d, w+d);
}
vec4i vec4i::operator+(float d) const
{
	return vec4i(x+d, y+d, z+d, w+d);
}
vec4i vec4i::operator+(double d) const
{
	return vec4i(x+d, y+d, z+d, w+d);
}
void vec4i::operator+=(int d)
{
	x += d;
	y += d;
	z += d;
	w += d;
}
void vec4i::operator+=(float d)
{
	x += d;
	y += d;
	z += d;
	w += d;
}
void vec4i::operator+=(double d)
{
	x += d;
	y += d;
	z += d;
	w += d;
}
vec4i vec4i::operator-(const vec4i& o) const
{
	return vec4i(x-o.x, y-o.y, z-o.z, w-o.w);
}
void vec4i::operator-=(const vec4i& o)
{
	x -= o.x;
	y -= o.y;
	z -= o.z;
	w -= o.w;
}
vec4i vec4i::operator-(int d) const
{
	return vec4i(x-d, y-d, z-d, w-d);
}
vec4i vec4i::operator-(float d) const
{
	return vec4i(x-d, y-d, z-d, w-d);
}
vec4i vec4i::operator-(double d) const
{
	return vec4i(x-d, y-d, z-d, w-d);
}
void vec4i::operator-=(int d)
{
	x -= d;
	y -= d;
	z -= d;
	w -= d;
}
void vec4i::operator-=(float d)
{
	x -= d;
	y -= d;
	z -= d;
	w -= d;
}
void vec4i::operator-=(double d)
{
	x -= d;
	y -= d;
	z -= d;
	w -= d;
}
vec4i vec4i::operator/(const vec4i& o) const
{
	return vec4i(x/o.x, y/o.y, z/o.z, w/o.w);
}
void vec4i::operator/=(const vec4i& o)
{
	x /= o.x;
	y /= o.y;
	z /= o.z;
	w /= o.w;
}
vec4i vec4i::operator/(int d) const
{
	return vec4i(x/d, y/d, z/d, w/d);
}
vec4i vec4i::operator/(float d) const
{
	return vec4i(x/d, y/d, z/d, w/d);
}
vec4i vec4i::operator/(double d) const
{
	return vec4i(x/d, y/d, z/d, w/d);
}
void vec4i::operator/=(int d)
{
	x /= d;
	y /= d;
	z /= d;
	w /= d;
}
void vec4i::operator/=(float d)
{
	x /= d;
	y /= d;
	z /= d;
	w /= d;
}
void vec4i::operator/=(double d)
{
	x /= d;
	y /= d;
	z /= d;
	w /= d;
}
vec4i vec4i::operator*(const vec4i& o) const
{
	return vec4i(x*o.x, y*o.y, z*o.z, w*o.w);
}
void vec4i::operator*=(const vec4i& o)
{
	x *= o.x;
	y *= o.y;
	z *= o.z;
	w *= o.w;
}
vec4i vec4i::operator*(int d) const
{
	return vec4i(x*d, y*d, z*d, w*d);
}
vec4i vec4i::operator*(float d) const
{
	return vec4i(x*d, y*d, z*d, w*d);
}
vec4i vec4i::operator*(double d) const
{
	return vec4i(x*d, y*d, z*d, w*d);
}
void vec4i::operator*=(int d)
{
	x *= d;
	y *= d;
	z *= d;
	w *= d;
}
void vec4i::operator*=(float d)
{
	x *= d;
	y *= d;
	z *= d;
	w *= d;
}
void vec4i::operator*=(double d)
{
	x *= d;
	y *= d;
	z *= d;
	w *= d;
}
bool vec4i::operator==(const vec4i& other) const
{
	return x==other.x && y==other.y && z==other.z && w==other.w;
}
bool vec4i::operator!=(const vec4i& other) const
{
	return x!=other.x || y!=other.y || z!=other.z || w!=other.w;
}
bool vec4i::operator>(const vec4i& other) const
{
	return size() > other.size();
}
bool vec4i::operator<(const vec4i& other) const
{
	return size() < other.size();
}
bool vec4i::operator>=(const vec4i& other) const
{
	return size() >= other.size();
}
bool vec4i::operator<=(const vec4i& other) const
{
	return size() <= other.size();
}
vec4i vec4i::operator%(const vec4i& other) const
{
	return vec4i(x%other.x, y%other.y, z%other.z, w%other.w);
}
vec4i vec4i::operator%(int d) const
{
	return vec4i(x%d, y%d, z%d, w%d);
}
void vec4i::operator%=(const vec4i& other)
{
	x %= other.x;
	y %= other.y;
	z %= other.z;
	w %= other.w;
}
void vec4i::operator%=(int d)
{
	x %= d;
	y %= d;
	z %= d;
	w %= d;
}
int vec4i::distsq(const vec4i& other) const
{
	int dx = other.x - x;
	int dy = other.y - y;
	int dz = other.z - z;
	int dw = other.w - w;
	return dx*dx + dy*dy + dz*dz + dw*dw;
}
int vec4i::dist(const vec4i& other) const
{
	float dx = (float)(other.x - x);
	float dy = (float)(other.y - y);
	float dz = (float)(other.z - z);
	float dw = (float)(other.w - w);
	return (int)sqrt(dx*dx + dy*dy + dz*dz + dw*dw);
}
int vec4i::dot(const vec4i& other) const
{
	return (int)(x*other.x + y*other.y + z*other.z + w*other.w);
}
int vec4i::sizesq() const
{
	return (float)x*x + (float)y*y + (float)z*z + (float)w*w;
}
int vec4i::size() const
{
	return (int)sqrt((float)x*x + (float)y*y + (float)z*z + (float)w*w);
}
int vec4i::cmax() const
{
	int r = x;
	r = r > y ? r : y;
	r = r > z ? r : z;
	r = r > w ? r : w;
	return r;
}
int vec4i::cmin() const
{
	int r = x;
	r = r < y ? r : y;
	r = r < z ? r : z;
	r = r < w ? r : w;
	return r;
}
vec4i vec4i::tolen(int len)
{
	return *this * (len / size());
}
void vec4i::normalize()
{
	*this /= size();
}
vec4i vec4i::unit() const
{
	return *this / size();
}

//swivel operators
vec2i vec4i::xy() const
{
	return vec2i(x, y);
}
vec2i vec4i::xz() const
{
	return vec2i(x, z);
}
vec2i vec4i::xw() const
{
	return vec2i(x, w);
}
vec2i vec4i::yx() const
{
	return vec2i(y, x);
}
vec2i vec4i::yz() const
{
	return vec2i(y, z);
}
vec2i vec4i::yw() const
{
	return vec2i(y, w);
}
vec2i vec4i::zx() const
{
	return vec2i(z, x);
}
vec2i vec4i::zy() const
{
	return vec2i(z, y);
}
vec2i vec4i::zw() const
{
	return vec2i(z, w);
}
vec2i vec4i::wx() const
{
	return vec2i(w, x);
}
vec2i vec4i::wy() const
{
	return vec2i(w, y);
}
vec2i vec4i::wz() const
{
	return vec2i(w, z);
}
vec3i vec4i::xyz() const
{
	return vec3i(x, y, z);
}
vec3i vec4i::xyw() const
{
	return vec3i(x, y, w);
}
vec3i vec4i::xzy() const
{
	return vec3i(x, z, y);
}
vec3i vec4i::xzw() const
{
	return vec3i(x, z, w);
}
vec3i vec4i::xwy() const
{
	return vec3i(x, w, y);
}
vec3i vec4i::xwz() const
{
	return vec3i(x, w, z);
}
vec3i vec4i::yxz() const
{
	return vec3i(y, x, z);
}
vec3i vec4i::yxw() const
{
	return vec3i(y, x, w);
}
vec3i vec4i::yzx() const
{
	return vec3i(y, z, x);
}
vec3i vec4i::yzw() const
{
	return vec3i(y, z, w);
}
vec3i vec4i::ywx() const
{
	return vec3i(y, w, x);
}
vec3i vec4i::ywz() const
{
	return vec3i(y, w, z);
}
vec3i vec4i::zxy() const
{
	return vec3i(z, x, y);
}
vec3i vec4i::zxw() const
{
	return vec3i(z, x, w);
}
vec3i vec4i::zyx() const
{
	return vec3i(z, y, x);
}
vec3i vec4i::zyw() const
{
	return vec3i(z, y, w);
}
vec3i vec4i::zwx() const
{
	return vec3i(z, w, x);
}
vec3i vec4i::zwy() const
{
	return vec3i(z, w, y);
}
vec3i vec4i::wxy() const
{
	return vec3i(w, x, y);
}
vec3i vec4i::wxz() const
{
	return vec3i(w, x, z);
}
vec3i vec4i::wyx() const
{
	return vec3i(w, y, x);
}
vec3i vec4i::wyz() const
{
	return vec3i(w, y, z);
}
vec3i vec4i::wzx() const
{
	return vec3i(w, z, x);
}
vec3i vec4i::wzy() const
{
	return vec3i(w, z, y);
}
vec4i vec4i::xywz() const
{
	return vec4i(x, y, w, z);
}
vec4i vec4i::xzyw() const
{
	return vec4i(x, z, y, w);
}
vec4i vec4i::xzwy() const
{
	return vec4i(x, z, w, y);
}
vec4i vec4i::xwyz() const
{
	return vec4i(x, w, y, z);
}
vec4i vec4i::xwzy() const
{
	return vec4i(x, w, z, y);
}
vec4i vec4i::yxzw() const
{
	return vec4i(y, x, z, w);
}
vec4i vec4i::yxwz() const
{
	return vec4i(y, x, w, z);
}
vec4i vec4i::yzxw() const
{
	return vec4i(y, z, x, w);
}
vec4i vec4i::yzwx() const
{
	return vec4i(y, z, w, x);
}
vec4i vec4i::ywxz() const
{
	return vec4i(y, w, x, z);
}
vec4i vec4i::ywzx() const
{
	return vec4i(y, w, z, x);
}
vec4i vec4i::zxyw() const
{
	return vec4i(z, x, y, w);
}
vec4i vec4i::zxwy() const
{
	return vec4i(z, x, w, y);
}
vec4i vec4i::zyxw() const
{
	return vec4i(z, y, x, w);
}
vec4i vec4i::zywx() const
{
	return vec4i(z, y, w, x);
}
vec4i vec4i::zwxy() const
{
	return vec4i(z, w, x, y);
}
vec4i vec4i::zwyx() const
{
	return vec4i(z, w, y, x);
}
vec4i vec4i::wxyz() const
{
	return vec4i(w, x, y, z);
}
vec4i vec4i::wxzy() const
{
	return vec4i(w, x, z, y);
}
vec4i vec4i::wyxz() const
{
	return vec4i(w, y, x, z);
}
vec4i vec4i::wyzx() const
{
	return vec4i(w, y, z, x);
}
vec4i vec4i::wzxy() const
{
	return vec4i(w, z, x, y);
}
vec4i vec4i::wzyx() const
{
	return vec4i(w, z, y, x);
}

vec4i operator-(vec4i v)
{
	return vec4i(-v.x, -v.y, -v.z, -v.w);
}
vec4i operator/(int n, vec4i v)
{
	return vec4i(n/v.x, n/v.y, n/v.z, n/v.w);
}
vec4i operator/(float n, vec4i v)
{
	return vec4i(n/v.x, n/v.y, n/v.z, n/v.w);
}
vec4i operator/(double n, vec4i v)
{
	return vec4i(n/v.x, n/v.y, n/v.z, n/v.w);
}
vec4i vmin(vec4i a, vec4i b)
{
	vec4i ret;
	ret.y = a.y < b.y ? a.y : b.y;
	ret.z = a.z < b.z ? a.z : b.z;
	ret.w = a.w < b.w ? a.w : b.w;
	return ret;
}
vec4i vmax(vec4i a, vec4i b)
{
	vec4i ret;
	ret.y = a.y > b.y ? a.y : b.y;
	ret.z = a.z > b.z ? a.z : b.z;
	ret.w = a.w > b.w ? a.w : b.w;
	return ret;
}
vec4i vmin(vec4i a, int b)
{
	vec4i ret;
	ret.y = a.y < b ? a.y : b;
	ret.z = a.z < b ? a.z : b;
	ret.w = a.w < b ? a.w : b;
	return ret;
}
vec4i vmax(vec4i a, int b)
{
	vec4i ret;
	ret.y = a.y > b ? a.y : b;
	ret.z = a.z > b ? a.z : b;
	ret.w = a.w > b ? a.w : b;
	return ret;
}
vec4i vmin(int a, vec4i b)
{
	vec4i ret;
	ret.y = a < b.y ? a : b.y;
	ret.z = a < b.z ? a : b.z;
	ret.w = a < b.w ? a : b.w;
	return ret;
}
vec4i vmax(int a, vec4i b)
{
	vec4i ret;
	ret.y = a > b.y ? a : b.y;
	ret.z = a > b.z ? a : b.z;
	ret.w = a > b.w ? a : b.w;
	return ret;
}
vec2d::vec2d()
{
}
vec2d::vec2d(double d)
{
	x = d;
	y = d;
}
vec2d::vec2d(double nx, double ny)
{
	x = nx;
	y = ny;
}
vec2d::vec2d(vec2f other)
{
	x = (double)other.x;
	y = (double)other.y;
}
vec2d::vec2d(vec3f other)
{
	x = (double)other.x;
	y = (double)other.y;
}
vec2d::vec2d(vec4f other)
{
	x = (double)other.x;
	y = (double)other.y;
}
vec2d::vec2d(vec2i other)
{
	x = (double)other.x;
	y = (double)other.y;
}
vec2d::vec2d(vec3i other)
{
	x = (double)other.x;
	y = (double)other.y;
}
vec2d::vec2d(vec4i other)
{
	x = (double)other.x;
	y = (double)other.y;
}
vec2d::vec2d(vec3d other)
{
	x = (double)other.x;
	y = (double)other.y;
}
vec2d::vec2d(vec4d other)
{
	x = (double)other.x;
	y = (double)other.y;
}
void vec2d::operator()(double d)
{
	x = d;
	y = d;
}
void vec2d::operator()(double nx, double ny)
{
	x = nx;
	y = ny;
}
vec2d vec2d::operator+(const vec2d& o) const
{
	return vec2d(x+o.x, y+o.y);
}
void vec2d::operator+=(const vec2d& o)
{
	x += o.x;
	y += o.y;
}
vec2d vec2d::operator+(double d) const
{
	return vec2d(x+d, y+d);
}
void vec2d::operator+=(double d)
{
	x += d;
	y += d;
}
vec2d vec2d::operator-(const vec2d& o) const
{
	return vec2d(x-o.x, y-o.y);
}
void vec2d::operator-=(const vec2d& o)
{
	x -= o.x;
	y -= o.y;
}
vec2d vec2d::operator-(double d) const
{
	return vec2d(x-d, y-d);
}
void vec2d::operator-=(double d)
{
	x -= d;
	y -= d;
}
vec2d vec2d::operator/(const vec2d& o) const
{
	return vec2d(x/o.x, y/o.y);
}
void vec2d::operator/=(const vec2d& o)
{
	x /= o.x;
	y /= o.y;
}
vec2d vec2d::operator/(double d) const
{
	return vec2d(x/d, y/d);
}
void vec2d::operator/=(double d)
{
	x /= d;
	y /= d;
}
vec2d vec2d::operator*(const vec2d& o) const
{
	return vec2d(x*o.x, y*o.y);
}
void vec2d::operator*=(const vec2d& o)
{
	x *= o.x;
	y *= o.y;
}
vec2d vec2d::operator*(double d) const
{
	return vec2d(x*d, y*d);
}
void vec2d::operator*=(double d)
{
	x *= d;
	y *= d;
}
bool vec2d::operator==(const vec2d& other) const
{
	return x==other.x && y==other.y;
}
bool vec2d::operator!=(const vec2d& other) const
{
	return x!=other.x || y!=other.y;
}
bool vec2d::operator>(const vec2d& other) const
{
	return size() > other.size();
}
bool vec2d::operator<(const vec2d& other) const
{
	return size() < other.size();
}
bool vec2d::operator>=(const vec2d& other) const
{
	return size() >= other.size();
}
bool vec2d::operator<=(const vec2d& other) const
{
	return size() <= other.size();
}
vec2d vec2d::operator%(const vec2d& other) const
{
	return vec2d(fmod(x,other.x), fmod(y,other.y));
}
vec2d vec2d::operator%(double d) const
{
	return vec2d(fmod(x,d), fmod(y,d));
}
void vec2d::operator%=(const vec2d& other)
{
	x = fmod(x, other.x);
	y = fmod(y, other.y);
}
void vec2d::operator%=(double d)
{
	x = fmod(x, d);
	y = fmod(y, d);
}
double vec2d::distsq(const vec2d& other) const
{
	double dx = other.x - x;
	double dy = other.y - y;
	return dx*dx + dy*dy;
}
double vec2d::dist(const vec2d& other) const
{
	float dx = (float)(other.x - x);
	float dy = (float)(other.y - y);
	return (double)sqrt(dx*dx + dy*dy);
}
double vec2d::dot(const vec2d& other) const
{
	return (double)(x*other.x + y*other.y);
}
double vec2d::sizesq() const
{
	return (float)x*x + (float)y*y;
}
double vec2d::size() const
{
	return (double)sqrt((float)x*x + (float)y*y);
}
double vec2d::cmax() const
{
	double r = x;
	r = r > y ? r : y;
	return r;
}
double vec2d::cmin() const
{
	double r = x;
	r = r < y ? r : y;
	return r;
}
vec2d vec2d::tolen(double len)
{
	return *this * (len / size());
}
void vec2d::normalize()
{
	*this /= size();
}
vec2d vec2d::unit() const
{
	return *this / size();
}

//swivel operators
vec2d vec2d::yx() const
{
	return vec2d(y, x);
}

vec2d operator-(vec2d v)
{
	return vec2d(-v.x, -v.y);
}
vec2d operator/(double n, vec2d v)
{
	return vec2d(n/v.x, n/v.y);
}
vec2d vmin(vec2d a, vec2d b)
{
	vec2d ret;
	ret.y = a.y < b.y ? a.y : b.y;
	return ret;
}
vec2d vmax(vec2d a, vec2d b)
{
	vec2d ret;
	ret.y = a.y > b.y ? a.y : b.y;
	return ret;
}
vec2d vmin(vec2d a, double b)
{
	vec2d ret;
	ret.y = a.y < b ? a.y : b;
	return ret;
}
vec2d vmax(vec2d a, double b)
{
	vec2d ret;
	ret.y = a.y > b ? a.y : b;
	return ret;
}
vec2d vmin(double a, vec2d b)
{
	vec2d ret;
	ret.y = a < b.y ? a : b.y;
	return ret;
}
vec2d vmax(double a, vec2d b)
{
	vec2d ret;
	ret.y = a > b.y ? a : b.y;
	return ret;
}
vec3d::vec3d()
{
}
vec3d::vec3d(double d)
{
	x = d;
	y = d;
	z = d;
}
vec3d::vec3d(double nx, double ny, double nz)
{
	x = nx;
	y = ny;
	z = nz;
}
vec3d::vec3d(vec2f other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = 0;
}
vec3d::vec3d(vec3f other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = (double)other.z;
}
vec3d::vec3d(vec4f other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = (double)other.z;
}
vec3d::vec3d(vec2i other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = 0;
}
vec3d::vec3d(vec3i other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = (double)other.z;
}
vec3d::vec3d(vec4i other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = (double)other.z;
}
vec3d::vec3d(vec2d other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = 0;
}
vec3d::vec3d(vec4d other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = (double)other.z;
}
vec3d::vec3d(vec2d other, double nz)
{
	x = (double)other.x;
	y = (double)other.y;
	z = nz;
}
void vec3d::operator()(double d)
{
	x = d;
	y = d;
	z = d;
}
void vec3d::operator()(double nx, double ny, double nz)
{
	x = nx;
	y = ny;
	z = nz;
}
vec3d vec3d::operator+(const vec3d& o) const
{
	return vec3d(x+o.x, y+o.y, z+o.z);
}
void vec3d::operator+=(const vec3d& o)
{
	x += o.x;
	y += o.y;
	z += o.z;
}
vec3d vec3d::operator+(double d) const
{
	return vec3d(x+d, y+d, z+d);
}
void vec3d::operator+=(double d)
{
	x += d;
	y += d;
	z += d;
}
vec3d vec3d::operator-(const vec3d& o) const
{
	return vec3d(x-o.x, y-o.y, z-o.z);
}
void vec3d::operator-=(const vec3d& o)
{
	x -= o.x;
	y -= o.y;
	z -= o.z;
}
vec3d vec3d::operator-(double d) const
{
	return vec3d(x-d, y-d, z-d);
}
void vec3d::operator-=(double d)
{
	x -= d;
	y -= d;
	z -= d;
}
vec3d vec3d::operator/(const vec3d& o) const
{
	return vec3d(x/o.x, y/o.y, z/o.z);
}
void vec3d::operator/=(const vec3d& o)
{
	x /= o.x;
	y /= o.y;
	z /= o.z;
}
vec3d vec3d::operator/(double d) const
{
	return vec3d(x/d, y/d, z/d);
}
void vec3d::operator/=(double d)
{
	x /= d;
	y /= d;
	z /= d;
}
vec3d vec3d::operator*(const vec3d& o) const
{
	return vec3d(x*o.x, y*o.y, z*o.z);
}
void vec3d::operator*=(const vec3d& o)
{
	x *= o.x;
	y *= o.y;
	z *= o.z;
}
vec3d vec3d::operator*(double d) const
{
	return vec3d(x*d, y*d, z*d);
}
void vec3d::operator*=(double d)
{
	x *= d;
	y *= d;
	z *= d;
}
bool vec3d::operator==(const vec3d& other) const
{
	return x==other.x && y==other.y && z==other.z;
}
bool vec3d::operator!=(const vec3d& other) const
{
	return x!=other.x || y!=other.y || z!=other.z;
}
bool vec3d::operator>(const vec3d& other) const
{
	return size() > other.size();
}
bool vec3d::operator<(const vec3d& other) const
{
	return size() < other.size();
}
bool vec3d::operator>=(const vec3d& other) const
{
	return size() >= other.size();
}
bool vec3d::operator<=(const vec3d& other) const
{
	return size() <= other.size();
}
vec3d vec3d::operator%(const vec3d& other) const
{
	return vec3d(fmod(x,other.x), fmod(y,other.y), fmod(z,other.z));
}
vec3d vec3d::operator%(double d) const
{
	return vec3d(fmod(x,d), fmod(y,d), fmod(z,d));
}
void vec3d::operator%=(const vec3d& other)
{
	x = fmod(x, other.x);
	y = fmod(y, other.y);
	z = fmod(z, other.z);
}
void vec3d::operator%=(double d)
{
	x = fmod(x, d);
	y = fmod(y, d);
	z = fmod(z, d);
}
double vec3d::distsq(const vec3d& other) const
{
	double dx = other.x - x;
	double dy = other.y - y;
	double dz = other.z - z;
	return dx*dx + dy*dy + dz*dz;
}
double vec3d::dist(const vec3d& other) const
{
	float dx = (float)(other.x - x);
	float dy = (float)(other.y - y);
	float dz = (float)(other.z - z);
	return (double)sqrt(dx*dx + dy*dy + dz*dz);
}
double vec3d::dot(const vec3d& other) const
{
	return (double)(x*other.x + y*other.y + z*other.z);
}
double vec3d::sizesq() const
{
	return (float)x*x + (float)y*y + (float)z*z;
}
double vec3d::size() const
{
	return (double)sqrt((float)x*x + (float)y*y + (float)z*z);
}
double vec3d::cmax() const
{
	double r = x;
	r = r > y ? r : y;
	r = r > z ? r : z;
	return r;
}
double vec3d::cmin() const
{
	double r = x;
	r = r < y ? r : y;
	r = r < z ? r : z;
	return r;
}
vec3d vec3d::tolen(double len)
{
	return *this * (len / size());
}
void vec3d::normalize()
{
	*this /= size();
}
vec3d vec3d::unit() const
{
	return *this / size();
}
vec3d vec3d::cross(const vec3d& other) const
{
	double a = y * other.z - z * other.y;
	double b = x * other.z - z * other.x;
	double c = x * other.y - y * other.x;
	return vec3d(a, -b, c);
}

//swivel operators
vec2d vec3d::xy() const
{
	return vec2d(x, y);
}
vec2d vec3d::xz() const
{
	return vec2d(x, z);
}
vec2d vec3d::yx() const
{
	return vec2d(y, x);
}
vec2d vec3d::yz() const
{
	return vec2d(y, z);
}
vec2d vec3d::zx() const
{
	return vec2d(z, x);
}
vec2d vec3d::zy() const
{
	return vec2d(z, y);
}
vec3d vec3d::xzy() const
{
	return vec3d(x, z, y);
}
vec3d vec3d::yxz() const
{
	return vec3d(y, x, z);
}
vec3d vec3d::yzx() const
{
	return vec3d(y, z, x);
}
vec3d vec3d::zxy() const
{
	return vec3d(z, x, y);
}
vec3d vec3d::zyx() const
{
	return vec3d(z, y, x);
}

vec3d operator-(vec3d v)
{
	return vec3d(-v.x, -v.y, -v.z);
}
vec3d operator/(double n, vec3d v)
{
	return vec3d(n/v.x, n/v.y, n/v.z);
}
vec3d vmin(vec3d a, vec3d b)
{
	vec3d ret;
	ret.y = a.y < b.y ? a.y : b.y;
	ret.z = a.z < b.z ? a.z : b.z;
	return ret;
}
vec3d vmax(vec3d a, vec3d b)
{
	vec3d ret;
	ret.y = a.y > b.y ? a.y : b.y;
	ret.z = a.z > b.z ? a.z : b.z;
	return ret;
}
vec3d vmin(vec3d a, double b)
{
	vec3d ret;
	ret.y = a.y < b ? a.y : b;
	ret.z = a.z < b ? a.z : b;
	return ret;
}
vec3d vmax(vec3d a, double b)
{
	vec3d ret;
	ret.y = a.y > b ? a.y : b;
	ret.z = a.z > b ? a.z : b;
	return ret;
}
vec3d vmin(double a, vec3d b)
{
	vec3d ret;
	ret.y = a < b.y ? a : b.y;
	ret.z = a < b.z ? a : b.z;
	return ret;
}
vec3d vmax(double a, vec3d b)
{
	vec3d ret;
	ret.y = a > b.y ? a : b.y;
	ret.z = a > b.z ? a : b.z;
	return ret;
}
vec4d::vec4d()
{
}
vec4d::vec4d(double d)
{
	x = d;
	y = d;
	z = d;
	w = d;
}
vec4d::vec4d(double nx, double ny, double nz, double nw)
{
	x = nx;
	y = ny;
	z = nz;
	w = nw;
}
vec4d::vec4d(vec2f other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = 0;
	w = 0;
}
vec4d::vec4d(vec3f other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = (double)other.z;
	w = 0;
}
vec4d::vec4d(vec4f other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = (double)other.z;
	w = (double)other.w;
}
vec4d::vec4d(vec2i other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = 0;
	w = 0;
}
vec4d::vec4d(vec3i other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = (double)other.z;
	w = 0;
}
vec4d::vec4d(vec4i other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = (double)other.z;
	w = (double)other.w;
}
vec4d::vec4d(vec2d other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = 0;
	w = 0;
}
vec4d::vec4d(vec3d other)
{
	x = (double)other.x;
	y = (double)other.y;
	z = (double)other.z;
	w = 0;
}
vec4d::vec4d(vec2d other, double nz, double nw)
{
	x = (double)other.x;
	y = (double)other.y;
	z = nz;
	w = nw;
}
vec4d::vec4d(vec3d other, double nw)
{
	x = (double)other.x;
	y = (double)other.y;
	z = (double)other.z;
	w = nw;
}
void vec4d::operator()(double d)
{
	x = d;
	y = d;
	z = d;
	w = d;
}
void vec4d::operator()(double nx, double ny, double nz, double nw)
{
	x = nx;
	y = ny;
	z = nz;
	w = nw;
}
vec4d vec4d::operator+(const vec4d& o) const
{
	return vec4d(x+o.x, y+o.y, z+o.z, w+o.w);
}
void vec4d::operator+=(const vec4d& o)
{
	x += o.x;
	y += o.y;
	z += o.z;
	w += o.w;
}
vec4d vec4d::operator+(double d) const
{
	return vec4d(x+d, y+d, z+d, w+d);
}
void vec4d::operator+=(double d)
{
	x += d;
	y += d;
	z += d;
	w += d;
}
vec4d vec4d::operator-(const vec4d& o) const
{
	return vec4d(x-o.x, y-o.y, z-o.z, w-o.w);
}
void vec4d::operator-=(const vec4d& o)
{
	x -= o.x;
	y -= o.y;
	z -= o.z;
	w -= o.w;
}
vec4d vec4d::operator-(double d) const
{
	return vec4d(x-d, y-d, z-d, w-d);
}
void vec4d::operator-=(double d)
{
	x -= d;
	y -= d;
	z -= d;
	w -= d;
}
vec4d vec4d::operator/(const vec4d& o) const
{
	return vec4d(x/o.x, y/o.y, z/o.z, w/o.w);
}
void vec4d::operator/=(const vec4d& o)
{
	x /= o.x;
	y /= o.y;
	z /= o.z;
	w /= o.w;
}
vec4d vec4d::operator/(double d) const
{
	return vec4d(x/d, y/d, z/d, w/d);
}
void vec4d::operator/=(double d)
{
	x /= d;
	y /= d;
	z /= d;
	w /= d;
}
vec4d vec4d::operator*(const vec4d& o) const
{
	return vec4d(x*o.x, y*o.y, z*o.z, w*o.w);
}
void vec4d::operator*=(const vec4d& o)
{
	x *= o.x;
	y *= o.y;
	z *= o.z;
	w *= o.w;
}
vec4d vec4d::operator*(double d) const
{
	return vec4d(x*d, y*d, z*d, w*d);
}
void vec4d::operator*=(double d)
{
	x *= d;
	y *= d;
	z *= d;
	w *= d;
}
bool vec4d::operator==(const vec4d& other) const
{
	return x==other.x && y==other.y && z==other.z && w==other.w;
}
bool vec4d::operator!=(const vec4d& other) const
{
	return x!=other.x || y!=other.y || z!=other.z || w!=other.w;
}
bool vec4d::operator>(const vec4d& other) const
{
	return size() > other.size();
}
bool vec4d::operator<(const vec4d& other) const
{
	return size() < other.size();
}
bool vec4d::operator>=(const vec4d& other) const
{
	return size() >= other.size();
}
bool vec4d::operator<=(const vec4d& other) const
{
	return size() <= other.size();
}
vec4d vec4d::operator%(const vec4d& other) const
{
	return vec4d(fmod(x,other.x), fmod(y,other.y), fmod(z,other.z), fmod(w,other.w));
}
vec4d vec4d::operator%(double d) const
{
	return vec4d(fmod(x,d), fmod(y,d), fmod(z,d), fmod(w,d));
}
void vec4d::operator%=(const vec4d& other)
{
	x = fmod(x, other.x);
	y = fmod(y, other.y);
	z = fmod(z, other.z);
	w = fmod(w, other.w);
}
void vec4d::operator%=(double d)
{
	x = fmod(x, d);
	y = fmod(y, d);
	z = fmod(z, d);
	w = fmod(w, d);
}
double vec4d::distsq(const vec4d& other) const
{
	double dx = other.x - x;
	double dy = other.y - y;
	double dz = other.z - z;
	double dw = other.w - w;
	return dx*dx + dy*dy + dz*dz + dw*dw;
}
double vec4d::dist(const vec4d& other) const
{
	float dx = (float)(other.x - x);
	float dy = (float)(other.y - y);
	float dz = (float)(other.z - z);
	float dw = (float)(other.w - w);
	return (double)sqrt(dx*dx + dy*dy + dz*dz + dw*dw);
}
double vec4d::dot(const vec4d& other) const
{
	return (double)(x*other.x + y*other.y + z*other.z + w*other.w);
}
double vec4d::sizesq() const
{
	return (float)x*x + (float)y*y + (float)z*z + (float)w*w;
}
double vec4d::size() const
{
	return (double)sqrt((float)x*x + (float)y*y + (float)z*z + (float)w*w);
}
double vec4d::cmax() const
{
	double r = x;
	r = r > y ? r : y;
	r = r > z ? r : z;
	r = r > w ? r : w;
	return r;
}
double vec4d::cmin() const
{
	double r = x;
	r = r < y ? r : y;
	r = r < z ? r : z;
	r = r < w ? r : w;
	return r;
}
vec4d vec4d::tolen(double len)
{
	return *this * (len / size());
}
void vec4d::normalize()
{
	*this /= size();
}
vec4d vec4d::unit() const
{
	return *this / size();
}

//swivel operators
vec2d vec4d::xy() const
{
	return vec2d(x, y);
}
vec2d vec4d::xz() const
{
	return vec2d(x, z);
}
vec2d vec4d::xw() const
{
	return vec2d(x, w);
}
vec2d vec4d::yx() const
{
	return vec2d(y, x);
}
vec2d vec4d::yz() const
{
	return vec2d(y, z);
}
vec2d vec4d::yw() const
{
	return vec2d(y, w);
}
vec2d vec4d::zx() const
{
	return vec2d(z, x);
}
vec2d vec4d::zy() const
{
	return vec2d(z, y);
}
vec2d vec4d::zw() const
{
	return vec2d(z, w);
}
vec2d vec4d::wx() const
{
	return vec2d(w, x);
}
vec2d vec4d::wy() const
{
	return vec2d(w, y);
}
vec2d vec4d::wz() const
{
	return vec2d(w, z);
}
vec3d vec4d::xyz() const
{
	return vec3d(x, y, z);
}
vec3d vec4d::xyw() const
{
	return vec3d(x, y, w);
}
vec3d vec4d::xzy() const
{
	return vec3d(x, z, y);
}
vec3d vec4d::xzw() const
{
	return vec3d(x, z, w);
}
vec3d vec4d::xwy() const
{
	return vec3d(x, w, y);
}
vec3d vec4d::xwz() const
{
	return vec3d(x, w, z);
}
vec3d vec4d::yxz() const
{
	return vec3d(y, x, z);
}
vec3d vec4d::yxw() const
{
	return vec3d(y, x, w);
}
vec3d vec4d::yzx() const
{
	return vec3d(y, z, x);
}
vec3d vec4d::yzw() const
{
	return vec3d(y, z, w);
}
vec3d vec4d::ywx() const
{
	return vec3d(y, w, x);
}
vec3d vec4d::ywz() const
{
	return vec3d(y, w, z);
}
vec3d vec4d::zxy() const
{
	return vec3d(z, x, y);
}
vec3d vec4d::zxw() const
{
	return vec3d(z, x, w);
}
vec3d vec4d::zyx() const
{
	return vec3d(z, y, x);
}
vec3d vec4d::zyw() const
{
	return vec3d(z, y, w);
}
vec3d vec4d::zwx() const
{
	return vec3d(z, w, x);
}
vec3d vec4d::zwy() const
{
	return vec3d(z, w, y);
}
vec3d vec4d::wxy() const
{
	return vec3d(w, x, y);
}
vec3d vec4d::wxz() const
{
	return vec3d(w, x, z);
}
vec3d vec4d::wyx() const
{
	return vec3d(w, y, x);
}
vec3d vec4d::wyz() const
{
	return vec3d(w, y, z);
}
vec3d vec4d::wzx() const
{
	return vec3d(w, z, x);
}
vec3d vec4d::wzy() const
{
	return vec3d(w, z, y);
}
vec4d vec4d::xywz() const
{
	return vec4d(x, y, w, z);
}
vec4d vec4d::xzyw() const
{
	return vec4d(x, z, y, w);
}
vec4d vec4d::xzwy() const
{
	return vec4d(x, z, w, y);
}
vec4d vec4d::xwyz() const
{
	return vec4d(x, w, y, z);
}
vec4d vec4d::xwzy() const
{
	return vec4d(x, w, z, y);
}
vec4d vec4d::yxzw() const
{
	return vec4d(y, x, z, w);
}
vec4d vec4d::yxwz() const
{
	return vec4d(y, x, w, z);
}
vec4d vec4d::yzxw() const
{
	return vec4d(y, z, x, w);
}
vec4d vec4d::yzwx() const
{
	return vec4d(y, z, w, x);
}
vec4d vec4d::ywxz() const
{
	return vec4d(y, w, x, z);
}
vec4d vec4d::ywzx() const
{
	return vec4d(y, w, z, x);
}
vec4d vec4d::zxyw() const
{
	return vec4d(z, x, y, w);
}
vec4d vec4d::zxwy() const
{
	return vec4d(z, x, w, y);
}
vec4d vec4d::zyxw() const
{
	return vec4d(z, y, x, w);
}
vec4d vec4d::zywx() const
{
	return vec4d(z, y, w, x);
}
vec4d vec4d::zwxy() const
{
	return vec4d(z, w, x, y);
}
vec4d vec4d::zwyx() const
{
	return vec4d(z, w, y, x);
}
vec4d vec4d::wxyz() const
{
	return vec4d(w, x, y, z);
}
vec4d vec4d::wxzy() const
{
	return vec4d(w, x, z, y);
}
vec4d vec4d::wyxz() const
{
	return vec4d(w, y, x, z);
}
vec4d vec4d::wyzx() const
{
	return vec4d(w, y, z, x);
}
vec4d vec4d::wzxy() const
{
	return vec4d(w, z, x, y);
}
vec4d vec4d::wzyx() const
{
	return vec4d(w, z, y, x);
}

vec4d operator-(vec4d v)
{
	return vec4d(-v.x, -v.y, -v.z, -v.w);
}
vec4d operator/(double n, vec4d v)
{
	return vec4d(n/v.x, n/v.y, n/v.z, n/v.w);
}
vec4d vmin(vec4d a, vec4d b)
{
	vec4d ret;
	ret.y = a.y < b.y ? a.y : b.y;
	ret.z = a.z < b.z ? a.z : b.z;
	ret.w = a.w < b.w ? a.w : b.w;
	return ret;
}
vec4d vmax(vec4d a, vec4d b)
{
	vec4d ret;
	ret.y = a.y > b.y ? a.y : b.y;
	ret.z = a.z > b.z ? a.z : b.z;
	ret.w = a.w > b.w ? a.w : b.w;
	return ret;
}
vec4d vmin(vec4d a, double b)
{
	vec4d ret;
	ret.y = a.y < b ? a.y : b;
	ret.z = a.z < b ? a.z : b;
	ret.w = a.w < b ? a.w : b;
	return ret;
}
vec4d vmax(vec4d a, double b)
{
	vec4d ret;
	ret.y = a.y > b ? a.y : b;
	ret.z = a.z > b ? a.z : b;
	ret.w = a.w > b ? a.w : b;
	return ret;
}
vec4d vmin(double a, vec4d b)
{
	vec4d ret;
	ret.y = a < b.y ? a : b.y;
	ret.z = a < b.z ? a : b.z;
	ret.w = a < b.w ? a : b.w;
	return ret;
}
vec4d vmax(double a, vec4d b)
{
	vec4d ret;
	ret.y = a > b.y ? a : b.y;
	ret.z = a > b.z ? a : b.z;
	ret.w = a > b.w ? a : b.w;
	return ret;
}

