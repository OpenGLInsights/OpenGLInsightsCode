/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef VBOMESH_3DS_H
#define VBOMESH_3DS_H

#include "vbomesh.h"
#include "matstack.h"

class VBOMesh3DS : VBOMesh
{
public:
	static bool registerLoader();
	static bool load(VBOMesh& mesh, const char* filename);
};

#endif
