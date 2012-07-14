
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
#define LINERENDERER_UNIFORM_ATTR_COLOR						3



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


LinesRendererVertexNOPR::LinesRendererVertexNOPR()
: gpupLine(NULL)
, gpupLine_MVP(0)
, gpupLine_radius(0)
, gpupLine_lineTexture(0)
, vertex0VBO(0)
, vertex1VBO(0)
, offsetDirUvVBO(0)
, lineVA(0)
{
	if(!GPUProgramManager::getInstance().loadGPUProgramFromDisk("vertexLine","./Ressources/Shaders/vertexLineNOPR.vp","./Ressources/Shaders/vertexLineNOPR.fp"))
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
	gpupLine_lineColor = gpupLine->getUniformLocation("lineColor");
	gpupLine_invScrRatio = gpupLine->getUniformLocation("invScrRatio");
	gpupLine_lineTexture = gpupLine->getUniformLocation("lineTexture");



	glGenBuffers(1, &vertex0VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex0VBO);
	glBufferData(GL_ARRAY_BUFFER, LINE_RENDERER_MAX_LINES*3*8*sizeof(float)  , NULL, GL_STREAM_DRAW);
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

LinesRendererVertexNOPR::~LinesRendererVertexNOPR()
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

void LinesRendererVertexNOPR::renderLines(const unsigned int _nbLines, const float* pLines, const float* pColors, float radius, const float* mvpMat, const float* mvMat, const float* pMat, GLuint texture, float screenRatio)
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


	glBindVertexArray(lineVA);
	for(int l=0;l<nbLines;++l)
	{
		glUniform3fv(gpupLine_lineColor,1,&pColors[l*3]);
		glDrawArrays(GL_TRIANGLE_STRIP,l*8,8);
	}
	glBindVertexArray(0);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////







































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
	glGenBuffers(1, &vertex1VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex1VBO);
	glBufferData(GL_ARRAY_BUFFER, LINE_RENDERER_MAX_LINES*3*8*sizeof(float)  , NULL, GL_STREAM_DRAW);
	glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
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
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glVertexAttribPointer(LINERENDERER_UNIFORM_ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, 0, GL_BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_POSITION);
		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_OTHERVERT);
		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_OFFSETDIR_UV);
		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_COLOR);
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
	if(colorVBO)
		glDeleteVertexArrays(1,&colorVBO);
	if(lineVA)
		glDeleteVertexArrays(1,&lineVA);
}

void LinesRendererVertex::renderLines(const unsigned int _nbLines, const float* pLines, const float* pColors, float radius, const float* mvpMat, const float* mvMat, const float* pMat, GLuint texture, float screenRatio)
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
	for(unsigned int l=0;l<nbLines;++l)
	{
		const int lA = l*6;
		const int lB = l*6 + 3;
		const int l24 = l*24;
		const float cR = pColors[l*3];
		const float cG = pColors[l*3+1];
		const float cB = pColors[l*3+2];
		color[l24   ] = cR;		color[l24+1 ] = cG;		color[l24+2 ] = cB;
		color[l24+3 ] = cR;		color[l24+4 ] = cG;		color[l24+5 ] = cB;
		color[l24+6 ] = cR;		color[l24+7 ] = cG;		color[l24+8 ] = cB;
		color[l24+9 ] = cR;		color[l24+10] = cG;		color[l24+11] = cB;
																		
		color[l24+12] = cR;		color[l24+13] = cG;		color[l24+14] = cB;
		color[l24+15] = cR;		color[l24+16] = cG;		color[l24+17] = cB;
		color[l24+18] = cR;		color[l24+19] = cG;		color[l24+20] = cB;
		color[l24+21] = cR;		color[l24+22] = cG;		color[l24+23] = cB;
	}

	//copy data into vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, vertex0VBO);
	glBufferSubData(GL_ARRAY_BUFFER,0,nbLines*8*3*sizeof(float),vertex0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex1VBO);
	glBufferSubData(GL_ARRAY_BUFFER,0,nbLines*8*3*sizeof(float),vertex1);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferSubData(GL_ARRAY_BUFFER,0,nbLines*8*3*sizeof(float),color);
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






























////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

LinesRendererVertexINSTANCING::LinesRendererVertexINSTANCING()
: gpupLine(NULL)
, gpupLine_MVP(0)
, gpupLine_radius(0)
, gpupLine_lineTexture(0)
, vertexVBO(0)
, colorVBO(0)
, offsetDirUvVBO(0)
, lineVA(0)
{
	if(!GPUProgramManager::getInstance().loadGPUProgramFromDisk("vertexLine","./Ressources/Shaders/vertexLineINSTANCING.vp","./Ressources/Shaders/vertexLineINSTANCING.fp"))
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



	glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, 2*LINE_RENDERER_MAX_LINES*3*sizeof(float)  , NULL, GL_STREAM_DRAW);
	glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, LINE_RENDERER_MAX_LINES*3*sizeof(float)  , NULL, GL_STREAM_DRAW);
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
	GLuint* trisStripElements = new GLuint[8];
	glGenBuffers(1, &linesEABO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linesEABO);
	GLuint lineID=0;
		trisStripElements[0] = 0;
		trisStripElements[1] = 1;
		trisStripElements[2] = 2;
		trisStripElements[3] = 3;
		trisStripElements[4] = 4;
		trisStripElements[5] = 5;
		trisStripElements[6] = 6;
		trisStripElements[7] = 7;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 8*sizeof(float), trisStripElements, GL_STATIC_DRAW);
	delete [] trisStripElements;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linesEABO);



	glGenVertexArrays(1, &lineVA);
    glBindVertexArray(lineVA);

		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_POSITION);
		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_OTHERVERT);
		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_OFFSETDIR_UV);
		glEnableVertexAttribArray(LINERENDERER_UNIFORM_ATTR_COLOR);

		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		glVertexAttribPointer(LINERENDERER_UNIFORM_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), GL_BUFFER_OFFSET(0));
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
		glVertexAttribPointer(LINERENDERER_UNIFORM_ATTR_OTHERVERT, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), GL_BUFFER_OFFSET(0+3*sizeof(float)));
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, offsetDirUvVBO);
		glVertexAttribPointer(LINERENDERER_UNIFORM_ATTR_OFFSETDIR_UV, 4, GL_FLOAT, GL_FALSE, 0, GL_BUFFER_OFFSET(0));
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glVertexAttribPointer(LINERENDERER_UNIFORM_ATTR_COLOR, 3, GL_FLOAT, GL_FALSE, 0, GL_BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glVertexAttribDivisor(LINERENDERER_UNIFORM_ATTR_POSITION		, 1);							//updated once per instance
		glVertexAttribDivisor(LINERENDERER_UNIFORM_ATTR_OTHERVERT		, 1);							//updated once per instance
		glVertexAttribDivisor(LINERENDERER_UNIFORM_ATTR_OFFSETDIR_UV	, 0);							//updated once per vertex
		glVertexAttribDivisor(LINERENDERER_UNIFORM_ATTR_COLOR			, 1);							//updated once per instance
	glBindVertexArray(0);
}

LinesRendererVertexINSTANCING::~LinesRendererVertexINSTANCING()
{
	if(vertexVBO)
		glDeleteBuffers(1,&vertexVBO);
	if(offsetDirUvVBO)
		glDeleteVertexArrays(1,&offsetDirUvVBO);
	if(colorVBO)
		glDeleteVertexArrays(1,&colorVBO);
	if(lineVA)
		glDeleteVertexArrays(1,&lineVA);
}

void LinesRendererVertexINSTANCING::renderLines(const unsigned int _nbLines, const float* pLines, const float* pColors, float radius, const float* mvpMat, const float* mvMat, const float* pMat, GLuint texture, float screenRatio)
{
	unsigned int nbLines = _nbLines;
	if(nbLines>LINE_RENDERER_MAX_LINES)
		nbLines=LINE_RENDERER_MAX_LINES;

	//copy data into vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferSubData(GL_ARRAY_BUFFER,0,	2*nbLines*3*sizeof(float),pLines);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferSubData(GL_ARRAY_BUFFER,0,	nbLines*3*sizeof(float),pColors);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glDisable(GL_CULL_FACE);	//no neend to cull, all triangles drawn will be visible
	
	gpupLine->bindGPUProgram();
	glUniformMatrix4fv(gpupLine_MVP, 1, GL_FALSE, mvpMat );
	glUniform1f(gpupLine_radius, radius);
	glUniform1f(gpupLine_invScrRatio, 1.0f/screenRatio);

	glUniform1i(gpupLine_lineTexture, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture);

	glBindVertexArray(lineVA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linesEABO);
	glDrawElementsInstanced(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_INT, GL_BUFFER_OFFSET(0),  nbLines);
	glBindVertexArray(0);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
