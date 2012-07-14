
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

#ifndef GPUPROGRAM
#define GPUPROGRAM

#include "GPUShader.h"
#include "stdlib.h"

class GPUProgram
{
private:

	/**
	 *	the GPU program id
	 */
	GLhandleARB id;

	/**
	 *	the vertex shader
	 */
	GPUShader vertexShader;
	/**
	* the fragment shader
	*/
	GPUShader fragmentShader;
	/**
	* the geometry shader
	*/
	GPUShader* geometryShader;

	/**
	 *	copy constructeur not allowed
	 */
	GPUProgram(GPUProgram&);

protected:

public:

	/**
	 *	Constructor
	 *
	 * @pre OpenGL context exist and OpenGL extensions has been loaded by GLEW.
	 * this pre-condition must be full-filled for all this class methods.
	 */
	GPUProgram();
	/**
	 *	Destructor
	 */
	~GPUProgram();


	/**
	 *	Load  gpu program (vertex and fragment shader) in memory
	 *
	 * @param vertexProgram : the vertex shader string
	 * @param fragmentProgram : the fragment shader string
	 *
	 * @return false if one of the shader don't compile or linking
	 */
	bool load(const char* vertexProgram, const char* fragmentProgram, const char* geometryProgram=NULL, int GS_inputPrimitiveType=GL_TRIANGLES, int GS_outputPrimitiveType=GL_TRIANGLE_STRIP, int GS_maxVerticesOut=3);
	/**
	 *	Unload the gpu program from the memory
	 */
	void unload();


	/**
	 *	Select this gpu program to render the following geometry
	 */
	void bindGPUProgram() const;
	/**
	 *	Deselect the GPUprogram
	 */
	static void unbindGPUProgram();


	/**
	 *	Get uniform variable location based on its name
	 *
	 * @param uniformName : uniform name
	 *
	 * (then to affect use glUniformXfARB or glUniformXfvARB)
	 */
	GLuint getUniformLocation(char* uniformName) const;
	/**
	 *	Get attribute variable location based on its name
	 *
	 * @param attributeName : attribute name
	 */
	GLuint getAttributeLocation(char* attributeName) const;
	/**
	 *	Set a texture unit number to a uniform
	 *
	 * @param uniformLocation : the texture sampler uniform location
	 * @param textureUnitNumber : the number of the associated texture unit
	 */
	static void setTextureToUniform(GLuint uniformLocation, int textureUnitNumber);

};


#endif
