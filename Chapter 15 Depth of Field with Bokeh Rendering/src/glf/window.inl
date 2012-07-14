//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <GL/freeglut.h>

namespace glf
{
	//--------------------------------------------------------------------------
	inline void SwapBuffers()
	{
		glutSwapBuffers();
		glGetError(); // 'glutSwapBuffers' generates an here with OpenGL 3 > core profile ... :/
	}
}//namespace glf
