
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

#include "OGLShapes.h"
#include "GL/gl.h"



static float cube_vertex[] = {
	-0.5f, 0.5f, 0.5f,	//x neg
	-0.5f, 0.5f,-0.5f,
	-0.5f,-0.5f,-0.5f,
	-0.5f,-0.5f, 0.5f,

	 0.5f, 0.5f, 0.5f,	//x pos
	 0.5f,-0.5f, 0.5f,
	 0.5f,-0.5f,-0.5f,
	 0.5f, 0.5f,-0.5f,

	 0.5f,-0.5f, 0.5f,	//y neg
	-0.5f,-0.5f, 0.5f,
	-0.5f,-0.5f,-0.5f,
	 0.5f,-0.5f,-0.5f,

	 0.5f, 0.5f, 0.5f,	//y pos
	 0.5f, 0.5f,-0.5f,
	-0.5f, 0.5f,-0.5f,
	-0.5f, 0.5f, 0.5f,

	 0.5f, 0.5f,-0.5f,	//z neg
	 0.5f,-0.5f,-0.5f,
	-0.5f,-0.5f,-0.5f,
	-0.5f, 0.5f,-0.5f,

	 0.5f, 0.5f, 0.5f,	//z pos
	-0.5f, 0.5f, 0.5f,
	-0.5f,-0.5f, 0.5f,
	 0.5f,-0.5f, 0.5f
};

static const float n3 = 0.57735f;
static float cube_normal_smooth[] = {
	-n3, n3, n3,	//x neg
	-n3, n3,-n3,
	-n3,-n3,-n3,
	-n3,-n3, n3,

	 n3, n3, n3,	//x pos
	 n3,-n3, n3,
	 n3,-n3,-n3,
	 n3, n3,-n3,

	 n3,-n3, n3,	//y neg
	-n3,-n3, n3,
	-n3,-n3,-n3,
	 n3,-n3,-n3,

	 n3, n3, n3,	//y pos
	 n3, n3,-n3,
	-n3, n3,-n3,
	-n3, n3, n3,

	 n3, n3,-n3,	//z neg
	 n3,-n3,-n3,
	-n3,-n3,-n3,
	-n3, n3,-n3,

	 n3, n3, n3,	//z pos
	-n3, n3, n3,
	-n3,-n3, n3,
	 n3,-n3, n3
};

static float cube_normal_flat[] = {
	-1.0f, 0.0f, 0.0f,	//x neg
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,

	 1.0f, 0.0f, 0.0f,	//x pos
	 1.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, 0.0f,

	 0.0f,-1.0f, 0.0f,	//y neg
	 0.0f,-1.0f, 0.0f,
	 0.0f,-1.0f, 0.0f,
	 0.0f,-1.0f, 0.0f,

	 0.0f, 1.0f, 0.0f,	//y pos
	 0.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,
	 0.0f, 1.0f, 0.0f,

	 0.0f, 0.0f,-1.0f,	//z neg
	 0.0f, 0.0f,-1.0f,
	 0.0f, 0.0f,-1.0f,
	 0.0f, 0.0f,-1.0f,

	 0.0f, 0.0f, 1.0f,	//z pos
	 0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f,
	 0.0f, 0.0f, 1.0f
};

static float cube_texCoord[] = {
	 0.0f, 0.0f,			//x neg
	 1.0f, 0.0f,
	 1.0f, 1.0f,
	 0.0f, 1.0f,
	 
	 0.0f, 0.0f,			//x pos
	 1.0f, 0.0f,
	 1.0f, 1.0f,
	 0.0f, 1.0f,
	 
	 0.0f, 0.0f,			//y neg
	 1.0f, 0.0f,
	 1.0f, 1.0f,
	 0.0f, 1.0f,
	 
	 0.0f, 0.0f,			//y pos
	 1.0f, 0.0f,
	 1.0f, 1.0f,
	 0.0f, 1.0f,
	 
	 0.0f, 0.0f,			//z neg
	 1.0f, 0.0f,
	 1.0f, 1.0f,
	 0.0f, 1.0f,
	 
	 0.0f, 0.0f,			//z pos
	 1.0f, 0.0f,
	 1.0f, 1.0f,
	 0.0f, 1.0f
};


void renderSmoothCube(bool emitNormal, bool emitTexCoord)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, cube_vertex);
	if(emitNormal)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, cube_normal_smooth);
	}
	if(emitTexCoord)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, cube_texCoord);
	}

	glDrawArrays(GL_QUADS,0,24);

	glDisableClientState(GL_VERTEX_ARRAY);
	if(emitNormal)
	{
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	if(emitTexCoord)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}


void renderFlatCube(bool emitNormal, bool emitTexCoord)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, cube_vertex);
	if(emitNormal)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, cube_normal_flat);
	}
	if(emitTexCoord)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, cube_texCoord);
	}

	glDrawArrays(GL_QUADS,0,24);

	glDisableClientState(GL_VERTEX_ARRAY);
	if(emitNormal)
	{
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	if(emitTexCoord)
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}


void renderScreenQuad()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0,1.0,0.0,1.0,-1.0,1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(0.0f, 1.0f);
	glEnd();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}