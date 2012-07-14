
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

#ifndef VECTOR2_H
#define VECTOR2_H

#include <iostream>

#include "MathLibFunc.h"


/**
 *	A 2-components vector class 
 *
 * @author Hillaire Sébastien
 */
class Vector2
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
	 *	constuctor.
	 */
	inline Vector2(void)
	{
		x = y = 0.0f;
	}
	/**
	 *	constructor with paramters.
	 *
	 * @param x : x coordinate
	 * @param y : y coordinate
	 */
	inline Vector2(const float x, const float y)
	{
		this->x = x;
		this->y = y;
	}

	/**
	 *	use to set parameters.
	 *
	 * @param x : x coordinate
	 * @param y : y coordinate
	 */
	inline void set(const float x, const float y)
	{
		this->x = x;
		this->y = y;
	}
	/**
	 *	set the null vector
	 */
	inline void zero(void)
	{
		x = y = 0.0f;
	}

	inline float operator[](int i) const
	{
#ifdef _DEBUG
		if( ( i < 0 ) && ( i >= 2 ) ) std::cout<<"const float Vector2::operator[](int i)const : i invalide : "<<i<<"."<<std::endl;
#endif
		return (&x)[i];
	}
	inline float & operator[](int i)
	{
#ifdef _DEBUG
		if( ( i < 0 ) && ( i >= 2 ) ) std::cout<<"const float Vector2::operator[](int i)const : i invalide : "<<i<<"."<<std::endl;
#endif
		return (&x)[i];
	}


	inline Vector2 operator-() const
	{
		return Vector2( -x, -y );
	}


	inline Vector2 operator+(const Vector2& v) const
	{
		return Vector2( x+v.x, y+v.y );
	}
	inline Vector2 operator-(const Vector2& v) const
	{
		return Vector2( x-v.x, y-v.y );
	}
	inline Vector2 operator*(const Vector2& v) const
	{
		return Vector2( x*v.x , y*v.y );
	}
	inline Vector2 operator/(const Vector2& v) const
	{
		return Vector2( x/v.x , y/v.y );
	}


	inline Vector2 operator*(const float v) const
	{
		return Vector2( x*v, y*v );
	}


	inline Vector2& operator+=(const Vector2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}
	inline Vector2& operator-=(const Vector2& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}
	inline Vector2& operator*=(const Vector2& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}
	inline Vector2& operator/=(const Vector2& v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}


	inline Vector2& operator+=(const float v) 
	{
		x += v;
		y += v;
		return *this;
	} 
	inline Vector2& operator-=(const float v) 
	{
		x -= v;
		y -= v;
		return *this;
	} 
	inline Vector2& operator*=(const float v) 
	{
		x *= v;
		y *= v;
		return *this;
	} 
	inline Vector2& operator/=(const float v)
	{
		float inv = 1.0f / v;
		x *= inv;
		y *= inv;
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


	inline bool operator==(const Vector2& v) const
	{
		return equals( v );
	}
	inline bool operator!=(const Vector2& v) const 
	{
		if( x!=v.x || y!=v.y ) return false;
		return true;
	}


	/**
	 *	Comparison fonction
	 *
	 * @param v : the vector to compare with this this
	 * @return true if both vectors are equal, els false.
	 */
	inline bool equals(const Vector2& v) const
	{
		return ( (x==v.x) && (y==v.y) );
	}

	/**
	 *	Set all vector component to its aboslute value
	 */
	inline void setPositiveValue()
	{
		x = ABS(x);
		y = ABS(y);
	}

	/**
	 *	return the length of the vector
	 *
	 * @return the length
	 */
	inline float length(void) const 
	{
		return sqrtf( x*x + y*y );
	}
	/**
	 *	return the squared length of the vector
	 *
	 * @return the squared length
	 */
	inline float lengthSquared(void) const
	{
		return ( x*x + y*y );
	}
	/**
	 *	normalize this vector
	 */
	inline void normalize(void)
	{
		float invLen = 1.0f / this->length();
		x *= invLen;
		y *= invLen;
	}

	/**
	 *	compute a dot product
	 *
	 * @param v : the other vector used to compute the dot product
	 *
	 * @return the dot product result
	 */
	inline float dot(const Vector2& v) const
	{
		return ( x*v.x + y*v.y );
	}

	/**
	 *	Linear interpolation. The result is stored in this vector.
	 *
	 * @param v1 : start quaternion
	 * @param v2 : end quaternion
	 * @param l : interpolation coefficient
	 */
	inline void lerp(const Vector2& v1, const Vector2& v2, const float l)
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
	 *	Get the non oriented angle between this vector and X axis.
	 *
	 * @return the angle in degree
	 */
	inline float getAngleWithX()
	{
		float len = this->length();
		if (len==0.0f)
		{
			//pour éviter la division par 0
			return 0.0f;
		}
		if (y<0.0f)
		{
			return -acosf(x/len)*(180.0f/PI);
		}else
		{
			return acosf(x/len)*(180.0f/PI);
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
	inline float getAngle(Vector2& v)
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


extern const Vector2 vec2_origin;
extern const Vector2 vec2_Xaxis;
extern const Vector2 vec2_Yaxis;


#endif
