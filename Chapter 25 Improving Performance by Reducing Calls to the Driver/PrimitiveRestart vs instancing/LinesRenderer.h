
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

#ifndef LINES_RENDERER
#define LINES_RENDERER

#include "GPUProgramManager.h"



#define LINE_RENDERER_MAX_LINES		8192*4
#define END_OF_PRIMITIVE_ID 		999999



class LinesRenderer
{
public:
	LinesRenderer() {}
	virtual ~LinesRenderer() {}
	virtual void renderLines(const unsigned int nbLines, const float* pLines, const float* pColors, float radius, const float* mvpMat, const float* mvMat, const float* pMat, GLuint texture, float screenRatio) = 0;
private:
	LinesRenderer(LinesRenderer&);
	LinesRenderer& operator=(const LinesRenderer&);
};









// same as LinesRendererVertex but without primitve restart
class LinesRendererVertexNOPR : public LinesRenderer
{
public:
	LinesRendererVertexNOPR();
	virtual ~LinesRendererVertexNOPR();
	void renderLines(const unsigned int nbLines, const float* pLines, const float* pColors, float radius, const float* mvpMat, const float* mvMat, const float* pMat, GLuint texture, float screenRatio);
private:
	const GPUProgram* gpupLine;
	GLuint gpupLine_MVP;
	GLuint gpupLine_radius;
	GLuint gpupLine_lineColor;
	GLuint gpupLine_invScrRatio;
	GLuint gpupLine_lineTexture;
	GLuint vertex0VBO;
	GLuint vertex1VBO;
	GLuint offsetDirUvVBO;
	GLuint lineVA;
	float vertex0[LINE_RENDERER_MAX_LINES*8*3];
	float vertex1[LINE_RENDERER_MAX_LINES*8*3];
};







class LinesRendererVertex : public LinesRenderer
{
public:
	LinesRendererVertex();
	virtual ~LinesRendererVertex();
	void renderLines(const unsigned int nbLines, const float* pLines, const float* pColors, float radius, const float* mvpMat, const float* mvMat, const float* pMat, GLuint texture, float screenRatio);
private:
	const GPUProgram* gpupLine;
	GLuint gpupLine_MVP;
	GLuint gpupLine_radius;
	GLuint gpupLine_invScrRatio;
	GLuint gpupLine_lineTexture;
	GLuint linesEABO;
	GLuint vertex0VBO;
	GLuint vertex1VBO;
	GLuint colorVBO;
	GLuint offsetDirUvVBO;
	GLuint lineVA;
	float vertex0[LINE_RENDERER_MAX_LINES*8*3];
	float vertex1[LINE_RENDERER_MAX_LINES*8*3];
	float color[LINE_RENDERER_MAX_LINES*8*3];
};









// same as LinesRendererVertex but using instancing
class LinesRendererVertexINSTANCING : public LinesRenderer
{
public:
	LinesRendererVertexINSTANCING();
	virtual ~LinesRendererVertexINSTANCING();
	void renderLines(const unsigned int nbLines, const float* pLines, const float* pColors, float radius, const float* mvpMat, const float* mvMat, const float* pMat, GLuint texture, float screenRatio);
private:
	const GPUProgram* gpupLine;
	GLuint gpupLine_MVP;
	GLuint gpupLine_radius;
	GLuint gpupLine_invScrRatio;
	GLuint gpupLine_lineTexture;
	GLuint linesEABO;
	GLuint vertexVBO;
	GLuint colorVBO;
	GLuint offsetDirUvVBO;
	GLuint lineVA;
};





#endif
