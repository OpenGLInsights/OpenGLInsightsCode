
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

#ifndef OGLTOOLS_OGLSHAPES
#define OGLTOOLS_OGLSHAPES



/**
 *	Render a cube with smooth normal. (front face in CCW)
 *
 * glNormal : smooth normal of vertex
 * glTexCoord : texture coordinate. The same texture for each face.
 */
void renderSmoothCube(bool emitNormal, bool emitTexCoord);

/**
 *	Render a cube with smooth normal. (front face in CCW)
 *
 * glNormal : flat normal of vertex for face
 * glTexCoord : texture coordinate. The same texture for each face.
 */
void renderFlatCube(bool emitNormal, bool emitTexCoord);

/**
 *	Render a 2D full-screen aligned quad for diuaply a texture in full screen.
 */
void renderScreenQuad();


#endif
