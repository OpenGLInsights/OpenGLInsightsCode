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

#include "lfbPages.h"
#include "imageBuffer.h"
#include "extraglenums.h"

PagesLFB::PagesLFB()
{
	semaphores = new ImageBuffer(GL_R32UI);
	counts = new ImageBuffer(GL_R32UI);
	pageSize = 4;
}
PagesLFB::~PagesLFB()
{
	delete semaphores;
	delete counts;
}
bool PagesLFB::resize(vec2i dim)
{
	if (!LinkedLFB::resize(dim))
		return false;
	
	semaphores->resize(sizeof(unsigned int) * totalPixels);
	counts->resize(sizeof(unsigned int) * totalPixels);
	
	memory["Semaphores"] = semaphores->size();
	memory["Counts"] = counts->size();
	
	zeroBuffer(semaphores);
	
	return true;
}
bool PagesLFB::resizePool(int allocs)
{
	//NOTE: we don't have access to the exact number of fragments
	//      totalFragments will commonly be higher than the true number
	return LinkedLFB::resizePool(allocs * pageSize);
}
void PagesLFB::initBuffers()
{
	LinkedLFB::initBuffers();
	zeroBuffer(counts);
	//don't need to zero semaphores each render
}
void PagesLFB::setDefines(Shader& program)
{
	LFB::setDefines(program);
	program.define("LFB_PAGE_SIZE", intToString(pageSize));
	program.define("LFB_METHOD_H", "lfbPages.glsl");
}
bool PagesLFB::setUniforms(Shader& program)
{
	if (!LinkedLFB::setUniforms(program))
		return false;

	if (!semaphores->ready || !counts->ready)
		return false;
	
	bool writing = state!=DRAWING;
	
	//linked lists already uses bind points 0-2
	if (writing)
		semaphores->bind(3, "semaphores", program, true, true);
	counts->bind(4, "counts", program, true, writing);
	
	glUniform1i(glGetUniformLocation(program, "pageSize"), pageSize);

	return true;
}
std::string PagesLFB::getName()
{
	return "LFB Pages";
}
