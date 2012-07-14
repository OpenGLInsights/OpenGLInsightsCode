
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

#ifndef GPUSHADER
#define GPUSHADER

#include <GL/glew.h>

/*#define GL_GEOMETRY_SHADER_ARB 0x8DD9
#define GL_GEOMETRY_VERTICES_OUT_ARB 0x8DDA
#define GL_GEOMETRY_INPUT_TYPE_ARB 0x8DDB
#define GL_GEOMETRY_OUTPUT_TYPE_ARB 0x8DDC
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES_ARB 0x8DE0*/
//must be the last inclusion


//Shader type
enum ShaderType
{
	GPU_VERTEX_SHADER	= GL_VERTEX_SHADER_ARB,
	GPU_FRAGMENT_SHADER	= GL_FRAGMENT_SHADER_ARB,
	GPU_GEOMETRY_SHADER	= GL_GEOMETRY_SHADER_ARB
};



/**
 * This class represent a vertex or fragment shader.
 *
 * @author Hillaire S�bastien
 */
class GPUShader
{
private:

	/**
	 *	Copy constructor forbiden
	 */
	GPUShader(GPUShader&);

	/**
	 *	The shader id
	 */
	GLhandleARB id;
	/**
	 *	Te shader type
	 */
	ShaderType shaderType;

protected:

public:

	/**
	 *	Constructor
	 *
	 * @param shaderType : this shader type
	 */
	GPUShader(ShaderType shaderType);
	/**
	 *	Destructor
	 */
	~GPUShader();


	/**
	 *	Use to compile this shader in memory
	 *
	 * @param program : the source program string
	 */
	void compile(const char* program);
	/**
	 *	Unload this shader from the memory
	 */
	void unload();

	/**
	 *	Give this shader id.
	 *
	 * @return ths id
	 */
	GLhandleARB getID();
	/**
	 *	Return the compile status
	 *
	 * @return true if the shader is compiled, false if an error occured during compilation
	 */
	bool getCompileStatus();
	/**
	 *	Return the shader type.
	 *
	 * @return the shader type
	 */
	ShaderType getShaderType();

};


#endif
