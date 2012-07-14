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


#ifndef __VSShaderLib__
#define __VSShaderLib__

#include <string>
#include <vector>
#include <map>
#include <GL/glew.h>


class VSShaderLib
{
public:
	
	/// Types of Vertex Attributes
	enum AttribType {
		VERTEX_COORD_ATTRIB,
		NORMAL_ATTRIB,
		TEXTURE_COORD_ATTRIB,
		TANGENT_ATTRIB,
		BITANGENT_ATTRIB,
		VERTEX_ATTRIB1,
		VERTEX_ATTRIB2,
		VERTEX_ATTRIB3,
		VERTEX_ATTRIB4
	};

	/// Types of Shaders
	enum ShaderType {
		VERTEX_SHADER,
		GEOMETRY_SHADER,
		TESS_CONTROL_SHADER,
		TESS_EVAL_SHADER,
		FRAGMENT_SHADER,
		COUNT_SHADER_TYPE
	};

	/// Just a helper define
	static const int MAX_TEXTURES = 8;

	VSShaderLib();
	~VSShaderLib();

	/** Init should be called for every shader instance
	  * prior to any other function
	*/
	void init();
	
	/** Loads the text in the file to the source of the specified shader 
	  *
	  * \param st one of the enum values of ShaderType
	  *	\param filename the file where the source is to be found
	*/
	void loadShader(VSShaderLib::ShaderType st, std::string fileName);

	/** bind a user-defined varying out variable to a 
	  * fragment shader color number
	  * Note: linking is required for this operation to take effect
	  * (call method prepareProgram afterwards)
	  *
	  * \param index the fragment colour number
	  * \param the name of the fragment's shader variable
	*/
	void setProgramOutput(int index, std::string name);

	/** returns the fragment shader color number bound to 
	  * a user-defined varying out variable
	  * 
	  * Note: linking is required for this operation to take effect
	  * (call method prepareProgram afterwards)
	  *
	  * \param the name of the fragment's shader variable
	  * \returns the fragment colour number
	*/
	GLint getProgramOutput(std::string name);

	/** Defines semantics for the input vertex attributes. This is
	  * required for other libraries to know how to send data to the shader
	  * Note: linking is required for this operation to take effect
	  * (call method prepareProgram)
	  *
	  * \param the semantic of the attribute
	  * \param the name of the vertex attribute
	*/
	void setVertexAttribName(VSShaderLib::AttribType at, std::string name);

	/** Prepares program for usage. Links it and collects information
	  * about uniform variables and uniform blocks
	*/
	void prepareProgram();

	/// generic function to set the uniform <name> to value
	void setUniform(std::string name, void *value);
	/// For int and bool uniforms. Sets the uniform <name> to the int value
	void setUniform(std::string name, int value);
	/// For float uniforms. Sets the uniform <name> to the float value
	void setUniform(std::string name, float value);
	/// sets a uniform block as a whole
	static void setBlock(std::string name, void *value);
	/// sets a uniform inside a named block
	static void setBlockUniform(std::string blockName, 
								std::string uniformName, 
								void *value);
	/// sets an element of an array of uniforms inside a block
	static void setBlockUniformArrayElement(std::string blockName, 
								std::string uniformName,
								int arrayIndex, 
								void * value);

	/// returns the program index
	GLuint getProgramIndex();
	/// returns a shader index
	GLuint getShaderIndex(VSShaderLib::ShaderType);

	/// returns a string with a shader's infolog
	std::string getShaderInfoLog(VSShaderLib::ShaderType);
	/// returns a string with the program's infolog
	std::string getProgramInfoLog();
	/// returns a string will all info logs
	std::string getAllInfoLogs();
	/// returns GL_VALIDATE_STATUS for the program 
	bool isProgramValid();
	/// returns true if compiled, false otherwise
	bool isShaderCompiled(VSShaderLib::ShaderType);
	/// returns true if linked, false otherwise
	bool isProgramLinked();


protected:

	// AUX STRUCTURES

	/// stores information for uniforms
	typedef struct uniforms {
		GLenum type;
		GLuint location;
		GLuint size;
		GLuint stride;
	}myUniforms;

	/// stores information for block uniforms
	typedef struct blockUniforms {
		GLenum type;
		GLuint offset;
		GLuint size;
		GLuint arrayStride;
	} myBlockUniform;

	/// stores information for a block and its uniforms
	class UniformBlock {

		public:
			/// size of the uniform block
			int size;
			/// buffer bound to the index point
			GLuint buffer;
			/// binding index
			GLuint bindingIndex;
			/// uniforms information
			std::map<std::string, myBlockUniform > uniformOffsets;
	};

	// VARIABLES

	/// stores if init has been called
	bool pInited;


	/// blockCount is used to assign binding indexes
	static int spBlockCount;

	/// Stores info on all blocks found
	static std::map<std::string, UniformBlock> spBlocks;

	/// stores the OpenGL shader types
	static GLenum spGLShaderTypes[VSShaderLib::COUNT_SHADER_TYPE];
	
	/// stores the text string related to each type
	static std::string spStringShaderTypes[VSShaderLib::COUNT_SHADER_TYPE];

	/// aux string used to return the shaders infologs
	std::string pResult;

	/// stores the shaders and program indices
	GLuint pShader[VSShaderLib::COUNT_SHADER_TYPE], pProgram;

	/// stores info on the uniforms
	std::map<std::string, myUniforms> pUniforms;

	// AUX FUNCTIONS

	/// aux function to get info on the uniforms referenced by the shaders
	void addUniforms();
	
	/// aux function to store the info of a uniform
	void addUniform(std::string name, GLenum type, unsigned int size);

	/// aux function to get info on the blocks referenced by the shaders
	void addBlocks();

	/// determines the size in bytes based on the OpenGL type
	int typeSize(int type);

	/// aux function to read the shader's source code from file
	char *textFileRead(std::string fileName);
};

	

#endif
