
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

#include "Camera3D.h"
#include "GL/gl.h"


//movement mask
const char FORWARD		= 1<<0;
const char BACKWARD		= 1<<1;
const char LEFT			= 1<<2;
const char RIGHT		= 1<<3;
const char UP			= 1<<4;
const char DOWN			= 1<<5;


Camera3D::Camera3D()
{
	this->movements = 0;
	this->tranSpeed = 0.001f;
	this->rotSpeedX = 0.0001f;
	this->rotSpeedY = 0.0001f;
}
Camera3D::~Camera3D()
{
}


void Camera3D::setForward(bool b)
{
	if(b)	this->movements |= FORWARD;
	else	this->movements &= ~FORWARD;
}
void Camera3D::setBackward(bool b)
{
	if(b)	this->movements |= BACKWARD;
	else	this->movements &= ~BACKWARD;
}
void Camera3D::setLeft(bool b)
{
	if(b)	this->movements |= LEFT;
	else	this->movements &= ~LEFT;
}
void Camera3D::setRight(bool b)
{
	if(b)	this->movements |= RIGHT;
	else	this->movements &= ~RIGHT;
}
void Camera3D::setUp(bool b)
{
	if(b)	this->movements |= UP;
	else	this->movements &= ~UP;
}
void Camera3D::setDown(bool b)
{
	if(b)	this->movements |= DOWN;
	else	this->movements &= ~DOWN;
}


void Camera3D::update(float elapsedTime, float mouseOffsetPitch, float mouseOffsetYaw)
{
	Matrix4 mat;
	float rotationPitch, rotationYaw;
	Vector3 translation(0.0f,0.0f,0.0f);

	//update angles
	this->getPitch(rotationPitch);
	this->getYaw(rotationYaw);
	rotationPitch = this->pitch +	mouseOffsetPitch * this->rotSpeedX * elapsedTime;
	rotationYaw   = this->yaw	+	mouseOffsetYaw	 * this->rotSpeedY * elapsedTime;
	//rotation matrix computation
	this->setOrientation(rotationYaw, rotationPitch, 0);

	//update position if needed
	if(this->movements)
	{
		//compute direction
		if(this->movements&FORWARD)		translation-= this->Z;	//
		if(this->movements&BACKWARD)	translation+= this->Z;	//in OpenGL, we look toward the negative z axis
		if(this->movements&LEFT)		translation-= this->X;	//
		if(this->movements&RIGHT)		translation+= this->X;	//
		if(this->movements&UP)			translation+= this->Y;
		if(this->movements&DOWN)		translation-= this->Y;
		//add translation based on speed
		if(translation.x || translation.y || translation.z )
		{
			translation.normalize();
			translation *= this->tranSpeed*elapsedTime;
			this->position += translation;
		}
	}

}

void Camera3D::multOpenGLMatrix()
{
	Matrix4 mt, mr;
	mt.setTranslation(-this->position.x,-this->position.y,-this->position.z);
	mr.genTransformMat(this->pitch,this->yaw,0);
	mt = mr * mt;
	//apply camera transformation matrix
	glMultMatrixf(mt.toFloatPtr());
}

void Camera3D::getViewMatrix(Matrix4& viewMat)
{
	Matrix4 mt, mr;
	mt.setTranslation(-this->position.x,-this->position.y,-this->position.z);
	mr.genTransformMat(this->pitch,this->yaw,0);
	viewMat = mr * mt;
}







