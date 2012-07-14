/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"
/* texture.c pknowles 2010-05-19 18:49:47 */

#include "texture.h"
#include "includegl.h"

#include "imgpng.h"

static int lastWidth = 0;
static int lastHeight = 0;

static bool repeatTex = false;
static bool mipmapTex = false;
static float mipmapAnisotropy = 2.0;

int getLastImageWidth()
{
	return lastWidth;
}

int getLastImageHeight()
{
	return lastHeight;
}

int onbits(int num)
{
	int i = 1;
	int ret = 0;
	while (i <= num)
	{
		if (num & i)
			++ret;
		i = i << 1;
	}
	return ret;
}

unsigned int getTexture(const char* filename)
{
	GLuint tex;
	QI::ImagePNG img;
	if (!img.loadImage(filename))
	{
		//printf("Could not load %s\n", filename);
		return 0;
	}
	lastWidth = img.width;
	lastHeight = img.height;
	tex = getTextureI(&img);
	return tex;
}

unsigned int getTextureI(QI::Image* img)
{
	GLuint tex;
	GLuint format;
	
	if (img == NULL)
		return 0;

	if (img->channels == 1)
		format = GL_LUMINANCE;
	else if (img->channels == 2)
		format = GL_LUMINANCE_ALPHA;
	else if (img->channels == 3)
		format = GL_RGB;
	else if (img->channels == 4)
		format = GL_RGBA;
	else
		return 0;
	
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeatTex ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeatTex ? GL_REPEAT : GL_CLAMP_TO_EDGE);

	GLenum filter;
	if (mipmapTex)
		filter = GL_LINEAR_MIPMAP_LINEAR;
	else
		filter = GL_LINEAR;

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	
	if (mipmapTex && mipmapAnisotropy > 1.0)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, mipmapAnisotropy);

	//glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, format, img->width, img->height, 0, format, GL_UNSIGNED_BYTE, img->data);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, format, img->width, img->height, format, GL_UNSIGNED_BYTE, img->data);
	if (mipmapTex)
		glGenerateMipmap(GL_TEXTURE_2D);
	return tex;
}

void setTextureRepeat(bool enable)
{
	repeatTex = enable;
}

void setTextureMipmap(float anisotropy)
{
	float maxAnisotropy; 
	//a crash on the following line probably means GL isn't initialized yet
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	mipmapAnisotropy = anisotropy;
	mipmapTex = anisotropy > 0.0;
}

