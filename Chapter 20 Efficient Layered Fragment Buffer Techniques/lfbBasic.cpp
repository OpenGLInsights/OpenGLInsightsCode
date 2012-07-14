/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include <assert.h>
#include <stdio.h>

#include <string>
#include <vector>
#include <map>
#include <set>

#include "pyarlib/shaderutil.h"
#include "pyarlib/shader.h"
#include "pyarlib/util.h"

#include "lfbBasic.h"
#include "imageBuffer.h"

BasicLFB::BasicLFB()
{
	numLayers = LFB_DEFAULT_NUMLAYERS;
	counts = new ImageBuffer(GL_R32UI);
	data = new ImageBuffer(lfbDataType);
	query = 0;
	waitingForQuery = false;
}
BasicLFB::~BasicLFB()
{
	delete counts;
	delete data;
	if (query)
		glDeleteQueries(1, &query);
}
bool BasicLFB::resize(vec2i dim)
{
	bool resized = false;
	if (LFB::resize(dim))
	{
		resized = true;
		counts->resize(sizeof(unsigned int) * totalPixels);
		memory["Counts"] = counts->size();
	}

	allocFragments = dim.x * dim.y * numLayers;
	
	//attempt reallocation if needed
	if (data->size() / lfbDataStride != allocFragments)
	{
		resized = true;
		//common for the brute force method to cause out-of-memory
		while (!data->resize(lfbDataStride * allocFragments))
		{
			//if resize failed, halve layers and retry
			numLayers >>= 1;
			printf("Falling back to %i layers\n", numLayers);
			allocFragments = dim.x * dim.y * numLayers;
			if (allocFragments == 0)
			{
				//just in case even 1 layer is too much
				//data remains unallocated and setUniforms will return false
				break;
			}
		}
	}
		
	memory["Data"] = data->size();
		
	if (!query)
		glGenQueries(1, &query);
	
	return resized;
}
void BasicLFB::setDefines(Shader& program)
{
	LFB::setDefines(program);
	program.define("LFB_METHOD_H", "lfbBasic.glsl");
}
bool BasicLFB::setUniforms(Shader& program)
{
	if (!counts->ready || !data->ready)
		return false;
	CHECKERROR;

	glUniform2i(glGetUniformLocation(program, "winSize"), size.x, size.y);
	glUniform1i(glGetUniformLocation(program, "dataDepth"), numLayers);
	CHECKERROR;

	ImageBuffer::unbindAll();
	
	bool writing = state!=DRAWING;
	
	//bind(bind point index, name, shader program, bool read, bool write)
	counts->bind(0, "counts", program, true, writing);

	#if 1
	data->bind(1, "data", program, !writing, writing);
	#else
	//AMD fix: something is wrong with image unit binding on AMD cards.
	//I randomly came across this hack, simply using two variable names.
	//use data for writing in lfbRender and anotherData in lfbDisplay
	//perhaps an image unit is reserved for one program only and it can't be changed?
	//generally AMD is more strict than nvidia, perhaps I've missed something?
	if (writing)
		data->bind(1, "data", program, false, true);
	if (!writing)
		data->bind(2, "anotherData", program, true, false);
	#endif
	
	CHECKERROR;
	return true;
}
void BasicLFB::begin()
{
	CHECKERROR;
	if (profile) profile->begin();
	
	LFB::begin();
	
	//zero lookup tables
	zeroBuffer(counts);
	glMemoryBarrierEXT(GL_BUFFER_UPDATE_BARRIER_BIT_EXT);
	
	if (profile) profile->time("Zero");
	
	CHECKERROR;
	//don't start a new query until the result has been read from the current one
	//NOTE: ATI doesn't appear to like a sample query and time query at the same time
	//      so profile->time is not called between begin/end query
	if (!waitingForQuery)
		glBeginQuery(GL_SAMPLES_PASSED, query);
	CHECKERROR;
}
bool BasicLFB::count()
{
	CHECKERROR;
	glMemoryBarrierEXT(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT);

	LFB::count();
	
	//end query if one was running
	if (!waitingForQuery)
	{
		glEndQuery(GL_SAMPLES_PASSED);
		waitingForQuery = true;
	}
	CHECKERROR;

	if (profile) profile->time("Render");
	
	//read query results if available. if not, try again next frame
	int available;
	glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &available);
	if (available)
	{
		glGetQueryObjectuiv(query, GL_QUERY_RESULT, (GLuint*)&totalFragments);
		waitingForQuery = false;
	}

	if (profile) profile->time("Read Total");
	
	//the maximum depth complexity is unknown so
	//no point resizing or re-rendering
	return false;
}
int BasicLFB::end()
{
	LFB::end();
	return totalFragments;
}
void BasicLFB::setNumLayers(int n)
{
	numLayers = n;
}
std::string BasicLFB::getName()
{
	return "LFB Basic";
}
