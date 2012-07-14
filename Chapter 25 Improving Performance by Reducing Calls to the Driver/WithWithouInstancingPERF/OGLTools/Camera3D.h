
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

#ifndef OGLTOOLS_CAMERA3D
#define OGLTOOLS_CAMERA3D

#include "MathLib.h"
#include "OrientPos.h"

/**
 *	This class represent a free 3D camera.
 *
 * @author Hillaire Sébastien
 */
class Camera3D : public OrientPos
{
private:

	/**
	 * rotation speed over X axis (rad/pixel/ms)
	 */
	float rotSpeedX;
	/**
	 * rotation speed over Y axis (rad/pixel/ms)
	 */
	float rotSpeedY;
	/**
	 * translation speed (unit/ms)
	 */
	float tranSpeed;

	/**
	 *	Bits indicating current movements
	 */
	char movements;


protected:

public:

	/**
	 *	Constructor
	 */
	Camera3D();
	/**
	 *	Destructor
	 */
	~Camera3D();
 
	/**
	 *	Set rotation properties
	 *
	 * @param xAxis : rotation speed over X axis in rad/pixel/ms
	 * @param yAxis : rotation speed over Y axis in rad/pixel/ms
	 */
	void setRotationSpeed(float xAxis, float yAxis);
	/**
	 *	Set translation properties
	 *
	 * @param speed : translation speed in unit/ms
	 */
	void setTranslationSpeed(float speed);

	/**
	 *	Set this movement state
	 *
	 * @param b : the new state
	 */
	void setForward(bool b);
	/**
	 *	Set this movement state
	 *
	 * @param b : the new state
	 */
	void setBackward(bool b);
	/**
	 *	Set this movement state
	 *
	 * @param b : the new state
	 */
	void setLeft(bool b);
	/**
	 *	Set this movement state
	 *
	 * @param b : the new state
	 */
	void setRight(bool b);
	/**
	 *	Set this movement state
	 *
	 * @param b : the new state
	 */
	void setUp(bool b);
	/**
	 *	Set this movement state
	 *
	 * @param b : the new state
	 */
	void setDown(bool b);

	/**
	 *	Upadate camera with elapsedTime
	 *
	 * @param elapsedTime : in millisecond
	 * @param mouseOffsetX : mouse movement on screen on x axis
	 * @param mouseOffsetY : mouse movement on screen on y axis
	 */
	void update(float elapsedTime, float mouseOffsetPitch, float mouseOffsetYaw);

	/**
	 *	Mult the camera matrix with OpenGL
	 */
	void multOpenGLMatrix();
	
	void getViewMatrix(Matrix4& viewMat);
};


#endif
