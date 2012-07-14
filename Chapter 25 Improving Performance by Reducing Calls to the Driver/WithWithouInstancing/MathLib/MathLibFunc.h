
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


#ifndef MATHLIBFUNC_H
#define MATHLIBFUNC_H

#include "math.h"
#include "stdlib.h"



//constants
#ifndef PI
#define PI			3.1415926535897932384626433832795f
#endif


//conversion
#ifndef DEG2RAD
#define DEG2RAD		(PI / 180.0f)
#endif
#ifndef RAD2DEG
#define RAD2DEG		(180.0f / PI)
#endif


/**
 *	compute absolute value of a float
 *
 * @param x : the value
 * @return absolute value
 */
inline float ABS(const float x)
{
	return ((x<0.0f) ? -x : x);
}

/**
 *	compute the maximum of two float
 *
 * @param v1 : 1st value
 * @param v2 : 2nd value
 * @return the maximum between v1 and v2
 */
inline float MAX(const float v1, const float v2)
{
	return ((v1>v2) ? v1 : v2);
}

/**
 *	compute the minimum of two float
 *
 * @param v1 : 1st value
 * @param v2 : 2nd value
 * @return the minimum between v1 and v2
 */
inline float MIN(const float v1, const float v2)
{
	return ((v1<v2) ? v1 : v2);
}

/**
 *	tell if a value is a power of 2.
 * REM : isPower2(0)==false and isPower2(1)==true 
 *
 * @param x : the value
 *
 * @return true if x is a power of 2
 */
inline bool isPower2(const int x)
{
	return ((x > 0) && ((x & (x - 1)) == 0));
}


/**
 *	Compute  a power of 2 number being higher than x. If x is a power of 2,
 * x is returned.
 * REM : nextPower2(0)==0 and nextPower2(1)==1 
 *
 * @param x : the value
 *
 * @return a power of 2 number
 */
inline int nextPower2(int x)
{
    int i = x & (~x + 1);

    while (i < x)
        i <<= 1;	

    return i;
}


/**
 *	Simple linear interpolation beatween two entities
 *
 * @pre 0<=t<=1
 * @pre + and - operator and the entities must be defined
 * @pre operator between an entity and a float mustr be defined
 *
 * @param a : reference to the first entity
 * @param b : reference to the second entity
 * @param t : interpolation value from a to b
 *
 * @return interpolated entity
 */
template <typename T> inline T lerp(const T& a, const T& b, const float t)
{
	return a + (b - a) * t;
}


/**
 *	Simple bilinear interpolation beatween two entities
 *
 * @pre 0<=u<=1
 * @pre 0<=v<=1
 * @pre + and - operator and the entities must be defined
 * @pre operator between an entity and a float mustr be defined
 *
 * @param a : reference to the first entity
 * @param b : reference to the second entity
 * @param c : reference to the third entity
 * @param d : reference to the fourth entity
 * @param u : interpolation value from a to b (give iv1) and from c to d (give iv2)
 * @param v : interpolation value from iv1 to iv2
 *
 * @return interpolated entity
 */
template <typename T> inline T bilerp(const T &a, const T &b, const T &c, const T &d, const float u, const float v)
{
	float iv1 = lerp(a,b,u);
	float iv2 = lerp(c,d,u);

	return linearInterpolation(iv1,iv2,v);
}

/**
 *	@return a random value in [0,1] range
 */
inline float rand01()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

/**
 * @param min : the minimum value
 * @param max : the maximum value
 *
 * @return a random value in [min,max] range
 */
inline float randRange(const float min, const float max)
{
	return min + (max-min)*static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}


#endif