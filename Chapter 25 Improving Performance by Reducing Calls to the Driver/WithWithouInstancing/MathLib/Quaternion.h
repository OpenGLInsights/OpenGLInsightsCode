
/**********************************************************************\
* AUTHOR : HILLAIRE Sébastien
*		   Somme method comme from David HENRY MD5 loader
*
* MAIL   : hillaire_sebastien@yahoo.fr
* SITE   : sebastien.hillaire.free.fr
*
*	You are free to totally or partially use this file/code.
* If you do, please credit me in your software or demo and leave this
* note.
*	Share your work and your ideas as much as possible!
\*********************************************************************/

#ifndef QUATERNION_H
#define QUATERNION_H

#include <iostream>

#include "MathLibFunc.h"
#include "Matrix4.h"

/** 
 *	A quaternion class.
 *
 * @author Hillaire Sébastien
 *
 */
class Quaternion
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
	 *	constructor
	 */
	inline Quaternion(void)
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
	inline Quaternion(const float x, const float y, const float z, const float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	/**
	 *	Use to modify parameters.
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
	 *	set the null quaterion!
	 */
	inline void zero(void)
	{
		x = y = z = w = 0.0f;
	}

	/**
	 *	compute w from x,y and z
	 */
	inline void computeW(void)
	{
		float t = 1.0f - (x*x) - (y*y) - (z*z);
		if (t<=0.0f)
			w=0.0f;
		else
			w= sqrtf(t);
	}


	inline float operator[](int i) const
	{
#ifdef _DEBUG
		if( ( i < 0 ) && ( i >= 4 ) ) std::cout<<"const float Quaternion::operator[](int i)const : i invalide : "<<i<<"."<<std::endl;
#endif
		return (&x)[i];
	}
	inline float & operator[](int i)
	{
#ifdef _DEBUG
		if( ( i < 0 ) && ( i >= 4 ) ) std::cout<<"const float Quaternion::operator[](int i)const : i invalide : "<<i<<"."<<std::endl;
#endif
		return (&x)[i];
	}

	inline Quaternion operator-() const
	{
		return Quaternion( -x, -y , -z , -w );
	}

	/**
	 *	Inverse the rotation of the quaternion
	 */
	inline void negateRotation()
	{
		this->x = -this->x;
		this->y = -this->y;
		this->z = -this->z;
	}


	inline Quaternion operator+(const Quaternion& v) const
	{
		return Quaternion( x+v.x, y+v.y, z+v.z, w+v.w );
	}
	inline Quaternion operator-(const Quaternion& v) const
	{
		return Quaternion( x-v.x, y-v.y, z-v.z, w-v.w );
	}
	inline Quaternion operator*(const Quaternion& q) const
	{
		return Quaternion(
			w*q.x + x*q.w + y*q.z - z*q.y,
			w*q.y + y*q.w + z*q.x - x*q.z,
			w*q.z + z*q.w + x*q.y - y*q.x,
			w*q.w - x*q.x - y*q.y - z*q.z );
	}

	/**
	 *
	 */
	inline void multVector3(const Quaternion& qIn, const Vector3& vIn)
	{
		this->w = - (qIn.x * vIn.x) - (qIn.y * vIn.y) - (qIn.z * vIn.z);
		this->x =   (qIn.w * vIn.x) + (qIn.y * vIn.z) - (qIn.z * vIn.y);
		this->y =   (qIn.w * vIn.y) + (qIn.z * vIn.x) - (qIn.x * vIn.z);
		this->z =   (qIn.w * vIn.z) + (qIn.x * vIn.y) - (qIn.y * vIn.x);
	}

	/**
	 *	Rotate a point around the quaternion
	 */
	inline void rotatePoint(const Vector3& vIn, Vector3& vOut) const
	{
		Quaternion tmp, inv, final;

		inv = *this;
		inv.negateRotation();
		inv.normalize();

		tmp.multVector3(*this, vIn);
		final = tmp * inv;

		vOut.set(final.x, final.y, final.z);
	}


	inline Quaternion operator*(const float v) const
	{
		return Quaternion( x*v, y*v, z*v, w*v );
	}


	inline Quaternion& operator+=(const Quaternion& q)
	{
		x += q.x;
		y += q.y;
		z += q.z;
		w += q.w;
		return *this;
	}
	inline Quaternion& operator-=(const Quaternion& q)
	{
		x -= q.x;
		y -= q.y;
		z -= q.z;
		w -= q.w;
		return *this;
	}
	inline Quaternion& operator*=(const Quaternion& q)
	{
		float tx,ty,tz;
		tx = w*q.x + x*q.w + y*q.z - z*q.y;
		ty = w*q.y + y*q.w + z*q.x - x*q.z;
		tz = w*q.z + z*q.w + x*q.y - y*q.x;
		w  = w*q.w - x*q.x - y*q.y - z*q.z;
		x = tx;
		y = ty;
		z = tz;
		return *this;
	}


	inline Quaternion& operator+=(const float v) 
	{
		x += v;
		y += v;
		z += v;
		w += v;
		return *this;
	} 
	inline Quaternion& operator-=(const float v) 
	{
		x -= v;
		y -= v;
		z -= v;
		z -= v;
		return *this;
	} 
	inline Quaternion& operator*=(const float v) 
	{
		x *= v;
		y *= v;
		z *= v;
		w *= v;
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


	inline bool operator==(const Quaternion& q) const
	{
		return equals( q );
	}
	inline bool operator!=(const Quaternion& q) const 
	{
		if( x!=q.x || y!=q.y || z!=q.z || w!=q.w ) return false;
		return true;
	}


	/**
	 *	Comparisonb function
	 *
	 * @param q : the quaternion to compare
	 * @return true if both are equals, else false.
	 */
	inline bool equals(const Quaternion& q) const
	{
		return ( (x==q.x) && (y==q.y) && (z==q.z) && (w==q.w) );
	}

	/**
	 *	Set the absolute value of each quaternio parameters
	 */
	inline void setPositiveValue()
	{
		x = ABS(x);	y = ABS(y);	z = ABS(z);	w = ABS(w);
	}

	/**
	 *	Give the length of this quaternion
	 *
	 * @return the length
	 */
	inline float length(void) const 
	{
		return sqrtf( x*x + y*y + z*z + w*w );
	}
	/**
	 *	Give the length squared of this quaternion
	 *
	 * @return the length squared
	 */
	inline float lengthSquared(void) const
	{
		return ( x*x + y*y + z*z + w*w );
	}
	/**
	 *	Normalize this quaternion
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
	 *	Compute a dot product
	 *
	 * @param q : the quaternino used to compute a dot product
	 *
	 * @return the dot product value
	 */
	inline float dot(const Quaternion& q) const
	{
		return ( x*q.x + y*q.y + z*q.z + w*q.w );
	}

	/**
	 *	Linear interpolation. The result is stored in this quaterion.
	 *
	 * @param q1 : start quaternion
	 * @param q2 : end quaternion
	 * @param l : interpolation coefficient
	 */
	inline void lerp(const Quaternion& q1, const Quaternion& q2, const float l)
	{
		if ( l <= 0.0f )
		{
			(*this) = q1;
		} else if ( l >= 1.0f )
		{
			(*this) = q2;
		} else
		{
			(*this) = q1 + (q2 - q1)*l;
		}
	}

	/**
	 *	Spherical interpolation. The result is stored in this quaterion.
	 *
	 * @param q1 : start quaternion
	 * @param q2 : end quaternion
	 * @param l : interpolation coefficient
	 */
	inline void slerp( const Quaternion& q1, const Quaternion& q2, float t )
	{
		Quaternion	temp;
		float	omega, cosom, sinom, scale0, scale1;

		if(t<=0.0f)
		{
			*this = q1;
			return;
		}
		if(t>=1.0f)
		{
			*this = q2;
			return;
		}
		if(q1==q2)
		{
			*this = q2;
			return;
		}

		cosom = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
		if( cosom < 0.0f )
		{
			temp = -q2;
			cosom = -cosom;
		} else
		{
			temp = q2;
		}

		if((1.0f-cosom ) > 1e-6f) 
		{
			omega = acosf( cosom );
			sinom = 1.0f / sinf( omega );
			scale0 = sinf( ( 1.0f - t ) * omega ) * sinom;
			scale1 = sinf( t * omega ) * sinom;
		}
		else
		{
			scale0 = 1.0f - t;
			scale1 = t;
		}

		*this = (q1*scale0) + (temp*scale1);
	}

	/**
	 *	inverse the rotation.
	 */
	inline float inverse()
	{
		this->x = -this->x;
		this->y = -this->y;
		this->z = -this->z;
	}

	/**
	 *	Convert this quaterion in a 4x4 matrix.
	 *
	 * @param mat : the resulting matrix
	 */
	inline void toMatrix4(Matrix4& mat)
	{
		float* ptr = mat.toFloatPtr();
		float	wx, wy, wz;
		float	xx, yy, yz;
		float	xy, xz, zz;
		float	x2, y2, z2;

		x2 = x + x;
		y2 = y + y;
		z2 = z + z;

		xx = x * x2;
		xy = x * y2;
		xz = x * z2;

		yy = y * y2;
		yz = y * z2;
		zz = z * z2;

		wx = w * x2;
		wy = w * y2;
		wz = w * z2;

		mat.identity();
		ptr[0]  = 1.0f - ( yy + zz );
		ptr[4]  = xy - wz;
		ptr[8]  = xz + wy;
		
		ptr[1]  = xy + wz;
		ptr[5]  = 1.0f - ( xx + zz );
		ptr[9]  = yz - wx;
		
		ptr[2]  = xz - wy;
		ptr[6]  = yz + wx;
		ptr[10] = 1.0f - ( xx + yy );
	}
};

extern const Quaternion quat_Xaxis;
extern const Quaternion quat_Yaxis;
extern const Quaternion quat_Zaxis;

#endif
