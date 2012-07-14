
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

#ifndef OGLTOOLS_ORIENTPOS
#define OGLTOOLS_ORIENTPOS

#include "MathLib.h"

/**
 *	This class represent the position and orientation of a objet in a 3D virtual environment
 *
 * @author Hillaire Sébastien
 */
class OrientPos
{
private:

	/**
	 *
	 */
	void genLocalBaseFromYPRAngles();

protected:

	/**
	 *	yaw angle
	 */
	float yaw;
	/**
	 *	pitch angle
	 */
	float pitch;
	/**
	 *	roll angle
	 */
	float roll;
	/**
	 *	position
	 */
	Vector3 position;
	/**
	 *	left vector
	 */
	Vector3 X;
	/**
	 *	up vector
	 */
	Vector3 Y;
	/**
	 *	forward vector
	 */
	Vector3 Z;

public:

	/**
	 *	Constructor
	 */
	OrientPos();
	/**
	 *	Destructor
	 */
	~OrientPos();


	/**
	 * @param v : position
	 */
	inline void setPosition(Vector3& v)
	{
		this->position = v;
	}
	/**
	 * @param yaw : yaw angle
	 * @param pitch : pitch angle
	 * @param roll : roll angle
	 */
	inline void setOrientation(float yaw, float pitch, float roll)
	{
		this->yaw = yaw;
		this->pitch = pitch;
		this->roll = roll;
		this->genLocalBaseFromYPRAngles();
	}
	/**
	 * @param yaw : yaw angle
	 */
	inline void setYaw(float yaw)
	{
		this->yaw = yaw;
		this->genLocalBaseFromYPRAngles();
	}
	/**
	 * @param pitch : pitch angle
	 */
	inline void setPitch(float pitch)
	{
		this->pitch = pitch;
		this->genLocalBaseFromYPRAngles();
	}
	/**
	 * @param roll : roll angle
	 */
	inline void setRoll(float roll)
	{
		this->roll = roll;
		this->genLocalBaseFromYPRAngles();
	}


	/**
	 * @param v : the position
	 */
	inline void getPosition(Vector3& v)
	{
		v = this->position;
	}
	/**
	 * @param yaw : yaw angle
	 * @param pitch : pitch angle
	 * @param roll : roll angle
	 */
	inline void getOrientation(float& yaw, float& pitch, float& roll)
	{
		yaw = this->yaw;
		pitch = this->pitch;
		roll = this->roll;
	}
	/**
	 * @param yaw : yaw angle
	 */
	inline void getYaw(float& yaw)
	{
		yaw = this->yaw;
	}
	/**
	 * @param pitch : pitch angle
	 */
	inline void getPitch(float& pitch)
	{
		pitch = this->pitch;
	}
	/**
	 * @param roll : roll angle
	 */
	inline void getRoll(float& roll)
	{
		roll = this->roll;
	}

	
	/**
	 *	Transform v from global space into this local space
	 * @param v : the point to transform (param and result)
	 */
	void transformIntoLocalSpace(Vector3& v);
	/**
	 *	Transform back  v from this local space to global space
	 * @param v : the point to transform (param and result)
	 */
	void transformBackFromLocalSpace(Vector3& v);

	void renderDebug();
	void multOpenGLMatrix();
};

#endif
