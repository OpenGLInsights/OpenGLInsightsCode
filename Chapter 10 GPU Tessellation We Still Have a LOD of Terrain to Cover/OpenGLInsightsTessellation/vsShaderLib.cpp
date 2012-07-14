/** ----------------------------------------------------------
 * \class VSShaderLib
 *
 * Lighthouse3D
 *
 * VSShaderLib - Very Simple Shader Library
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 * This class aims at making life simpler
 * when using shaders and uniforms
 *
 * \version 0.2.1
 *		Added more attrib defs, namely
 *			tangents, bi tangents, and 4 custom
 * 
 * version 0.2.0
 *		Added methods to set uniforms
 *		Added methods to set blocks
 *		Renamed to VSShaderLib
 *
 * version 0.1.0 
 * Initial Release
 *
 * This lib requires:
 *
 * GLEW (http://glew.sourceforge.net/)
 *
 ---------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include "vsShaderLib.h"

// pre conditions are established with asserts
// if having errors using the lib switch to Debug mode
#include <assert.h>

GLenum 
VSShaderLib::spGLShaderTypes[VSShaderLib::COUNT_SHADER_TYPE] = {
								GL_VERTEX_SHADER, 
								GL_GEOMETRY_SHADER,
								GL_TESS_CONTROL_SHADER,
								GL_TESS_EVALUATION_SHADER,
								GL_FRAGMENT_SHADER};


std::string 
VSShaderLib::spStringShaderTypes[VSShaderLib::COUNT_SHADER_TYPE] = {
								"Vertex Shader",
								"Geometry Shader",
								"Tesselation Control Shader",
								"Tesselation Evaluation Shader",
								"Fragment Shader"};


std::map<std::string, VSShaderLib::UniformBlock> VSShaderLib::spBlocks;


int VSShaderLib::spBlockCount = 1;


VSShaderLib::VSShaderLib(): pProgram(0), pInited(false) {

	for (int i = 0; i < VSShaderLib::COUNT_SHADER_TYPE; ++i) {
		pShader[i] = 0;
	}
}


VSShaderLib::~VSShaderLib() {

	if (pProgram)
		glDeleteProgram(pProgram);

	for (int i = 0; i < VSShaderLib::COUNT_SHADER_TYPE; ++i) {
		if (pShader[i])
			glDeleteShader(pShader[i]);
	}
	pUniforms.clear();
}


void 
VSShaderLib::init() {

	pInited = true;
	pProgram = glCreateProgram();
}


void 
VSShaderLib::loadShader(VSShaderLib::ShaderType st, std::string fileName) {

	// init should always be called first
	assert(pInited == true);

	char *s = NULL;

	s = textFileRead(fileName);

	if (s != NULL) {
		const char * ss = s;

		pShader[st] = glCreateShader(spGLShaderTypes[st]);
		glShaderSource(pShader[st], 1, &ss,NULL);
		glAttachShader(pProgram, pShader[st]);
		glCompileShader(pShader[st]);

		free(s);
	}
}


void
VSShaderLib::prepareProgram() {

	glLinkProgram(pProgram);
	addUniforms();
	addBlocks();
}


void 
VSShaderLib::setProgramOutput(int index, std::string name) {

	glBindFragDataLocation(pProgram, index, name.c_str());
}


GLint
VSShaderLib::getProgramOutput(std::string name) {

	return glGetFragDataLocation(pProgram, name.c_str());
}


void
VSShaderLib::setVertexAttribName(VSShaderLib::AttribType at, std::string name) {

	glBindAttribLocation(pProgram,at,name.c_str());
}


GLuint
VSShaderLib::getProgramIndex() {

	return pProgram;
}


GLuint
VSShaderLib::getShaderIndex(VSShaderLib::ShaderType aType) {

	return pShader[aType];
}


void 
VSShaderLib::setBlock(std::string name, void *value) {

	assert(spBlocks.count(name) != 0);

	glBindBuffer(GL_UNIFORM_BUFFER, spBlocks[name].buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, spBlocks[name].size, value);
	glBindBuffer(GL_UNIFORM_BUFFER,0);
	
}


void 
VSShaderLib::setBlockUniform(std::string blockName, 
						std::string uniformName, 
						void *value) {

	assert(spBlocks.count(blockName) && 
		   spBlocks[blockName].uniformOffsets.count(uniformName));

	UniformBlock b;
	b = spBlocks[blockName];

	myBlockUniform bUni;
	bUni = b.uniformOffsets[uniformName];

	glBindBuffer(GL_UNIFORM_BUFFER, b.buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, bUni.offset, bUni.size, value);
	glBindBuffer(GL_UNIFORM_BUFFER,0);
}


void 
VSShaderLib::setBlockUniformArrayElement(std::string blockName, 
								std::string uniformName,
								int arrayIndex, 
								void * value) {

	assert(spBlocks.count(blockName) && 
		   spBlocks[blockName].uniformOffsets.count(uniformName));

	UniformBlock b;
	b = spBlocks[blockName];

	myBlockUniform bUni;
	bUni = b.uniformOffsets[uniformName];

	glBindBuffer(GL_UNIFORM_BUFFER, b.buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 
						bUni.offset + bUni.arrayStride * arrayIndex, 
						bUni.arrayStride, value);
	glBindBuffer(GL_UNIFORM_BUFFER,0);
}


void 
VSShaderLib::setUniform(std::string name, int value) {

//	assert(pUniforms.count(name) != 0);

	int val = value;
	myUniforms u = pUniforms[name];
	glProgramUniform1i(pProgram, u.location, val);

}


void 
VSShaderLib::setUniform(std::string name, float value) {

	assert(pUniforms.count(name) != 0);

	float val = value;
	myUniforms u = pUniforms[name];
	glProgramUniform1f(pProgram, u.location, val);
}


void 
VSShaderLib::setUniform(std::string name, void *value) {

//	assert(pUniforms.count(name) != 0);

	myUniforms u = pUniforms[name];
	switch (u.type) {
	
		// Floats
		case GL_FLOAT: 
			glProgramUniform1fv(pProgram, u.location, u.size, (const GLfloat *)value);
			break;
		case GL_FLOAT_VEC2:  
			glProgramUniform2fv(pProgram, u.location, u.size, (const GLfloat *)value);
			break;
		case GL_FLOAT_VEC3:  
			glProgramUniform3fv(pProgram, u.location, u.size, (const GLfloat *)value);
			break;
		case GL_FLOAT_VEC4:  
			glProgramUniform4fv(pProgram, u.location, u.size, (const GLfloat *)value);
			break;

		// Doubles
		case GL_DOUBLE: 
			glProgramUniform1dv(pProgram, u.location, u.size, (const GLdouble *)value);
			break;
		case GL_DOUBLE_VEC2:  
			glProgramUniform2dv(pProgram, u.location, u.size, (const GLdouble *)value);
			break;
		case GL_DOUBLE_VEC3:  
			glProgramUniform3dv(pProgram, u.location, u.size, (const GLdouble *)value);
			break;
		case GL_DOUBLE_VEC4:  
			glProgramUniform4dv(pProgram, u.location, u.size, (const GLdouble *)value);
			break;

		// Samplers, Ints and Bools
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_1D_ARRAY:
		case GL_SAMPLER_2D_ARRAY:
		case GL_SAMPLER_1D_ARRAY_SHADOW:
		case GL_SAMPLER_2D_ARRAY_SHADOW:
		case GL_SAMPLER_2D_MULTISAMPLE:
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_SAMPLER_CUBE_SHADOW:
		case GL_SAMPLER_BUFFER:
		case GL_SAMPLER_2D_RECT:
		case GL_SAMPLER_2D_RECT_SHADOW:
		case GL_INT_SAMPLER_1D:
		case GL_INT_SAMPLER_2D:
		case GL_INT_SAMPLER_3D:
		case GL_INT_SAMPLER_CUBE:
		case GL_INT_SAMPLER_1D_ARRAY:
		case GL_INT_SAMPLER_2D_ARRAY:
		case GL_INT_SAMPLER_2D_MULTISAMPLE:
		case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_INT_SAMPLER_BUFFER:
		case GL_INT_SAMPLER_2D_RECT:
		case GL_UNSIGNED_INT_SAMPLER_1D:
		case GL_UNSIGNED_INT_SAMPLER_2D:
		case GL_UNSIGNED_INT_SAMPLER_3D:
		case GL_UNSIGNED_INT_SAMPLER_CUBE:
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_BUFFER:
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
		case GL_BOOL:  
		case GL_INT : 
			glProgramUniform1iv(pProgram, u.location, u.size, (const GLint *)value);
			break;
		case GL_BOOL_VEC2:
		case GL_INT_VEC2:  
			glProgramUniform2iv(pProgram, u.location, u.size, (const GLint *)value);
			break;
		case GL_BOOL_VEC3:
		case GL_INT_VEC3:  
			glProgramUniform3iv(pProgram, u.location, u.size, (const GLint *)value);
			break;
		case GL_BOOL_VEC4:
		case GL_INT_VEC4:  
			glProgramUniform4iv(pProgram, u.location, u.size, (const GLint *)value);
			break;

		// Unsigned ints
		case GL_UNSIGNED_INT: 
			glProgramUniform1uiv(pProgram, u.location, u.size, (const GLuint *)value);
			break;
		case GL_UNSIGNED_INT_VEC2:  
			glProgramUniform2uiv(pProgram, u.location, u.size, (const GLuint *)value);
			break;
		case GL_UNSIGNED_INT_VEC3:  
			glProgramUniform3uiv(pProgram, u.location, u.size, (const GLuint *)value);
			break;
		case GL_UNSIGNED_INT_VEC4:  
			glProgramUniform4uiv(pProgram, u.location, u.size, (const GLuint *)value);
			break;

		// Float Matrices
		case GL_FLOAT_MAT2:
			glProgramUniformMatrix2fv(pProgram, u.location, u.size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT3:
			glProgramUniformMatrix3fv(pProgram, u.location, u.size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT4:
			glProgramUniformMatrix4fv(pProgram, u.location, u.size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT2x3:
			glProgramUniformMatrix2x3fv(pProgram, u.location, u.size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT2x4:
			glProgramUniformMatrix2x4fv(pProgram, u.location, u.size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT3x2:
			glProgramUniformMatrix3x2fv(pProgram, u.location, u.size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT3x4:
			glProgramUniformMatrix3x4fv(pProgram, u.location, u.size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT4x2:
			glProgramUniformMatrix4x2fv(pProgram, u.location, u.size, false, (const GLfloat *)value);
			break;
		case GL_FLOAT_MAT4x3:
			glProgramUniformMatrix4x3fv(pProgram, u.location, u.size, false, (const GLfloat *)value);
			break;

		// Double Matrices
		case GL_DOUBLE_MAT2:
			glProgramUniformMatrix2dv(pProgram, u.location, u.size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT3:
			glProgramUniformMatrix3dv(pProgram, u.location, u.size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT4:
			glProgramUniformMatrix4dv(pProgram, u.location, u.size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT2x3:
			glProgramUniformMatrix2x3dv(pProgram, u.location, u.size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT2x4:
			glProgramUniformMatrix2x4dv(pProgram, u.location, u.size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT3x2:
			glProgramUniformMatrix3x2dv(pProgram, u.location, u.size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT3x4:
			glProgramUniformMatrix3x4dv(pProgram, u.location, u.size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT4x2:
			glProgramUniformMatrix4x2dv(pProgram, u.location, u.size, false, (const GLdouble *)value);
			break;
		case GL_DOUBLE_MAT4x3:
			glProgramUniformMatrix4x3dv(pProgram, u.location, u.size, false, (const GLdouble *)value);
			break;
	}
}


std::string
VSShaderLib::getShaderInfoLog(VSShaderLib::ShaderType st) {

    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	pResult = "";

	if (pShader[st]) {
		glGetShaderiv(pShader[st], GL_INFO_LOG_LENGTH,&infologLength);

		if (infologLength > 0)
		{
			infoLog = (char *)malloc(infologLength);
			glGetShaderInfoLog(pShader[st], infologLength, &charsWritten, infoLog);
			if (charsWritten)
				pResult = infoLog;
			else
				pResult= "OK";
			free(infoLog);
		}
	}
	else
		pResult = "Shader not loaded";
	return pResult;
}


std::string
VSShaderLib::getProgramInfoLog() {

    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	pResult = "";

	if (pProgram) {

		glGetProgramiv(pProgram, GL_INFO_LOG_LENGTH,&infologLength);

		if (infologLength > 0)
		{
			infoLog = (char *)malloc(infologLength);
			glGetProgramInfoLog(pProgram, infologLength, &charsWritten, infoLog);
			pResult = infoLog;
			if (charsWritten)
				pResult = infoLog;
			else
				pResult= "OK";
			free(infoLog);
		}
	}
	return pResult;
}


bool
VSShaderLib::isProgramValid() {

	GLint b = GL_FALSE;

	if (pProgram) {
	
		glValidateProgram(pProgram);
		glGetProgramiv(pProgram, GL_VALIDATE_STATUS,&b);
	}

	return (b != GL_FALSE);
}


bool
VSShaderLib::isShaderCompiled(VSShaderLib::ShaderType aType) {

	GLint b = GL_FALSE;

	if (pShader[aType]) {
	
		glGetShaderiv(pShader[aType], GL_INFO_LOG_LENGTH, &b);
	}

	return (b != GL_FALSE);
}


bool
VSShaderLib::isProgramLinked() {

	GLint b = GL_FALSE;

	if (pProgram) {
	
		glGetProgramiv(pProgram, GL_LINK_STATUS, &b);
	}

	return (b != GL_FALSE);
}


std::string 
VSShaderLib::getAllInfoLogs() {

	std::string s;

	for (int i = 0; i < VSShaderLib::COUNT_SHADER_TYPE; ++i) {
		if (pShader[i]) {
			getShaderInfoLog((VSShaderLib::ShaderType)i);
			s += VSShaderLib::spStringShaderTypes[i] + ": " + pResult + "\n";
		}
	}

	if (pProgram) {
		getProgramInfoLog();
		s += "Program: " + pResult;
		if (isProgramValid())
			s += " - Valid\n";
		else
			s += " - Not Valid\n";
	}

	pResult = s;
	return pResult;
}


// PRIVATE METHODS

char *
VSShaderLib::textFileRead(std::string fileName) {


	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fileName != "") {
		fp = fopen(fileName.c_str(),"rt");

		if (fp != NULL) {
      
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}


void
VSShaderLib::addBlocks() {

	int count, dataSize, actualLen, activeUnif, maxUniLength;
	int uniType, uniSize, uniOffset, uniMatStride, uniArrayStride, auxSize;
	char *name, *name2;

	UniformBlock block;

	glGetProgramiv(pProgram, GL_ACTIVE_UNIFORM_BLOCKS, &count);

	for (int i = 0; i < count; ++i) {
		// Get buffers name
		glGetActiveUniformBlockiv(pProgram, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &actualLen);
		name = (char *)malloc(sizeof(char) * actualLen);
		glGetActiveUniformBlockName(pProgram, i, actualLen, NULL, name);

		if (!spBlocks.count(name)) {
			// Get buffers size
			block = spBlocks[name];
			glGetActiveUniformBlockiv(pProgram, i, GL_UNIFORM_BLOCK_DATA_SIZE, &dataSize);
			//printf("DataSize:%d\n", dataSize);
			glGenBuffers(1, &block.buffer);
			glBindBuffer(GL_UNIFORM_BUFFER, block.buffer);
			glBufferData(GL_UNIFORM_BUFFER, dataSize, NULL, GL_DYNAMIC_DRAW);
			glUniformBlockBinding(pProgram, i, spBlockCount);
			glBindBufferRange(GL_UNIFORM_BUFFER, spBlockCount, block.buffer, 0, dataSize);

			glGetActiveUniformBlockiv(pProgram, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &activeUnif);

			unsigned int *indices;
			indices = (unsigned int *)malloc(sizeof(unsigned int) * activeUnif);
			glGetActiveUniformBlockiv(pProgram, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (int *)indices);
			
			glGetProgramiv(pProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniLength);
			name2 = (char *)malloc(sizeof(char) * maxUniLength);

			for (int k = 0; k < activeUnif; ++k) {
		
				myBlockUniform bUni;

				glGetActiveUniformName(pProgram, indices[k], maxUniLength, &actualLen, name2);
				glGetActiveUniformsiv(pProgram, 1, &indices[k], GL_UNIFORM_TYPE, &uniType);
				glGetActiveUniformsiv(pProgram, 1, &indices[k], GL_UNIFORM_SIZE, &uniSize);
				glGetActiveUniformsiv(pProgram, 1, &indices[k], GL_UNIFORM_OFFSET, &uniOffset);
				glGetActiveUniformsiv(pProgram, 1, &indices[k], GL_UNIFORM_MATRIX_STRIDE, &uniMatStride);
				glGetActiveUniformsiv(pProgram, 1, &indices[k], GL_UNIFORM_ARRAY_STRIDE, &uniArrayStride);
			
				if (uniArrayStride > 0)
					auxSize = uniArrayStride * uniSize;
				
				else if (uniMatStride > 0) {

					switch(uniType) {
						case GL_FLOAT_MAT2:
						case GL_FLOAT_MAT2x3:
						case GL_FLOAT_MAT2x4:
						case GL_DOUBLE_MAT2:
						case GL_DOUBLE_MAT2x3:
						case GL_DOUBLE_MAT2x4:
							auxSize = 2 * uniMatStride;
							break;
						case GL_FLOAT_MAT3:
						case GL_FLOAT_MAT3x2:
						case GL_FLOAT_MAT3x4:
						case GL_DOUBLE_MAT3:
						case GL_DOUBLE_MAT3x2:
						case GL_DOUBLE_MAT3x4:
							auxSize = 3 * uniMatStride;
							break;
						case GL_FLOAT_MAT4:
						case GL_FLOAT_MAT4x2:
						case GL_FLOAT_MAT4x3:
						case GL_DOUBLE_MAT4:
						case GL_DOUBLE_MAT4x2:
						case GL_DOUBLE_MAT4x3:
							auxSize = 4 * uniMatStride;
							break;
					}
				}
				else
					auxSize = typeSize(uniType);

				bUni.offset = uniOffset;
				bUni.type = uniType;
				bUni.size = auxSize;
				bUni.arrayStride = uniArrayStride;

				block.uniformOffsets[name2] = bUni;


			}
			free(name2);

			block.size = dataSize;
			block.bindingIndex = spBlockCount;
			spBlocks[name] = block;
			spBlockCount++;
		}
		else
			glUniformBlockBinding(pProgram, i, spBlocks[name].bindingIndex);

	}

}


void 
VSShaderLib::addUniforms() {

	int count;
	GLsizei actualLen;
	GLint size;
	GLint uniArrayStride;
	GLenum type;
	char *name;

	int maxUniLength;
	glGetProgramiv(pProgram, GL_ACTIVE_UNIFORMS, &count);

	glGetProgramiv(pProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniLength);

	name = (char *)malloc(sizeof(char) * maxUniLength);

	unsigned int loc;
	for (int i = 0; i < count; ++i) {

		glGetActiveUniform(pProgram, i, maxUniLength, &actualLen, &size, &type, name);
		// -1 indicates that is not an active uniform, although it may be present in a
		// uniform block
		loc = glGetUniformLocation(pProgram, name);
		glGetActiveUniformsiv(pProgram, 1, &loc, GL_UNIFORM_ARRAY_STRIDE, &uniArrayStride);
		if (loc != -1)
			addUniform(name, type, size);
	}
	free(name);
}


void
VSShaderLib::addUniform(std::string name, GLenum type, unsigned int size) {

	myUniforms u;
	u.type = type;
	u.location =  glGetUniformLocation(pProgram, name.c_str());
	u.size = size;
	pUniforms[name] = u;
}


int 
VSShaderLib::typeSize(int type) {

	int s;

	switch(type) {
	
		case GL_FLOAT: 
			s = sizeof(float);
			break;
		case GL_FLOAT_VEC2:  
			s = sizeof(float)*2;
			break;
		case GL_FLOAT_VEC3:  
			s = sizeof(float)*3;
			break;
		case GL_FLOAT_VEC4:  
			s = sizeof(float)*4;
			break;

		// Doubles
		case GL_DOUBLE: 
			s = sizeof(double);
			break;
		case GL_DOUBLE_VEC2:  
			s = sizeof(double) * 2;
			break;
		case GL_DOUBLE_VEC3:  
			s = sizeof(double) * 3;
			break;
		case GL_DOUBLE_VEC4:  
			s = sizeof(double) * 4;
			break;

		// Samplers, Ints and Bools
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_1D_ARRAY:
		case GL_SAMPLER_2D_ARRAY:
		case GL_SAMPLER_1D_ARRAY_SHADOW:
		case GL_SAMPLER_2D_ARRAY_SHADOW:
		case GL_SAMPLER_2D_MULTISAMPLE:
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_SAMPLER_CUBE_SHADOW:
		case GL_SAMPLER_BUFFER:
		case GL_SAMPLER_2D_RECT:
		case GL_SAMPLER_2D_RECT_SHADOW:
		case GL_INT_SAMPLER_1D:
		case GL_INT_SAMPLER_2D:
		case GL_INT_SAMPLER_3D:
		case GL_INT_SAMPLER_CUBE:
		case GL_INT_SAMPLER_1D_ARRAY:
		case GL_INT_SAMPLER_2D_ARRAY:
		case GL_INT_SAMPLER_2D_MULTISAMPLE:
		case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_INT_SAMPLER_BUFFER:
		case GL_INT_SAMPLER_2D_RECT:
		case GL_UNSIGNED_INT_SAMPLER_1D:
		case GL_UNSIGNED_INT_SAMPLER_2D:
		case GL_UNSIGNED_INT_SAMPLER_3D:
		case GL_UNSIGNED_INT_SAMPLER_CUBE:
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_BUFFER:
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
		case GL_BOOL:  
		case GL_INT : 
			s = sizeof(int);
			break;
		case GL_BOOL_VEC2:
		case GL_INT_VEC2:  
			s = sizeof(int) * 2;
			break;
		case GL_BOOL_VEC3:
		case GL_INT_VEC3:  
			s = sizeof(int) * 3;
			break;
		case GL_BOOL_VEC4:
		case GL_INT_VEC4:  
			s = sizeof(int) * 4;
			break;

		// Unsigned ints
		case GL_UNSIGNED_INT: 
			s = sizeof(unsigned int);
			break;
		case GL_UNSIGNED_INT_VEC2:  
			s = sizeof(unsigned int) * 2;
			break;
		case GL_UNSIGNED_INT_VEC3:  
			s = sizeof(unsigned int) * 3;
			break;
		case GL_UNSIGNED_INT_VEC4:  
			s = sizeof(unsigned int) * 4;
			break;

		// Float Matrices
		case GL_FLOAT_MAT2:
			s = sizeof(float) * 4;
			break;
		case GL_FLOAT_MAT3:
			s = sizeof(float) * 9;
			break;
		case GL_FLOAT_MAT4:
			s = sizeof(float) * 16;
			break;
		case GL_FLOAT_MAT2x3:
			s = sizeof(float) * 6;
			break;
		case GL_FLOAT_MAT2x4:
			s = sizeof(float) * 8;
			break;
		case GL_FLOAT_MAT3x2:
			s = sizeof(float) * 6;
			break;
		case GL_FLOAT_MAT3x4:
			s = sizeof(float) * 12;
			break;
		case GL_FLOAT_MAT4x2:
			s = sizeof(float) * 8;
			break;
		case GL_FLOAT_MAT4x3:
			s = sizeof(float) * 12;
			break;

		// Double Matrices
		case GL_DOUBLE_MAT2:
			s = sizeof(double) * 4;
			break;
		case GL_DOUBLE_MAT3:
			s = sizeof(double) * 9;
			break;
		case GL_DOUBLE_MAT4:
			s = sizeof(double) * 16;
			break;
		case GL_DOUBLE_MAT2x3:
			s = sizeof(double) * 6;
			break;
		case GL_DOUBLE_MAT2x4:
			s = sizeof(double) * 8;
			break;
		case GL_DOUBLE_MAT3x2:
			s = sizeof(double) * 6;
			break;
		case GL_DOUBLE_MAT3x4:
			s = sizeof(double) * 12;
			break;
		case GL_DOUBLE_MAT4x2:
			s = sizeof(double) * 8;
			break;
		case GL_DOUBLE_MAT4x3:
			s = sizeof(double) * 12;
			break;
		default: return 0;
	}
	return s;
}

