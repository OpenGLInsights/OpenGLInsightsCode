
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

#ifndef LOAD_TEXTURES_FUNCTIONS
#define LOAD_TEXTURES_FUNCTIONS


#include "stdio.h"

#include "GL/glew.h"
#include "GL/glut.h"



/**
 * Load a texture from a file to GPU memory
 * @param filepath: the path to a bitmap file
 * @param texture: the texture id
 * @return true if everything has been loaded
 */
bool loadTexture(const char* filepath, GLuint* texture);

/**
 *	Free memory used by a teture
 * @param s: the skeleton to delete
 */
void unloadTexture(GLuint* texture);







#endif

