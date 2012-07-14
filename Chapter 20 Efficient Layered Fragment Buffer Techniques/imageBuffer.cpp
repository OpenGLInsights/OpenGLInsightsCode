/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "pyarlib/prec.h"
#include "pyarlib/shader.h"
#include "pyarlib/includegl.h"

#include <set>
#include <algorithm>
#include <assert.h>
#include <stdio.h>

#include "imageBuffer.h"

static std::set<int> boundImageUnits;

unsigned int getReadWrite(bool read, bool write)
{
	assert(read || write);
	if (read)
	{
		if (write)
			return GL_READ_WRITE;
		else
			return GL_READ_ONLY;
	}
	return GL_WRITE_ONLY;
}
	
ImageBuffer::ImageBuffer(unsigned int format)
{
	ready = false;

	currentSize = 0;
	this->format = format;
	
	textureObject = 0;
	bufferObject = 0;
}
ImageBuffer::~ImageBuffer()
{
	if (currentSize > 0)
		release();
}
void ImageBuffer::release()
{
	glDeleteTextures(1, &textureObject);
	glDeleteBuffers(1, &bufferObject);
	currentSize = 0;
	
	ready = false;
}
void ImageBuffer::create(int size)
{
	CHECKERROR;
	
	glGenBuffers(1, &bufferObject);
	glGenTextures(1, &textureObject);
	
	ready = false;
	
	if (allocate(size))
	{
		//map the GL_TEXTURE_BUFFER texture to the buffer object
		glBindTexture(GL_TEXTURE_BUFFER, textureObject);
		glTexBuffer(GL_TEXTURE_BUFFER, format, bufferObject);
		glBindTexture(GL_TEXTURE_BUFFER, 0);
	}
}
bool ImageBuffer::allocate(int bytes)
{
	CHECKERROR;

	glBindBuffer(GL_TEXTURE_BUFFER, bufferObject);
	glBufferData(GL_TEXTURE_BUFFER, bytes, NULL, GL_DYNAMIC_DRAW);

	//at a guess, sometimes memory gets transferred to virtual memory which would explain some weird slowdowns
	//this was an attempt to stop that happening. I'd prefer a simple crash to 1fps
	//glMakeBufferResidentNV(GL_TEXTURE_BUFFER, GL_READ_WRITE);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	
	if (glGetError() == GL_OUT_OF_MEMORY)
	{
		printf("ERROR: Out of memory.\n");
		release();
		return false;
	}
	else
	{
		currentSize = bytes;
		return true;
	}
}
bool ImageBuffer::resize(int size)
{
	size = std::max(0, size);
	
	static int maxSize = 0;
	if (maxSize == 0)
		glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &maxSize);

	if (size == currentSize)
		return false;

	if (size > 1024*1024*1024)
	{
		//sometimes opengl does not give OUT_OF_MEMORY and simply freezes
		printf("Error: TextureBuffer is >1GB: %.1fMB. Aborting.\n", size/1024.0f/1024.0f);
		return false;
	}
	else if (size > maxSize)
	{
		printf("Warning: TextureBuffer(%.1fMB) larger than\n\t GL_MAX_TEXTURE_BUFFER_SIZE(%.1fMB).\n",
			size/1024.0f/1024.0f,
			maxSize/1024.0f/1024.0f);
	}

	if (currentSize == 0)
		create(size); //if no data is allocated, create the buffer (calls allocate)
	else if (size > 0)
	{
		allocate(size); //resize data for the buffer

		//QUICK-FIX: on AMD cards textureObject appears stuck pointing to the wrong memory location after a resize
		//even if glTexBuffer is called again. Hence delete, generate and rebind:
		glDeleteTextures(1, &textureObject);
		glGenTextures(1, &textureObject);
		glBindTexture(GL_TEXTURE_BUFFER, textureObject);
		glTexBuffer(GL_TEXTURE_BUFFER, format, bufferObject);
		glBindTexture(GL_TEXTURE_BUFFER, 0);
	}
	else
		release(); //resizing to zero, release the data
	
	ready = (currentSize > 0);
	
	return true;
}
bool ImageBuffer::bind(int index, const char* name, unsigned int program, bool read, bool write)
{
	GLuint loc = glGetUniformLocation(program, name);
	if (loc == (GLuint)-1)
	{
		printf("Error: %s does not exist in program %i\n", name, program);
		return false;
	}
	else
	{
		//NOTE: I've seen other code bind the TEXTURE_BUFFER to (ACTIVE_TEXTURE0 + index) here
		//As far as I can tell, this is not needed - "texture image units" (glActiveTexture) and image units are completely separate
		glUniform1i(loc, index);
		GLenum readwrite = getReadWrite(read, write);
		glBindImageTextureEXT(index, textureObject, 0, GL_FALSE, 0, readwrite, format);
		boundImageUnits.insert(index);
	}
	return true;
}
void ImageBuffer::unbindAll()
{
	for (std::set<int>::iterator it = boundImageUnits.begin(); it != boundImageUnits.end(); ++it)
	{
		glBindImageTextureEXT(*it, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	}
	boundImageUnits.clear();
}
void* ImageBuffer::map(bool read, bool write)
{
	void* ptr;
	GLenum readwrite = getReadWrite(read, write);
	glBindBuffer(GL_TEXTURE_BUFFER, bufferObject);
	ptr = glMapBuffer(GL_TEXTURE_BUFFER, readwrite);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	return ptr;
}
bool ImageBuffer::unmap()
{
	bool ok;
	glBindBuffer(GL_TEXTURE_BUFFER, bufferObject);
	ok = (glUnmapBuffer(GL_TEXTURE_BUFFER) != GL_FALSE);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	return ok;
}
void ImageBuffer::copy(ImageBuffer& dest, int offsetFrom, int offsetTo, int size)
{
	if (size < 0)
		size = currentSize;
	assert(currentSize >= offsetFrom + size);
	assert(dest.currentSize >= offsetTo + size);
	glBindBuffer(GL_COPY_READ_BUFFER, bufferObject);
	glBindBuffer(GL_COPY_WRITE_BUFFER, dest.bufferObject);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, offsetFrom, offsetTo, size);
}
