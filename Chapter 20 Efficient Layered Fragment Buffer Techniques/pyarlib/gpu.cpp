/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <iostream>
#include <fstream>
#include <string>

#include "gpu.h"
#include "util.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "shader.h"
#include "shaderutil.h"

using namespace std;

unsigned int getGLReadWrite(bool read, bool write)
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

RenderTarget::RenderTarget(GLuint type) : GPUObject(type)
{
	if (type == (GLuint)-1)
		size = vec2i(-1);
}
/*
void RenderTarget::resize(vec2i size)
{
	if (size.x == 0 || size.y == 0)
	{
		release();
		return;
	}

	int cpp = channelsPerPixel(format);
	GLenum informat = defaultFormat(cpp);
	if (format == GL_DEPTH24_STENCIL8 ||
		format == GL_DEPTH_COMPONENT ||
		format == GL_DEPTH_COMPONENT16 ||
		format == GL_DEPTH_COMPONENT24 ||
		format == GL_DEPTH_COMPONENT32F)
		informat = GL_DEPTH_COMPONENT;

	switch (type)
	{
	case GL_TEXTURE_2D:
		if (object == 0)
			glGenTextures(1, &object);
		bind();		
		applyAttribsToBound();
		glTexImage2D(type, 0, format, size.x, size.y, 0, informat, GL_UNSIGNED_BYTE, NULL);
		this->size = size;
		unbind();
		break;
	case GL_RENDERBUFFER:
		if (object == 0)
			glGenRenderbuffers(1, &object);
		bind();
		glRenderbufferStorage(type, format, size.x, size.y);
		this->size = size;
		unbind();
		break;
	}
}
bool RenderTarget::release()
{
	size = vec2i(0);
	if (object == 0)
		return false;

	switch (type)
	{
	case GL_TEXTURE_2D:
		glDeleteTextures(1, &object);
		break;
	case GL_RENDERBUFFER:
		glDeleteRenderbuffers(1, &object);
		break;
	}
	object = 0;
	return true;
}
void RenderTarget::bind()
{
	switch (type)
	{
	case GL_TEXTURE_2D:
		glBindTexture(type, object);
		break;
	case GL_RENDERBUFFER:
		glBindRenderbuffer(type, object);
		break;
	}
}
void RenderTarget::unbind()
{
	switch (type)
	{
	case GL_TEXTURE_2D:
		glBindTexture(type, 0);
		break;
	case GL_RENDERBUFFER:
		glBindRenderbuffer(type, 0);
		break;
	}
}*/

TextureAttribs::TextureAttribs()
{
	mipmap = false;
	anisotropy = 0;
	nearest = false;
	repeat = false;
}

void TextureAttribs::applyAttribs(GLuint target)
{	
	CHECKERROR;
	GLuint wrap = repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
	GLuint mag_filter = nearest ? GL_NEAREST : GL_LINEAR;
	GLuint min_filter;
	if (mipmap)
	{
		min_filter = nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR;
		if (anisotropy > 1)
			glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
	}
	else
		min_filter = mag_filter;

	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag_filter);
	CHECKERROR;
}

bool Texture::resize(vec2i size)
{
	if (size.x == this->size.x && size.y == this->size.y)
		return false;
	if (size.x == 0 || size.y == 0)
	{
		release();
		return true;
	}

	int cpp = channelsPerPixel(format);
	GLenum informat = defaultFormat(cpp);
	if (format == GL_DEPTH24_STENCIL8 ||
		format == GL_DEPTH_COMPONENT ||
		format == GL_DEPTH_COMPONENT16 ||
		format == GL_DEPTH_COMPONENT24 ||
		format == GL_DEPTH_COMPONENT32F)
		informat = GL_DEPTH_COMPONENT;

	if (object == 0)
		glGenTextures(1, &object);

	bind();
	applyAttribs(type);
	if (multisample)
	{
		CHECKERROR;
		glTexImage2DMultisample(type, samples, format, size.x, size.y, GL_FALSE);
		CHECKERROR;
	}
	else
		glTexImage2D(type, 0, format, size.x, size.y, 0, informat, GL_UNSIGNED_BYTE, NULL);
	this->size = size;
	unbind();

	CHECKERROR;
	return true;
}

bool Texture::release()
{
	size = vec2i(0);
	if (object == 0)
		return false;

	glDeleteTextures(1, &object);
	object = 0;
	return true;
}
void Texture::bind()
{
	glBindTexture(type, object);
}
void Texture::unbind()
{
	glBindTexture(type, 0);
}
void Texture::genMipmap()
{
	bind();
	if (mipmap)
		glGenerateMipmap(type);
	unbind();
}

Texture2D::Texture2D(vec2i size, GLuint format, int samples) : Texture(samples>0?GL_TEXTURE_2D_MULTISAMPLE:GL_TEXTURE_2D)
{
	multisample = samples > 0;
	this->samples = samples;
	this->format = format;
	resize(size);
}
void Texture2D::randomize()
{
	assert(!multisample);
	int cpp = channelsPerPixel(format);
	GLenum informat = defaultFormat(cpp);
	int n = size.x * size.y * cpp;
	char* dat = new char[n];
	for (int i = 0; i < n; ++i)
		dat[i] = rand() % 256;
	bind();
	glTexImage2D(type, 0, format, size.x, size.y, 0, informat, GL_UNSIGNED_BYTE, dat);
	delete[] dat;
}

Texture3D::Texture3D(vec3i size, GLuint format, int samples) : Texture(GL_TEXTURE_3D)
{
	multisample = samples > 0;
	this->samples = samples;
	this->format = format;
	resize(size);
}

bool Texture3D::resize(vec3i size)
{
	layers = size.z;
	return resize(vec2i(size.x, size.y));
}

bool Texture3D::resize(vec2i size)
{
	if (size.x == this->size.x && size.y == this->size.y)
		return false;
	if (size.x == 0 || size.y == 0)
	{
		release();
		return true;
	}

	int cpp = channelsPerPixel(format);
	GLenum informat = defaultFormat(cpp);
	if (format == GL_DEPTH24_STENCIL8 ||
		format == GL_DEPTH_COMPONENT ||
		format == GL_DEPTH_COMPONENT16 ||
		format == GL_DEPTH_COMPONENT24 ||
		format == GL_DEPTH_COMPONENT32F)
		informat = GL_DEPTH_COMPONENT;

	if (object == 0)
		glGenTextures(1, &object);
	bind();
	applyAttribs(type);
	glTexImage3D(type, 0, format, size.x, size.y, layers, 0, informat, GL_UNSIGNED_BYTE, NULL);
	this->size = size;
	unbind();
	return true;
}

TextureCubeMap::TextureCubeMap(vec2i size, GLuint format, int samples) : Texture(GL_TEXTURE_CUBE_MAP)
{
	multisample = samples > 0;
	this->samples = samples;
	this->format = format;
	resize(size);
}

bool TextureCubeMap::resize(vec2i size)
{
	if (size.x == this->size.x && size.y == this->size.y)
		return false;
	
	if (size.x == 0 || size.y == 0)
	{
		release();
		return true;
	}
	
	assert(size.x == size.y); //cubemaps must have equal dimensions

	int cpp = channelsPerPixel(format);
	GLenum informat = defaultFormat(cpp);
	if (format == GL_DEPTH24_STENCIL8 ||
		format == GL_DEPTH_COMPONENT ||
		format == GL_DEPTH_COMPONENT16 ||
		format == GL_DEPTH_COMPONENT24 ||
		format == GL_DEPTH_COMPONENT32F)
		informat = GL_DEPTH_COMPONENT;

	if (object == 0)
		glGenTextures(1, &object);

	glBindTexture(type, object);
	applyAttribs(type);
	for (int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, size.x, size.y, 0, informat, GL_UNSIGNED_BYTE, NULL);
	}
	glBindTexture(type, 0);
	this->size = size;
	return true;
}

RenderBuffer::RenderBuffer(vec2i size, GLuint format, int samples) : RenderTarget(GL_RENDERBUFFER)
{
	multisample = samples > 0;
	this->samples = samples;
	this->format = format;
	resize(size);
}

bool RenderBuffer::resize(vec2i size)
{
	if (size.x == this->size.x && size.y == this->size.y)
		return false;
	if (size.x == 0 || size.y == 0)
	{
		release();
		return true;
	}

	if (object == 0)
		glGenRenderbuffers(1, &object);
	bind();
	if (multisample)
	{
		glRenderbufferStorageMultisample(type, samples, format, size.x, size.y);
	}
	else
		glRenderbufferStorage(type, format, size.x, size.y);
	this->size = size;
	unbind();
	return true;
}
bool RenderBuffer::release()
{
	size = vec2i(0);
	if (object == 0)
		return false;

	glDeleteRenderbuffers(1, &object);
	object = 0;
	return true;
}
void RenderBuffer::bind()
{
	glBindRenderbuffer(type, object);
}
void RenderBuffer::unbind()
{
	glBindRenderbuffer(type, 0);
}

GPUBuffer::GPUBuffer(GLenum type, GLenum access, bool writeable) : GPUObject(type)
{
	this->image = 0;
	this->address = 0;
	this->access = access;
	this->dataSize = 0;
	this->writeable = writeable;
	this->bindless = false;
}
void GPUBuffer::bind()
{
	glBindBuffer(type, object);
}
void GPUBuffer::unbind()
{
	glBindBuffer(type, 0);
}
bool GPUBuffer::resize(int bytes, bool force)
{
	if (dataSize == bytes || (!force && bytes < dataSize))
		return false;
		
	if (dataSize == 0) //if not allocated
		glGenBuffers(1, &object); //bytes must be non-zero
		
	if (bytes == 0)
		release();
	else
	{
		bind();
		glBufferData(type, bytes, NULL, access);
		if (bindless)
		{
			glMakeBufferResidentNV(type, writeable ? GL_READ_WRITE : GL_READ_ONLY);
			glGetBufferParameterui64vNV(type, GL_BUFFER_GPU_ADDRESS_NV, &address);
		}
		unbind();
		dataSize = bytes;
	}
	return true;
}
void GPUBuffer::buffer(const void* data, int bytes, int byteOffset)
{
	int maxSize = bytes + byteOffset;
	if (dataSize < maxSize)
		resize(maxSize);
		
	if (maxSize == 0)
		return; //buffer may not exist. nothing to do anyway
	
	bind();
	if (dataSize == bytes)
		glBufferData(type, bytes, data, access);
	else
		glBufferSubData(type, byteOffset, bytes, data);
	unbind();
}
void GPUBuffer::createImage(GLenum format)
{
	if (image == 0)
		glGenTextures(1, &image);
		
	imageFormat = format;
	
	glBindTexture(GL_TEXTURE_BUFFER, image);
	glTexBuffer(GL_TEXTURE_BUFFER, format, object);
	glBindTexture(GL_TEXTURE_BUFFER, 0);
}
void* GPUBuffer::map(bool read, bool write)
{
	if (!dataSize)
		return NULL;
	bind();
	void* ptr = glMapBuffer(type, getGLReadWrite(read, write));
	unbind();
	return ptr;
}
void* GPUBuffer::map(unsigned int offset, unsigned int size, bool read, bool write)
{
	if (!dataSize)
		return NULL;
	bind();
	void* ptr = glMapBufferRange(type, offset, size, getGLReadWrite(read, write));
	unbind();
	return ptr;
}
bool GPUBuffer::unmap()
{
	if (!dataSize)
		return true;
	bind();
	bool ok = (glUnmapBuffer(type) == GL_TRUE);
	unbind();
	return ok;
}
int GPUBuffer::size()
{
	return dataSize;
}
bool GPUBuffer::release()
{
	if (!object)
		return false;
	assert(dataSize > 0);
	glDeleteBuffers(1, &object);
	object = 0;
	address = 0;
	dataSize = 0;
	return true;
}
bool GPUBuffer::setUniform(Shader* program, const std::string& name) const
{
	GLuint loc = glGetUniformLocation(*program, name.c_str());
	if (loc == (GLuint)-1)
		return false;
	else
	{
		if (bindless)
		{
			glProgramUniformui64NV(*program, loc, address);
		}
		else if (image)
		{
			//NOTE: I've seen other code bind the TEXTURE_BUFFER to (ACTIVE_TEXTURE0 + index) here
			//As far as I can tell, this is not needed - "texture image units" (glActiveTexture) and image units are completely separate
			int index = program->unique("image", name);
			glUniform1i(loc, index);
			GLenum readwrite = getGLReadWrite(true, true);
			glBindImageTextureEXT(index, image, 0, GL_FALSE, 0, readwrite, imageFormat);
		}
	}
	return true;
}
FrameBuffer::FrameBuffer(vec2i size) : GPUObject(GL_FRAMEBUFFER)
{
	depth = NULL;
	stencil = NULL;

	hasResize = false;
	hasAttach = false;
	maxAttach = 0;
	
	for (int i = 0; i < 16; ++i)
		colour[i] = NULL;
		
	resize(size);
}
bool FrameBuffer::attach(GLenum attachment, RenderTarget* target)
{
	if (!target || !*target)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, 0, 0);
		CHECKERROR;
		return true;
	}

	switch (target->type)
	{
	case (GLuint)-1:
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, 0, 0);
		CHECKERROR;
		return true;
	case GL_TEXTURE_2D:
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target->type, *target, 0);
		break;
	case GL_TEXTURE_3D:
	case GL_TEXTURE_CUBE_MAP:
	case GL_TEXTURE_2D_MULTISAMPLE:
		glFramebufferTexture(GL_FRAMEBUFFER, attachment, *target, 0);
		break;
	case GL_RENDERBUFFER:
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, target->type, *target);
		break;
	default:
		return false;
	}
	assert(!CHECKERROR);

	//printf("%i %i == %i %i\n", size.x, size.y, target.size.x, target.size.y);
	assert(size.x == target->size.x && size.y == target->size.y);
	return true;
}
bool FrameBuffer::resize(vec2i size)
{
	if (maxAttach == 0)
	{
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttach);
		maxAttach = maxAttach > 16 ? 16 : maxAttach;
	}

	if (size.x == this->size.x && size.y == this->size.y)
		return false;

	if (size.x == 0 || size.y == 0)
	{
		return release();
	}
	else
		hasResize = true;

	this->size = size;

	if (!object)
		glGenFramebuffers(1, &object);
		
	if (depth) depth->resize(size);
	if (stencil) stencil->resize(size);
	for (int i = 0; i < maxAttach; ++i)
		if (colour[i]) colour[i]->resize(size);
		
	return true;
}
void FrameBuffer::attach()
{
	CHECKERROR;
	//an opengl FBO must exist (which it will with positive FrameBuffer size) to attach()
	assert(size.x > 0 && size.y > 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, object);
	CHECKERROR;

	if (!attach(GL_DEPTH_ATTACHMENT, depth))
		printf("Error: Unknown FBO depth attachment type.\n");
CHECKERROR;
	if (!attach(GL_STENCIL_ATTACHMENT, stencil))
		printf("Error: Unknown FBO stencil attachment type.\n");
CHECKERROR;
	GLenum colourBuffers[16];
	int numColourBuffers = 0;
	for (int i = 0; i < maxAttach; ++i)
	{
		//hasColour = hasColour || (colour[i] && *colour[i]);
		if (!attach(GL_COLOR_ATTACHMENT0 + i, colour[i]))
			printf("Error: Unknown FBO colour[%i] attachment type.\n", i);
		if (colour[i])
			colourBuffers[numColourBuffers++] = GL_COLOR_ATTACHMENT0 + i;
	}

	//TODO: should glReadBuffer be changed?
	if (numColourBuffers == 0)
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	else
		glDrawBuffers(numColourBuffers, colourBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE: /*printf("GL_FRAMEBUFFER_COMPLETE\n");*/ break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: printf("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT\n"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: printf("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT\n"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: printf("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT\n"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: printf("GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\n"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: printf("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT\n"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: printf("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT\n"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: printf("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT\n"); break;
	case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE: printf("GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE\n"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: printf("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n"); break;
	case GL_FRAMEBUFFER_UNSUPPORTED: printf("GL_FRAMEBUFFER_UNSUPPORTED_EXT\n"); break;
	default:  printf("Unknown framebuffer status error: %i\n", status); break;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	hasAttach = true;
}
void FrameBuffer::bind()
{
	if (!hasAttach)
		printf("Error: Binding FBO without attaching. Add render targets and call attach()\n");
	if (!hasResize)
		printf("Error: Binding FBO without allocating. Call resize()\n");

	glGetIntegerv(GL_VIEWPORT, backupViewport);
	glBindFramebuffer(GL_FRAMEBUFFER, object);
	glViewport(0, 0, size.x, size.y);
}
void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(
		backupViewport[0],
		backupViewport[1],
		backupViewport[2],
		backupViewport[3]);
}
bool FrameBuffer::release()
{
	if (object == 0)
		return false;

	if (depth) depth->release();
	if (depth) stencil->release();
	for (int i = 0; i < maxAttach; ++i)
		if (colour[i]) colour[i]->release();
	glDeleteFramebuffers(1, &object);
	object = 0;
	size  = vec2i(0);

	return true;
}
void FrameBuffer::blit(GLuint target, bool blitDepth, vec2i offset, vec2i scaleTo)
{
	if (scaleTo.x < 0) scaleTo.x = size.x;
	if (scaleTo.y < 0) scaleTo.y = size.y;
	glBindFramebuffer(GL_READ_FRAMEBUFFER, object);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target);
	scaleTo += offset;
	if (colour[0])
		glBlitFramebuffer(0, 0, size.x, size.y, offset.x, offset.y, scaleTo.x, scaleTo.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	if (blitDepth && depth)
		glBlitFramebuffer(0, 0, size.x, size.y, offset.x, offset.y, scaleTo.x, scaleTo.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); //do I really need to unbind??
}

