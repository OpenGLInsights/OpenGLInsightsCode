/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef VBOMESH_CTM_H
#define VBOMESH_CTM_H

#include "vbomesh.h"

class VBOMeshCTM : VBOMesh
{
public:
	static bool smoothGeneratedNormals;
	static bool registerLoader();
	static bool load(VBOMesh& mesh, const char* filename);
};

#endif
