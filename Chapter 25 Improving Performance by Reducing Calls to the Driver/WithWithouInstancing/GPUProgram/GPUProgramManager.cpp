
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

#include "GPUProgramManager.h"

#include <string>
#include <fstream>

#include <iostream>

GPUProgramManager* GPUProgramManager::gpuProgramManager = NULL;


GPUProgramManager::GPUProgramManager()
{
}

GPUProgramManager::~GPUProgramManager()
{
}

GPUProgramManager::GPUProgramManager(GPUProgramManager&)
{
	//not used
}

GPUProgramManager& GPUProgramManager::getInstance()
{
	if(gpuProgramManager==NULL)
	{
		//instance creation
		gpuProgramManager = new GPUProgramManager();
	}
	return *gpuProgramManager;
}

void GPUProgramManager::destroy()
{
	if(gpuProgramManager!=NULL)
	{
		//unload and delete all gpu program
		std::map<std::string, GPUProgram*>::iterator it;
		for(it = gpuProgramManager->gpuPrograms.begin(); it!=gpuProgramManager->gpuPrograms.end(); ++it)
		{
			it->second->unload();
			delete it->second;
		}

		//delete instance
		delete gpuProgramManager;
	}
	gpuProgramManager = NULL;
}


bool GPUProgramManager::loadGPUProgram(const char* name, const char* vertexProgram, const char* fragmentProgram, const char* geometryProgram, int GS_inputPrimitiveType, int GS_outputPrimitiveType, int GS_maxVerticesOut)
{
	bool ret = false;
	//load the gpu program using vertex and fragment shader source
	GPUProgram* gpup = new GPUProgram();
	if(gpup->load(vertexProgram,fragmentProgram,geometryProgram,GS_inputPrimitiveType,GS_outputPrimitiveType,GS_maxVerticesOut))
	{
		this->gpuPrograms[name] = gpup;
		ret = true;
	}
	else
		delete gpup;
	return ret;
}


bool GPUProgramManager::loadGPUProgramFromDisk(const char* name, const char* vertexProgramFilepath, const char* fragmentProgramFilepath, const char* geometryProgramFilepath, int GS_inputPrimitiveType, int GS_outputPrimitiveType, int GS_maxVerticesOut)
{
	int length;
	char* buffer;
	std::string vpSource("");
	std::string fpSource("");
	std::string gpSource("");

	//test if specified files exist on the disk
	std::ifstream fileVP(vertexProgramFilepath, std::ios::binary);
	if((fileVP.rdstate() & std::ifstream::failbit ) != 0)
		return false;
	std::ifstream fileFP(fragmentProgramFilepath, std::ios::binary);
	if((fileFP.rdstate() & std::ifstream::failbit ) != 0)
	{
		fileVP.close();
		return false;
	}
	
	std::ifstream fileGP;
	if(geometryProgramFilepath!=NULL)
	{
		fileGP.open(geometryProgramFilepath, std::ios::binary);
		std::ifstream fileGP(geometryProgramFilepath, std::ios::binary);
		if((fileGP.rdstate() & std::ifstream::failbit ) != 0)
		{
			fileVP.close();
			fileFP.close();
			return false;
		}
	}

	//get length of file content
	fileVP.seekg (0, std::ios::end);
	length = fileVP.tellg();
	fileVP.seekg (0, std::ios::beg);
	// allocate memory:
	buffer = new char [length];
	// read data as a block
	fileVP.read (buffer,length);
	vpSource.assign(buffer,length);
	delete [] buffer;

	//get length of file content
	fileFP.seekg (0, std::ios::end);
	length = fileFP.tellg();
	fileFP.seekg (0, std::ios::beg);
	// allocate memory:
	buffer = new char [length];
	// read data as a block
	fileFP.read (buffer,length);
	fpSource.assign(buffer,length);
	delete [] buffer;

	//closing file
	fileVP.close();
	fileFP.close();
	
	if(geometryProgramFilepath!=NULL)
	{
		//get length of file content
		fileGP.seekg (0, std::ios::end);
		length = fileGP.tellg();
		fileGP.seekg (0, std::ios::beg);
		// allocate memory:
		buffer = new char [length];
		// read data as a block
		fileGP.read (buffer,length);
		gpSource.assign(buffer,length);
		delete [] buffer;
		fileGP.close();

		//load gpu program
		this->loadGPUProgram( name, vpSource.c_str(), fpSource.c_str(), gpSource.c_str(), GS_inputPrimitiveType, GS_outputPrimitiveType, GS_maxVerticesOut );
	}
	else
	{
		//load gpu program
		this->loadGPUProgram( name, vpSource.c_str(), fpSource.c_str() );
	}

	return true;
}


const GPUProgram* GPUProgramManager::getGPUProgram(const char* name) const
{
	GPUProgram* ret = NULL;
	//search for the program
	std::map<std::string, GPUProgram*>::const_iterator it = this->gpuPrograms.find(name);
	if(it!=this->gpuPrograms.end())
	{
		ret = it->second;
	}
	else
	{
		ret = NULL;
	}

	return ret;
}



