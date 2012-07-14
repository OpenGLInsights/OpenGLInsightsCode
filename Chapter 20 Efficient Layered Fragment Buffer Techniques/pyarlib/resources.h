/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#ifndef RESOURCES_H
#define RESOURCES_H

#ifdef _WIN32

//on windows, EMBED() by adding the file to resources.rc
//the resource ID must be the filename in quotes. eg "filetoembed.txt"
#define EMBED(n, f)
#define RESOURCE(n) loadResource(#n)
#define RESOURCELEN(n) loadResourceLen(#n)

const char* loadResource(const char* name);
int loadResourceLen(const char* name);

#else
#include "embed.h"
#define EMBED(n, f) BINDATA(n, f)
#define RESOURCE(n) &n
#define RESOURCELEN(n) n##_len

#endif

#endif
