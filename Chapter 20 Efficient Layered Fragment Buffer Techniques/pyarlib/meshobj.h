/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef VBOMESH_OBJ_H
#define VBOMESH_OBJ_H

#include "vbomesh.h"

class VBOMeshOBJ : VBOMesh
{
public:
	static bool registerLoader();
	static bool load(VBOMesh& mesh, const char* filename);
};

#endif
