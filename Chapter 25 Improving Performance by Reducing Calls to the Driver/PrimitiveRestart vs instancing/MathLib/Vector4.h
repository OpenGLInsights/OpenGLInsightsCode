
/**********************************************************************\
* AUTHOR : HILLAIRE Sébastien
*
* MAIL   : hillaire_sebastien@yahoo.fr
* SITE   : sebastien.hillaire.free.fr
*
*	You are free to totally or partially use this file/code.
* If you do, please credit me in your software or demo and leave this
* note.
*	Share your work and your ideas as much as possible!
\*********************************************************************/

#ifndef VECTOR4_H
#define VECTOR4_H

#include <iostream>

#include "MathLibFunc.h"


/**
 *	A 4-components vector class 
 *
 * @author Hillaire Sébastien
 */
class Vector4
{

private:

protected:

public:

	/**
	 *	x coordinate
	 */
	float x;
	/**
	 *	y coordinate
	 */
	float y;
	/**
	 *	z coordinate
	 */
	float z;
	/**
	 *	w coordinate
	 */
	float w;

	/**
	 *	constuctor.
	 */
	inline Vector4(void)
	{
		x = y = z = w = 0.0f;
	}
	/**
	 *	constructor with paramters.
	 *
	 * @param x : x coordinate
	 * @param y : y coordinate
	 * @param z : z coordinate
	 * @param w : w coordinate
	 */
	inline Vector4(const float x, const float y, const float z, const float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	/**
	 *	use to set parameters.
	 *
	 * @param x : x coordinate
	 * @param y : y coordinate
	 * @param z : z coordinate
	 * @param w : w coordinate
	 */
	inline void set(const float x, const float y, const float z, const float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
	/**
	 *	set the null vector
	 */
	inline void zero(void)
	{
		x = y = z = w = 0.0f;
	}

	inline float operator[](int i) const
	{
#ifdef _DEBUG
		if( ( i < 0 ) && ( i >= 4 ) ) std::cout<<"const float Vector4::operator[](int i)const : i invalide : "<<i<<"."<<std::endl;
#endif
		return (&x)[i];
	}
	inline float & operator[](int i)
	{
#ifdef _DEBUG
		if( ( i < 0 ) && ( i >= 4 ) ) std::cout<<"const float Vector4::operator[](int i)const : i invalide : "<<i<<"."<<std::endl;
#endif
		return (&x)[i];
	}


	inline Vector4 operator-() const
	{
		return Vector4( -x, -y , -z , -w );
	}


	inline Vector4 operator+(const Vector4& v) const
	{
		return Vector4( x+v.x, y+v.y, z+v.z, w+v.w );
	}
	inline Vector4 operator-(const Vector4& v) const
	{
		return Vector4( x-v.x, y-v.y, z-v.z, w-v.w );
	}
	inline Vector4 operator*(const Vector4& v) const
	{
		return Vector4( x*v.x , y*v.y, z*v.z, w*v.w );
	}
	inline Vector4 operator/(const Vector4& v) const
	{
		return Vector4( x/v.x , y/v.y, z/v.z, w/v.w );
	}


	inline Vector4 operator*(const float v) const
	{
		return Vector4( x*v, y*v, z*v, w*v );
	}

	inline Vector4& operator+=(const Vector4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}
	inline Vector4& operator-=(const Vector4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}
	inline Vector4& operator*=(const Vector4& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}
	inline Vector4& operator/=(const Vector4& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
	}


	inline Vector4& operator+=(const float v) 
	{
		x += v;
		y += v;
		z += v;
		w += v;
		return *this;
	} 
	inline Vector4& operator-=(const float v) 
	{
		x -= v;
		y -= v;
		z -= v;
		z -= v;
		return *this;
	} 
	inline Vector4& operator*=(const float v) 
	{
		x *= v;
		y *= v;
		z *= v;
		w *= v;
		return *this;
	} 
	inline Vector4& operator/=(const float v)
	{
		float inv = 1.0f / v;
		x *= inv;
		y *= inv;
		z *= inv;
		w *= inv;
		return *this;
	}


	inline const float* toFloatPtr(void) const
	{
		return &x;
	}
	inline float* toFloatPtr(void)
	{
		return &x;
	}


	inline bool operator==(const Vector4& v) const
	{
		return equals( v );
	}
	inline bool operator!=(const Vector4& v) const 
	{
		if( x!=v.x || y!=v.y || z!=v.z || w!=v.w ) return false;
		return true;
	}


	/**
	 *	Comparison fonction
	 *
	 * @param v : the vector to compare with this this
	 * @return true if both vectors are equal, els false.
	 */
	inline bool equals(const Vector4& v) const
	{
		return ( (x==v.x) && (y==v.y) && (z==v.z) && (w==v.w) );
	}

	/**
	 *	Set all vector component to its aboslute value
	 */
	inline void setPositiveValue()
	{
		x = ABS(x);
		y = ABS(y);
		z = ABS(z);
		w = ABS(w);
	}

	/**
	 *	return the length of the vector
	 *
	 * @return the length
	 */
	inline float length(void) const 
	{
		return sqrtf( x*x + y*y + z*z + w*w );
	}
	/**
	 *	return the squared length of the vector
	 *
	 * @return the squared length
	 */
	inline float lengthSquared(void) const
	{
		return ( x*x + y*y + z*z + w*w );
	}
	/**
	 *	normalize this vector
	 */
	inline void normalize(void)
	{
		float invLen = 1.0f / this->length();
		x *= invLen;
		y *= invLen;
		z *= invLen;
		w *= invLen;
	}

	/**
	 *	compute a dot product
	 *
	 * @param v : the other vector used to compute the dot product
	 *
	 * @return the dot product result
	 */
	inline float dot(const Vector4& v) const
	{
		return ( x*v.x + y*v.y + z*v.z + w*v.w );
	}

	/**
	 *	Linear interpolation. The result is stored in this vector.
	 *
	 * @param v1 : start quaternion
	 * @param v2 : end quaternion
	 * @param l : interpolation coefficient
	 */
	inline void lerp(const Vector4& v1, const Vector4& v2, const float l)
	{
		if ( l <= 0.0f )
		{
			(*this) = v1;
		} else if ( l >= 1.0f )
		{
			(*this) = v2;
		} else
		{
			(*this) = v1 + (v2 - v1)*l;
		}
	}

	/**
	 *	Get the non oriented angle between this vector and an other vector.
	 *
	 * @pre : v is normalized
	 * @pre : this is normalized
	 *
	 * @param v : the other vector
	 *
	 * @return the angle in degree
	 */
	inline float getAngle(Vector4& v)
	{
		float dot = this->dot(v);
		if (y<0.0f)
		{
			return -acosf(dot)*(180.0f/PI);
		}else
		{
			return acosf(dot)*(180.0f/PI);
		}
	}
};


extern const Vector4 vec4_origin1;
extern const Vector4 vec4_origin0;
extern const Vector4 vec4_Xaxis1;
extern const Vector4 vec4_Yaxis1;
extern const Vector4 vec4_Zaxis1;
extern const Vector4 vec4_Xaxis0;
extern const Vector4 vec4_Yaxis0;
extern const Vector4 vec4_Zaxis0;

#endif
