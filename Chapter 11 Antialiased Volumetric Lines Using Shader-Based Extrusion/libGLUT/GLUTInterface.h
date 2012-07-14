
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



#ifndef GLUT_INTERFACE
#define GLUT_INTERFACE

#include "stdlib.h"


class GLUTInterface
{
	private:

		static GLUTInterface * currentGLUTInterface;

	protected:

		virtual void enterCurrent() =0;

		virtual void leaveCurrent() =0;

	public:

		GLUTInterface();

		virtual ~GLUTInterface();

		static GLUTInterface* getCurrentGLUTInterface(){return GLUTInterface::currentGLUTInterface;}

		void setCurrent();

		virtual void glut_reshapeWin(int largeur,int hauteur) =0;

		virtual void glut_render(void) =0;

		virtual void glut_program(void) =0;

		virtual void glut_mouseClic(int boutton,int etat,int x,int y) =0;

		virtual void glut_mouseClicMove(int x,int y) =0;

		virtual void glut_mouseMove(int x,int y) =0;

		virtual void glut_keyboard(unsigned char key,int x,int y) =0;

		virtual void glut_keyboardUp(unsigned char key,int x,int y) =0;

		virtual void glut_keyboardSpecial(int key, int x, int y) =0;

		virtual void glut_keyboardSpecialUp(int key, int x, int y) =0;
};

#endif
