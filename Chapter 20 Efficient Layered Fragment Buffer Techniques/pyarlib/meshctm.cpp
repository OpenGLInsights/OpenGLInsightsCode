/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <stdio.h>
#include <string.h>
#include <string>
#include <map>

#include "mesh/openctm/openctm.h"

#include "vbomesh.h"
#include "meshctm.h"

bool VBOMeshCTM::smoothGeneratedNormals = false;
bool VBOMeshCTM::registerLoader()
{
	return VBOMesh::registerLoader(".ctm", load);
}

bool VBOMeshCTM::load(VBOMesh& mesh, const char* filename)
{
	mesh.release();
	mesh.error = false;
	
	//http://openctm.sourceforge.net/media/DevelopersManual.pdf
	CTMcontext context;

	// Create a new importer context
	context = ctmNewContext(CTM_IMPORT);

	// Load the OpenCTM file
	ctmLoad(context, filename);
	CTMenum err = ctmGetError(context);
	if(err != CTM_NONE)
	{
		printf("Error opening %s\n", filename);
		ctmFreeContext(context);
		mesh.error = true;
		return false;
	}

	// Access the mesh data
	mesh.numVertices = ctmGetInteger(context, CTM_VERTEX_COUNT);
	int norms = ctmGetInteger(context, CTM_HAS_NORMALS);
	int texcs = ctmGetInteger(context, CTM_UV_MAP_COUNT);
	int other = ctmGetInteger(context, CTM_ATTRIB_MAP_COUNT);
	mesh.numPolygons = ctmGetInteger(context, CTM_TRIANGLE_COUNT);
	mesh.numIndices = 3 * mesh.numPolygons;

	mesh.sub[VERTICES] = (float*)ctmGetFloatArray(context, CTM_VERTICES);
	if (norms)
		mesh.sub[NORMALS] = (float*)ctmGetFloatArray(context, CTM_NORMALS);
	if (texcs)
		mesh.sub[TEXCOORDS] = (float*)ctmGetFloatArray(context, CTM_UV_MAP_1);
	if (other)
		mesh.sub[TANGENTS] = (float*)ctmGetFloatArray(context, CTM_ATTRIB_MAP_1);
		
	delete[] mesh.dataIndices;
	mesh.dataIndices = new unsigned int[mesh.numIndices];
	mesh.indexed = true;

	memcpy(mesh.dataIndices, ctmGetIntegerArray(context, CTM_INDICES), mesh.numIndices * sizeof(unsigned int));
	
	if (!norms)
	{
		printf("Generating normals\n");
		
		float* tmp = NULL;
		if (smoothGeneratedNormals)
		{
			//generate normals as though vertices were smoothed a little
			tmp = mesh.sub[VERTICES];
			mesh.sub[VERTICES] = new float[mesh.numVertices*3];
			memcpy(mesh.sub[VERTICES], tmp, mesh.numVertices*3*sizeof(float));
			
			mesh.averageVertices();
		}
		
		mesh.generateNormals();
		
		if (tmp)
		{
			//free tmp data - use actual vertices
			delete[] mesh.sub[VERTICES];
			mesh.sub[VERTICES] = tmp;
		}
	}

	mesh.interleave(false);

	//important - remove pointers. we didn't allocate that data
	mesh.sub[VERTICES] = NULL;
	mesh.sub[NORMALS] = NULL;
	mesh.sub[TEXCOORDS] = NULL;
	mesh.sub[TANGENTS] = NULL;

	// Free the context
	ctmFreeContext(context);

	return true;

}
