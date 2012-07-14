/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#include <assert.h>
#include <stdio.h>

#include <string>
#include <map>
#include <vector>
#include <set>

#include "pyarlib/shader.h"
#include "pyarlib/shaderutil.h"
#include "pyarlib/includegl.h"
#include "pyarlib/util.h"

#include "prefixSums.h"

#include "imageBuffer.h"

static Shader shaderPSums("prefixSums");

static ImageBuffer* justOneInt = NULL;

unsigned int prefixSums(ImageBuffer* data)
{
	//remember the previous shader program
	int programBackup;
	glGetIntegerv(GL_CURRENT_PROGRAM, &programBackup);

	//read and check data's info
	assert(data->getFormat() == GL_R32UI);
	int sizeInBytes = data->size();
	int sizeInInts = sizeInBytes / sizeof(unsigned int);
	int sumsSize = 1 << ilog2(sizeInInts);
	//assert(nextPowerOf2(sumsSize) == sumsSize);
	if (sizeInInts != nextPowerOf2(sumsSize) && sizeInInts != nextPowerOf2(sumsSize)+1)
		printf("Warning: strange prefix sums size\n");
	
	//everything is done in the vertex shader. ignore fragments
	glEnable(GL_RASTERIZER_DISCARD);
		
	//first sums pass - "up-sweep"
	shaderPSums.use();
	data->bind(0, "sums", shaderPSums, true, true);
	shaderPSums.set("pass", 0);
	for (int step = 2; step <= sumsSize; step <<= 1)
	{
		shaderPSums.set("stepSize", step);
		int kernelSize = sumsSize/step;
		glDrawArrays(GL_POINTS, 0, kernelSize);
		glMemoryBarrierEXT(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT);
	}
	
	//the bit that's "slow"
	//could be caused by sync/blocking issues or the entire buffer is being mapped (less likely)
	//as a workaround, "justOneInt" is used as a temporary
	#if 0
	glBindBuffer(GL_TEXTURE_BUFFER, *data);
	CHECKERROR;
	//unsigned int* sums = (unsigned int*)glMapBufferRange(GL_TEXTURE_BUFFER, (sumsSize-1)*sizeof(unsigned int), sizeof(unsigned int), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	unsigned int* sums = (unsigned int*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, sumsSize*sizeof(unsigned int), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	CHECKERROR;
	//int totalFragments = sums[0];
	//sums[0] = 0; //this line would replace the glBufferSubData block below
	glUnmapBuffer(GL_TEXTURE_BUFFER);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	CHECKERROR;
	#endif
	
	//save the total sum
	if (!justOneInt)
	{
		justOneInt = new ImageBuffer(GL_R32UI);
		justOneInt->resize(sizeof(unsigned int));
	}
	data->copy(*justOneInt, (sumsSize-1)*sizeof(unsigned int), 0, sizeof(unsigned int));

	//write zero to the very last position
	glBindBuffer(GL_TEXTURE_BUFFER, *data); //the * operator dereferences data which then returns its bufferObject
	unsigned int zero = 0;
	glBufferSubData(
		GL_TEXTURE_BUFFER,
		(sumsSize-1)*sizeof(unsigned int),
		sizeof(unsigned int),
		&zero);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	
	//second sums pass - "down-sweep"
	shaderPSums.set("pass", 1);
	for (int step = sumsSize; step >= 2; step >>= 1)
	{
		shaderPSums.set("stepSize", step);
		int kernelSize = sumsSize/step;
		glDrawArrays(GL_POINTS, 0, kernelSize);
		glMemoryBarrierEXT(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT);
	}
	shaderPSums.unuse();
	
	//read the saved total sum
	unsigned int prefixSumsTotal;
	prefixSumsTotal = *(unsigned int*)justOneInt->map(true, false);
	justOneInt->unmap();
	CHECKERROR;

	//cleanup and return total sum	
	glDisable(GL_RASTERIZER_DISCARD);
	glUseProgram(programBackup);
	return prefixSumsTotal;
}
