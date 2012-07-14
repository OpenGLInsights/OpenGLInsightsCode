
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

#include "GPUProgram.h"

#include <iostream>




GPUProgram::GPUProgram() :  vertexShader(GPU_VERTEX_SHADER), fragmentShader(GPU_FRAGMENT_SHADER), geometryShader(NULL)
{
	this->id = glCreateProgramObjectARB();
}

GPUProgram::~GPUProgram()
{
	this->unload();
}

GPUProgram::GPUProgram(GPUProgram&) :
  vertexShader(GPU_VERTEX_SHADER)
, fragmentShader(GPU_FRAGMENT_SHADER)
{
	//not used
}


bool GPUProgram::load(const char* vertexProgram, const char* fragmentProgram, const char* geometryProgram, int GS_inputPrimitiveType, int GS_outputPrimitiveType, int GS_maxVerticesOut)
{
	//load vertex shader
	this->vertexShader.compile(vertexProgram);
	if(vertexShader.getCompileStatus())
		glAttachObjectARB(this->id, vertexShader.getID());
	else
	    return false;
	//load fragment program
	this->fragmentShader.compile(fragmentProgram);
	if(fragmentShader.getCompileStatus())
	    glAttachObjectARB(this->id, fragmentShader.getID());
	else
	    return false;
	//load geometry program if one is specified
	if(geometryProgram!=NULL)
	{
	    this->geometryShader = new GPUShader(GPU_GEOMETRY_SHADER);
	    this->geometryShader->compile(geometryProgram);
	    if(geometryShader->getCompileStatus())
		glAttachObjectARB(this->id, geometryShader->getID());
	    else
		return false;
	
	    //specify required parameter by geometry shader
	    glProgramParameteriARB(this->id,GL_GEOMETRY_INPUT_TYPE_ARB,GS_inputPrimitiveType);
	    glProgramParameteriARB(this->id,GL_GEOMETRY_OUTPUT_TYPE_ARB,GS_outputPrimitiveType);
	    int GS_HardwareLimit_maxVerticesOut;
	    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_ARB,&GS_HardwareLimit_maxVerticesOut);
	    if(GS_HardwareLimit_maxVerticesOut<GS_maxVerticesOut)
	    {
			std::cout << GS_maxVerticesOut << "max vertices produced specified, but your graphic card is limited to "<< GS_HardwareLimit_maxVerticesOut<<"."<< std::endl;
			glProgramParameteriARB(this->id,GL_GEOMETRY_VERTICES_OUT_ARB,GS_HardwareLimit_maxVerticesOut);
	    }
	    else
			glProgramParameteriARB(this->id,GL_GEOMETRY_VERTICES_OUT_ARB,GS_maxVerticesOut);	    
	}


	//linking
	glLinkProgramARB(this->id);

	GLint linkStatus;
	glGetObjectParameterivARB(this->id, GL_OBJECT_LINK_STATUS_ARB, &linkStatus);
	if(linkStatus==GL_FALSE)
	{
		int maxLength;
		glGetObjectParameterivARB(this->id, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);

		GLcharARB* pInfoLog;
		pInfoLog = new GLcharARB[maxLength];
		int length;
		glGetInfoLogARB(this->id, maxLength, &length, pInfoLog);
		std::cout << "Error, GPU program using '"<< vertexProgram<<"' and '"<< fragmentProgram<<"', link failed : " << std::endl << pInfoLog << std::endl;
		delete[] pInfoLog;
		return false;
	}

	return true;
}

void GPUProgram::unload()
{
	vertexShader.unload();
	fragmentShader.unload();
	if(geometryShader!=NULL)
	{
	    geometryShader->unload();
	    glDetachObjectARB(this->id, geometryShader->getID());
	}
	glDetachObjectARB(this->id, vertexShader.getID());
	glDetachObjectARB(this->id, fragmentShader.getID());
	glDeleteObjectARB(this->id);
}

void GPUProgram::bindGPUProgram() const
{
	glUseProgramObjectARB(this->id);
}

void GPUProgram::unbindGPUProgram()
{
	glUseProgramObjectARB(0);
}

GLuint GPUProgram::getUniformLocation(char* uniformName) const
{
	GLuint ret;
    glUseProgramObjectARB(this->id);
	ret = glGetUniformLocationARB(this->id, uniformName);
    glUseProgramObjectARB(0);
	return ret;
}

GLuint GPUProgram::getAttributeLocation(char* attributeName) const
{
	GLuint ret;
    glUseProgramObjectARB(this->id);
	ret = glGetAttribLocation(this->id, attributeName);
    glUseProgramObjectARB(0);
	return ret;
}

void GPUProgram::setTextureToUniform(GLuint uniformLocation, int textureUnitNumber)
{
	glUniform1iARB(uniformLocation, textureUnitNumber);
}


