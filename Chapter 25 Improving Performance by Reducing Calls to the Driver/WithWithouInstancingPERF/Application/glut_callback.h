
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

#ifndef GLUT_BASE_CALLBACK
#define GLUT_BASE_CALLBACK

#include "GL/glew.h"
#include "GL/glut.h"

void glut_reshapeWin(int largeur,int hauteur);

void glut_render(void);

void glut_program(void);

void glut_mouseClic(int boutton,int etat,int x,int y);
void glut_mouseClicMove(int x,int y);
void glut_mouseMove(int x,int y);

void glut_keyboard(unsigned char key,int x,int y);
void glut_base_keyboardUp(unsigned char key,int x,int y);
void glut_keyboardSpecial(int key, int x, int y);
void glut_keyboardSpecialUp(int key, int x, int y);

void glut_setup();

void quit();

#endif