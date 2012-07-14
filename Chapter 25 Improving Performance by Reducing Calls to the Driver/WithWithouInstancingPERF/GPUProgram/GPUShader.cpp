
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

#include "GPUShader.h"

#include <iostream>
#include "string.h"


#define FRAGMENT_PROGRAM_STRING		"fragment_program"
#define VERTEX_PROGRAM_STRING		"vertex_program"
#define GEOMETRY_PROGRAM_STRING		"geometry_program"


GPUShader::GPUShader(ShaderType shaderType)
{
	this->shaderType = shaderType;
	this->id = glCreateShaderObjectARB(shaderType);
}

GPUShader::~GPUShader()
{
	this->unload();
}

GPUShader::GPUShader(GPUShader&)
{
	//not used
}

void GPUShader::compile(const char* program)
{
	//compile the shader
	GLint sourceLen = strlen(program);
	glShaderSourceARB(this->id, 1, &program, &sourceLen);
	glCompileShaderARB(this->id);

	if(!this->getCompileStatus())
	{
		int maxLength=0;
		glGetObjectParameterivARB(this->id, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
		GLcharARB* pInfoLog= new GLcharARB[maxLength];
		int length;
		glGetInfoLogARB(this->id, maxLength, &length, pInfoLog);
		std::cout << "Error, Shader compilation failed for " << (this->shaderType==GPU_GEOMETRY_SHADER?GEOMETRY_PROGRAM_STRING:(this->shaderType==GPU_VERTEX_SHADER?VERTEX_PROGRAM_STRING:FRAGMENT_PROGRAM_STRING))
			<< " :'" << this->id << "' :" << std::endl << std::endl << pInfoLog << std::endl;
		delete[] pInfoLog;
	}
	else
	{
		int maxLength=0;
		glGetObjectParameterivARB(this->id, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
		GLcharARB* pInfoLog= new GLcharARB[maxLength];
		int length=0;
		glGetInfoLogARB(this->id, maxLength, &length, pInfoLog);
		if(length>0)
		    std::cout << "Warning for " << (this->shaderType==GPU_GEOMETRY_SHADER?GEOMETRY_PROGRAM_STRING:(this->shaderType==GPU_VERTEX_SHADER?VERTEX_PROGRAM_STRING:FRAGMENT_PROGRAM_STRING))
			<< " :'" << this->id << "' :" << pInfoLog << std::endl<< std::endl<<program<< std::endl<< std::endl;
		delete[] pInfoLog;
	}
}

void GPUShader::unload()
{
	glDeleteObjectARB(this->id);
}

GLhandleARB GPUShader::getID()
{
	return this->id;
}

bool GPUShader::getCompileStatus()
{
	int ret = 0;
	glGetObjectParameterivARB(this->id, GL_OBJECT_COMPILE_STATUS_ARB, &ret);
	return (ret==GL_TRUE);
}

ShaderType GPUShader::getShaderType()
{
	return this->shaderType;
}
