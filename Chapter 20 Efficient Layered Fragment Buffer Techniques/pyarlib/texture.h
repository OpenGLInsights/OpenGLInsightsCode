/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

/* texture.h pknowles 2010-05-19 18:49:47 */

/*
Compile:
	Include texture.h
	Add tecture.c, png_loader.c and deflate.c to the makefile
	Compile with -lGL
Usage:
	Use getTexture(filename) to load a png image in to OpenGL
	texture memory. The returned GLuint is OpenGL's reference
	to this texture. Use with glBindTexture(GL_TEXTURE2D, texture).
Notes:
	You must enable GL_TEXTURE_2D before use.
	Enable GL_BLEND for transparency.
	Change the blend method using glBlendFunc. A common example is
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	In general, draw all transparent objects last and disable the depth
	write using glDepthMask(TRUE) while drawing.
	If you do not use additive blending depth sorting manually is
	necessary for correct transparency blending.
*/

#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

//struct Image;
//typedef struct Image Image;

namespace QI {
	struct Image;
};

unsigned int getTexture(const char* filename);
unsigned int getTextureI(QI::Image* img);
int getLastImageWidth();
int getLastImageHeight();
void setTextureRepeat(bool enable = true);
void setTextureMipmap(float anisotropy = 1.0);

#endif
