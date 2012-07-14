
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

#ifndef SPHERE_H
#define SPHERE_H

#include "MathLibFunc.h"
#include "Vector3.h"


/**
 *	A sphere class. The sphere is caracterized by its center and radius.
 *
 * @author Hillaire Sébastien
 */
class Sphere
{

private:

	/**
	 * the radius of the sphere
	 * REM : must be >0
	 */
	float r;

protected:

public:

	/**
	 * sphere center
	 */
	Vector3 c;

	/**
	 *	constructor
	 */
	inline Sphere(void)
	{
		c.set(0.0f,0.0f,0.0f);
		r = 1.0f;
	}
	/**
	 *	constructor with parameters.
	 *
	 * @param c : center
	 * @param r : radius
	 *
	 * @pre : r>=0
	 */
	inline Sphere(const Vector3 c, const float r)
	{
		this->c = c;
		this->setRadius(r);
	}

	/**
	 * radius accessor
	 *
	 * @return the radius. (always a positive value)
	 */
	inline float getRadius()const {return this->r;}
	/**
	 * radius modifier
	 *
	 * @param r : the radius. if r is negative, we use the absolute value.
	 */
	inline void setRadius(float r){this->r = ABS(r);}

	/**
	 *	Sphere modifier
	 *
	 * @param c : center
	 * @param r : radius
	 */
	inline void set(const Vector3 c, const float r)
	{
		this->c = c;
		this->r = r;
	}


	inline bool operator==(const Sphere& s) const
	{
		return equals( s );
	}
	inline bool operator!=(const Sphere& s) const 
	{
		if( r!=s.r || c!=s.c ) return false;
		return true;
	}


	/**
	 *	Comparison function
	 *
	 * @param s : the sphere to compare with
	 * @return true if both sphere are equals, else false
	 */
	inline bool equals(const Sphere& s) const
	{
		return ( (r==s.r) && (c==s.c) );
	}

	/**
	*	Compute the intersection point between this sphere and a line who pass on a point.
	*
	* @param dO : the point
	* @param dV : the line direction
	* @param dest : the intersection point. REM : dest is vec3_origin if there is no intersection point
	*/
	inline void intersectSphere(Vector3& dO, Vector3& dV, Vector3& dest)
	{
		Vector3 Q = c - dO;
		float c = Q.length();
		float v = Q.dot(dV);
		float d = (r*r) - ( c*c - v*v );

		if(d<0.0f)
			dest = vec3_origin;
		else
			dest = dO + dV*(v-sqrtf(d));
	}

	/**
	*	Compute the intersection point between this sphere and a line who pass on a point.
	*
	* @param dO : the point
	* @param dV : the line direction
	* @return the intersection point distance. -1 if there is no intersection point
	*/
	inline float intersectSphere(Vector3& dO, Vector3& dV)
	{
		Vector3 Q = c - dO;
		float c = Q.length();
		float v = Q.dot(dV);
		float d = (r*r) - ( c*c - v*v );

		if(d<0.0f)
			return -1.0f;
		else
			return (v - sqrtf(d));
	}
};


extern const Sphere sphere_ident;


#endif
