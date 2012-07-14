
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

#include "OrientPos.h"
#include "gl.h"



OrientPos::OrientPos()
{
	this->yaw = 0.0f;
	this->pitch = 0.0f;
	this->roll = 0.0f;
	this->position.zero();
	this->X	= vec3_Xaxis;
	this->Y	= vec3_Yaxis;
	this->Z	= vec3_Zaxis;
}
OrientPos::~OrientPos()
{
}


void OrientPos::genLocalBaseFromYPRAngles()
{
	Matrix4 mat;
	mat.genTransformMat(this->pitch,this->yaw,this->roll);
	this->X.set(mat[0],mat[4],mat[8]);
	this->Y.set(mat[1],mat[5],mat[9]);
	this->Z.set(mat[2],mat[6],mat[10]);
}
void OrientPos::transformIntoLocalSpace(Vector3& v)
{
	Vector3 d = v - this->position;
	v.set( this->X.dot(d), this->Y.dot(d), this->Z.dot(d));
}
void OrientPos::transformBackFromLocalSpace(Vector3& v)
{
	v = this->position + this->X*v.x + this->Y*v.y + this->Z*v.z;
}




void OrientPos::multOpenGLMatrix()
{
	Matrix4 mt, mr;
	mt.setTranslation(this->position.x,this->position.y,this->position.z);
	mr.genTransformMat(-this->pitch,-this->yaw,-this->roll);
	mt = mt * mr;
	//apply camera transformation matrix
	glMultMatrixf(mt.toFloatPtr());
}

void OrientPos::renderDebug()
{
	glBegin(GL_LINES);
	glColor3f(1,0,0);
	glVertex3f(this->position.x,this->position.y,this->position.z);
	glVertex3f(this->position.x+this->X.x,this->position.y+this->X.y,this->position.z+this->X.z);
	glColor3f(0,1,0);
	glVertex3f(this->position.x,this->position.y,this->position.z);
	glVertex3f(this->position.x+this->Y.x,this->position.y+this->Y.y,this->position.z+this->Y.z);
	glColor3f(0,0,1);
	glVertex3f(this->position.x,this->position.y,this->position.z);
	glVertex3f(this->position.x+this->Z.x,this->position.y+this->Z.y,this->position.z+this->Z.z);
	glEnd();
}


