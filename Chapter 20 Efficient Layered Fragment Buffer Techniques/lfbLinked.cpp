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

#include "lfbLinked.h"
#include "imageBuffer.h"
#include "extraglenums.h"

//this is stupid, but it works
static ImageBuffer* justOneInt = NULL;

LinkedLFB::LinkedLFB()
{
	alloc = new ImageBuffer(GL_R32UI);
	headPtrs = new ImageBuffer(GL_R32UI);
	nextPtrs = new ImageBuffer(GL_R32UI);
	data = new ImageBuffer(lfbDataType);
}
LinkedLFB::~LinkedLFB()
{
	delete alloc;
	delete headPtrs;
	delete nextPtrs;
	delete data;
}
bool LinkedLFB::resize(vec2i dim)
{
	if (!LFB::resize(dim))
		return false;
	
	alloc->resize(sizeof(unsigned int));
	headPtrs->resize(sizeof(unsigned int) * totalPixels);
	
	memory["Counter"] = alloc->size();
	memory["Head Ptrs"] = headPtrs->size();
	
	return true;
}
bool LinkedLFB::resizePool(int allocs)
{
	int frags = allocs;
	
	totalFragments = frags;
	
	//is a reallocation needed?
	if (frags < allocFragments * LFB_UNDERALLOCATE || allocFragments < frags)
	{
		//yes, allocate a little more than needed
		allocFragments = (int)(frags * LFB_OVERALLOCATE);
		nextPtrs->resize(allocFragments * sizeof(unsigned int));
		data->resize(allocFragments * lfbDataStride);
		
		memory["Next Ptrs"] = nextPtrs->size();
		memory["Data"] = data->size();
		
		if (!data->ready) //debugging
			printf("Error resizing pool data %ix%i=%.2fMB\n", allocFragments, lfbDataStride, allocFragments * lfbDataStride / 1024.0 / 1024.0);
		return true;
	}
	return false;
}
void LinkedLFB::initBuffers()
{
	//index zero is reserved as NULL pointer, so we start at one
	//yes, this means allocating all of 4 extra bytes, but it means zeroBuffer() can be reused
	static const int one = 1;
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, *alloc);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(unsigned int), &one, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	
	//zero the head pointers, or rather, set each pointer to NULL
	zeroBuffer(headPtrs);
}
void LinkedLFB::setDefines(Shader& program)
{
	LFB::setDefines(program);
	program.define("LFB_METHOD_H", "lfbLinked.glsl");
}
bool LinkedLFB::setUniforms(Shader& program)
{
	if (!alloc->ready || !headPtrs->ready)
		return false;
	
	if (state != PRE_INIT && (!nextPtrs->ready || !data->ready) && allocFragments > 0)
		return false;

	ImageBuffer::unbindAll();
	
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, *alloc);
	
	glUniform2i(glGetUniformLocation(program, "winSize"), size.x, size.y);
	glUniform1i(glGetUniformLocation(program, "fragAlloc"), allocFragments);
		
	//writing, depending on the state, determines READ_ONLY, WRITE_ONLY and READ_WRITE ImageBuffer data
	bool writing = state!=DRAWING;
	
	headPtrs->bind(0, "headPtrs", program, true, writing);
	if (nextPtrs->ready)
		nextPtrs->bind(1, "nextPtrs", program, !writing, writing);
	if (data->ready)
		data->bind(2, "data", program, !writing, writing);
	
	return true;
}
void LinkedLFB::begin()
{
	//mark the start of the frame for profiler averaging
	if (profile) profile->begin();
	
	//parent begin - may trigger ::resize()
	LFB::begin();
	
	//zero alloc counter and head pointers
	initBuffers();
	glMemoryBarrierEXT(GL_BUFFER_UPDATE_BARRIER_BIT_EXT);
	
	if (profile) profile->time("Zero");
}
bool LinkedLFB::count()
{
	glMemoryBarrierEXT(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT);
	glMemoryBarrierEXT(GL_ATOMIC_COUNTER_BARRIER_BIT_EXT);
	
	if (profile) profile->time("Render");
	
	LFB::count();
	
	//read the alloc atomic counter to find the number of fragments (or pages)
	//if this number is greater than that allocated, a re-render is needed
	#if 0
	//in some strange circumstances (found during the grid tests) this is really slow
	unsigned int* d = (unsigned int*)alloc->map(true, false);
	unsigned int numAllocs = d[0];
	alloc->unmap();
	#else
	//this is stupid but works, somehow avoiding the sync/blocking issue
	if (!justOneInt)
	{
		justOneInt = new ImageBuffer(GL_R32UI);
		justOneInt->resize(alloc->size());
	}
	alloc->copy(*justOneInt);
	unsigned int* d = (unsigned int*)justOneInt->map(true, false);
	unsigned int numAllocs = d[0];
	justOneInt->unmap();
	#endif
	
	if (profile) profile->time("Read Total");
	
	//FIXME: if pool is reducing in size a re-render is still done
	if (resizePool(numAllocs))
	{
		printf("Linked List Re-Render\n");
		initBuffers();
		return true; //please do a second pass - we didn't allocate enough
	}
	else
		return false; //render is done, no second pass needed
}
int LinkedLFB::end()
{
	LFB::end();
	glMemoryBarrierEXT(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT);
	
	//NOTE: profile will average this time, so for it to become significant many re-renders must be done
	if (profile) profile->time("Re-Render");
	
	return totalFragments - 1; //take one because the element zero is "null"
}
std::string LinkedLFB::getName()
{
	return "LFB Linked";
}


