/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#ifndef SHADER_UTIL_H
#define SHADER_UTIL_H

#include "includegl.h"
#include "vec.h"

struct mat44;

bool loadGlewExtensions();
GLenum defaultFormat(int cpp);
int bytesPerPixel(GLenum format);
int channelsPerPixel(GLenum format);
#define setActiveTexture(index, name, texture) _setActiveTexture(index, name, texture, GL_TEXTURE_2D, __LINE__)
void _setActiveTexture(int i, const char* name, GLuint tex, GLuint target = GL_TEXTURE_2D, int line = 0);
void genCubePerspective(mat44 modelviewProjection[6], float n, float f);
void genCubeDirections(mat44 modelview[6]);
void drawSSQuad(vec2i size);
void drawSSQuad(vec3i size);
void drawSSQuad(int w = -1, int h = -1, int layers = 1);
void drawCube();
const char* getLastShaderError();

#endif
