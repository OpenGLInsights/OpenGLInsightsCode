
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

#ifndef VECTOR3_H
#define VECTOR3_H

#include <iostream>

#include "MathLibFunc.h"


/**
 *	A 3-components vector class 
 *
 * @author Hillaire Sébastien
 */
class Vector3
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
	 *	constuctor.
	 */
	inline Vector3(void)
	{
		x = y = z = 0.0f;
	}
	/**
	 *	constructor with paramters.
	 *
	 * @param x : x coordinate
	 * @param y : y coordinate
	 * @param z : z coordinate
	 */
	inline Vector3(const float x, const float y, const float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	/**
	 *	use to set parameters.
	 *
	 * @param x : x coordinate
	 * @param y : y coordinate
	 * @param z : z coordinate
	 */
	inline void set(const float x, const float y, const float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	/**
	 *	set the null vector
	 */
	inline void zero(void)
	{
		x = y = z = 0.0f;
	}

	inline float operator[](int i) const
	{
#ifdef _DEBUG
		if( ( i < 0 ) && ( i >= 3 ) ) std::cout<<"const float Vector3::operator[](int i)const : i invalide : "<<i<<"."<<std::endl;
#endif
		return (&x)[i];
	}
	inline float & operator[](int i)
	{
#ifdef _DEBUG
		if( ( i < 0 ) && ( i >= 3 ) ) std::cout<<"const float Vector3::operator[](int i)const : i invalide : "<<i<<"."<<std::endl;
#endif
		return (&x)[i];
	}


	inline Vector3 operator-() const
	{
		return Vector3( -x, -y , -z );
	}


	inline Vector3 operator+(const Vector3& v) const
	{
		return Vector3( x+v.x, y+v.y, z+v.z );
	}
	inline Vector3 operator-(const Vector3& v) const
	{
		return Vector3( x-v.x, y-v.y, z-v.z );
	}
	inline Vector3 operator*(const Vector3& v) const
	{
		return Vector3( x*v.x , y*v.y , z*v.z );
	}
	inline Vector3 operator/(const Vector3& v) const
	{
		return Vector3( x/v.x , y/v.y , z/v.z );
	}


	inline Vector3 operator*(const float v) const
	{
		return Vector3( x*v, y*v, z*v );
	}


	inline Vector3& operator+=(const Vector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	inline Vector3& operator-=(const Vector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	inline Vector3& operator*=(const Vector3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	inline Vector3& operator/=(const Vector3& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}


	inline Vector3& operator+=(const float v) 
	{
		x += v;
		y += v;
		z += v;
		return *this;
	} 
	inline Vector3& operator-=(const float v) 
	{
		x -= v;
		y -= v;
		z -= v;
		return *this;
	} 
	inline Vector3& operator*=(const float v) 
	{
		x *= v;
		y *= v;
		z *= v;
		return *this;
	} 
	inline Vector3& operator/=(const float v)
	{
		float inv = 1.0f / v;
		x *= inv;
		y *= inv;
		z *= inv;
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


	inline bool operator==(const Vector3& v) const
	{
		return equals( v );
	}
	inline bool operator!=(const Vector3& v) const 
	{
		if( x!=v.x || y!=v.y || z!=v.z ) return false;
		return true;
	}


	/**
	 *	Comparison fonction
	 *
	 * @param v : the vector to compare with this this
	 * @return true if both vectors are equal, els false.
	 */
	inline bool equals(const Vector3& v) const
	{
		return ( (x==v.x) && (y==v.y) && (z==v.z) );
	}

	/**
	 *	Set all vector component to its aboslute value
	 */
	inline void setPositiveValue()
	{
		x = ABS(x);
		y = ABS(y);
		z = ABS(z);
	}

	/**
	 *	return the length of the vector
	 *
	 * @return the length
	 */
	inline float length(void) const 
	{
		return sqrtf( x*x + y*y + z*z );
	}
	/**
	 *	return the squared length of the vector
	 *
	 * @return the squared length
	 */
	inline float lengthSquared(void) const
	{
		return ( x*x + y*y + z*z );
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
	}

	/**
	 *	compute a dot product
	 *
	 * @param v : the other vector used to compute the dot product
	 *
	 * @return the dot product result
	 */
	inline float dot(const Vector3& v) const
	{
		return ( x*v.x + y*v.y + z*v.z );
	}

	/**
	 *	Store a cross product in this vector.
	 *
	 * @param v : the first vector of the cross product
	 * @param w : the last vector of the cross product
	 */
	inline void cross( const Vector3& v, const Vector3& w )
	{
		x = v.y*w.z - v.z*w.y;
		y = v.z*w.x - v.x*w.z;
		z = v.x*w.y - v.y*w.x;
	}

	/**
	 *	Linear interpolation. The result is stored in this vector.
	 *
	 * @param v1 : start quaternion
	 * @param v2 : end quaternion
	 * @param l : interpolation coefficient
	 */
	inline void lerp(const Vector3& v1, const Vector3& v2, const float l)
	{
		if ( l <= 0.0f )
		{
			// l<=0 donc on positionne la borne inférieur qui est v1
			(*this) = v1;
		} else if ( l >= 1.0f )
		{
			//l>=1 donc on positionne la borne supérieur qui est v2
			(*this) = v2;
		} else
		{
			//on interpole
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
	inline float getAngle(Vector3& v)
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


extern const Vector3 vec3_origin;
extern const Vector3 vec3_Xaxis;
extern const Vector3 vec3_Yaxis;
extern const Vector3 vec3_Zaxis;


#endif
