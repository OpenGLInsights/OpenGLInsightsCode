
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

#ifndef GPUPROGRAMMANAGER_H
#define GPUPROGRAMMANAGER_H

#include <map>

#include "GPUProgram.h"
#include <string>



#define GL_BUFFER_OFFSET(i) ((char*)NULL+(i))

/**
 * Implement a singleton pattern.
 *
 *	A gpu program mananger use to send gpu program to graphic card. The source of shaders
 * can be loaded from program memory or from disk.
 *
 */
class GPUProgramManager
{
private:

	/**
	 *	The instance of the gpu manager. (pattern singleton)
	 */
	static GPUProgramManager* gpuProgramManager;

	/**
	 *	Constructor
	 */
	GPUProgramManager();
	/**
	 *	Destructor
	 */
	~GPUProgramManager();
	/**
	 *	 copy Constructor
	 */
	GPUProgramManager(GPUProgramManager&);

	/**
	 *	The map of gpuProgram sorted by name
	 */
	std::map<std::string, GPUProgram*> gpuPrograms;

protected:

public:

	/**
	 *	Use to get the manager
	 *
	 * @return a reference one the manager
	 */
	static GPUProgramManager& getInstance();

	/**
	 *	Used to destroy the manager. All gpu program pointer become invalid.
	 */
	static void destroy();

	///////////////////////////////////
	//	Manipulation de program GPU  //
	///////////////////////////////////

	/**
	 *	Load, compile and link a gpu program in memory.
	 *
	 * @param name : the gpu program name
	 * @param vertexProgram : the vertex shader source
	 * @param fragmentProgram : the fragment shader source
	 *
	 * @return true if all works well
	 */
	bool loadGPUProgram(const char* name, const char* vertexProgram, const char* fragmentProgram, const char* geometryProgram=NULL, int GS_inputPrimitiveType=GL_TRIANGLES, int GS_outputPrimitiveType=GL_TRIANGLE_STRIP, int GS_maxVerticesOut=3);
	/**
	 *	Load, compile and link a gpu program in memory currently stored on the disk.
	 * REM : the shader file must a blank line at its end.
	 *
	 * @param name : the gpu program name
	 * @param vertexProgramFilepath : the vertex shader source
	 * @param fragmentProgramFilepath : the fragment shader source
	 *
	 * @return true if all works well
	 */
	bool loadGPUProgramFromDisk(const char* name, const char* vertexProgramFilepath, const char* fragmentProgramFilepath, const char* geometryProgramFilepath=NULL, int GS_inputPrimitiveType=GL_TRIANGLES, int GS_outputPrimitiveType=GL_TRIANGLE_STRIP, int GS_maxVerticesOut=3);
       
	////////////////////////////
	//	Acc�s au program GPU  //
	////////////////////////////

	/**
	 *	Used to get a gpu program
	 *
	 * @param name : the gpu program name we search for
	 *
	 * @return a pointer to the found gpu program or NULL if not found.
	 */
	const GPUProgram* getGPUProgram(const char* name) const;


};


#endif
