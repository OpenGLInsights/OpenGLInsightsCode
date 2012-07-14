
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

#include "TestInstancing.h"

#define LINERENDERER_UNIFORM_ATTR_VERTEX					0
#define LINERENDERER_UNIFORM_ATTR_POSITION					1
#define LINERENDERER_UNIFORM_ATTR_COLOR						2
#define LINERENDERER_UNIFORM_ATTR_MATERIAL					3



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


TestInstancing::TestInstancing()
: gpupRenderSimple(NULL)
, gpupRenderSimple_MVP(0)
, gpupRenderPosition(NULL)
, gpupRenderPosition_MVP(0)
, gpupRenderPosition_MatParam(0)
, eabo(0)
, vao(0)
, vertexVBO(0)
, positionVBO(0)
, colorVBO(0)
{
	//
	if(!GPUProgramManager::getInstance().loadGPUProgramFromDisk("renderSimple","./Ressources/Shaders/renderSimple.vp","./Ressources/Shaders/renderSimple.fp"))
	{
		printf("GPUProgram renderSimple not loaded!");
		system("pause");
		return;
	}
	gpupRenderSimple = GPUProgramManager::getInstance().getGPUProgram("renderSimple");
	if(!gpupRenderSimple)
	{
		printf("GPUProgram renderSimple not built!");
		system("pause");
		return;
	}
	gpupRenderSimple_MVP = gpupRenderSimple->getUniformLocation("MVP");
	
	//
	if(!GPUProgramManager::getInstance().loadGPUProgramFromDisk("renderPosition","./Ressources/Shaders/renderPosition.vp","./Ressources/Shaders/renderPosition.fp"))
	{
		printf("GPUProgram renderPosition not loaded!");
		system("pause");
		return;
	}
	gpupRenderPosition = GPUProgramManager::getInstance().getGPUProgram("renderPosition");
	if(!gpupRenderPosition)
	{
		printf("GPUProgram renderPosition not built!");
		system("pause");
		return;
	}
	gpupRenderPosition_MVP = gpupRenderPosition->getUniformLocation("MVP");
	gpupRenderPosition_Position = gpupRenderPosition->getUniformLocation("Position");
	gpupRenderPosition_Color = gpupRenderPosition->getUniformLocation("Color");
	gpupRenderPosition_MatParam = gpupRenderPosition->getUniformLocation("MatParam");
	
	//
	if(!GPUProgramManager::getInstance().loadGPUProgramFromDisk("renderPositionInstancing","./Ressources/Shaders/renderPositionInstance.vp","./Ressources/Shaders/renderPositionInstance.fp"))
	{
		printf("GPUProgram renderPositionInstancing not loaded!");
		system("pause");
		return;
	}
	gpupRenderPositionInstancing = GPUProgramManager::getInstance().getGPUProgram("renderPositionInstancing");
	if(!gpupRenderPositionInstancing)
	{
		printf("GPUProgram renderPositionInstancing not built!");
		system("pause");
		return;
	}
	gpupRenderPositionInstancing_MVP = gpupRenderPositionInstancing->getUniformLocation("MVP");



	GLfloat triangle[9] = {0.0f,0.0f,0.0f, 1.0f,0.0f,0.0f, 0.5f, 0.5f,0.0f};
	glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, 3*3*sizeof(GLfloat)  , triangle, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &positionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, TEST_INSTANCING_MAX_INSTANCE*3*sizeof(GLfloat)  , NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, TEST_INSTANCING_MAX_INSTANCE*3*sizeof(GLfloat)  , NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &matParamVBO);
    glBindBuffer(GL_ARRAY_BUFFER, matParamVBO);
	glBufferData(GL_ARRAY_BUFFER, TEST_INSTANCING_MAX_INSTANCE*3*sizeof(GLfloat)  , NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint triangleId[3] = {0,1,2};
	glGenBuffers(1, &eabo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eabo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*sizeof(GLuint), triangleId, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//
	glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		glVertexAttribPointer(LINERENDERER_UNIFORM_ATTR_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, GL_BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_VERTEX);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eabo);

	glBindVertexArray(0);
	//
	glGenVertexArrays(1, &vaoInstancing);
    glBindVertexArray(vaoInstancing);

		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		glVertexAttribPointer(LINERENDERER_UNIFORM_ATTR_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, GL_BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glVertexAttribPointer(LINERENDERER_UNIFORM_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, 0, GL_BUFFER_OFFSET(0));
		/*glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glVertexAttribPointer(LINERENDERER_UNIFORM_ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, 0, GL_BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, matParamVBO);
		glVertexAttribPointer(LINERENDERER_UNIFORM_ATTR_MATERIAL, 3, GL_FLOAT, GL_FALSE, 0, GL_BUFFER_OFFSET(0));*/
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_VERTEX);
		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_POSITION);
		/*glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_COLOR);
		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_MATERIAL);*/

		glVertexAttribDivisor(LINERENDERER_UNIFORM_ATTR_VERTEX		, 0);							//updated once per vertex
		glVertexAttribDivisor(LINERENDERER_UNIFORM_ATTR_POSITION	, 1);							//updated once per instance
		/*glVertexAttribDivisor(LINERENDERER_UNIFORM_ATTR_COLOR		, 1);							//updated once per instance
		glVertexAttribDivisor(LINERENDERER_UNIFORM_ATTR_MATERIAL	, 1);							//updated once per instance*/

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eabo);

	glBindVertexArray(0);
}

TestInstancing::~TestInstancing()
{
	if(positionVBO)
		glDeleteBuffers(1,&positionVBO);
	if(colorVBO)
		glDeleteBuffers(1,&colorVBO);
	if(vertexVBO)
		glDeleteBuffers(1,&vertexVBO);
	if(matParamVBO)
		glDeleteBuffers(1,&matParamVBO);
	if(eabo)
		glDeleteBuffers(1,&eabo);
	if(vao)
		glDeleteVertexArrays(1,&vao);
}

void TestInstancing::render(const unsigned int nbInstance, const float* matParam, const float* color, const float* position, const float* mvpMat, const float* mvMat, const float* pMat)
{
	if(position==NULL)
	{
		///// render triangle at the same place

	/*	gpupRenderSimple->bindGPUProgram();
		glUniformMatrix4fv(gpupRenderSimple_MVP, 1, GL_FALSE, mvpMat );

		glBindVertexArray(vao);

		for(unsigned int i=0;i<nbInstance;++i)
		{
			glDrawArrays(GL_TRIANGLES,0,3);
		}

		glBindVertexArray(0);

		GPUProgram::unbindGPUProgram();*/
	}
	else
	{
		///// render triangles with position offset

		gpupRenderPosition->bindGPUProgram();
		glUniformMatrix4fv(gpupRenderSimple_MVP, 1, GL_FALSE, mvpMat );

		glBindVertexArray(vao);

		for(unsigned int i=0;i<nbInstance;++i)
		{
			glUniform3fv(gpupRenderPosition_Position,1,&position[i*3]);
			//glUniform3fv(gpupRenderPosition_Color,1,&color[i*3]);
			//glUniform3fv(gpupRenderPosition_MatParam,1,&matParam[i*3]);

			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, GL_BUFFER_OFFSET(0));
		}

		glBindVertexArray(0);

		GPUProgram::unbindGPUProgram();
	}
}

void TestInstancing::renderInstancing(const unsigned int nbInstance, const float* matParam, const float* color, const float* position, const float* mvpMat, const float* mvMat, const float* pMat)
{
	if(position==NULL)
	{
		///// render triangle at the same place

	/*	gpupRenderSimple->bindGPUProgram();
		glUniformMatrix4fv(gpupRenderSimple_MVP, 1, GL_FALSE, mvpMat );

		glBindVertexArray(vao);

		glDrawElementsInstanced(GL_TRIANGLES, 3, GL_UNSIGNED_INT, GL_BUFFER_OFFSET(0),  nbInstance);

	//	glBindVertexArray(0);

	//	GPUProgram::unbindGPUProgram();*/
	}
	else
	{
		///// render triangles with position offset
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
		glBufferSubData(GL_ARRAY_BUFFER,0,nbInstance*3*sizeof(GLfloat),position);
		/*glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferSubData(GL_ARRAY_BUFFER,0,nbInstance*3*sizeof(GLfloat),color);
		glBindBuffer(GL_ARRAY_BUFFER, matParamVBO);
		glBufferSubData(GL_ARRAY_BUFFER,0,nbInstance*3*sizeof(GLfloat),matParam);*/
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		gpupRenderPositionInstancing->bindGPUProgram();
		glUniformMatrix4fv(gpupRenderPositionInstancing_MVP, 1, GL_FALSE, mvpMat );

		glBindVertexArray(vaoInstancing);

		glDrawElementsInstanced(GL_TRIANGLES, 3, GL_UNSIGNED_INT, GL_BUFFER_OFFSET(0),  nbInstance);

	//	glBindVertexArray(0);

	//	GPUProgram::unbindGPUProgram();
	}
}



