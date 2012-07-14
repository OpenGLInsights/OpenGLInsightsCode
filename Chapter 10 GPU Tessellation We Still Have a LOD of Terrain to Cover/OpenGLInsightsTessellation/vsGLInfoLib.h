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

#ifndef __VSGLIL__
#define __VSGLIL__

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <ostream>
#include <fstream>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#ifdef _WIN32
#pragma comment(lib,"glew32.lib")
#endif


class VSGLInfoLib {

#define VSGL_VERSION_4_1
#undef VSGL_VERSION_4_2

public:
	/// sets the output stream for the messages
	// if null, cout is used
	static void setOutputStream(std::ostream *outStream);

	/// get vendor, version and other general info
	static void getGeneralInfo(); 

	/// checks if an extension is supported
	static bool isExtensionSupported(std::string extName);


	// Buffers
	/// returns a vector with all named buffers
	static std::vector<unsigned int> &getBufferNames();
	/// display current binded buffer info
	static void getCurrentBufferInfo();
	/// display the buffer information
	static void getBufferInfo(GLenum target, int bufferName);


	// Textures
	/// returns a vector with all named textures 
	static std::vector<unsigned int> &getTextureNames();
	/// get the current texture bound to target
	static int getCurrentTexture(GLenum textureTarget);
	/// get active texture unit
	static int getCurrentTextureActiveUnitInfo();
	/// display current texture bindings
	static void getCurrentTextureInfo();
	/// display detailed texture info
	static void getTextureInfo(GLenum textureTarget, GLenum textureID);


	// GLSL
	/// returns a vector with all named programs 
	static std::vector<unsigned int> &getProgramNames();
	/// returns a vector with all named shaders 
	static std::vector<unsigned int> &getShaderNames();
	/// returns a vector with all named VAOs 
	static std::vector<unsigned int> &getVAONames();
	/// display detailed VAO info
	static void getVAOInfo(unsigned int buffer);
	/// display detailed info for uniforms in a program
	static void getUniformsInfo(unsigned int program);
	/// display a uniform's value(s)
	static void getUniformInfo(unsigned int program, 
							std::string uniName);
	/// display the values for a uniform in a named block
	static void getUniformInBlockInfo(unsigned int program,
									std::string blockName,
									std::string uniName);
	/// display detailed info for attributes in a program
	static void getAttributesInfo(unsigned int program);
	/// display detailed info for a program
	static void getProgramInfo(unsigned int program);


private:

	// automatic init
	static bool init();

	enum Types {
		DONT_KNOW, INT, UNSIGNED_INT, FLOAT, DOUBLE};

	static void addMessage(std::string format, ...);
	static void addNewLine();

	static bool __spInit;
	static std::ostream *spOutS;

	static char spAux[256];

	static std::vector<unsigned int> spResult;

	static std::map<int, std::string> spInternalF;
	static std::map<int, std::string> spDataF;
	static std::map<int, std::string> spTextureDataType;
	static std::map<int, std::string> spGLSLType;
	static std::map<int, std::string> spTextureFilter;
	static std::map<int, int> 	spGLSLTypeSize;
	static std::map<int, std::string> spTextureWrap;
	static std::map<int, std::string> spTextureCompFunc;
	static std::map<int, std::string> spTextureCompMode;
	static std::map<int, std::string> spTextureUnit;
	static std::map<int, std::string> spHint;
	static std::map<int, std::string> spTextureTarget;
	static std::map<int, std::string> spBufferAccess;
	static std::map<int, std::string> spBufferUsage;
	static std::map<int, std::string> spBufferBinding;
	static std::map<int, int> spTextureBound;
	static std::map<int, int> spBufferBound;
	static std::map<int, int> spBoundBuffer;
	static std::map<int, std::string> spShaderType;
	static std::map<int, std::string> spTransFeedBufferMode;
	static std::map<int, std::string> spGLSLPrimitives;
	static std::map<int, std::string> spShaderPrecision;

	static std::map<int, std::string> spTessGenSpacing;
	static std::map<int, std::string> spVertexOrder;


	VSGLInfoLib();
	~VSGLInfoLib();


	static Types  getType(GLenum type);
	static int getRows(GLenum type);
	static int getColumns(GLenum type);
	static void displayUniformf(float *f, int rows, int columns);
	static void displayUniformi(int *f, int rows, int columns);
	static void displayUniformui(unsigned int *f, int rows, int columns);
	static void displayUniformd(double *f, int rows, int columns);
	static int getUniformByteSize(int size, int uniType, int arrayStride, int matStride);
};


#endif