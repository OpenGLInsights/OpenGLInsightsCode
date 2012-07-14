
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

#ifndef PLANE_H
#define PLANE_H

#include "MathLibFunc.h"
#include "Vector3.h"


/**
 *	A plane class. The plane is caracterized by a normal n (perpendicular to the plane) and a distance d from the origin.
 *
 * @author Hillaire Sébastien
 */
class Plane
{

private:

protected:

public:

	/**
	 * the normal
	 */
	Vector3 n;

	/**
	 * d component of plane equation
	 */
	float d;

	/**
	 *	constructor
	 */
	inline Plane(void)
	{
		this->n.set(0.0f,1.0f,0.0f);
		this->d = 0;
	}
	/**
	 *	constructor with paramters.
	 *
	 * @param n : the normal
	 * @param d : d component of plane equation
	 */
	inline Plane(const Vector3 n, const float d)
	{
		this->n = n;
		this->d = d;
	}

	/**
	 *	Use to modify the plane.
	 *
	 * @param n : the normal
	 * @param d : d component of plane equation
	 */
	inline void set(const Vector3 n, const float d)
	{
		this->n = n;
		this->d = d;
	}
	
	/**
	 * Create a plane from two vectors and one point
	 *
	 * @pre v1 and v2 must not be parallel
	 * @pre v1 and v2 must not be equals
	 *
	 * @param v1 : first vector
	 * @param v2 : second vector
	 * @param p  : a point in space
	 */
	inline void create(Vector3& v1, Vector3& v2, Vector3& p)
	{
#ifdef NDEBUG
		if(v1==v2) return;
#endif
		
		//calcul et normalisation de la normal du plan
		this->n.cross(v1,v2);
		this->n.normalize();
		//on calcul d pour le point par lequel le plan passe
		d = - this->n.dot(p);
	}


	inline bool operator==(const Plane& p) const
	{
		return equals( p );
	}
	inline bool operator!=(const Plane& p) const 
	{
		if( d!=p.d || n!=p.n ) return false;
		return true;
	}

	/**
	 *	Comparison function
	 *
	 * @param p : the plane to compare with this
	 * @return true if both plane are equal, else false.
	 */
	inline bool equals(const Plane& p) const
	{
		return ( (d==p.d) && (n==p.n) );
	}


	/**
	 * Compute an orthographic distance between a point and this plane.(absolute value)
	 *
	 * THIS METHODS RELIES ON distanceFromPointToPlane WHO HAVE NOT BEEN TESTED YET
	 *
	 * @param p : pointer to the point
	 */
	inline float distanceFromPointToPlaneABS(Vector3& p)
	{
		return ABS(this->distanceFromPointToPlane(p));			//valeur absolue
	}

	/**
	 * Compute an orthographic distance between a point and this plane.
	 *
	 * THIS METHOS HAVE NOT BEEN TESTED YET
	 * (For developer : should we use sqrt(dotp) instead of dotp?)
	 *
	 * @param p : pointer to the point
	 */
	inline float distanceFromPointToPlane(Vector3& p)
	{
		float dotp = p.dot(this->n);	//distance from origin to the orthogonally projected point one the plane along the normal
		return -dotp-d;
	}

	/**
	 *	Project a point on the plane along the plane normal
	 * REM : the projected point is the nearest point of the plane to this point
	 *
	 * @param p : the point to project
	 * @param dest : the projected point
	 */
	inline void projectOnPlane(Vector3& p, Vector3& dest)
	{
		float dotp = p.dot(this->n);	//the distance of the point to the plane along the plane normal

		dest = p - (this->n*dotp) - (this->n*d);
	}

	/**
	 *	Compute the distance of the point projected on the plane along an arbitrary direction
	 *
	 * @param origin : the point to project
	 * @param direction : the direction of projection
	 * @return : the distance
	 */
	inline float intersect_value(Vector3& origin, Vector3& direction)
	{
		float numer = this->n.dot(origin) + d;
		float denom = this->n.dot(direction);
		return (numer/denom);	//REM : denom may be equal 0
	}

	/**
	 *	Compute the projected point on the plane along an arbitrary direction
	 *
	 * @param origin : the point to project
	 * @param direction : the direction of projection
	 * @param dest : the projected point
	 */
	inline void intersect(Vector3& origin, Vector3& direction, Vector3& dest)
	{
		float numer = this->n.dot(origin) + d;
		float denom = this->n.dot(direction);
		dest = origin - direction*(numer/denom);	//REM : denom may be equal 0
	}

};


extern const Plane plane_x;
extern const Plane plane_y;
extern const Plane plane_z;


#endif
