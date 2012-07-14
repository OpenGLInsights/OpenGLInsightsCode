/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef PYAR_INCLUDE_GL_H
#define PYAR_INCLUDE_GL_H

#include <GL/glew.h>

#if defined(_WIN32) || defined(_WIN64)
#pragma warning (disable:4996)
#define NOMINMAX
#include <windows.h>

#define snprintf sprintf_s
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#endif