/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <stdio.h>
#include <string.h>
#include <string>
#include <map>

#include "mesh/simpleobj/obj.h"
#include "vbomesh.h"
#include "material.h"
#include "imgpng.h"
#include "fileutil.h"

#include "meshobj.h"

using namespace QI;

bool VBOMeshOBJ::registerLoader()
{
	return VBOMesh::registerLoader(".obj", load);
}
bool VBOMeshOBJ::load(VBOMesh& mesh, const char* filename)
{
	mesh.release();
	mesh.error = false;
	
	std::string path = basefilepath(filename);
	
	OBJMesh* obj = objMeshLoad(filename);
	if (!obj)
	{
		mesh.error = true;
		printf("Error opening %s\n", filename);
		return false;
	}
	
	//printf("v%i t%c n%c i%i m%i\n", obj->numVertices, obj->hasTexCoords?'t':'f', obj->hasNormals?'t':'f', obj->numIndices, obj->numFacesets);
	
	//don't copy. it's already in the right format
	mesh.dataIndices = obj->indices;
	mesh.data = obj->vertices;
	mesh.numVertices = obj->numVertices;
	mesh.numIndices = obj->numIndices;
	mesh.numPolygons = mesh.numIndices / 3;
	
	for (int i = 0; i < obj->numMaterials; ++i)
	{
		Material* m = new Material;
		if (obj->materials[i].texture)
			m->imgColour = new ImagePNG((path + "/" + obj->materials[i].texture).c_str());
		mesh.addMaterial(m, obj->materials[i].name);
	}
	
	for (int i = 0; i < obj->numFacesets; ++i)
		if (obj->facesets[i].material >= 0)
			mesh.useMaterial(obj->facesets[i].indexStart, obj->facesets[i].indexEnd, obj->materials[obj->facesets[i].material].name);
	
	/*
	for (int i = 0; i < obj->numFacesets; ++i)
		printf("%i %i->%i m%i(%s) s%i\n", i, obj->facesets[i].indexStart, obj->facesets[i].indexEnd,
			obj->facesets[i].material,
			obj->facesets[i].material >= 0 ? obj->materials[obj->facesets[i].material].name : "<none>",
			obj->facesets[i].smooth);
	*/
	
	obj->indices = NULL;
	obj->vertices = NULL;
	obj->numIndices = 0;
	obj->numVertices = 0;
	//... stolen
	
	mesh.has[VERTICES] = true;
	mesh.has[NORMALS] = obj->hasNormals > 0 ? true : false;
	mesh.has[TEXCOORDS] = obj->hasTexCoords > 0 ? true : false;
	mesh.has[TANGENTS] = false;
	mesh.interleaved = true;
	mesh.indexed = true;
	mesh.calcInternal();
	mesh.primitives = GL_TRIANGLES;
	
	//since pointers were set to NULL, this is ok
	objMeshFree(&obj);
	
	if (!mesh.has[NORMALS])
	{
		printf("Generating normals\n");
		mesh.generateNormals();
	}
	return true;

}
