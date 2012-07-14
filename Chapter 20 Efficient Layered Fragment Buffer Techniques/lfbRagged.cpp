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

#include "lfbRagged.h"
#include "imageBuffer.h"
#include "prefixSums.h"

//when blending, this shader copies the blend texture into the ImageBuffer
static Shader shaderCopy("lfbCopy");
static Shader shaderSort("lfbRaggedSort");

void RaggedLFB::backupFBO()
{
	//the user may have bound an FBO, to render the transparency to a
	//texture. this stores whatever FBO is bound for replacement after blending
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &backupFBOHandle);
}
void RaggedLFB::restoreFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, backupFBOHandle);
}

RaggedLFB::RaggedLFB()
{
	countUsingBlending = false;
	globalSort = false;
	prefixSumsSize = 0;
	
	offsets = new ImageBuffer(GL_R32UI);
	data = new ImageBuffer(lfbDataType);
	ids = new ImageBuffer(GL_R32UI);
	
	blendFBO = 0;
	blendTex = 0;
}
RaggedLFB::~RaggedLFB()
{
	delete offsets;
	delete data;
	glDeleteFramebuffers(1, &blendFBO);
	glDeleteTextures(1, &blendTex);
}
void RaggedLFB::useBlending(bool enable)
{
	//check this was called outside rendering
	if (state != FIRST_PASS)
		countUsingBlending = enable;
	else
	{
		printf("Error: cannot change RaggedLFB counting mode while rendering\n");
		return;
	}
	
	//as an alternative to atomicAdd()-ing to counts, blending can be used
	if (totalPixels && countUsingBlending)
	{
		//create FBO and blend texture
		if (!blendFBO)
			glGenFramebuffers(1, &blendFBO);
		if (!blendTex)
			glGenTextures(1, &blendTex);
	
		//set up blendTex
		prefixSumsHeight = size.y + ceil(prefixSumsSize - totalPixels, size.x);
		assert(prefixSumsHeight * size.x >= prefixSumsSize);
		glBindTexture(GL_TEXTURE_2D, blendTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		CHECKERROR;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, size.x, prefixSumsHeight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
		CHECKERROR;
		glBindTexture(GL_TEXTURE_2D, 0);
		
		memory["Blend"] = size.x * prefixSumsHeight * 4;
	
		//attach blendTex to blendFBO	
		CHECKERROR;
		glBindFramebuffer(GL_FRAMEBUFFER, blendFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blendTex, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		CHECKERROR;
	}
	else if (blendFBO || blendTex)
	{
		//free blending data
		glDeleteFramebuffers(1, &blendFBO);
		glDeleteTextures(1, &blendTex);
		blendFBO = 0;
		blendTex = 0;
		memory.erase("Blend");
	}
}
void RaggedLFB::useGlobalSort(bool enable)
{
	globalSort = enable;
	
	if (!globalSort && ids)
		ids->resize(0);
}
bool RaggedLFB::resize(vec2i dim)
{
	if (!LFB::resize(dim))
		return false; //don't need to do anything

	assert(dim.x > 0 && dim.y > 0);
	
	//the prefix sum algorithm used requires 2^n data
	prefixSumsSize = nextPowerOf2(totalPixels);
	offsets->resize(sizeof(unsigned int) * (prefixSumsSize + 1));
	
	memory["Offsets"] = offsets->size();

	//a resize of the blend texture is needed
	useBlending(countUsingBlending);
	
	return true; //needed to resize
}
void RaggedLFB::setDefines(Shader& program)
{
	LFB::setDefines(program);
	program.define("COUNT_USING_BLENDING", intToString((int)countUsingBlending));
	program.define("LFB_METHOD_H", "lfbRagged.glsl");
	program.define("GLOBAL_SORT", intToString((int)globalSort));
}
bool RaggedLFB::setUniforms(Shader& program)
{
	if (!offsets->ready)
		return false;
		
	if ((state == SECOND_PASS || state == DRAWING) && (!data->ready || (globalSort && !ids->ready)) && allocFragments > 0)
		return false;
	
	//writing, depending on the state, determines READ_ONLY, WRITE_ONLY and READ_WRITE ImageBuffer data
	bool writing = state!=DRAWING;

	ImageBuffer::unbindAll();

	glUniform2i(glGetUniformLocation(program, "winSize"), size.x, size.y);
	if (state != SORTING)
		offsets->bind(1, "offsets", program, true, writing);
	if (data->size() > 0)
		data->bind(2, "data", program, !writing, writing);
	
	if (globalSort)
	{
		if (state == SORTING || state == SECOND_PASS)
			if (ids->size() > 0)
				ids->bind(3, "fragIDs", program, true, writing);
	}
	
	glUniform1i(glGetUniformLocation(program, "depthOnly"), (state==FIRST_PASS?1:0));
	
	return true;
}
void RaggedLFB::initBlending()
{
	backupFBO();
		
	glBindFramebuffer(GL_FRAMEBUFFER, blendFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blendTex, 0);
	glPushAttrib(GL_COLOR_BUFFER_BIT);
	glClearColor(0,0,0,0);
	//clear the entire offset table
	//(including areas outside the current viewport)
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
}
void RaggedLFB::copyBlendResult()
{
	//cleanup blending state
	glPopAttrib();
	restoreFBO();

	//copy blending results (the count data) into offsets
	shaderCopy.use();
	glBindTexture(GL_TEXTURE_2D, blendTex);
	shaderCopy.set("blendCount", 0); //set the sampler2D
	offsets->bind(1, "offsets", shaderCopy, false, true);
	shaderCopy.set("width", size.x);
	glEnable(GL_RASTERIZER_DISCARD);
	glDrawArrays(GL_POINTS, 0, prefixSumsSize);
	glDisable(GL_RASTERIZER_DISCARD);
	glBindTexture(GL_TEXTURE_2D, 0);
	shaderCopy.unuse();
}
void RaggedLFB::begin()
{
	//mark the start of the frame for profiler averaging
	if (profile) profile->begin();
	
	//parent begin - may trigger ::resize()
	LFB::begin();
	
	//zero lookup tables
	if (!countUsingBlending)
	{
		zeroBuffer(offsets);
		glMemoryBarrierEXT(GL_BUFFER_UPDATE_BARRIER_BIT_EXT);
	}
	
	if (profile) profile->time("Zero");
	
	//set up blending FBO
	if (countUsingBlending)
	{
		initBlending();
		if (profile) profile->time("Clear");
	}
}
bool RaggedLFB::count()
{
	CHECKERROR;
	//finish all the imageAtomicAdds for fragment counting
	glMemoryBarrierEXT(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT);
	LFB::count();
	
	if (profile) profile->time("Count");
	
	if (countUsingBlending)
	{
		copyBlendResult();
		if (profile) profile->time("Copy");
	}
	
	//perform parallel prefix sums
	totalFragments = prefixSums(offsets);
	
	if (profile) profile->time("PSums");
	
	
	//Counts are not stored as they can be reconstructed
	//as the difference between consecutive offsets. Having the total
	//stored at the end of the offsets saves checking offset array bounds in the shader.
	//However, after rendering they have shifted which means a zero needs to be stored at array index -1 to avoid the if statement
	//The simplest solution was to keep the if statment (very minimal performance overhead)
	/*
	//copy the total into the offset data
	glBindBuffer(GL_TEXTURE_BUFFER, *offsets); //the * operator dereferences data which then returns its bufferObject
	glBufferSubData(
		GL_TEXTURE_BUFFER,
		(prefixSumsSize)*sizeof(unsigned int),
		sizeof(unsigned int),
		&totalFragments);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	*/
	
	//allocate data
	allocFragments = ceil(totalFragments, 8) * 8;
	data->resize(allocFragments * lfbDataStride);
	if (globalSort)
		ids->resize(allocFragments * sizeof(int));
	memory["Data"] = data->size();
	
	if (profile) profile->time("Alloc");
	
	return true; //always need to do a second pass
}
int RaggedLFB::end()
{
	glMemoryBarrierEXT(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT);
	if (profile) profile->time("Render");
	
	state = SORTING;

	//global sort
	if (globalSort && allocFragments > 0)
	{
		assert(allocFragments % 8 == 0);
		shaderSort.use();
		setUniforms(shaderSort);
		glDrawArrays(GL_POINTS, 0, allocFragments / 8);
		shaderSort.unuse();
		if (profile) profile->time("Sort");
	}
	
	LFB::end();
	return totalFragments;
}
std::string RaggedLFB::getName()
{
	return "LFB Ragged";
}

