
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

#ifndef TEST_INSTANCING
#define TEST_INSTANCING

#include "GPUProgramManager.h"



#define TEST_INSTANCING_MAX_INSTANCE		8192


class TestInstancing
{
public:
	TestInstancing();
	virtual ~TestInstancing();
	void render(const unsigned int nbInstance, const float* position, const float* mvpMat, const float* mvMat, const float* pMat);
	void renderInstancing(const unsigned int nbInstance, const float* position, const float* mvpMat, const float* mvMat, const float* pMat);
private:
	const GPUProgram* gpupRenderSimple;
	GLuint gpupRenderSimple_MVP;
	const GPUProgram* gpupRenderPosition;
	GLuint gpupRenderPosition_MVP;
	GLuint gpupRenderPosition_Position;
	const GPUProgram* gpupRenderPositionInstancing;
	GLuint gpupRenderPositionInstancing_MVP;

	GLuint eabo;
	GLuint vertexVBO;
	GLuint positionVBO;
	GLuint vao;
	GLuint vaoInstancing;
};







#endif
