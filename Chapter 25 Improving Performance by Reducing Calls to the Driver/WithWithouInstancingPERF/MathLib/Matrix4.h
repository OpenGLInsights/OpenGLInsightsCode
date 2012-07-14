
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

#ifndef MATRIX4_H
#define MATRIX4_H

#include <iostream>

#include "Vector3.h"
#include "Vector4.h"

/**
 *	A 4x4 matrix class. The 16 components of the matrix are oriented to match OpenGL style.
 *
 * @author Hillaire Sébastien
 */
class Matrix4
{
private:
	/**
	 * the matrix
	 */
	float mat[16];

protected:

public:

	/**
	 *	default constructor
	 */
	inline Matrix4(void)
	{
		memset(mat,0,sizeof(float)*16);
	}

	/**
	 *	constructor with initialisation
	 *
	 * @param xx : coordinate 0 of the matrix
	 * @param xy : coordinate 1 of the matrix
	 * @param xz : coordinate 2 of the matrix
	 * @param xw : coordinate 3 of the matrix
	 * @param yx : coordinate 4 of the matrix
	 * @param yy : coordinate 5 of the matrix
	 * @param yz : coordinate 6 of the matrix
	 * @param yw : coordinate 7 of the matrix
	 * @param zx : coordinate 8 of the matrix
	 * @param zy : coordinate 9 of the matrix
	 * @param zz : coordinate 10 of the matrix
	 * @param zw : coordinate 11 of the matrix
	 * @param wx : coordinate 12 of the matrix
	 * @param wy : coordinate 13 of the matrix
	 * @param wz : coordinate 14 of the matrix
	 * @param ww : coordinate 15 of the matrix
	 */
	inline Matrix4( const float xx, const float xy, const float xz, const float xw,
		const float yx, const float yy, const float yz, const float yw,
		const float zx, const float zy, const float zz, const float zw,
		const float wx, const float wy, const float wz, const float ww )
	{
		mat[0] = xx; mat[4] = xy; mat[8] =  xz; mat[12] = xw;
		mat[1] = yx; mat[5] = yy; mat[9] =  yz; mat[13] = yw;
		mat[2] = zx; mat[6] = zy; mat[10] = zz; mat[14] = zw;
		mat[3] = wx; mat[7] = wy; mat[11] = wz; mat[15] = ww;
	}

	/**
	 *	matrix modifier
	 *
	 * @param xx : coordinate 0 of the matrix
	 * @param xy : coordinate 1 of the matrix
	 * @param xz : coordinate 2 of the matrix
	 * @param xw : coordinate 3 of the matrix
	 * @param yx : coordinate 4 of the matrix
	 * @param yy : coordinate 5 of the matrix
	 * @param yz : coordinate 6 of the matrix
	 * @param yw : coordinate 7 of the matrix
	 * @param zx : coordinate 8 of the matrix
	 * @param zy : coordinate 9 of the matrix
	 * @param zz : coordinate 10 of the matrix
	 * @param zw : coordinate 11 of the matrix
	 * @param wx : coordinate 12 of the matrix
	 * @param wy : coordinate 13 of the matrix
	 * @param wz : coordinate 14 of the matrix
	 * @param ww : coordinate 15 of the matrix
	 */
	inline void Matrix4::set( const float xx, const float xy, const float xz, const float xw,
		const float yx, const float yy, const float yz, const float yw,
		const float zx, const float zy, const float zz, const float zw,
		const float wx, const float wy, const float wz, const float ww )
	{
		mat[0] = xx; mat[4] = xy; mat[8] =  xz; mat[12] = xw;
		mat[1] = yx; mat[5] = yy; mat[9] =  yz; mat[13] = yw;
		mat[2] = zx; mat[6] = zy; mat[10] = zz; mat[14] = zw;
		mat[3] = wx; mat[7] = wy; mat[11] = wz; mat[15] = ww;
	}

	//to get a specific value of the matrix
	inline float operator[]( int i ) const
	{
#ifdef _DEBUG
		if( ( i < 0 ) && ( i >= 16 ) ) std::cout<<"const float Matrix4::operator[](int i)const : i invalide : "<<i<<"."<<std::endl;
#endif
		return mat[ i ];
	}


	/**
	 *	@return a contant float pointer to the matrice
	 */
	inline const float* toFloatPtr(void) const
	{
		return mat;
	}
	/**
	 *	@return a flot pointer to the matrice
	 */
	inline float* toFloatPtr(void)
	{
		return mat;
	}


	/**
	 *	Vector3 projection into matrix
	 */
	inline Vector3 operator* ( const Vector3& m ) const
	{
		return Vector3(	mat[0]*m[0] + mat[4]*m[1] + mat[8] *m[2] + mat[12],
			mat[1]*m[0] + mat[5]*m[1] + mat[9] *m[2] + mat[13],
			mat[2]*m[0] + mat[6]*m[1] + mat[10]*m[2] + mat[14]
			);
	}
	/**
	 *	Vector4 projection into matrix
	 */
	inline Vector4 operator* ( const Vector4& m ) const
	{
		return Vector4(	mat[0]*m[0] + mat[4]*m[1] + mat[8] *m[2] + mat[12]*m[3],
			mat[1]*m[0] + mat[5]*m[1] + mat[9] *m[2] + mat[13]*m[3],
			mat[2]*m[0] + mat[6]*m[1] + mat[10]*m[2] + mat[14]*m[3],
			mat[3]*m[0] + mat[7]*m[1] + mat[11]*m[2] + mat[15]*m[3]
			);
	}

	/**
	 *	matrix multiplication
	 */
	inline Matrix4 operator* ( const Matrix4& m ) const
	{
		Matrix4 ret;
		float *rm = ret.mat;
		rm[0] = mat[0] * m[0] + mat[4] * m[1] + mat[8] * m[2] + mat[12] * m[3];
		rm[1] = mat[1] * m[0] + mat[5] * m[1] + mat[9] * m[2] + mat[13] * m[3];
		rm[2] = mat[2] * m[0] + mat[6] * m[1] + mat[10] * m[2] + mat[14] * m[3];
		rm[3] = mat[3] * m[0] + mat[7] * m[1] + mat[11] * m[2] + mat[15] * m[3];
		rm[4] = mat[0] * m[4] + mat[4] * m[5] + mat[8] * m[6] + mat[12] * m[7];
		rm[5] = mat[1] * m[4] + mat[5] * m[5] + mat[9] * m[6] + mat[13] * m[7];
		rm[6] = mat[2] * m[4] + mat[6] * m[5] + mat[10] * m[6] + mat[14] * m[7];
		rm[7] = mat[3] * m[4] + mat[7] * m[5] + mat[11] * m[6] + mat[15] * m[7];
		rm[8] = mat[0] * m[8] + mat[4] * m[9] + mat[8] * m[10] + mat[12] * m[11];
		rm[9] = mat[1] * m[8] + mat[5] * m[9] + mat[9] * m[10] + mat[13] * m[11];
		rm[10] = mat[2] * m[8] + mat[6] * m[9] + mat[10] * m[10] + mat[14] * m[11];
		rm[11] = mat[3] * m[8] + mat[7] * m[9] + mat[11] * m[10] + mat[15] * m[11];
		rm[12] = mat[0] * m[12] + mat[4] * m[13] + mat[8] * m[14] + mat[12] * m[15];
		rm[13] = mat[1] * m[12] + mat[5] * m[13] + mat[9] * m[14] + mat[13] * m[15];
		rm[14] = mat[2] * m[12] + mat[6] * m[13] + mat[10] * m[14] + mat[14] * m[15];
		rm[15] = mat[3] * m[12] + mat[7] * m[13] + mat[11] * m[14] + mat[15] * m[15];
		return ret;
	}

	inline Matrix4 operator+ ( const Matrix4& m ) const
	{
		Matrix4 ret;
		float *rm = ret.mat;
		rm[0] = mat[0] + m[0]; rm[4] = mat[4] + m[4]; rm[8] = mat[8] + m[8]; rm[12] = mat[12] + m[12];
		rm[1] = mat[1] + m[1]; rm[5] = mat[5] + m[5]; rm[9] = mat[9] + m[9]; rm[13] = mat[13] + m[13];
		rm[2] = mat[2] + m[2]; rm[6] = mat[6] + m[6]; rm[10] = mat[10] + m[10]; rm[14] = mat[14] + m[14];
		rm[3] = mat[3] + m[3]; rm[7] = mat[7] + m[7]; rm[11] = mat[11] + m[11]; rm[15] = mat[15] + m[15];
		return ret;
	}
	inline Matrix4 operator- ( const Matrix4& m ) const
	{
		Matrix4 ret;
		float *rm = ret.mat;
		rm[0] = mat[0] - m[0]; rm[4] = mat[4] - m[4]; rm[8] = mat[8] - m[8]; rm[12] = mat[12] - m[12];
		rm[1] = mat[1] - m[1]; rm[5] = mat[5] - m[5]; rm[9] = mat[9] - m[9]; rm[13] = mat[13] - m[13];
		rm[2] = mat[2] - m[2]; rm[6] = mat[6] - m[6]; rm[10] = mat[10] - m[10]; rm[14] = mat[14] - m[14];
		rm[3] = mat[3] - m[3]; rm[7] = mat[7] - m[7]; rm[11] = mat[11] - m[11]; rm[15] = mat[15] - m[15];
		return ret;
	}

	inline void operator= ( const Matrix4& m )
	{
		mat[0] = m[0]; mat[4] = m[4]; mat[8]  = m[8];  mat[12] = m[12];
		mat[1] = m[1]; mat[5] = m[5]; mat[9]  = m[9];  mat[13] = m[13];
		mat[2] = m[2]; mat[6] = m[6]; mat[10] = m[10]; mat[14] = m[14];
		mat[3] = m[3]; mat[7] = m[7]; mat[11] = m[11]; mat[15] = m[15];
	}

	inline bool operator==( const Matrix4& m ) const
	{
		return this->equals(m);
	}
	inline bool operator!=( const Matrix4& a ) const
	{
		for(int i=0;i<16;i++)
		{
			if(a[i]!=this->mat[i]) return true;
		}
		return false;
	}

	/**
	 *	equality function
	 *
	 * @param m : the matrix to compare this with
	 * @return true if both matrix are equals.
	 */
	inline bool equals(const Matrix4& m) const
	{
		for(int i=0;i<16;i++)
		{
			if(m[i]!=this->mat[i]) return false;
		}
		return true;
	}


	/**
	 *	set the zero matrix.
	 */
	inline void zero() 
	{
		mat[0] = 0.0f; mat[4] = 0.0f; mat[8 ] = 0.0f; mat[12] = 0.0f;
		mat[1] = 0.0f; mat[5] = 0.0f; mat[9 ] = 0.0f; mat[13] = 0.0f;
		mat[2] = 0.0f; mat[6] = 0.0f; mat[10] = 0.0f; mat[14] = 0.0f;
		mat[3] = 0.0f; mat[7] = 0.0f; mat[11] = 0.0f; mat[15] = 0.0f;
	}

	/**
	 *	set the identity matrix.
	 */
	inline void identity() 
	{
		mat[0] = 1.0f; mat[4] = 0.0f; mat[8 ] = 0.0f; mat[12] = 0.0f;
		mat[1] = 0.0f; mat[5] = 1.0f; mat[9 ] = 0.0f; mat[13] = 0.0f;
		mat[2] = 0.0f; mat[6] = 0.0f; mat[10] = 1.0f; mat[14] = 0.0f;
		mat[3] = 0.0f; mat[7] = 0.0f; mat[11] = 0.0f; mat[15] = 1.0f;
	}

	/**
	 *	transpose the matrix.
	 */
	inline void transpose() 
	{
		float ret[16];
		ret[0] = mat[0 ]; ret[4] = mat[1 ]; ret[8 ] = mat[2 ]; ret[12] = mat[3];
		ret[1] = mat[4 ]; ret[5] = mat[5 ]; ret[9 ] = mat[6 ]; ret[13] = mat[7];
		ret[2] = mat[8 ]; ret[6] = mat[9 ]; ret[10] = mat[10]; ret[14] = mat[11];
		ret[3] = mat[12]; ret[7] = mat[13]; ret[11] = mat[14]; ret[15] = mat[15];
		memcpy(mat,ret,sizeof(float)*16);
	}

	/**
	 *	compute the inverse matrix
	 */
	inline void inverse() 
	{
		float ret[16];
		float det;
		det  = mat[0] * mat[5] * mat[10];
		det += mat[4] * mat[9] * mat[2];
		det += mat[8] * mat[1] * mat[6];
		det -= mat[8] * mat[5] * mat[2];
		det -= mat[4] * mat[1] * mat[10];
		det -= mat[0] * mat[9] * mat[6];
		det =  1.0f / det;				//pas de sécurité pour division pas zéro...
		ret[0]  =  (mat[5] * mat[10] - mat[9] * mat[6]) * det;
		ret[1]  = -(mat[1] * mat[10] - mat[9] * mat[2]) * det;
		ret[2]  =  (mat[1] * mat[6 ] - mat[5] * mat[2]) * det;
		ret[3]  =  0.0f;
		ret[4]  = -(mat[4] * mat[10] - mat[8] * mat[6]) * det;
		ret[5]  =  (mat[0] * mat[10] - mat[8] * mat[2]) * det;
		ret[6]  = -(mat[0] * mat[6 ] - mat[4] * mat[2]) * det;
		ret[7]  =  0.0f;
		ret[8]  =  (mat[4] * mat[9] - mat[8] * mat[5]) * det;
		ret[9]  = -(mat[0] * mat[9] - mat[8] * mat[1]) * det;
		ret[10] =  (mat[0] * mat[5] - mat[4] * mat[1]) * det;
		ret[11] =  0.0f;
		ret[12] = -(mat[12] * ret[0] + mat[13] * ret[4] + mat[14] * ret[8 ]);
		ret[13] = -(mat[12] * ret[1] + mat[13] * ret[5] + mat[14] * ret[9 ]);
		ret[14] = -(mat[12] * ret[2] + mat[13] * ret[6] + mat[14] * ret[10]);
		ret[15] =  1.0f;
		memcpy(mat,ret,sizeof(float)*16);
	}


	/**
	 *	Compute a rotation matrixe around X axis.
	 *
	 * @param rad : rotation angle in radian
	 */
	inline void setXRotation(float rad) 
	{
		float c = cos(rad);
		float s = sin(rad);
		mat[0] = 1.0f; mat[4] = 0.0f;	mat[8 ] = 0.0f;	mat[12] = 0.0f;
		mat[1] = 0.0f; mat[5] = c;		mat[9 ] = -s;	mat[13] = 0.0f;
		mat[2] = 0.0f; mat[6] = s;		mat[10] =  c;	mat[14] = 0.0f;
		mat[3] = 0.0f; mat[7] = 0.0f;	mat[11] = 0.0f;	mat[15] = 1.0f;
	}

	/**
	 *	Compute a rotation matrixe around Y axis.
	 *
	 * @param rad : rotation angle in radian
	 */
	inline void setYRotation(float rad) 
	{
		float c = cos(rad);
		float s = sin(rad);
		mat[0] =  c;	mat[4] = 0.0f; mat[8 ] = s;		mat[12] = 0.0f;
		mat[1] = 0.0f;	mat[5] = 1.0f; mat[9 ] = 0.0f;	mat[13] = 0.0f;
		mat[2] = -s;	mat[6] = 0.0f; mat[10] = c;		mat[14] = 0.0f;
		mat[3] = 0.0f;	mat[7] = 0.0f; mat[11] = 0.0f;	mat[15] = 1.0f;
	}

	/**
	 *	Compute a rotation matrixe around Z axis.
	 *
	 * @param rad : rotation angle in radian
	 */
	inline void setZRotation(float rad) 
	{
		float c = cos(rad);
		float s = sin(rad);
		mat[0] = c;		mat[4] = -s;	mat[8 ] = 0.0f;	mat[12] = 0.0f;
		mat[1] = s;		mat[5] =  c;	mat[9 ] = 0.0f;	mat[13] = 0.0f;
		mat[2] = 0.0f;	mat[6] = 0.0f;	mat[10] = 1.0f;	mat[14] = 0.0f;
		mat[3] = 0.0f;	mat[7] = 0.0f;	mat[11] = 0.0f;	mat[15] = 1.0f;
	}

	/**
	 *	Compute a translation matrix.
	 *
	 * @param v : a reference to the translation vector.
	 */
	inline void setTranslation(const Vector3& v) 
	{
		mat[0] = 1.0f; mat[4] = 0.0f; mat[8 ] = 0.0f; mat[12] = v[0];
		mat[1] = 0.0f; mat[5] = 1.0f; mat[9 ] = 0.0f; mat[13] = v[1];
		mat[2] = 0.0f; mat[6] = 0.0f; mat[10] = 1.0f; mat[14] = v[2];
		mat[3] = 0.0f; mat[7] = 0.0f; mat[11] = 0.0f; mat[15] = 1.0f;
	}
	/**
	 *	Compute a translation matrix.
	 *
	 * @param x : value of the translation vector on x axis
	 * @param y : value of the translation vector on y axis
	 * @param z : value of the translation vector on z axis
	 */
	inline void setTranslation(float x, float y, float z) 
	{
		mat[0] = 1.0f; mat[4] = 0.0f; mat[8 ] = 0.0f; mat[12] = x;
		mat[1] = 0.0f; mat[5] = 1.0f; mat[9 ] = 0.0f; mat[13] = y;
		mat[2] = 0.0f; mat[6] = 0.0f; mat[10] = 1.0f; mat[14] = z;
		mat[3] = 0.0f; mat[7] = 0.0f; mat[11] = 0.0f; mat[15] = 1.0f;
	}

	/**
	 *	compute a scale matrix.
	 *
	 * @param v : the scale vector.
	 */
	inline void setScale(const Vector3& v) 
	{
		mat[0] = v[0];	mat[4] = 0.0f;	mat[8 ] = 0.0f;	mat[12] = 0.0f;
		mat[1] = 0.0f;	mat[5] = v[1];	mat[9 ] = 0.0f;	mat[13] = 0.0f;
		mat[2] = 0.0f;	mat[6] = 0.0f;	mat[10] = v[2]; mat[14] = 0.0f;
		mat[3] = 0.0f;	mat[7] = 0.0f;	mat[11] = 0.0f;	mat[15] = 1.0f;
	}
	/**
	 *	compute a scale matrix.
	 *
	 * @param x : value of the scale vector on x axis
	 * @param y : value of the scale vector on y axis
	 * @param z : value of the scale vector on z axis
	 */
	inline void setScale(float x, float y, float z) 
	{
		mat[0] = x;		mat[4] = 0.0f;	mat[8] = 0.0f;	mat[12] = 0.0f;
		mat[1] = 0.0f;	mat[5] = y;		mat[9] = 0.0f;	mat[13] = 0.0f;
		mat[2] = 0.0f;	mat[6] = 0.0f;	mat[10] = z;	mat[14] = 0.0f;
		mat[3] = 0.0f;	mat[7] = 0.0f;	mat[11] = 0.0f;	mat[15] = 1.0f;
	}

	/**
	 *	compute a projection matrix for OpenGL.
	 *
	 * @param fov : vision angle
	 * @param aspect : width/heigth ratio
	 * @param znear : near cliping plane distance
	 * @param zfar : far clipping plane distance
	 */
	inline void setPerspective(float fov,float aspect,float znear,float zfar) 
	{
		float y = tan(fov * PI / 360.0f);
		float x = y * aspect;
		mat[0] = 1.0f/x;mat[4] = 0.0f;	mat[8] = 0.0f;	mat[12] = 0.0f;
		mat[1] = 0.0f;	mat[5] = 1.0f/y;mat[9] = 0.0f;	mat[13] = 0.0f;
		//pas de protection pour la division pas zéro...
		mat[2] = 0.0;	mat[6] = 0.0;	mat[10] = -(zfar+znear)/(zfar-znear); mat[14] = -(2.0f*zfar*znear)/(zfar-znear);
		mat[3] = 0.0;	mat[7] = 0.0;	mat[11] = -1.0; mat[15] = 0.0;
	}


	/**
	 * rotation matrix
	 *
	 * @param pitch : pitch angle in radian
	 * @param yaw : yaw angle in radian
	 * @param roll : roll angle in radian
	 */
	inline void genTransformMat(float pitch, float yaw, float roll)
	{
		float cx = cosf(pitch);
		float sx = sinf(pitch);
		float cy = cosf(yaw);
		float sy = sinf(yaw);
		float cz = cosf(roll);
		float sz = sinf(roll);

		float cxsy = cx * sy;
		float sxsy = sx * sy;

		this->identity();

		mat[0]  =    cy * cz;
		mat[1]  =   -cy * sz;
		mat[2]  =   -sy;
		mat[4]  = -sxsy * cz + cx * sz;
		mat[5]  =  sxsy * sz + cx * cz;
		mat[6]  =   -sx * cy;
		mat[8]  =  cxsy * cz + sx * sz;
		mat[9]  = -cxsy * sz + sx * cz;
		mat[10] =    cx * cy;

		this->transpose();
	}
};

#endif
