/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include <stdio.h>

#include <string>
#include <map>
#include <vector>
#include <set>
#include <sstream>

#include "lfb.h"

#include "pyarlib/includegl.h"
#include "pyarlib/shader.h"
#include "pyarlib/shaderutil.h"
#include "pyarlib/util.h"
#include "imageBuffer.h"

ImageBuffer* LFB::zeroes = NULL;

static Shader shaderZeroes("lfbZero");

LFB::LFB() : size(0, 0)
{
	maxFrags = 16;

	totalPixels = size.x * size.y;
	totalFragments = 0;
	allocFragments = 0;
	state = PRE_INIT;
	profile = NULL;
	
	lfbDataType = GL_RGBA32F;
	lfbDataStride = 4 * sizeof(float);
}
LFB::~LFB()
{
}
bool LFB::resize(vec2i dim)
{
	if (size == dim)
		return false;
		
	size = dim;
	
	totalPixels = size.x * size.y;
	
	printf("LFB resize: %ix%i\n", size.x, size.y);
	
	return true;
}
void LFB::zeroBuffer(ImageBuffer* buffer)
{
	if (buffer->size() == 0)
		return; //nothing to do

	CHECKERROR;
	if (!zeroes)
	{
		zeroes = new ImageBuffer(GL_R32UI);
	}
	if (buffer->size() > zeroes->size())
	{
		zeroes->resize(buffer->size());
		memory["Zeroes"] = zeroes->size();

		int currentProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
		if (currentProgram != 0)
			printf("Warning: program active before init zero buffer %s:%i\n", __FILE__, __LINE__);
		
		//generate zero buffer
		glEnable(GL_RASTERIZER_DISCARD);
		shaderZeroes.use();
		zeroes->bind(0, "tozero", shaderZeroes, false, true);
		
		glDrawArrays(GL_POINTS, 0, zeroes->size() / sizeof(unsigned int));
		glMemoryBarrierEXT(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT);
		
		shaderZeroes.unuse();
		glDisable(GL_RASTERIZER_DISCARD);
		
		#if 0
		//at one point I thought the above was failing
		unsigned int* d = (unsigned int*)zeroes->map(true, true);
		for (int i = 0; i < zeroes->size() / (int)sizeof(unsigned int); ++i)
		{
			//printf("%i %i\n", i, d[i]);
			d[i] = 0;
		}
		zeroes->unmap();
		#endif
	}
		
	//FIXME: really could do multiple copies and allocate less zero data
	CHECKERROR;
	zeroes->copy(*buffer, 0, 0, buffer->size());
	CHECKERROR;
}
int LFB::getTotalPixels()
{
	return totalPixels;
}
void LFB::setMaxFrags(int n)
{
	maxFrags = mymax(2, n);
}
void LFB::setDefines(Shader& program)
{
	
	if (program.define("MAX_FRAGS", maxFrags))
		printf("%s MAX_FRAGS = %i\n", program.name().c_str(), maxFrags);
	
	//LFB_READONLY could also be defined here, however
	//in case additional operations need to be performed, such
	//as writing after sorting, LFB_READONLY is left for each shader to define itself
}
void LFB::begin()
{
	//assume the current viewport is the resolution of the LFB
	//FIXME: for a more general purpose LFB, set resize() to public. perhaps allow a 1D and 3D resize too
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	vec2i dim(viewport[2], viewport[3]);
	dim.x = std::max(dim.x, 32);
	dim.y = std::max(dim.y, 32);
	resize(dim);
	
	state = FIRST_PASS;
}
bool LFB::count()
{
	state = SECOND_PASS;
	return false; //is a second pass needed? no by default
}
int LFB::end()
{
	state = DRAWING;
	return 0;
}
void LFB::sort(bool inMainVideoMem)
{
	printf("sort not implemented\n");
	//a shader which simply calls loadFragments, sortFragments and writeFragments would be used here
	//useless for transparency
}
std::string LFB::getMemoryInfo()
{
	std::stringstream ret;
	int total = 0;
	for (std::map<std::string, int>::iterator it = memory.begin(); it != memory.end(); ++it)
	{
		total += it->second;
		ret << it->first << ": " << humanBytes(it->second, false) << std::endl;
	}
	ret << "Total" << ": " << humanBytes(total, false) << std::endl;
	return ret.str();
}




