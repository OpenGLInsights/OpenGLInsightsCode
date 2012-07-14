
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

#include "GLUTTimeCounter.h"


GLUTTimeCounter::GLUTTimeCounter()
{
	//initialisation du temps à 0 : temps au lancement de l'application.
	this->lastResetTime = 0;
}

GLUTTimeCounter::~GLUTTimeCounter()
{
}

void GLUTTimeCounter::reset()
{
	//on récupère le temps écouler depuis le début de l'application
	this->lastResetTime = glutGet(GLUT_ELAPSED_TIME);
}

int GLUTTimeCounter::getElapsedTime()
{
	// on retourne la différence entre le temps courant et le temps du dernier reset.
	return (glutGet(GLUT_ELAPSED_TIME) - this->lastResetTime);
}

float GLUTTimeCounter::getElapsedTimef()
{
	// on retourne la différence entre le temps courant et le temps du dernier reset converti en float.
	return (float)(glutGet(GLUT_ELAPSED_TIME) - this->lastResetTime);
}

int GLUTTimeCounter::getElapsedTimeSinceAppStart()
{
	//on retourne le temps écoulé depuis le début de l'application.
	return glutGet(GLUT_ELAPSED_TIME);
}

float GLUTTimeCounter::getElapsedTimeSinceAppStartf()
{
	//on retourne le temps écoulé depuis le début de l'application converti en float.
	return (float)glutGet(GLUT_ELAPSED_TIME);
}

