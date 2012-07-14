
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

#include "LinesRenderer.h"

#define LINERENDERER_UNIFORM_ATTR_POSITION					0
#define LINERENDERER_UNIFORM_ATTR_OTHERVERT					1
#define LINERENDERER_UNIFORM_ATTR_OFFSETDIR_UV				2



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////



LinesRendererSimple::LinesRendererSimple()
: gpupLine(NULL)
, gpupLine_MVP(0)
, lineVBO(0)
, lineVA(0)
{
	if(!GPUProgramManager::getInstance().loadGPUProgramFromDisk("simpleLine","./Ressources/Shaders/simpleLine.vp","./Ressources/Shaders/simpleLine.fp"))
	{
		printf("GPUProgram simpleLine not loaded!");
		system("pause");
		return;
	}
	gpupLine = GPUProgramManager::getInstance().getGPUProgram("simpleLine");
	if(!gpupLine)
	{
		printf("GPUProgram simpleLine not built!");
		system("pause");
		return;
	}
	gpupLine_MVP = gpupLine->getUniformLocation("MVP");

	glGenBuffers(1, &lineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferData(GL_ARRAY_BUFFER, LINE_RENDERER_MAX_LINES*2*3*sizeof(float), NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &lineVA);
    glBindVertexArray(lineVA);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glVertexAttribPointer(LINERENDERER_UNIFORM_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, 0, GL_BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_POSITION);
	glBindVertexArray(0);
}

LinesRendererSimple::~LinesRendererSimple()
{
	if(lineVBO)
		glDeleteBuffers(1,&lineVBO);
	if(lineVA)
		glDeleteVertexArrays(1,&lineVA);
}

void LinesRendererSimple::renderLines(const unsigned int _nbLines, const float* pLines, float radius, const float* mvpMat, const float* mvMat, const float* pMat, GLuint texture, float screenRatio)
{
	unsigned int nbLines = _nbLines;
	if(nbLines>LINE_RENDERER_MAX_LINES)
		nbLines=LINE_RENDERER_MAX_LINES;

	glLineWidth(radius);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferSubData(GL_ARRAY_BUFFER,0,nbLines*2*3*sizeof(float),pLines);		//send data to the graphic card
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	gpupLine->bindGPUProgram();
	glUniformMatrix4fv(gpupLine_MVP, 1, GL_FALSE, mvpMat );

	glBindVertexArray(lineVA);
	glDrawArraysInstanced(GL_LINES, 0, nbLines*2, 1);							//render lines as usual
	glBindVertexArray(0);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////



LinesRendererVertex::LinesRendererVertex()
: gpupLine(NULL)
, gpupLine_MVP(0)
, gpupLine_radius(0)
, gpupLine_lineTexture(0)
, vertex0VBO(0)
, vertex1VBO(0)
, offsetDirUvVBO(0)
, lineVA(0)
{
	if(!GPUProgramManager::getInstance().loadGPUProgramFromDisk("vertexLine","./Ressources/Shaders/vertexLine.vp","./Ressources/Shaders/vertexLine.fp"))
	{
		printf("GPUProgram vertexLine not loaded!");
		system("pause");
		return;
	}
	gpupLine = GPUProgramManager::getInstance().getGPUProgram("vertexLine");
	if(!gpupLine)
	{
		printf("GPUProgram vertexLine not built!");
		system("pause");
		return;
	}
	gpupLine_MVP = gpupLine->getUniformLocation("MVP");
	gpupLine_radius = gpupLine->getUniformLocation("radius");
	gpupLine_invScrRatio = gpupLine->getUniformLocation("invScrRatio");
	gpupLine_lineTexture = gpupLine->getUniformLocation("lineTexture");



	glGenBuffers(1, &vertex0VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex0VBO);
	glBufferData(GL_ARRAY_BUFFER, LINE_RENDERER_MAX_LINES*3*8*sizeof(float)  , NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &vertex1VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex1VBO);
	glBufferData(GL_ARRAY_BUFFER, LINE_RENDERER_MAX_LINES*3*8*sizeof(float)  , NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);



	float* offsetDirUv = new float[LINE_RENDERER_MAX_LINES*4*8];	// will contains {xy projection space offsets, uv}
	for(int v=0; v<LINE_RENDERER_MAX_LINES*4*8; v+=4*8)
	{
		offsetDirUv[v   ] = 1.0f;		offsetDirUv[v+1 ] = 1.0f;		offsetDirUv[v+2 ] = 1.0f;		offsetDirUv[v+3 ] = 0.0f;
		offsetDirUv[v+4 ] = 1.0f;		offsetDirUv[v+5 ] =-1.0f;		offsetDirUv[v+6 ] = 1.0f;		offsetDirUv[v+7 ] = 1.0f;
		offsetDirUv[v+8 ] = 0.0f;		offsetDirUv[v+9 ] = 1.0f;		offsetDirUv[v+10] = 0.5f;		offsetDirUv[v+11] = 0.0f;
		offsetDirUv[v+12] = 0.0f;		offsetDirUv[v+13] =-1.0f;		offsetDirUv[v+14] = 0.5f;		offsetDirUv[v+15] = 1.0f;
		offsetDirUv[v+16] = 0.0f;		offsetDirUv[v+17] =-1.0f;		offsetDirUv[v+18] = 0.5f;		offsetDirUv[v+19] = 0.0f;
		offsetDirUv[v+20] = 0.0f;		offsetDirUv[v+21] = 1.0f;		offsetDirUv[v+22] = 0.5f;		offsetDirUv[v+23] = 1.0f;
		offsetDirUv[v+24] = 1.0f;		offsetDirUv[v+25] =-1.0f;		offsetDirUv[v+26] = 0.0f;		offsetDirUv[v+27] = 0.0f;
		offsetDirUv[v+28] = 1.0f;		offsetDirUv[v+29] = 1.0f;		offsetDirUv[v+30] = 0.0f;		offsetDirUv[v+31] = 1.0f;
	}
	glGenBuffers(1, &offsetDirUvVBO);
    glBindBuffer(GL_ARRAY_BUFFER, offsetDirUvVBO);
	glBufferData(GL_ARRAY_BUFFER, LINE_RENDERER_MAX_LINES*4*8*sizeof(float)  , offsetDirUv, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete [] offsetDirUv;



	//striped triangles for lines rendering with primitive restart
	GLuint* trisStripElements = new GLuint[LINE_RENDERER_MAX_LINES*8 + LINE_RENDERER_MAX_LINES];
	glGenBuffers(1, &linesEABO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linesEABO);
	GLuint lineID=0;
	for(int t=0; t<(LINE_RENDERER_MAX_LINES*8 + LINE_RENDERER_MAX_LINES); t+=(8+1), lineID+=8)
	{
		trisStripElements[t  ] = lineID;
		trisStripElements[t+1] = lineID+1;
		trisStripElements[t+2] = lineID+2;
		trisStripElements[t+3] = lineID+3;
		trisStripElements[t+4] = lineID+4;
		trisStripElements[t+5] = lineID+5;
		trisStripElements[t+6] = lineID+6;
		trisStripElements[t+7] = lineID+7;
		trisStripElements[t+8] = END_OF_PRIMITIVE_ID;
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (LINE_RENDERER_MAX_LINES*8 + LINE_RENDERER_MAX_LINES)*sizeof(float), trisStripElements, GL_STATIC_DRAW);
	delete [] trisStripElements;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linesEABO);



	glGenVertexArrays(1, &lineVA);
    glBindVertexArray(lineVA);
		glBindBuffer(GL_ARRAY_BUFFER, vertex0VBO);
		glVertexAttribPointer(LINERENDERER_UNIFORM_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, 0, GL_BUFFER_OFFSET(0));
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, vertex1VBO);
		glVertexAttribPointer(LINERENDERER_UNIFORM_ATTR_OTHERVERT, 3, GL_FLOAT, GL_FALSE, 0, GL_BUFFER_OFFSET(0));
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, offsetDirUvVBO);
		glVertexAttribPointer(LINERENDERER_UNIFORM_ATTR_OFFSETDIR_UV, 4, GL_FLOAT, GL_FALSE, 0, GL_BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_POSITION);
		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_OTHERVERT);
		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_OFFSETDIR_UV);
	glBindVertexArray(0);
}

LinesRendererVertex::~LinesRendererVertex()
{
	if(vertex0VBO)
		glDeleteBuffers(1,&vertex0VBO);
	if(vertex1VBO)
		glDeleteVertexArrays(1,&vertex1VBO);
	if(offsetDirUvVBO)
		glDeleteVertexArrays(1,&offsetDirUvVBO);
	if(lineVA)
		glDeleteVertexArrays(1,&lineVA);
}

void LinesRendererVertex::renderLines(const unsigned int _nbLines, const float* pLines, float radius, const float* mvpMat, const float* mvMat, const float* pMat, GLuint texture, float screenRatio)
{
	unsigned int nbLines = _nbLines;
	if(nbLines>LINE_RENDERER_MAX_LINES)
		nbLines=LINE_RENDERER_MAX_LINES;

	//transform data into vbo arrays with vertex duplication for extrusion
	for(unsigned int l=0;l<nbLines;++l)
	{
		const int lA = l*6;
		const int lB = l*6 + 3;
		const int l24 = l*24;
		vertex0[l24   ] = pLines[lA  ];		vertex0[l24+1 ] = pLines[lA+1];		vertex0[l24+2 ] = pLines[lA+2];
		vertex0[l24+3 ] = pLines[lA  ];		vertex0[l24+4 ] = pLines[lA+1];		vertex0[l24+5 ] = pLines[lA+2];
		vertex0[l24+6 ] = pLines[lA  ];		vertex0[l24+7 ] = pLines[lA+1];		vertex0[l24+8 ] = pLines[lA+2];
		vertex0[l24+9 ] = pLines[lA  ];		vertex0[l24+10] = pLines[lA+1];		vertex0[l24+11] = pLines[lA+2];
		
		vertex0[l24+12] = pLines[lB  ];		vertex0[l24+13] = pLines[lB+1];		vertex0[l24+14] = pLines[lB+2];
		vertex0[l24+15] = pLines[lB  ];		vertex0[l24+16] = pLines[lB+1];		vertex0[l24+17] = pLines[lB+2];
		vertex0[l24+18] = pLines[lB  ];		vertex0[l24+19] = pLines[lB+1];		vertex0[l24+20] = pLines[lB+2];
		vertex0[l24+21] = pLines[lB  ];		vertex0[l24+22] = pLines[lB+1];		vertex0[l24+23] = pLines[lB+2];
	}
	for(unsigned int l=0;l<nbLines;++l)
	{
		const int lA = l*6;
		const int lB = l*6 + 3;
		const int l24 = l*24;
		vertex1[l24   ] = pLines[lB  ];		vertex1[l24+1 ] = pLines[lB+1];		vertex1[l24+2 ] = pLines[lB+2];
		vertex1[l24+3 ] = pLines[lB  ];		vertex1[l24+4 ] = pLines[lB+1];		vertex1[l24+5 ] = pLines[lB+2];
		vertex1[l24+6 ] = pLines[lB  ];		vertex1[l24+7 ] = pLines[lB+1];		vertex1[l24+8 ] = pLines[lB+2];
		vertex1[l24+9 ] = pLines[lB  ];		vertex1[l24+10] = pLines[lB+1];		vertex1[l24+11] = pLines[lB+2];
																						
		vertex1[l24+12] = pLines[lA  ];		vertex1[l24+13] = pLines[lA+1];		vertex1[l24+14] = pLines[lA+2];
		vertex1[l24+15] = pLines[lA  ];		vertex1[l24+16] = pLines[lA+1];		vertex1[l24+17] = pLines[lA+2];
		vertex1[l24+18] = pLines[lA  ];		vertex1[l24+19] = pLines[lA+1];		vertex1[l24+20] = pLines[lA+2];
		vertex1[l24+21] = pLines[lA  ];		vertex1[l24+22] = pLines[lA+1];		vertex1[l24+23] = pLines[lA+2];
	}

	//copy data into vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, vertex0VBO);
	glBufferSubData(GL_ARRAY_BUFFER,0,nbLines*8*3*sizeof(float),vertex0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex1VBO);
	glBufferSubData(GL_ARRAY_BUFFER,0,nbLines*8*3*sizeof(float),vertex1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glDisable(GL_CULL_FACE);	//no neend to cull, all triangles drawn will be visible
	
	gpupLine->bindGPUProgram();
	glUniformMatrix4fv(gpupLine_MVP, 1, GL_FALSE, mvpMat );
	glUniform1f(gpupLine_radius, radius);
	glUniform1f(gpupLine_invScrRatio, 1.0f/screenRatio);

	glUniform1i(gpupLine_lineTexture, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture);


	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(END_OF_PRIMITIVE_ID);

	glBindVertexArray(lineVA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linesEABO);
	glDrawElements(GL_TRIANGLE_STRIP, nbLines*(8+1), GL_UNSIGNED_INT, GL_BUFFER_OFFSET(0));
	glBindVertexArray(0);

	glDisable(GL_PRIMITIVE_RESTART);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////



LinesRendererGeometry::LinesRendererGeometry()
: gpupLine(NULL)
, gpupLine_mvMat(0)
, gpupLine_pMat(0)
, gpupLine_radius(0)
, gpupLine_gradientTexture(0)
, lineVBO(0)
, lineVA(0)
{
	if(!GPUProgramManager::getInstance().loadGPUProgramFromDisk("geometryLine","./Ressources/Shaders/geometryLine.vp","./Ressources/Shaders/geometryLine.fp","./Ressources/Shaders/geometryLine.gp",GL_LINES,GL_TRIANGLE_STRIP,16))
	{
		printf("GPUProgram geometryLine not loaded!");
		system("pause");
		return;
	}
	gpupLine = GPUProgramManager::getInstance().getGPUProgram("geometryLine");
	if(!gpupLine)
	{
		printf("GPUProgram geometryLine not built!");
		system("pause");
		return;
	}
	gpupLine_mvMat = gpupLine->getUniformLocation("mvMat");
	gpupLine_pMat = gpupLine->getUniformLocation("pMat");
	gpupLine_radius = gpupLine->getUniformLocation("radius");
	gpupLine_gradientTexture = gpupLine->getUniformLocation("gradientTexture");


	glGenBuffers(1, &lineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferData(GL_ARRAY_BUFFER, LINE_RENDERER_MAX_LINES*2*3*sizeof(float), NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &lineVA);
    glBindVertexArray(lineVA);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glVertexAttribPointer(LINERENDERER_UNIFORM_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, 0, GL_BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_POSITION);
	glBindVertexArray(0);
}

LinesRendererGeometry::~LinesRendererGeometry()
{
	if(lineVBO)
		glDeleteBuffers(1,&lineVBO);
	if(lineVA)
		glDeleteVertexArrays(1,&lineVA);
}

void LinesRendererGeometry::renderLines(const unsigned int _nbLines, const float* pLines, float radius, const float* mvpMat, const float* mvMat, const float* pMat, GLuint texture, float screenRatio)
{
	unsigned int nbLines = _nbLines;
	if(nbLines>LINE_RENDERER_MAX_LINES)
		nbLines=LINE_RENDERER_MAX_LINES;

	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferSubData(GL_ARRAY_BUFFER,0,nbLines*2*3*sizeof(float),pLines);		//send data to the graphic card
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	gpupLine->bindGPUProgram();
	glUniformMatrix4fv(gpupLine_mvMat, 1, GL_FALSE, mvMat);
	glUniformMatrix4fv(gpupLine_pMat, 1, GL_FALSE, pMat );
	glUniform1f(gpupLine_radius, radius);

	glUniform1i(gpupLine_gradientTexture, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture);

	//enable back face culling to avoid paying for back face shading
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glBindVertexArray(lineVA);
	glDrawArraysInstanced(GL_LINES, 0, nbLines*2, 1);							//render lines as usual and let the shader do the trick
	glBindVertexArray(0);
	
	glDisable(GL_CULL_FACE);
}









































