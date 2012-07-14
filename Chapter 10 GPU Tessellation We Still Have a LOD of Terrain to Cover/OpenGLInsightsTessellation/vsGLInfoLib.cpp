/** ----------------------------------------------------------
 * \class VSGLInfoLib
 *
 * Lighthouse3D
 *
 * VSGLInfoLib - Very Simple GL Information Library
 *
 *	
 * \version 0.1.0
 *  - Initial Release
 *
 * This class provides information about GL stuff
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 ---------------------------------------------------------------*/

#include "vsGLInfoLib.h"

// static local variables
std::map<int, std::string> VSGLInfoLib::spInternalF;
std::map<int, std::string> VSGLInfoLib::spDataF;
std::map<int, std::string> VSGLInfoLib::spTextureDataType;
std::map<int, std::string> VSGLInfoLib::spGLSLType;
std::map<int, int> VSGLInfoLib::spGLSLTypeSize;
std::map<int, std::string> VSGLInfoLib::spTextureFilter;
std::map<int, std::string> VSGLInfoLib::spTextureWrap;
std::map<int, std::string> VSGLInfoLib::spTextureCompFunc;
std::map<int, std::string> VSGLInfoLib::spTextureCompMode;
std::map<int, std::string> VSGLInfoLib::spTextureUnit;
std::map<int, int> VSGLInfoLib::spTextureBound;
std::map<int, std::string> VSGLInfoLib::spHint;
std::map<int, std::string> VSGLInfoLib::spTextureTarget;
std::map<int, std::string> VSGLInfoLib::spBufferAccess;
std::map<int, std::string> VSGLInfoLib::spBufferUsage;
std::map<int, std::string> VSGLInfoLib::spBufferBinding;
std::map<int, int> VSGLInfoLib::spBufferBound;
std::map<int, int> VSGLInfoLib::spBoundBuffer;
std::map<int, std::string> VSGLInfoLib::spShaderType;
std::map<int, std::string> VSGLInfoLib::spTransFeedBufferMode;
std::map<int, std::string> VSGLInfoLib::spGLSLPrimitives;
std::map<int, std::string> VSGLInfoLib::spTessGenSpacing;
std::map<int, std::string> VSGLInfoLib::spVertexOrder;
std::map<int, std::string> VSGLInfoLib::spShaderPrecision;

std::vector<unsigned int> VSGLInfoLib::spResult;
std::ostream *VSGLInfoLib::spOutS = (std::iostream *)&std::cout;
bool VSGLInfoLib::__spInit = VSGLInfoLib::init();
char VSGLInfoLib::spAux[256];


// sets the output stream
void
VSGLInfoLib::setOutputStream(std::ostream *outStream) {

	if (!outStream)
		// if null use cout
		spOutS = (std::iostream *)&std::cout;
	else
		spOutS = outStream;

}


// check if an extension is supported
bool
VSGLInfoLib::isExtensionSupported(std::string extName) {

	int max, i = 0;
	char *s;

	glGetIntegerv(GL_NUM_EXTENSIONS, &max);
	do {
		s = (char *)glGetStringi(GL_EXTENSIONS, ++i);
	}
	while (i < max && strcmp(s,extName.c_str()) != 0);

	if (i == max)
		return false;
	else
		return true;
}


// general information
void
VSGLInfoLib::getGeneralInfo() {

	int info;
	addNewLine();
	addMessage("General Information");
	addMessage("Vendor: %s", glGetString (GL_VENDOR));
	addMessage("Renderer: %s", glGetString (GL_RENDERER));
	addMessage("Version: %s", glGetString (GL_VERSION));
	addMessage("GLSL: %s", glGetString (GL_SHADING_LANGUAGE_VERSION));
	glGetIntegerv (GL_NUM_EXTENSIONS, &info);
	addMessage("Num. Extensions: %d", info);
}


/* ------------------------------------------------------

		Buffers

-------------------------------------------------------- */

// display info for currently bound buffers
void
VSGLInfoLib::getCurrentBufferInfo() {

	int info;

	addNewLine();
	addMessage("Current Buffer Bindings");

	// iterate for all buffer types
	std::map<int, int>::iterator iter = spBufferBound.begin();
	for ( ; iter != spBufferBound.end(); ++iter) {
		// get current binding for a type of buffer
		glGetIntegerv(iter->first, &info);
		// if a buffer is bound get its info
		if (info) {

			addMessage("Buffer Info for name: %d", info);
			addMessage("    Buffer Type: %s", spBufferBinding[iter->first].c_str());

			glGetBufferParameteriv(iter->second, GL_BUFFER_ACCESS, &info);
			addMessage("    Access: %s", spBufferAccess[info].c_str());

			glGetBufferParameteriv(iter->second, GL_BUFFER_MAPPED, &info);
			addMessage("    Mapped: %d", info);

			glGetBufferParameteriv(iter->second, GL_BUFFER_SIZE, &info);
			addMessage("    Size: %d", info);

			glGetBufferParameteriv(iter->second, GL_BUFFER_USAGE, &info);
			addMessage("    Usage: %s", spBufferUsage[info].c_str());
		}
	}
}


// gets all the names currently bound to buffers
std::vector<unsigned int> &
VSGLInfoLib::getBufferNames() {

	spResult.clear();
	for (unsigned int i = 0; i < 65535; ++i) {

		if (glIsBuffer(i)) {
			spResult.push_back(i);


		}
	}
	return spResult;
}


// display buffer info
void
VSGLInfoLib::getBufferInfo(GLenum target, int bufferName) {

	int info, prevBuffer;

	addNewLine();
	addMessage("Info for buffer name: %d target: %s", bufferName, spBufferBinding[spBoundBuffer[target]].c_str());

	// get previously bound buffer
	glGetIntegerv(spBoundBuffer[target], &prevBuffer);
	// bind requested buffer to get info
	glBindBuffer(target,bufferName);

	glGetBufferParameteriv(target, GL_BUFFER_ACCESS, &info);
	addMessage("    Access: %s", spBufferAccess[info].c_str());

	glGetBufferParameteriv(target, GL_BUFFER_MAPPED, &info);
	addMessage("    Mapped: %d", info);

	glGetBufferParameteriv(target, GL_BUFFER_SIZE, &info);
	addMessage("    Size: %d", info);

	glGetBufferParameteriv(target, GL_BUFFER_USAGE, &info);
	addMessage("    Usage: %s", spBufferUsage[info].c_str());

	// re-bind previous buffer
	glBindBuffer(target, prevBuffer);
}


/* ------------------------------------------------------

		Textures

-------------------------------------------------------- */


// gets all the names currently bound to textures
std::vector<unsigned int> &
VSGLInfoLib::getTextureNames() {

	spResult.clear();
	for (unsigned int i = 0; i < 65535; ++i) {

		if (glIsTexture(i))
		 spResult.push_back(i);
	}
	return spResult;
}


// gets the current texture bindings for all texture units
void 
VSGLInfoLib::getCurrentTextureInfo() {

	int info, activeUnit, anyBindings;
	std::vector<int> textures;

	// get current active unit, for later restoration
	glGetIntegerv(GL_ACTIVE_TEXTURE, &activeUnit);
	addNewLine();
	addMessage("Current Texture Bindings");
	addMessage("Currently Active Texture Unit: %s", spTextureUnit[activeUnit].c_str());

	addMessage("Texture Bindings:");
	// for each unit
	for (int i = 0; i < 8; ++i) {

		glActiveTexture(GL_TEXTURE0 + i);
		textures.clear();
		anyBindings = 0;
		std::map<int, int>::iterator iterTT;
		iterTT = spTextureBound.begin();
		for ( ; iterTT != spTextureBound.end(); ++iterTT) {

			glGetIntegerv((iterTT)->second, &info);
			textures.push_back(info);
			anyBindings |= info;
		}

		if (anyBindings) {
			addMessage("Unit %d", i);
			iterTT = spTextureBound.begin();
			for (int j = 0 ; iterTT != spTextureBound.end(); ++j, ++iterTT) {

				if (textures[j])
					addMessage("      %s: %d", spTextureTarget[(iterTT)->first].c_str(), textures[j]);
			}

		}
	}

	// restore previous active texture unit
	glActiveTexture(activeUnit);
}


// returns the name of the currently bound texture for a target
int
VSGLInfoLib::getCurrentTexture(GLenum textureTarget) {

	int t;
	glGetIntegerv(spTextureBound[textureTarget], &t);
	return(t);

}


// returns the currently active texture unit
int
VSGLInfoLib::getCurrentTextureActiveUnitInfo() {

	int activeUnit;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &activeUnit);
	return(activeUnit);

}


// sends to the stream all the texture info
void
VSGLInfoLib::getTextureInfo(GLenum textureTarget, GLenum textureID) {


	addNewLine();
	if (!glIsTexture(textureID)) { 
		addMessage("name: %d is not a texture", textureID);	
		return;
	}


	if (!glIsTexture(textureID)) {
		addMessage("ID %d does not represent a valid texture name", textureID);
		return;
	}

	addMessage("Texture Info - target: %s tex ID %d", spTextureTarget[textureTarget].c_str(), textureID);

	int prevTex = getCurrentTexture(textureTarget);
	glBindTexture(textureTarget, textureID);

	int info;

	// Dimensions
	glGetTexLevelParameteriv(textureTarget, 0, 
		GL_TEXTURE_WIDTH, &info);
	addMessage("Dimensions");
	addMessage("    Width: %d", info);

	if (textureTarget != GL_TEXTURE_1D) {
		
		glGetTexLevelParameteriv(textureTarget, 0, 
			GL_TEXTURE_HEIGHT, &info);
		addMessage("    Height: %d", info);

		if (textureTarget != GL_TEXTURE_2D) {
			glGetTexLevelParameteriv(textureTarget, 0, 
				GL_TEXTURE_DEPTH, &info);
			addMessage("    Depth: %d", info);
		}
	}

	// Internal Format
	glGetTexLevelParameteriv(textureTarget, 0, 
		GL_TEXTURE_INTERNAL_FORMAT, &info);
	addMessage("Internal Format: %s", spInternalF[info].c_str());

	// Compression info
	glGetTexLevelParameteriv(textureTarget, 0, 
		GL_TEXTURE_COMPRESSED, &info);
	// sizes
	if (info) {
		glGetIntegerv(GL_TEXTURE_COMPRESSION_HINT, &info);
		addMessage("Compression Hint: %s", spHint[info].c_str());
		glGetTexLevelParameteriv(textureTarget, 0, 
			GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &info);
		addMessage("Compressed Size: %d", info);
	}
	else { 
		addMessage("Bits per channel -");
		glGetTexLevelParameteriv(textureTarget, 0, 
								GL_TEXTURE_RED_SIZE, &info);
		if (info)
			addMessage("    Red: %d", info);
		glGetTexLevelParameteriv(textureTarget, 0, 
								GL_TEXTURE_GREEN_SIZE, &info);
		if (info)
			addMessage("    Green: %d", info);
		glGetTexLevelParameteriv(textureTarget, 0, 
								GL_TEXTURE_BLUE_SIZE, &info);
		if (info)
			addMessage("    Blue: %d", info);
		glGetTexLevelParameteriv(textureTarget, 0, 
								GL_TEXTURE_ALPHA_SIZE, &info);
		if (info)
			addMessage("    Alpha: %d", info);

		glGetTexLevelParameteriv(textureTarget, 0, 
								GL_TEXTURE_DEPTH_SIZE, &info);
		if (info)
			addMessage("    Depth: %d", info);

		glGetTexLevelParameteriv(textureTarget, 0, 
								GL_TEXTURE_STENCIL_SIZE, &info);
		if (info)
			addMessage("    Stencil: %d", info);


		glGetTexLevelParameteriv(textureTarget, 0, 
								GL_TEXTURE_RED_TYPE, &info);
			addMessage("Data Type: %s", spTextureDataType[info].c_str());
		
	}

	// Get texture sampling state
	glGetTexParameteriv(textureTarget, GL_TEXTURE_MIN_FILTER, &info);
	addMessage("Min Filter: %s", spTextureFilter[info].c_str());
	glGetTexParameteriv(textureTarget, GL_TEXTURE_MAG_FILTER, &info);
	addMessage("Mag Filter: %s", spTextureFilter[info].c_str());

	// wrapping info
	glGetTexParameteriv(textureTarget, GL_TEXTURE_WRAP_S, &info);
	addMessage("Wrap S: %s", spTextureWrap[info].c_str());
	if (textureTarget != GL_TEXTURE_1D) {
		glGetTexParameteriv(textureTarget, GL_TEXTURE_WRAP_T, &info);
		addMessage("Wrap T: %s", spTextureWrap[info].c_str());
		if (textureTarget != GL_TEXTURE_2D) {
			glGetTexParameteriv(textureTarget, GL_TEXTURE_WRAP_R, &info);
			addMessage("Wrap R: %s", spTextureWrap[info].c_str());
		}
	}

	// mipmap info
	glGetTexParameteriv(textureTarget, GL_TEXTURE_BASE_LEVEL, &info);
	addMessage("Base Level: %d", info);
	glGetTexParameteriv(textureTarget, GL_TEXTURE_MAX_LEVEL, &info);
	addMessage("Max Level: %d", info);

	// compare info
	glGetTexParameteriv(textureTarget, GL_TEXTURE_COMPARE_MODE, &info);
	addMessage("Compare Mode : %s", spTextureCompMode[info].c_str());
	if (info != GL_NONE) {
		glGetTexParameteriv(textureTarget, GL_TEXTURE_COMPARE_FUNC, &info);
		addMessage("Compare Mode : %s", spTextureCompFunc[info].c_str());

	}
	glBindTexture(textureTarget, prevTex);
}


/* ------------------------------------------------------

		GLSL

-------------------------------------------------------- */


// gets all the names currently boundo to programs
std::vector<unsigned int> &
VSGLInfoLib::getProgramNames() {

	spResult.clear();
	for (unsigned int i = 0; i < 65535; ++i) {

		if (glIsProgram(i))
		 spResult.push_back(i);
	}
	return spResult;
}


// gets all the names currently bound to Shaders
std::vector<unsigned int> &
VSGLInfoLib::getShaderNames() {

	spResult.clear();
	for (unsigned int i = 0; i < 65535; ++i) {

		if (glIsShader(i))
		 spResult.push_back(i);
	}
	return spResult;
}


// gets all the names currently bound to VAOs
std::vector<unsigned int> &
VSGLInfoLib::getVAONames() {

	spResult.clear();
	for (unsigned int i = 0; i < 65535; ++i) {

		if (glIsVertexArray(i))
		 spResult.push_back(i);
	}
	return spResult;
}


// display VAO information, including its attributes
void 
VSGLInfoLib::getVAOInfo(unsigned int buffer) {

	int count, info, prevBuffer;

	addNewLine();

	// is it a VAO?
	if (!glIsVertexArray(buffer)) { 
		addMessage("name: %d is not a VAO", buffer);	
		return;
	}

	addMessage("VAO Info for name: %d", buffer);	
	
	// bind requested VAO
	// should be able to get previously bounded VAO...
	glBindVertexArray(buffer);

	// get element array buffer name
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &info);
	if (info)
		addMessage("Element Array: %d", info);

	// get info for each attrib mapped buffer
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &count);
	for (int i = 0; i < count; ++i) {

		glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &info);
		if (info) {
			addMessage("Attrib index: %d", i);

			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &info);
			addMessage("    Buffer bound: %d", info);

			glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prevBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, info);
			glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &info);
			glBindBuffer(GL_ARRAY_BUFFER, prevBuffer);
			addMessage("    Size: %d", info);

			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &info);
			addMessage("    Components: %d", info);

			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, &info);
			addMessage("    Data Type: %s", spDataF[info].c_str());

			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &info);
			addMessage("    Stride: %d", info);

			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &info);
			addMessage("    Normalized: %d", info);

			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_DIVISOR, &info);
			addMessage("    Divisor: %d", info);

			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_INTEGER, &info);
			addMessage("    Integer: %d", info);
		}
	}
}


// display info for all active uniforms in a program
void
VSGLInfoLib::getUniformsInfo(unsigned int program) {

	addNewLine();

	// is it a program ?
	if (!glIsProgram(program)) {
		addMessage("name: %d is not a program", program);	
		return;
	}

	int activeUnif, actualLen, index, uniType, 
		uniSize, uniMatStride, uniArrayStride, uniOffset;
	char name[256];

	// Get uniforms info (not in named blocks)
	addMessage("Uniforms Info for program: %d", program);
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &activeUnif);

	for (unsigned int i = 0; i < (unsigned int)activeUnif; ++i) {
		glGetActiveUniformsiv(program, 1, &i, GL_UNIFORM_BLOCK_INDEX, &index);
		if (index == -1) {
			glGetActiveUniformName(program, i, 256, &actualLen, name);	
			glGetActiveUniformsiv(program, 1, &i, GL_UNIFORM_TYPE, &uniType);
			addMessage("%s", name);
			addMessage("    %s", spGLSLType[uniType].c_str());
			addMessage("    location: %d", i);

			glGetActiveUniformsiv(program, 1, &i, GL_UNIFORM_SIZE, &uniSize);
			glGetActiveUniformsiv(program, 1, &i, GL_UNIFORM_ARRAY_STRIDE, &uniArrayStride);

			int auxSize;
			if (uniArrayStride > 0)
				auxSize = uniArrayStride * uniSize;
			else
				auxSize = spGLSLTypeSize[uniType];

			addMessage("    size: %d", auxSize);
			if (uniArrayStride > 0)
				addMessage("    stride: %d", uniArrayStride);
		}
	}
	// Get named blocks info
	int count, dataSize, info;
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &count);

	for (int i = 0; i < count; ++i) {
		// Get blocks name
		glGetActiveUniformBlockName(program, i, 256, NULL, name);
		glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &dataSize);
		addMessage("%s\n  Size %d", name, dataSize);

		glGetActiveUniformBlockiv(program, i,  GL_UNIFORM_BLOCK_BINDING, &index);
		addMessage("  Block binding point: %d", index);
		glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, index, &info);
		addMessage("  Buffer bound to binding point: %d {", info);


		glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &activeUnif);

		unsigned int *indices;
		indices = (unsigned int *)malloc(sizeof(unsigned int) * activeUnif);
		glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (int *)indices);
			
		for (int k = 0; k < activeUnif; ++k) {
		
			glGetActiveUniformName(program, indices[k], 256, &actualLen, name);
			glGetActiveUniformsiv(program, 1, &indices[k], GL_UNIFORM_TYPE, &uniType);
			addMessage("\t%s\n\t    %s", name, spGLSLType[uniType].c_str());

			glGetActiveUniformsiv(program, 1, &indices[k], GL_UNIFORM_OFFSET, &uniOffset);
			addMessage("\t    offset: %d", uniOffset);

			glGetActiveUniformsiv(program, 1, &indices[k], GL_UNIFORM_SIZE, &uniSize);

			glGetActiveUniformsiv(program, 1, &indices[k], GL_UNIFORM_ARRAY_STRIDE, &uniArrayStride);

			glGetActiveUniformsiv(program, 1, &indices[k], GL_UNIFORM_MATRIX_STRIDE, &uniMatStride);

			int auxSize;
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
				auxSize = spGLSLTypeSize[uniType];

			auxSize = getUniformByteSize(uniSize, uniType, uniArrayStride, uniMatStride);
			addMessage("\t    size: %d", auxSize);
			if (uniArrayStride > 0)
				addMessage("\t    array stride: %d", uniArrayStride);
			if (uniMatStride > 0)
				addMessage("\t    mat stride: %d", uniMatStride);
		}
		addMessage("    }");
	}
}


// display the values for uniforms in the default block
void
VSGLInfoLib::getUniformInfo(unsigned int program, std::string uniName) {

	addNewLine();

	// is it a program ?
	if (!glIsProgram(program)) {
		addMessage("name: %d is not a program", program);	
		return;
	}

	GLenum type;
	GLsizei l;
	GLint s;
	char c[50];
	int loc = glGetUniformLocation((int)program, uniName.c_str());
	glGetActiveUniform(program, loc, 0, &l, &s, &type, c);

	if (loc != -1) {
		addMessage("Values for uniform %s in program %d", uniName.c_str(), program);
		int rows = getRows(type), columns = getColumns(type);
		if (getType(type) == FLOAT) {
			float f[16];
			glGetUniformfv(program, loc, f);
			displayUniformf(f,rows,columns);
		}
		else if (getType(type) == INT) {
			int f[16];
			glGetUniformiv(program, loc, f);
			displayUniformi(f,rows,columns);
		}
		else if (getType(type) == UNSIGNED_INT) {
			unsigned int f[16];
			glGetUniformuiv(program, loc, f);
			displayUniformui(f,rows,columns);
		}
		else if (getType(type) == DOUBLE) {
			double f[16];
			glGetUniformdv(program, loc, f);
			displayUniformd(f,rows,columns);
		}
	}
	else
	  addMessage("%s is not an active uniform in program %u", uniName.c_str(), program);
}


// display the values for a uniform in a named block
void 
VSGLInfoLib::getUniformInBlockInfo(unsigned int program, 
				std::string blockName, 
				std::string uniName) {

	addNewLine();

	// is it a program ?
	if (!glIsProgram(program)) {
		addMessage("name: %d is not a program", program);	
		return;
	}

	int index = glGetUniformBlockIndex(program, blockName.c_str());
	if (index == GL_INVALID_INDEX) {
		addMessage("%s is not a valid uniform name in block %s", uniName.c_str(), blockName.c_str());
		return;
	}
	int bindIndex,bufferIndex;
	glGetActiveUniformBlockiv(program, index,  GL_UNIFORM_BLOCK_BINDING, &bindIndex);
	addMessage("Block binding point: %d", bindIndex);
	glGetIntegeri_v(GL_UNIFORM_BUFFER_BINDING, bindIndex, &bufferIndex);
	addMessage("Buffer bound to binding point: %d ", bufferIndex);
	unsigned int uniIndex;
	const char *c = uniName.c_str();
	glGetUniformIndices(program, 1, &c, &uniIndex);
	addMessage("Index of Uniform: %u", uniIndex);
	
	int uniType, uniOffset, uniSize, uniArrayStride, uniMatStride;
	glGetActiveUniformsiv(program, 1, &uniIndex, GL_UNIFORM_TYPE, &uniType);

	glGetActiveUniformsiv(program, 1, &uniIndex, GL_UNIFORM_OFFSET, &uniOffset);

	glGetActiveUniformsiv(program, 1, &uniIndex, GL_UNIFORM_SIZE, &uniSize);

	glGetActiveUniformsiv(program, 1, &uniIndex, GL_UNIFORM_ARRAY_STRIDE, &uniArrayStride);

	glGetActiveUniformsiv(program, 1, &uniIndex, GL_UNIFORM_MATRIX_STRIDE, &uniMatStride);

	int auxSize = getUniformByteSize(uniSize, uniType, uniArrayStride, uniMatStride);
	// get previously bound buffer
	int prevBuffer;
	glGetIntegerv(spBoundBuffer[GL_UNIFORM_BUFFER], &prevBuffer);

	glBindBuffer(GL_UNIFORM_BUFFER, bufferIndex);

	int rows = getRows(uniType);
	int columns = auxSize / (rows * sizeof(float));

	if (getType(uniType) == FLOAT) {
		float f[16];
		glGetBufferSubData(GL_UNIFORM_BUFFER, uniOffset, auxSize, f);
		displayUniformf(f,rows,columns);
	}
	else if (getType(uniType) == INT) {
		int f[16];
		glGetBufferSubData(GL_UNIFORM_BUFFER, uniOffset, auxSize, f);
		displayUniformi(f,rows,columns);
	}
	else if (getType(uniType) == UNSIGNED_INT) {
		unsigned int f[16];
		glGetBufferSubData(GL_UNIFORM_BUFFER, uniOffset, auxSize, f);
		displayUniformui(f,rows,columns);
	}
	else if (getType(uniType) == DOUBLE) {
		double f[16];
		glGetBufferSubData(GL_UNIFORM_BUFFER, uniOffset, auxSize, f);
		displayUniformd(f,rows,columns);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, prevBuffer);
}


// display information for a program's attributes
void
VSGLInfoLib::getAttributesInfo(unsigned int program) {

	int activeAttr, size, loc;
	GLsizei length;
	GLenum type;
	char name[256];

	addNewLine();

	// check if it is a program
	if (!glIsProgram(program)) {
		addMessage("name: %d is not a program", program);	
		return;
	}

	addMessage("Attribute Info for program %d", program);
	// how many attribs?
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &activeAttr);
	// get location and type for each attrib
	for (unsigned int i = 0; i < (unsigned int)activeAttr; ++i) {

		glGetActiveAttrib(program,	i, 256, &length, &size, &type, name);
		loc = glGetAttribLocation(program, name);
		addMessage("%s loc: %d type: %s", name, loc, spGLSLType[type].c_str());
	}
}


// display program's information
void 
VSGLInfoLib::getProgramInfo(unsigned int program) {

	addNewLine();

	// check if name is really a program
	if (!glIsProgram(program)) {
		addMessage("Name %u is not a program", program);
		return;
	}

	addMessage("Program Information for name %u", program);
	unsigned int shaders[5];
	int count, info, linked;
	bool geom= false, tess = false;

	// Get the shader's name
	addMessage("    Shaders {");
	glGetProgramiv(program, GL_ATTACHED_SHADERS,&count);
	glGetAttachedShaders(program, count, NULL, shaders);
	for (int i = 0;  i < count; ++i) {

		glGetShaderiv(shaders[i], GL_SHADER_TYPE, &info);
		addMessage("\t%s: %d", spShaderType[info].c_str(), shaders[i]);
		if (info == GL_GEOMETRY_SHADER)
			geom = true;
		if (info == GL_TESS_EVALUATION_SHADER || info == GL_TESS_CONTROL_SHADER)
			tess = true;
	}
	addMessage("    }");

	// Get program info
	glGetProgramiv(program, GL_PROGRAM_SEPARABLE, &info);
	addMessage("    Program Separable: %d", info);
	
	glGetProgramiv(program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, &info);
	addMessage("    Program Binary Retrievable Hint: %d", info);

	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	addMessage("    Link Status: %d", linked);

	glGetProgramiv(program, GL_VALIDATE_STATUS, &info);
	addMessage("    Validate_Status: %d", info);

	glGetProgramiv(program, GL_DELETE_STATUS, &info);
	addMessage("    Delete_Status: %d", info);

	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &info);
	addMessage("    Active_Attributes: %d", info);

	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &info);
	addMessage("    Active_Uniforms: %d", info);

	glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &info);
	addMessage("    Active_Uniform_Blocks: %d", info);

#ifdef VSGL_VERSION_4_2
	glGetProgramiv(program, GL_ACTIVE_ATOMIC_COUNTER_BUFFERS, &info);
	addMessage("    Active_Atomic Counters: %d", info);
#endif
	// check if trans feed is active
	glGetProgramiv(program, GL_TRANSFORM_FEEDBACK_BUFFER_MODE, &info);
	addMessage("    Transform Feedback Buffer Mode: %s", spTransFeedBufferMode[info].c_str());

	glGetProgramiv(program, GL_TRANSFORM_FEEDBACK_VARYINGS, &info);
	addMessage("    Transform Feedback Varyings: %d", info);

	// Geometry shader info, if present
	if (geom && linked) {
		glGetProgramiv(program, GL_GEOMETRY_VERTICES_OUT, &info);
		addMessage("    Geometry Vertices Out: %d", info);

		glGetProgramiv(program, GL_GEOMETRY_INPUT_TYPE, &info);
		addMessage("    Geometry Input Type: %s", spGLSLPrimitives[info].c_str());

		glGetProgramiv(program, GL_GEOMETRY_OUTPUT_TYPE, &info);
		addMessage("    Geometry Output Type: %s", spGLSLPrimitives[info].c_str());
	
		glGetProgramiv(program, GL_GEOMETRY_SHADER_INVOCATIONS, &info);
		addMessage("    Geometry Shader Invocations: %d", info);
	}
	// tessellation shaders info, if present
	if (tess && linked) {
		glGetProgramiv(program, GL_TESS_CONTROL_OUTPUT_VERTICES, &info);
		addMessage("    Tess Control Output Vertices: %d", info);

		glGetProgramiv(program, GL_TESS_GEN_MODE, &info);
		addMessage("    Tess Gen Mode: %s", spGLSLPrimitives[info].c_str());

		glGetProgramiv(program, GL_TESS_GEN_SPACING, &info);
		addMessage("    Tess Spacing: %s", spTessGenSpacing[info].c_str());

		glGetProgramiv(program, GL_TESS_GEN_VERTEX_ORDER, &info);
		addMessage("    Tess Vertex Order: %s", spVertexOrder[info].c_str()); 

		glGetProgramiv(program, GL_TESS_GEN_POINT_MODE, &info);
		addMessage("    Tess Gen Point Mode: %d", info); 
	}
	addMessage("");
}


/* ----------------------------------------------

		private auxiliary functions

----------------------------------------------- */

// add a newline to the stream
void
VSGLInfoLib::addNewLine() {

	*spOutS << "\n";
}


// Printf style! plus a newline at the end
void 
VSGLInfoLib::addMessage(std::string format, ...) {

	va_list args;
	va_start(args,format);
	vsprintf(spAux, format.c_str(), args);
	va_end(args);

	*spOutS << spAux << "\n";
}


// init the library
// fills up the maps with enum to string
// to display human-readable messages
bool 
VSGLInfoLib::init() {

	spShaderPrecision[GL_LOW_FLOAT] = "GL_LOW_FLOAT";
	spShaderPrecision[GL_MEDIUM_FLOAT] = "GL_MEDIUM_FLOAT";
	spShaderPrecision[GL_HIGH_FLOAT] = "GL_HIGH_FLOAT";
	spShaderPrecision[GL_LOW_INT] = "GL_LOW_INT";
	spShaderPrecision[GL_MEDIUM_INT] = "GL_MEDIUM_INT";
	spShaderPrecision[GL_HIGH_INT] = "GL_HIGH_INT";

	spTessGenSpacing[GL_EQUAL] = "GL_EQUAL";
	spTessGenSpacing[GL_FRACTIONAL_EVEN] = "GL_FRACTIONAL_EVEN";
	spTessGenSpacing[GL_FRACTIONAL_ODD] = "GL_FRACTIONAL_ODD";

	spVertexOrder[GL_CCW] = "GL_CCW";
	spVertexOrder[GL_CW] = "GL_CW";

	spGLSLPrimitives[GL_QUADS] = "GL_QUADS";
	spGLSLPrimitives[GL_ISOLINES] = "GL_ISOLINES";
	spGLSLPrimitives[GL_POINTS] = "GL_POINTS";
	spGLSLPrimitives[GL_LINES] = "GL_LINES";
	spGLSLPrimitives[GL_LINES_ADJACENCY] = "GL_LINES_ADJACENCY";
	spGLSLPrimitives[GL_TRIANGLES] = "GL_TRIANGLES";
	spGLSLPrimitives[GL_LINE_STRIP] = "GL_LINE_STRIP";
	spGLSLPrimitives[GL_TRIANGLE_STRIP] = "GL_TRIANGLE_STRIP";
	spGLSLPrimitives[GL_TRIANGLES_ADJACENCY] = "GL_TRIANGLES_ADJACENCY";

	spTransFeedBufferMode[GL_SEPARATE_ATTRIBS] = "GL_SEPARATE_ATTRIBS";
	spTransFeedBufferMode[GL_INTERLEAVED_ATTRIBS] = "GL_INTERLEAVED_ATTRIBS";

	spShaderType[GL_VERTEX_SHADER] = "GL_VERTEX_SHADER";
	spShaderType[GL_GEOMETRY_SHADER] = "GL_GEOMETRY_SHADER";
	spShaderType[GL_TESS_CONTROL_SHADER] = "GL_TESS_CONTROL_SHADER";
	spShaderType[GL_TESS_EVALUATION_SHADER] = "GL_TESS_EVALUATION_SHADER";
	spShaderType[GL_FRAGMENT_SHADER] = "GL_FRAGMENT_SHADER";

	spHint[GL_FASTEST] = "GL_FASTEST";
	spHint[GL_NICEST] = "GL_NICEST";
	spHint[GL_DONT_CARE] = "GL_DONT_CARE";

	spBufferBinding[GL_ARRAY_BUFFER_BINDING] = "GL_ARRAY_BUFFER";
	spBufferBinding[GL_ELEMENT_ARRAY_BUFFER_BINDING] = "GL_ELEMENT_ARRAY_BUFFER";
	spBufferBinding[GL_PIXEL_PACK_BUFFER_BINDING] = "GL_PIXEL_PACK_BUFFER";
	spBufferBinding[GL_PIXEL_UNPACK_BUFFER_BINDING] = "GL_PIXEL_UNPACK_BUFFER";
	spBufferBinding[GL_TRANSFORM_FEEDBACK_BUFFER_BINDING] = "GL_TRANSFORM_FEEDBACK_BUFFER";
	spBufferBinding[GL_UNIFORM_BUFFER_BINDING] = "GL_UNIFORM_BUFFER";

#ifdef VSGL_VERSION_4_2
	spBufferBinding[GL_TEXTURE_BUFFER_BINDING] = "GL_TEXTURE_BUFFER";
	spBufferBinding[GL_COPY_READ_BUFFER_BINDING] = "GL_COPY_READ_BUFFER";
	spBufferBinding[GL_COPY_WRITE_BUFFER_BINDING] = "GL_COPY_WRITE_BUFFER";
	spBufferBinding[GL_DRAW_INDIRECT_BUFFER_BINDING] = "GL_DRAW_INDIRECT_BUFFER";
	spBufferBinding[GL_ATOMIC_COUNTER_BUFFER_BINDING] = "GL_ATOMIC_COUNTER_BUFFER";
#endif

	spBufferBound[GL_ARRAY_BUFFER_BINDING] = GL_ARRAY_BUFFER;
	spBufferBound[GL_ELEMENT_ARRAY_BUFFER_BINDING] = GL_ELEMENT_ARRAY_BUFFER;
	spBufferBound[GL_PIXEL_PACK_BUFFER_BINDING] = GL_PIXEL_PACK_BUFFER;
	spBufferBound[GL_PIXEL_UNPACK_BUFFER_BINDING] = GL_PIXEL_UNPACK_BUFFER;
	spBufferBound[GL_TRANSFORM_FEEDBACK_BUFFER_BINDING] = GL_TRANSFORM_FEEDBACK_BUFFER;
	spBufferBound[GL_UNIFORM_BUFFER_BINDING] = GL_UNIFORM_BUFFER;

#ifdef VSGL_VERSION_4_2
	spBufferBound[GL_TEXTURE_BUFFER_BINDING] = GL_TEXTURE_BUFFER;
	spBufferBound[GL_COPY_READ_BUFFER_BINDING] = GL_COPY_READ_BUFFER;
	spBufferBound[GL_COPY_WRITE_BUFFER_BINDING] = GL_COPY_WRITE_BUFFER;
	spBufferBound[GL_DRAW_INDIRECT_BUFFER_BINDING] = GL_DRAW_INDIRECT_BUFFER;
	spBufferBound[GL_ATOMIC_COUNTER_BUFFER_BINDING] = GL_ATOMIC_COUNTER_BUFFER;
#endif

	spBoundBuffer[GL_ARRAY_BUFFER] = GL_ARRAY_BUFFER_BINDING;
	spBoundBuffer[GL_ELEMENT_ARRAY_BUFFER] = GL_ELEMENT_ARRAY_BUFFER_BINDING;
	spBoundBuffer[GL_PIXEL_PACK_BUFFER] = GL_PIXEL_PACK_BUFFER_BINDING;
	spBoundBuffer[GL_PIXEL_UNPACK_BUFFER] = GL_PIXEL_UNPACK_BUFFER_BINDING;
	spBoundBuffer[GL_TRANSFORM_FEEDBACK_BUFFER] = GL_TRANSFORM_FEEDBACK_BUFFER_BINDING;
	spBoundBuffer[GL_UNIFORM_BUFFER] = GL_UNIFORM_BUFFER_BINDING;

#ifdef VSGL_VERSION_4_2
	spBoundBuffer[GL_TEXTURE_BUFFER] = GL_TEXTURE_BUFFER_BINDING;
	spBoundBuffer[GL_COPY_READ_BUFFER] = GL_COPY_READ_BUFFER_BINDING;
	spBoundBuffer[GL_COPY_WRITE_BUFFER] = GL_COPY_WRITE_BUFFER_BINDING;
	spBoundBuffer[GL_DRAW_INDIRECT_BUFFER] = GL_DRAW_INDIRECT_BUFFER;
	spBoundBuffer[GL_ATOMIC_COUNTER_BUFFER] = GL_ATOMIC_COUNTER_BUFFER;
#endif

	spBufferUsage[GL_STREAM_DRAW] = "GL_STREAM_DRAW";
	spBufferUsage[GL_STREAM_READ] = "GL_STREAM_READ";
	spBufferUsage[GL_STREAM_COPY] = "GL_STREAM_COPY";
	spBufferUsage[GL_STATIC_DRAW] = "GL_STATIC_DRAW";
	spBufferUsage[GL_STATIC_READ] = "GL_STATIC_READ";
	spBufferUsage[GL_STATIC_COPY] = "GL_STATIC_COPY";
	spBufferUsage[GL_DYNAMIC_DRAW] = "GL_DYNAMIC_DRAW";
	spBufferUsage[GL_DYNAMIC_READ] = "GL_DYNAMIC_READ";
	spBufferUsage[GL_DYNAMIC_COPY] = "GL_DYNAMIC_COPY";

	spBufferAccess[GL_READ_ONLY] = "GL_READ_ONLY";
	spBufferAccess[GL_WRITE_ONLY] = "GL_WRITE_ONLY";
	spBufferAccess[GL_READ_WRITE] = "GL_READ_WRITE";

	spTextureTarget[GL_TEXTURE_1D] = "GL_TEXTURE_1D";
	spTextureTarget[GL_TEXTURE_1D_ARRAY] = "GL_TEXTURE_1D_ARRAY";
	spTextureTarget[GL_TEXTURE_2D] = "GL_TEXTURE_2D";
	spTextureTarget[GL_TEXTURE_2D_ARRAY] = "GL_TEXTURE_2D_ARRAY";
	spTextureTarget[GL_TEXTURE_2D_MULTISAMPLE] = "GL_TEXTURE_2D_MULTISAMPLE";
	spTextureTarget[GL_TEXTURE_2D_MULTISAMPLE_ARRAY] = "GL_TEXTURE_2D_MULTISAMPLE_ARRAY";
	spTextureTarget[GL_TEXTURE_3D] = "GL_TEXTURE_3D";
	spTextureTarget[GL_TEXTURE_BUFFER] = "GL_TEXTURE_BUFFER";
	spTextureTarget[GL_TEXTURE_CUBE_MAP] = "GL_TEXTURE_CUBE_MAP";
	spTextureTarget[GL_TEXTURE_RECTANGLE] = "GL_TEXTURE_RECTANGLE";

	spTextureBound[GL_TEXTURE_1D] = GL_TEXTURE_BINDING_1D;
	spTextureBound[GL_TEXTURE_1D_ARRAY] = GL_TEXTURE_BINDING_1D_ARRAY;
	spTextureBound[GL_TEXTURE_2D] = GL_TEXTURE_BINDING_2D;
	spTextureBound[GL_TEXTURE_2D_ARRAY] = GL_TEXTURE_BINDING_2D_ARRAY;
	spTextureBound[GL_TEXTURE_2D_MULTISAMPLE] = GL_TEXTURE_BINDING_2D_MULTISAMPLE;
	spTextureBound[GL_TEXTURE_2D_MULTISAMPLE_ARRAY] = GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY;
	spTextureBound[GL_TEXTURE_3D] = GL_TEXTURE_BINDING_3D;
	spTextureBound[GL_TEXTURE_BUFFER] = GL_TEXTURE_BINDING_BUFFER;
	spTextureBound[GL_TEXTURE_CUBE_MAP] = GL_TEXTURE_BINDING_CUBE_MAP;
	spTextureBound[GL_TEXTURE_RECTANGLE] = GL_TEXTURE_BINDING_RECTANGLE;

	spTextureUnit[GL_TEXTURE0] = "GL_TEXTURE0";
	spTextureUnit[GL_TEXTURE1] = "GL_TEXTURE1";
	spTextureUnit[GL_TEXTURE2] = "GL_TEXTURE2";
	spTextureUnit[GL_TEXTURE3] = "GL_TEXTURE3";
	spTextureUnit[GL_TEXTURE4] = "GL_TEXTURE4";
	spTextureUnit[GL_TEXTURE5] = "GL_TEXTURE5";
	spTextureUnit[GL_TEXTURE6] = "GL_TEXTURE6";
	spTextureUnit[GL_TEXTURE7] = "GL_TEXTURE7";

	spTextureCompMode[GL_NONE] = "GL_NONE";	
	spTextureCompFunc[GL_COMPARE_REF_TO_TEXTURE] = "GL_COMPARE_REF_TO_TEXTURE";	

	spTextureCompFunc[GL_LEQUAL] = "GL_LEQUAL";	
	spTextureCompFunc[GL_GEQUAL] = "GL_GEQUAL";	
	spTextureCompFunc[GL_LESS] = "GL_LESS";	
	spTextureCompFunc[GL_GREATER] = "GL_GREATER";	
	spTextureCompFunc[GL_EQUAL] = "GL_EQUAL";	
	spTextureCompFunc[GL_NOTEQUAL] = "GL_NOTEQUAL";	
	spTextureCompFunc[GL_ALWAYS] = "GL_ALWAYS";	
	spTextureCompFunc[GL_NEVER] = "GL_NEVER";

	spTextureWrap[GL_CLAMP_TO_EDGE] = "GL_CLAMP_TO_EDGE";
	spTextureWrap[GL_CLAMP_TO_BORDER] = "GL_CLAMP_TO_BORDER";
	spTextureWrap[GL_MIRRORED_REPEAT] = "GL_MIRRORED_REPEAT"; 
	spTextureWrap[GL_REPEAT] = "GL_REPEAT";

	spTextureFilter[GL_NEAREST] = "GL_NEAREST";
	spTextureFilter[GL_LINEAR] = "GL_LINEAR";
	spTextureFilter[GL_NEAREST_MIPMAP_NEAREST] = "GL_NEAREST_MIPMAP_NEAREST";
	spTextureFilter[GL_LINEAR_MIPMAP_NEAREST] = "GL_LINEAR_MIPMAP_NEAREST";
	spTextureFilter[GL_NEAREST_MIPMAP_LINEAR] = "GL_NEAREST_MIPMAP_LINEAR";
	spTextureFilter[GL_LINEAR_MIPMAP_LINEAR] = "GL_LINEAR_MIPMAP_LINEAR";

	spGLSLTypeSize[GL_FLOAT] = sizeof(float); 
	spGLSLTypeSize[GL_FLOAT_VEC2] = sizeof(float)*2; 
	spGLSLTypeSize[GL_FLOAT_VEC3] = sizeof(float)*3; 
	spGLSLTypeSize[GL_FLOAT_VEC4] = sizeof(float)*4; 

	spGLSLTypeSize[GL_DOUBLE] = sizeof(double); 
	spGLSLTypeSize[GL_DOUBLE_VEC2] = sizeof(double)*2; 
	spGLSLTypeSize[GL_DOUBLE_VEC3] = sizeof(double)*3; 
	spGLSLTypeSize[GL_DOUBLE_VEC4] = sizeof(double)*4; 

	spGLSLTypeSize[GL_SAMPLER_1D] = sizeof(int); 
	spGLSLTypeSize[GL_SAMPLER_2D] = sizeof(int); 
	spGLSLTypeSize[GL_SAMPLER_3D] = sizeof(int); 
	spGLSLTypeSize[GL_SAMPLER_CUBE] = sizeof(int); 
	spGLSLTypeSize[GL_SAMPLER_1D_SHADOW] = sizeof(int); 
	spGLSLTypeSize[GL_SAMPLER_2D_SHADOW] = sizeof(int); 
	spGLSLTypeSize[GL_SAMPLER_1D_ARRAY] = sizeof(int); 
	spGLSLTypeSize[GL_SAMPLER_2D_ARRAY] = sizeof(int); 
	spGLSLTypeSize[GL_SAMPLER_1D_ARRAY_SHADOW] = sizeof(int); 
	spGLSLTypeSize[GL_SAMPLER_2D_ARRAY_SHADOW] = sizeof(int); 
	spGLSLTypeSize[GL_SAMPLER_2D_MULTISAMPLE] = sizeof(int); 
	spGLSLTypeSize[GL_SAMPLER_2D_MULTISAMPLE_ARRAY] = sizeof(int); 
	spGLSLTypeSize[GL_SAMPLER_CUBE_SHADOW] = sizeof(int); 
	spGLSLTypeSize[GL_SAMPLER_BUFFER] = sizeof(int); 
	spGLSLTypeSize[GL_SAMPLER_2D_RECT] = sizeof(int); 
	spGLSLTypeSize[GL_SAMPLER_2D_RECT_SHADOW] = sizeof(int); 
	spGLSLTypeSize[GL_INT_SAMPLER_1D] = sizeof(int); 
	spGLSLTypeSize[GL_INT_SAMPLER_2D] = sizeof(int); 
	spGLSLTypeSize[GL_INT_SAMPLER_3D] = sizeof(int); 
	spGLSLTypeSize[GL_INT_SAMPLER_CUBE] = sizeof(int); 
	spGLSLTypeSize[GL_INT_SAMPLER_1D_ARRAY] = sizeof(int); 
	spGLSLTypeSize[GL_INT_SAMPLER_2D_ARRAY] = sizeof(int); 
	spGLSLTypeSize[GL_INT_SAMPLER_2D_MULTISAMPLE] = sizeof(int); 
	spGLSLTypeSize[GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY] = sizeof(int); 
	spGLSLTypeSize[GL_INT_SAMPLER_BUFFER] = sizeof(int); 
	spGLSLTypeSize[GL_INT_SAMPLER_2D_RECT] = sizeof(int); 
	spGLSLTypeSize[GL_UNSIGNED_INT_SAMPLER_1D] = sizeof(int); 
	spGLSLTypeSize[GL_UNSIGNED_INT_SAMPLER_2D] = sizeof(int); 
	spGLSLTypeSize[GL_UNSIGNED_INT_SAMPLER_3D] = sizeof(int); 
	spGLSLTypeSize[GL_UNSIGNED_INT_SAMPLER_CUBE] = sizeof(int); 
	spGLSLTypeSize[GL_UNSIGNED_INT_SAMPLER_1D_ARRAY] = sizeof(int); 
	spGLSLTypeSize[GL_UNSIGNED_INT_SAMPLER_2D_ARRAY] = sizeof(int); 
	spGLSLTypeSize[GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE] = sizeof(int); 
	spGLSLTypeSize[GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY] = sizeof(int); 
	spGLSLTypeSize[GL_UNSIGNED_INT_SAMPLER_BUFFER] = sizeof(int); 
	spGLSLTypeSize[GL_UNSIGNED_INT_SAMPLER_2D_RECT] = sizeof(int); 
	spGLSLTypeSize[GL_BOOL] = sizeof(int); 
	spGLSLTypeSize[GL_INT] = sizeof(int); 
	spGLSLTypeSize[GL_BOOL_VEC2] = sizeof(int)*2; 
	spGLSLTypeSize[GL_INT_VEC2] = sizeof(int)*2; 
	spGLSLTypeSize[GL_BOOL_VEC3] = sizeof(int)*3; 
	spGLSLTypeSize[GL_INT_VEC3] = sizeof(int)*3;  
	spGLSLTypeSize[GL_BOOL_VEC4] = sizeof(int)*4; 
	spGLSLTypeSize[GL_INT_VEC4] = sizeof(int)*4; 

	spGLSLTypeSize[GL_UNSIGNED_INT] = sizeof(int); 
	spGLSLTypeSize[GL_UNSIGNED_INT_VEC2] = sizeof(int)*2; 
	spGLSLTypeSize[GL_UNSIGNED_INT_VEC3] = sizeof(int)*2; 
	spGLSLTypeSize[GL_UNSIGNED_INT_VEC4] = sizeof(int)*2; 

	spGLSLTypeSize[GL_FLOAT_MAT2] = sizeof(float)*4; 
	spGLSLTypeSize[GL_FLOAT_MAT3] = sizeof(float)*9; 
	spGLSLTypeSize[GL_FLOAT_MAT4] = sizeof(float)*16; 
	spGLSLTypeSize[GL_FLOAT_MAT2x3] = sizeof(float)*6; 
	spGLSLTypeSize[GL_FLOAT_MAT2x4] = sizeof(float)*8; 
	spGLSLTypeSize[GL_FLOAT_MAT3x2] = sizeof(float)*6; 
	spGLSLTypeSize[GL_FLOAT_MAT3x4] = sizeof(float)*12; 
	spGLSLTypeSize[GL_FLOAT_MAT4x2] = sizeof(float)*8; 
	spGLSLTypeSize[GL_FLOAT_MAT4x3] = sizeof(float)*12; 
	spGLSLTypeSize[GL_DOUBLE_MAT2] = sizeof(double)*4; 
	spGLSLTypeSize[GL_DOUBLE_MAT3] = sizeof(double)*9; 
	spGLSLTypeSize[GL_DOUBLE_MAT4] = sizeof(double)*16; 
	spGLSLTypeSize[GL_DOUBLE_MAT2x3] = sizeof(double)*6; 
	spGLSLTypeSize[GL_DOUBLE_MAT2x4] = sizeof(double)*8; 
	spGLSLTypeSize[GL_DOUBLE_MAT3x2] = sizeof(double)*6; 
	spGLSLTypeSize[GL_DOUBLE_MAT3x4] = sizeof(double)*12; 
	spGLSLTypeSize[GL_DOUBLE_MAT4x2] = sizeof(double)*8; 
	spGLSLTypeSize[GL_DOUBLE_MAT4x3] = sizeof(double)*12; 



	spGLSLType[GL_FLOAT] = "GL_FLOAT"; 
	spGLSLType[GL_FLOAT_VEC2] = "GL_FLOAT_VEC2";  
	spGLSLType[GL_FLOAT_VEC3] = "GL_FLOAT_VEC3";  
	spGLSLType[GL_FLOAT_VEC4] = "GL_FLOAT_VEC4";  
	spGLSLType[GL_DOUBLE] = "GL_DOUBLE"; 
	spGLSLType[GL_DOUBLE_VEC2] = "GL_DOUBLE_VEC2";  
	spGLSLType[GL_DOUBLE_VEC3] = "GL_DOUBLE_VEC3";  
	spGLSLType[GL_DOUBLE_VEC4] = "GL_DOUBLE_VEC4";  
	spGLSLType[GL_SAMPLER_1D] = "GL_SAMPLER_1D";
	spGLSLType[GL_SAMPLER_2D] = "GL_SAMPLER_2D";
	spGLSLType[GL_SAMPLER_3D] = "GL_SAMPLER_3D";
	spGLSLType[GL_SAMPLER_CUBE] = "GL_SAMPLER_CUBE";
	spGLSLType[GL_SAMPLER_1D_SHADOW] = "GL_SAMPLER_1D_SHADOW";
	spGLSLType[GL_SAMPLER_2D_SHADOW] = "GL_SAMPLER_2D_SHADOW";
	spGLSLType[GL_SAMPLER_1D_ARRAY] = "GL_SAMPLER_1D_ARRAY";
	spGLSLType[GL_SAMPLER_2D_ARRAY] = "GL_SAMPLER_2D_ARRAY";
	spGLSLType[GL_SAMPLER_1D_ARRAY_SHADOW] = "GL_SAMPLER_1D_ARRAY_SHADOW";
	spGLSLType[GL_SAMPLER_2D_ARRAY_SHADOW] = "GL_SAMPLER_2D_ARRAY_SHADOW";
	spGLSLType[GL_SAMPLER_2D_MULTISAMPLE] = "GL_SAMPLER_2D_MULTISAMPLE";
	spGLSLType[GL_SAMPLER_2D_MULTISAMPLE_ARRAY] = "GL_SAMPLER_2D_MULTISAMPLE_ARRAY";
	spGLSLType[GL_SAMPLER_CUBE_SHADOW] = "GL_SAMPLER_CUBE_SHADOW";
	spGLSLType[GL_SAMPLER_BUFFER] = "GL_SAMPLER_BUFFER";
	spGLSLType[GL_SAMPLER_2D_RECT] = "GL_SAMPLER_2D_RECT";
	spGLSLType[GL_SAMPLER_2D_RECT_SHADOW] = "GL_SAMPLER_2D_RECT_SHADOW";
	spGLSLType[GL_INT_SAMPLER_1D] = "GL_INT_SAMPLER_1D";
	spGLSLType[GL_INT_SAMPLER_2D] = "GL_INT_SAMPLER_2D";
	spGLSLType[GL_INT_SAMPLER_3D] = "GL_INT_SAMPLER_3D";
	spGLSLType[GL_INT_SAMPLER_CUBE] = "GL_INT_SAMPLER_CUBE";
	spGLSLType[GL_INT_SAMPLER_1D_ARRAY] = "GL_INT_SAMPLER_1D_ARRAY";
	spGLSLType[GL_INT_SAMPLER_2D_ARRAY] = "GL_INT_SAMPLER_2D_ARRAY";
	spGLSLType[GL_INT_SAMPLER_2D_MULTISAMPLE] = "GL_INT_SAMPLER_2D_MULTISAMPLE";
	spGLSLType[GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY] = "GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
	spGLSLType[GL_INT_SAMPLER_BUFFER] = "GL_INT_SAMPLER_BUFFER";
	spGLSLType[GL_INT_SAMPLER_2D_RECT] = "GL_INT_SAMPLER_2D_RECT";
	spGLSLType[GL_UNSIGNED_INT_SAMPLER_1D] = "GL_UNSIGNED_INT_SAMPLER_1D";
	spGLSLType[GL_UNSIGNED_INT_SAMPLER_2D] = "GL_UNSIGNED_INT_SAMPLER_2D";
	spGLSLType[GL_UNSIGNED_INT_SAMPLER_3D] = "GL_UNSIGNED_INT_SAMPLER_3D";
	spGLSLType[GL_UNSIGNED_INT_SAMPLER_CUBE] = "GL_UNSIGNED_INT_SAMPLER_CUBE";
	spGLSLType[GL_UNSIGNED_INT_SAMPLER_1D_ARRAY] = "GL_UNSIGNED_INT_SAMPLER_1D_ARRAY";
	spGLSLType[GL_UNSIGNED_INT_SAMPLER_2D_ARRAY] = "GL_UNSIGNED_INT_SAMPLER_2D_ARRAY";
	spGLSLType[GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE] = "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE";
	spGLSLType[GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY] = "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY";
	spGLSLType[GL_UNSIGNED_INT_SAMPLER_BUFFER] = "GL_UNSIGNED_INT_SAMPLER_BUFFER";
	spGLSLType[GL_UNSIGNED_INT_SAMPLER_2D_RECT] = "GL_UNSIGNED_INT_SAMPLER_2D_RECT";
	spGLSLType[GL_BOOL] = "GL_BOOL";  
	spGLSLType[GL_INT] = "GL_INT";  
	spGLSLType[GL_BOOL_VEC2] = "GL_BOOL_VEC2";
	spGLSLType[GL_INT_VEC2] = "GL_INT_VEC2";  
	spGLSLType[GL_BOOL_VEC3] = "GL_BOOL_VEC3";
	spGLSLType[GL_INT_VEC3] = "GL_INT_VEC3";  
	spGLSLType[GL_BOOL_VEC4] = "GL_BOOL_VEC4";
	spGLSLType[GL_INT_VEC4] = "GL_INT_VEC4";  
	spGLSLType[GL_UNSIGNED_INT] = "GL_UNSIGNED_INT"; 
	spGLSLType[GL_UNSIGNED_INT_VEC2] = "GL_UNSIGNED_INT_VEC2";  
	spGLSLType[GL_UNSIGNED_INT_VEC3] = "GL_UNSIGNED_INT_VEC3";  
	spGLSLType[GL_UNSIGNED_INT_VEC4] = "GL_UNSIGNED_INT_VEC4";  
	spGLSLType[GL_FLOAT_MAT2] = "GL_FLOAT_MAT2";
	spGLSLType[GL_FLOAT_MAT3] = "GL_FLOAT_MAT3";
	spGLSLType[GL_FLOAT_MAT4] = "GL_FLOAT_MAT4";
	spGLSLType[GL_FLOAT_MAT2x3] = "GL_FLOAT_MAT2x3";
	spGLSLType[GL_FLOAT_MAT2x4] = "GL_FLOAT_MAT2x4";
	spGLSLType[GL_FLOAT_MAT3x2] = "GL_FLOAT_MAT3x2";
	spGLSLType[GL_FLOAT_MAT3x4] = "GL_FLOAT_MAT3x4";
	spGLSLType[GL_FLOAT_MAT4x2] = "GL_FLOAT_MAT4x2";
	spGLSLType[GL_FLOAT_MAT4x3] = "GL_FLOAT_MAT4x3";
	spGLSLType[GL_DOUBLE_MAT2] = "GL_DOUBLE_MAT2";
	spGLSLType[GL_DOUBLE_MAT3] = "GL_DOUBLE_MAT3";
	spGLSLType[GL_DOUBLE_MAT4] = "GL_DOUBLE_MAT4";
	spGLSLType[GL_DOUBLE_MAT2x3] = "GL_DOUBLE_MAT2x3";
	spGLSLType[GL_DOUBLE_MAT2x4] = "GL_DOUBLE_MAT2x4";
	spGLSLType[GL_DOUBLE_MAT3x2] = "GL_DOUBLE_MAT3x2";
	spGLSLType[GL_DOUBLE_MAT3x4] = "GL_DOUBLE_MAT3x4";
	spGLSLType[GL_DOUBLE_MAT4x2] = "GL_DOUBLE_MAT4x2";
	spGLSLType[GL_DOUBLE_MAT4x3] = "GL_DOUBLE_MAT4x3";



	spTextureDataType[GL_NONE] = "GL_NONE";
	spTextureDataType[GL_SIGNED_NORMALIZED] = "GL_SIGNED_NORMALIZED";
	spTextureDataType[GL_UNSIGNED_NORMALIZED] = "GL_UNSIGNED_NORMALIZED";
	spTextureDataType[GL_FLOAT] = "GL_FLOAT";
	spTextureDataType[GL_INT] = "GL_INT";
	spTextureDataType[GL_UNSIGNED_INT] = "GL_UNSIGNED_INT";

	spDataF[GL_UNSIGNED_BYTE] = "GL_UNSIGNED_BYTE";
	spDataF[GL_BYTE] = "GL_BYTE";
	spDataF[GL_UNSIGNED_SHORT] = "GL_UNSIGNED_SHORT";
	spDataF[GL_SHORT] = "GL_SHORT";
	spDataF[GL_UNSIGNED_INT] = "GL_UNSIGNED_INT";
	spDataF[GL_INT] = "GL_INT";
	spDataF[GL_HALF_FLOAT] = "GL_HALF_FLOAT";
	spDataF[GL_FLOAT] = "GL_FLOAT";

	spDataF[GL_UNSIGNED_BYTE_3_3_2] = "GL_UNSIGNED_BYTE_3_3_2";
	spDataF[GL_UNSIGNED_BYTE_2_3_3_REV] = "GL_UNSIGNED_BYTE_2_3_3_REV";
	spDataF[GL_UNSIGNED_SHORT_5_6_5] = "GL_UNSIGNED_SHORT_5_6_5";
	spDataF[GL_UNSIGNED_SHORT_5_6_5_REV] = "GL_UNSIGNED_SHORT_5_6_5_REV";
	spDataF[GL_UNSIGNED_SHORT_4_4_4_4] = "GL_UNSIGNED_SHORT_4_4_4_4";
	spDataF[GL_UNSIGNED_SHORT_4_4_4_4_REV] = "GL_UNSIGNED_SHORT_4_4_4_4_REV";
	spDataF[GL_UNSIGNED_SHORT_5_5_5_1] = "GL_UNSIGNED_SHORT_5_5_5_1";
	spDataF[GL_UNSIGNED_SHORT_1_5_5_5_REV] = "GL_UNSIGNED_SHORT_1_5_5_5_REV";
	spDataF[GL_UNSIGNED_INT_8_8_8_8] = "GL_UNSIGNED_INT_8_8_8_8";
	spDataF[GL_UNSIGNED_INT_8_8_8_8_REV] = "GL_UNSIGNED_INT_8_8_8_8_REV";
	spDataF[GL_UNSIGNED_INT_10_10_10_2] = "GL_UNSIGNED_INT_10_10_10_2";
	spDataF[GL_UNSIGNED_INT_2_10_10_10_REV] = "GL_UNSIGNED_INT_2_10_10_10_REV";

	spInternalF[GL_STENCIL_INDEX] = "GL_STENCIL_INDEX";
	spInternalF[GL_DEPTH_COMPONENT] = "GL_DEPTH_COMPONENT";
	spInternalF[GL_DEPTH_STENCIL] = "GL_DEPTH_STENCIL";
	spInternalF[GL_DEPTH_COMPONENT16] = "GL_DEPTH_COMPONENT16";
	spInternalF[GL_DEPTH_COMPONENT24] = "GL_DEPTH_COMPONENT24";
	spInternalF[GL_DEPTH_COMPONENT32] = "GL_DEPTH_COMPONENT32";
	spInternalF[GL_DEPTH_COMPONENT32F] = "GL_DEPTH_COMPONENT32F";
	spInternalF[GL_DEPTH24_STENCIL8] = "GL_DEPTH24_STENCIL8";
	spInternalF[GL_DEPTH32F_STENCIL8] = "GL_DEPTH32F_STENCIL8";
	spInternalF[GL_RED_INTEGER] = "GL_RED_INTEGER";
	spInternalF[GL_GREEN_INTEGER] = "GL_GREEN_INTEGER";
	spInternalF[GL_BLUE_INTEGER] = "GL_BLUE_INTEGER";

	spInternalF[GL_RG_INTEGER] = "GL_RG_INTEGER";
	spInternalF[GL_RGB_INTEGER] = "GL_RGB_INTEGER";
	spInternalF[GL_RGBA_INTEGER] = "GL_RGBA_INTEGER";
	spInternalF[GL_BGR_INTEGER] = "GL_BGR_INTEGER";
	spInternalF[GL_BGRA_INTEGER] = "GL_BGRA_INTEGER";

	spInternalF[GL_RED] = "GL_RED";
	spInternalF[GL_RG] = "GL_RG";
	spInternalF[GL_RGB] = "GL_RGB";
	spInternalF[GL_RGBA] = "GL_RGBA";
	spInternalF[GL_R3_G3_B2] = "GL_R3_G3_B2";
	spInternalF[GL_RGB2_EXT] = "GL_RGB2_EXT";
	spInternalF[GL_COMPRESSED_RED] = "GL_COMPRESSED_RED";
	spInternalF[GL_COMPRESSED_RG] = "GL_COMPRESSED_RG";
	spInternalF[GL_COMPRESSED_RGB] = "GL_COMPRESSED_RGB";
	spInternalF[GL_COMPRESSED_RGBA] = "GL_COMPRESSED_RGBA";
	spInternalF[GL_COMPRESSED_SRGB] = "GL_COMPRESSED_SRGB";
	spInternalF[GL_COMPRESSED_SRGB_ALPHA] = "GL_COMPRESSED_SRGB_ALPHA";
	spInternalF[GL_COMPRESSED_RED_RGTC1] = "GL_COMPRESSED_RED_RGTC1";
	spInternalF[GL_COMPRESSED_SIGNED_RED_RGTC1] = "GL_COMPRESSED_SIGNED_RED_RGTC1";
	spInternalF[GL_COMPRESSED_RG_RGTC2] = "GL_COMPRESSED_RG_RGTC2";	spInternalF[GL_RG] = "GL_RG";
	spInternalF[GL_COMPRESSED_SIGNED_RG_RGTC2] = "GL_COMPRESSED_SIGNED_RG_RGTC2";

#ifdef VSGL_VERSION_4_2
	spInternalF[GL_COMPRESSED_RGBA_BPTC_UNORM] = "GL_COMPRESSED_RGBA_BPTC_UNORM";
	spInternalF[GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM] = "GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM";
	spInternalF[GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT] = "GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT";
	spInternalF[GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT] = "GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT";
#endif

	spInternalF[GL_R8] = "GL_R8";
	spInternalF[GL_R16] = "GL_R16";
	spInternalF[GL_RG8] = "GL_RG8";
	spInternalF[GL_RG16] = "GL_RG16";
	spInternalF[GL_R16F] = "GL_R16F";
	spInternalF[GL_R32F] = "GL_R32F";
	spInternalF[GL_RG16F] = "GL_RG16F";
	spInternalF[GL_RG32F] = "GL_RG32F";
	spInternalF[GL_R8I] = "GL_R8I";
	spInternalF[GL_R8UI] = "GL_R8UI";
	spInternalF[GL_R16I] = "GL_R16I";
	spInternalF[GL_R16UI] = "GL_R16UI";
	spInternalF[GL_R32I] = "GL_R32I";
	spInternalF[GL_R32UI] = "GL_R32UI";
	spInternalF[GL_RG8I] = "GL_RG8I";
	spInternalF[GL_RG8UI] = "GL_RG8UI";
	spInternalF[GL_RG16I] = "GL_RG16I";
	spInternalF[GL_RG16UI] = "GL_RG16UI";
	spInternalF[GL_RG32I] = "GL_RG32I";
	spInternalF[GL_RG32UI] = "GL_RG32UI";
	spInternalF[GL_RGB_S3TC] = "GL_RGB_S3TC";
	spInternalF[GL_RGB4_S3TC] = "GL_RGB4_S3TC";
	spInternalF[GL_RGBA_S3TC] = "GL_RGBA_S3TC";
	spInternalF[GL_RGBA4_S3TC] = "GL_RGBA4_S3TC";
	spInternalF[GL_RGBA_DXT5_S3TC] = "GL_RGBA_DXT5_S3TC";
	spInternalF[GL_RGBA4_DXT5_S3TC] = "GL_RGBA4_DXT5_S3TC";
	spInternalF[GL_COMPRESSED_RGB_S3TC_DXT1_EXT] = "GL_COMPRESSED_RGB_S3TC_DXT1_EXT";
	spInternalF[GL_COMPRESSED_RGBA_S3TC_DXT1_EXT] = "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT";
	spInternalF[GL_COMPRESSED_RGBA_S3TC_DXT3_EXT] = "GL_COMPRESSED_RGBA_S3TC_DXT3_EXT";
	spInternalF[GL_COMPRESSED_RGBA_S3TC_DXT5_EXT] = "GL_COMPRESSED_RGBA_S3TC_DXT5_EXT";
	spInternalF[GL_R1UI_V3F_SUN] = "GL_R1UI_V3F_SUN";
	spInternalF[GL_R1UI_C4UB_V3F_SUN] = "GL_R1UI_C4UB_V3F_SUN";
	spInternalF[GL_R1UI_C3F_V3F_SUN] = "GL_R1UI_C3F_V3F_SUN";
	spInternalF[GL_R1UI_N3F_V3F_SUN] = "GL_R1UI_N3F_V3F_SUN";
	spInternalF[GL_R1UI_C4F_N3F_V3F_SUN] = "GL_R1UI_C4F_N3F_V3F_SUN";
	spInternalF[GL_R1UI_T2F_V3F_SUN] = "GL_R1UI_T2F_V3F_SUN";
	spInternalF[GL_R1UI_T2F_N3F_V3F_SUN] = "GL_R1UI_T2F_N3F_V3F_SUN";
	spInternalF[GL_R1UI_T2F_C4F_N3F_V3F_SUN] = "GL_R1UI_T2F_C4F_N3F_V3F_SUN";
	spInternalF[GL_RGB_SIGNED_SGIX] = "GL_RGB_SIGNED_SGIX";
	spInternalF[GL_RGBA_SIGNED_SGIX] = "GL_RGBA_SIGNED_SGIX";
	spInternalF[GL_RGB16_SIGNED_SGIX] = "GL_RGB16_SIGNED_SGIX";
	spInternalF[GL_RGBA16_SIGNED_SGIX] = "GL_RGBA16_SIGNED_SGIX";
	spInternalF[GL_RGB_EXTENDED_RANGE_SGIX] = "GL_RGB_EXTENDED_RANGE_SGIX";
	spInternalF[GL_RGBA_EXTENDED_RANGE_SGIX] = "GL_RGBA_EXTENDED_RANGE_SGIX";
	spInternalF[GL_RGB16_EXTENDED_RANGE_SGIX] = "GL_RGB16_EXTENDED_RANGE_SGIX";
	spInternalF[GL_RGBA16_EXTENDED_RANGE_SGIX] = "GL_RGBA16_EXTENDED_RANGE_SGIX";
	spInternalF[GL_COMPRESSED_RGB_FXT1_3DFX] = "GL_COMPRESSED_RGB_FXT1_3DFX";
	spInternalF[GL_COMPRESSED_RGBA_FXT1_3DFX] = "GL_COMPRESSED_RGBA_FXT1_3DFX";
	spInternalF[GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV] = "GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV";
	spInternalF[GL_RGBA_FLOAT_MODE_ARB] = "GL_RGBA_FLOAT_MODE_ARB";
	spInternalF[GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI] = "GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI";
	spInternalF[GL_RGB_422_APPLE] = "GL_RGB_422_APPLE";
	spInternalF[GL_RGBA_SIGNED_COMPONENTS_EXT] = "GL_RGBA_SIGNED_COMPONENTS_EXT";
	spInternalF[GL_COMPRESSED_SRGB_S3TC_DXT1_EXT] = "GL_COMPRESSED_SRGB_S3TC_DXT1_EXT";
	spInternalF[GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT] = "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT";
	spInternalF[GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT] = "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT";
	spInternalF[GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT] = "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT";
	spInternalF[GL_COMPRESSED_LUMINANCE_LATC1_EXT] = "GL_COMPRESSED_LUMINANCE_LATC1_EXT";
	spInternalF[GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT] = "GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT";
	spInternalF[GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT] = "GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT";
	spInternalF[GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT] = "GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT";
	spInternalF[GL_RGBA_INTEGER_MODE_EXT] = "GL_RGBA_INTEGER_MODE_EXT";
	spInternalF[GL_COMPRESSED_RGBA_BPTC_UNORM_ARB] = "GL_COMPRESSED_RGBA_BPTC_UNORM_ARB";
	spInternalF[GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB] = "GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB";
	spInternalF[GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB] = "GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB";
	spInternalF[GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB] = "GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB";
	spInternalF[GL_RG_SNORM] = "GL_RG_SNORM";
	spInternalF[GL_RGB_SNORM] = "GL_RGB_SNORM";
	spInternalF[GL_RGBA_SNORM] = "GL_RGBA_SNORM";
	spInternalF[GL_R8_SNORM] = "GL_R8_SNORM";
	spInternalF[GL_RG8_SNORM] = "GL_RG8_SNORM";
	spInternalF[GL_RGB8_SNORM] = "GL_RGB8_SNORM";
	spInternalF[GL_RGBA8_SNORM] = "GL_RGBA8_SNORM";
	spInternalF[GL_R16_SNORM] = "GL_R16_SNORM";
	spInternalF[GL_RG16_SNORM] = "GL_RG16_SNORM";
	spInternalF[GL_RGB16_SNORM] = "GL_RGB16_SNORM";
	spInternalF[GL_RGBA16_SNORM] = "GL_RGBA16_SNORM";
	spInternalF[GL_RGB10_A2UI] = "GL_RGB10_A2UI";

	return true;
}


// aux function to display float based uniforms
void 
VSGLInfoLib::displayUniformf(float *f, int rows, int columns) {

	for (int i = 0; i < rows; ++i) {
		if (columns == 1)
			addMessage("%f", f[i*columns]);
		else if (columns == 2)
			addMessage("%f %f", f[i*columns], f[i*columns+1]);
		else if (columns == 3)
			addMessage("%f %f %f", f[i*columns], f[i*columns+1], f[i*columns+2]);
		else if (columns == 4)
			addMessage("%f %f %f %f", f[i*columns], f[i*columns+1], f[i*columns+2],  f[i*columns+3]);
	}
}


// aux function to display int based uniforms
void 
VSGLInfoLib::displayUniformi(int *f, int rows, int columns) {

	for (int i = 0; i < rows; ++i) {
		if (columns == 1)
			addMessage("%d", f[i*columns]);
		else if (columns == 2)
			addMessage("%d %d", f[i*columns], f[i*columns+1]);
		else if (columns == 3)
			addMessage("%d %d %d", f[i*columns], f[i*columns+1], f[i*columns+2]);
		else if (columns == 4)
			addMessage("%d %d %d %d", f[i*columns], f[i*columns+1], f[i*columns+2],  f[i*columns+3]);
	}
}


// aux function to display unsigned int based uniforms
void 
VSGLInfoLib::displayUniformui(unsigned int *f, int rows, int columns) {

	for (int i = 0; i < rows; ++i) {
		if (columns == 1)
			addMessage("%u", f[i*columns]);
		else if (columns == 2)
			addMessage("%u %u", f[i*columns], f[i*columns+1]);
		else if (columns == 3)
			addMessage("%u %u %u", f[i*columns], f[i*columns+1], f[i*columns+2]);
		else if (columns == 4)
			addMessage("%u %u %u %u", f[i*columns], f[i*columns+1], f[i*columns+2],  f[i*columns+3]);
	}
}


// aux function to display double based uniforms
void 
VSGLInfoLib::displayUniformd(double *f, int rows, int columns) {

	for (int i = 0; i < rows; ++i) {
		if (columns == 1)
			addMessage("%f", f[i*columns]);
		else if (columns == 2)
			addMessage("%f %f", f[i*columns], f[i*columns+1]);
		else if (columns == 3)
			addMessage("%f %f %f", f[i*columns], f[i*columns+1], f[i*columns+2]);
		else if (columns == 4)
			addMessage("%f %f %f %f", f[i*columns], f[i*columns+1], f[i*columns+2],  f[i*columns+3]);
	}

}


// gets the atomic data type
VSGLInfoLib::Types 
VSGLInfoLib::getType(GLenum type) {

	switch (type) {
		case GL_DOUBLE:
		case GL_DOUBLE_MAT2:
		case GL_DOUBLE_MAT2x3:
		case GL_DOUBLE_MAT2x4:
		case GL_DOUBLE_MAT3:
		case GL_DOUBLE_MAT3x2:
		case GL_DOUBLE_MAT3x4:
		case GL_DOUBLE_MAT4:
		case GL_DOUBLE_MAT4x2:
		case GL_DOUBLE_MAT4x3:
		case GL_DOUBLE_VEC2:
		case GL_DOUBLE_VEC3:
		case GL_DOUBLE_VEC4:
			return VSGLInfoLib::DOUBLE;
		case GL_FLOAT:
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT2x4:
		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT3x2:
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4:
		case GL_FLOAT_MAT4x2:
		case GL_FLOAT_MAT4x3:
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
			return VSGLInfoLib::FLOAT;
		case GL_BOOL:
		case GL_BOOL_VEC2:
		case GL_BOOL_VEC3:
		case GL_BOOL_VEC4:
		case GL_INT:
		case GL_INT_SAMPLER_1D:
		case GL_INT_SAMPLER_1D_ARRAY:
		case GL_INT_SAMPLER_2D:
		case GL_INT_SAMPLER_2D_ARRAY:
		case GL_INT_SAMPLER_2D_MULTISAMPLE:
		case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_INT_SAMPLER_2D_RECT:
		case GL_INT_SAMPLER_3D:
		case GL_INT_SAMPLER_BUFFER:
		case GL_INT_SAMPLER_CUBE:
		case GL_INT_VEC2:
		case GL_INT_VEC3:
		case GL_INT_VEC4:
		case GL_SAMPLER_1D:
		case GL_SAMPLER_1D_ARRAY:
		case GL_SAMPLER_1D_ARRAY_SHADOW:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_2D_ARRAY:
		case GL_SAMPLER_2D_ARRAY_SHADOW:
		case GL_SAMPLER_2D_MULTISAMPLE:
		case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_SAMPLER_2D_RECT:
		case GL_SAMPLER_2D_RECT_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_BUFFER:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_CUBE_SHADOW:
			return VSGLInfoLib::INT;
		case GL_UNSIGNED_INT:
		case GL_UNSIGNED_INT_SAMPLER_1D:
		case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D:
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
		case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
		case GL_UNSIGNED_INT_SAMPLER_3D:
		case GL_UNSIGNED_INT_SAMPLER_BUFFER:
		case GL_UNSIGNED_INT_SAMPLER_CUBE:
		case GL_UNSIGNED_INT_VEC2:
		case GL_UNSIGNED_INT_VEC3:
		case GL_UNSIGNED_INT_VEC4:
			return VSGLInfoLib::UNSIGNED_INT;

		default:
			return VSGLInfoLib::DONT_KNOW;

	}
}


// gets the number of rows for a GLSL type
int 
VSGLInfoLib::getRows(GLenum type) {

	switch(type) {
		case GL_DOUBLE_MAT2:
		case GL_DOUBLE_MAT2x3:
		case GL_DOUBLE_MAT2x4:
		case GL_FLOAT_MAT2:
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT2x4:
			return 2;

		case GL_DOUBLE_MAT3:
		case GL_DOUBLE_MAT3x2:
		case GL_DOUBLE_MAT3x4:
		case GL_FLOAT_MAT3:
		case GL_FLOAT_MAT3x2:
		case GL_FLOAT_MAT3x4:
			return 3;

		case GL_DOUBLE_MAT4:
		case GL_DOUBLE_MAT4x2:
		case GL_DOUBLE_MAT4x3:
		case GL_FLOAT_MAT4:
		case GL_FLOAT_MAT4x2:
		case GL_FLOAT_MAT4x3:
			return 4;

		default: return 1;
	}
}


// gets the number of columns for a GLSL type
int 
VSGLInfoLib::getColumns(GLenum type) {

	switch(type) {
		case GL_DOUBLE_MAT2:
		case GL_FLOAT_MAT2:
		case GL_DOUBLE_MAT3x2:
		case GL_FLOAT_MAT3x2:
		case GL_DOUBLE_MAT4x2:
		case GL_FLOAT_MAT4x2:
		case GL_UNSIGNED_INT_VEC2:
		case GL_INT_VEC2:
		case GL_BOOL_VEC2:
		case GL_FLOAT_VEC2:
		case GL_DOUBLE_VEC2:
			return 2;
			
		case GL_DOUBLE_MAT2x3:
		case GL_FLOAT_MAT2x3:
		case GL_DOUBLE_MAT3:
		case GL_FLOAT_MAT3:
		case GL_DOUBLE_MAT4x3:
		case GL_FLOAT_MAT4x3:
		case GL_UNSIGNED_INT_VEC3:
		case GL_INT_VEC3:
		case GL_BOOL_VEC3:
		case GL_FLOAT_VEC3:
		case GL_DOUBLE_VEC3:
			return 3;

		case GL_DOUBLE_MAT2x4:
		case GL_FLOAT_MAT2x4:
		case GL_DOUBLE_MAT3x4:
		case GL_FLOAT_MAT3x4:
		case GL_DOUBLE_MAT4:
		case GL_FLOAT_MAT4:
		case GL_UNSIGNED_INT_VEC4:
		case GL_INT_VEC4:
		case GL_BOOL_VEC4:
		case GL_FLOAT_VEC4:
		case GL_DOUBLE_VEC4:
			return 4;

		default: return 1;
	}
}


// aux function to get the size in bytes of a uniform
// it takes the strides into account
int 
VSGLInfoLib::getUniformByteSize(int uniSize, 
				int uniType, 
				int uniArrayStride, 
				int uniMatStride) {

	int auxSize;
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
		auxSize = spGLSLTypeSize[uniType];

	return auxSize;
}


