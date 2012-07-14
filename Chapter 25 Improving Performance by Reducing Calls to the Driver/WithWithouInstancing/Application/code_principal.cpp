
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

#include <iostream>
#include "GL/glew.h"
#include "GL\glut.h"
#include "glut_callback.h"

//#define ENABLE_FULL_SCREEN

void initOpenGL()
{
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
}

void main(int argc,char** argv)			// PRGM principal
{
	glutInit(&argc,argv);

	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);

#ifdef ENABLE_FULL_SCREEN
	//full screen required to get stable performance
    glutGameModeString( "1920x1080:32@60" );
    glutEnterGameMode();
#else
	glutInitWindowPosition(300, 300);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Anti-Aliased Volumetric Lines - Sébastien Hillaire");
#endif

	initOpenGL();
	glut_setup();

	glutMainLoop();

}
