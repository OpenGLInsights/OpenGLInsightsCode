/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include "includegl.h"

#include <stdio.h>

#include "img.h"

namespace QI
{

Image::Image()
{
	repeat = true;
	mipmap = false;
	nearest = false;
	anisotropy = 0;
	width = height = 0;
	channels = 0;
}
Image::~Image()
{
	width = height = 0;
	channels = 0;
}
bool Image::loadImage(std::string filename)
{
	printf("Error: loading using abstract class Image");
	return false;
}
bool Image::saveImage(std::string filename)
{
	printf("Error: saving using abstract class Image");
	return false;
}
unsigned int Image::bufferTexture()
{
	GLuint tex;
	GLuint format;

	if (width == 0 || height == 0 || channels == 0 || data.get() == NULL)
	{
		//printf("Error: cannot buffer incomplete image\n");
		return 0;
	}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	
	if (anisotropy > 0)
	{
		mipmap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);

	GLenum filterMag;
	GLenum filterMin;
	filterMag = nearest ? GL_NEAREST : GL_LINEAR;
	if (mipmap)
		filterMin = nearest ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR;
	else
		filterMin = nearest ? GL_NEAREST : GL_LINEAR;

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);

	switch (channels)
	{
	case 1: format = GL_LUMINANCE; break;
	case 2: format = GL_LUMINANCE_ALPHA; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	default:
		printf("Error: invalid png channel count: %i\n", channels);
		return 0;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data.get());

	if (mipmap)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}
void Image::bufferTexture(unsigned int object, unsigned long target)
{
	GLenum format;
	switch (channels)
	{
	case 1: format = GL_LUMINANCE; break;
	case 2: format = GL_LUMINANCE_ALPHA; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	default:
		printf("Error: invalid png channel count: %i\n", channels);
		return;
	}

	if (target == (unsigned long)-1)
		target = GL_TEXTURE_2D;
	GLenum bindTarget = target;
	if (target >= GL_TEXTURE_CUBE_MAP_POSITIVE_X && target < GL_TEXTURE_CUBE_MAP_POSITIVE_X + 6)
		bindTarget = GL_TEXTURE_CUBE_MAP;

	GLint internalFormat;
	glBindTexture(bindTarget, object);
	glGetTexLevelParameteriv(target, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
	glTexImage2D(target, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data.get());
	glBindTexture(bindTarget, 0);
}
void Image::readTexture(unsigned int id)
{
	glBindTexture(GL_TEXTURE_2D, id);

	int nwidth, nheight;
	int internalFormat;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &nwidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &nheight);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

	int nchannels;
	bool depth = false;
	switch (internalFormat)
	{
	case GL_DEPTH_COMPONENT:
	case GL_DEPTH_COMPONENT16:
	case GL_DEPTH_COMPONENT24:
	case GL_DEPTH_COMPONENT32:
		depth = true;
	case GL_RED:
	case GL_R32F:
	case GL_LUMINANCE:
		nchannels = 1;
		break;
	case GL_LUMINANCE_ALPHA:
		nchannels = 2;
		break;
	case GL_BGR:
	case GL_RGB:
	case GL_RGB32F:
		nchannels = 3;
		break;
	case GL_BGRA:
	case GL_RGBA:
	case GL_RGBA32F:
	case GL_RGBA32I:
	case GL_RGBA32UI:
	case GL_DEPTH24_STENCIL8:
		nchannels = 4;
		break;
	default:
		printf("Error: Cannot read unknown texture format\n");
		return;
	}

	if (nwidth != width || nheight != height || nchannels != channels)
	{
		width = nwidth;
		height = nheight;
		channels = nchannels;
		unsigned char* imageData = new unsigned char[width*height*channels];
		data = RCByteArray(imageData);
	}

	int format;
	switch (channels)
	{
	case 1: format = GL_LUMINANCE; break;
	case 2: format = GL_LUMINANCE_ALPHA; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	default:
		printf("Error: invalid png channel count: %i\n", channels);
		return;
	}
	if (depth)
		format = GL_DEPTH_COMPONENT;

	glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, data.get());
	glBindTexture(GL_TEXTURE_2D, 0);
}

}
