
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

#include "GLUTInterface.h"


GLUTInterface * GLUTInterface::currentGLUTInterface = NULL;


GLUTInterface::GLUTInterface()
{
}

GLUTInterface::~GLUTInterface()
{
}

void GLUTInterface::setCurrent()
{
	if(currentGLUTInterface!=NULL)
	{
		currentGLUTInterface->leaveCurrent();
	}
	currentGLUTInterface = this;	//changement d'interface courante
	currentGLUTInterface->enterCurrent();
}