/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <stdio.h>
#include <string.h>
#include <string>
#include <map>

#include "mesh/lib3ds/lib3ds.h"

#include "fileutil.h"
#include "matstack.h"
#include "vbomesh.h"
#include "material.h"
#include "imgpng.h"
#include "mesh3ds.h"

using namespace QI;

struct MeshNode
{
	Lib3dsNode* node;
	mat44 matrix;
	MeshNode(Lib3dsNode* n, mat44 m = mat44::identity())
	{
		node = n;
		matrix = m;
	}
};

bool VBOMesh3DS::registerLoader()
{
	return VBOMesh::registerLoader(".3ds", load);
}

bool VBOMesh3DS::load(VBOMesh& mesh, const char* filename)
{
	mesh.release();
	mesh.error = false;

	std::string path = basefilepath(filename);
	
	Lib3dsFile* f = lib3ds_file_open(filename);
	if (!f)
	{
		printf("Error loading %s\n", filename);
		mesh.error = true;
		return false;
	}
	
	int vertexOffset = 0;
	int indexOffset = 0;
	mesh.numVertices = 0;
	mesh.numIndices = 0;
	bool hasTexCoords = false;
	
	//extract all instances
	typedef std::pair<std::string, mat44> Instance;
	std::vector<Instance> instances;
	std::set<std::string> isInstanced;
	std::vector<MeshNode> nodes;
	for (Lib3dsNode* p = f->nodes; p != NULL; p = p->next)
		nodes.push_back(MeshNode(p, mat44(p->matrix))); //add root
	while (nodes.size() > 0)
	{
		//get next
		MeshNode n = nodes.back();
		nodes.pop_back();
		
		//add children
		for (Lib3dsNode* p = n.node->childs; p != NULL; p = p->next)
			nodes.push_back(MeshNode(p, n.matrix * mat44(p->matrix)));
		
		//if mesh instance, extract
		if (n.node->type == LIB3DS_NODE_MESH_INSTANCE)
		{
			Lib3dsMeshInstanceNode *inst = (Lib3dsMeshInstanceNode*)n.node;
			printf("\n");
			PRINTVEC3(*(vec4f*)inst->pos);
			PRINTVEC4(*(vec4f*)inst->rot);
			PRINTVEC3(*(vec4f*)inst->scl);
			PRINTVEC3(*(vec4f*)inst->pivot);
			//printf("%i\n", inst->hide);
			instances.push_back(Instance(n.node->name, n.matrix * mat44::translate(-*(vec3f*)inst->pivot)));
			isInstanced.insert(n.node->name);
		}
	}
	
	std::map<std::string, Lib3dsMesh*> meshByName;
	
	//perhaps some meshes were not referenced in the node hierarchy. chuck them in too :)
	for (int i = 0; i < f->nmeshes; ++i)
	{
		if (isInstanced.find(f->meshes[i]->name) == isInstanced.end())
		{
			//printf("3ds mesh %s UNINSTANCED\n", f->meshes[i]->name);
			instances.push_back(Instance(f->meshes[i]->name, mat44::identity()));
		}
		assert(meshByName.find(f->meshes[i]->name) == meshByName.end()); //I'd assume the mesh name must be unique
		meshByName[f->meshes[i]->name] = f->meshes[i];
	}
	
	//count num verts/indices
	for (std::vector<Instance>::iterator it = instances.begin(); it != instances.end(); ++it)
	{
		std::map<std::string, Lib3dsMesh*>::iterator found;
		found = meshByName.find(it->first);
		if (found == meshByName.end())
		{
			printf("missing mesh %s\n", (*it).first.c_str());
			continue;
		}
		Lib3dsMesh* m = found->second;
		mesh.numVertices += m->nvertices;
		mesh.numIndices += m->nfaces * 3;
		hasTexCoords = hasTexCoords || (m->texcos > 0);
	}
	
	//extract materials
	//FIXME: currently, only one material per mesh is supported - the material of the first face
	for (int i = 0; i < f->nmaterials; ++i)
	{
		Lib3dsTextureMap& texture = f->materials[i]->texture1_map;
		Lib3dsTextureMap& normalmap = f->materials[i]->bump_map;
		Material* mat = new Material;
		//printf("%s -> %s\n", f->materials[i]->name, (path + "/" + texture.name).c_str());
		if (strlen(texture.name))
		{
			mat->imgColour = new ImagePNG((path + "/" + texture.name).c_str());
			mat->imgColour->anisotropy = 4.0f;
		}
		if (strlen(normalmap.name))
		{
			mat->imgNormal = new ImagePNG((path + "/" + normalmap.name).c_str());
			mat->imgNormal->anisotropy = 4.0f;
		}
		else
		{
			/*
			//assume normal map is basename + dd
			std::string bn = basefilename(texture.name);
			mat->imgNormal = new ImagePNG((path + "/" + bn + "dd.png").c_str());
			mat->imgNormal->anisotropy = 4.0f;
			if (mat->texNormal)
				printf("%s\n", (path + "/" + bn + "dd.png").c_str());
			*/
		}
		mesh.addMaterial(mat, f->materials[i]->name);
	}
	
	//sort meshes by material, to reduce facesets and hence bind/unbind calls
	typedef std::map<int, std::vector<int> > MaterialInstances;
	MaterialInstances materialInstances;
	for (int i = 0; i < (int)instances.size(); ++i)
	{
		std::map<std::string, Lib3dsMesh*>::iterator found;
		found = meshByName.find(instances[i].first);
		if (found == meshByName.end())
			continue;
		Lib3dsMesh* m = found->second;
		
		int material = -1;
		if (m->nfaces > 0)
			material = m->faces[0].material;
		
		//append mesh to material instance list. create if one doesn't exist for this material
		if (materialInstances.find(material) == materialInstances.end())
			materialInstances[material] = std::vector<int>();
		materialInstances[material].push_back(i);
	}
	
	mesh.sub[VERTICES] = new float[mesh.numVertices*3];
	if (hasTexCoords)
		mesh.sub[TEXCOORDS] = new float[mesh.numVertices*2];
	mesh.dataIndices = new unsigned int[mesh.numIndices];
	
	int lastMaterialIndex = 0;
	for (MaterialInstances::iterator it = materialInstances.begin(); it != materialInstances.end(); ++it)
	{
		for (int i = 0; i < (int)it->second.size(); ++i)
		{
			Instance& inst = instances[it->second[i]];
			std::map<std::string, Lib3dsMesh*>::iterator found;
			found = meshByName.find(inst.first);
			if (found == meshByName.end())
				continue;
			Lib3dsMesh* m = found->second;
			//printf("3ds mesh %s\n", m->name);
			assert(m->vertices);
		
			//swivel x/y/z
			mat44 mat = mat44::zero();
			mat.d[1][0] = 1.0;
			mat.d[0][2] = 1.0;
			mat.d[2][1] = 1.0;
			mat *= mat44(m->matrix);
			mat *= inst.second;
			mat *= mat44(m->matrix).inverse();
			//mat *= mat44::translate(m->matrix[3][0], m->matrix[3][1], m->matrix[3][2]);
			//PRINTMAT44(mat44(m->matrix));
		
			//printf("\tverts=%i\n", m->nvertices);
			for (int v = 0; v < m->nvertices; ++v)
			{
				*(vec3f*)&mesh.sub[VERTICES][(vertexOffset+v)*3] = mat * 
					vec4f(m->vertices[v][0], m->vertices[v][1], m->vertices[v][2], 1.0f);
				if (m->texcos)
				{
					mesh.sub[TEXCOORDS][(vertexOffset+v)*2+0] = m->texcos[v][0];
					mesh.sub[TEXCOORDS][(vertexOffset+v)*2+1] = m->texcos[v][1];
				}
				else if (hasTexCoords)
				{
					mesh.sub[TEXCOORDS][(vertexOffset+v)*2+0] = 0.0f;
					mesh.sub[TEXCOORDS][(vertexOffset+v)*2+1] = 0.0f;
				}
			}
		
			mesh.indexed = true;
			for (int n = 0; n < m->nfaces; ++n)
			{
				mesh.dataIndices[(indexOffset+n*3)+0] = vertexOffset + m->faces[n].index[0];
				mesh.dataIndices[(indexOffset+n*3)+1] = vertexOffset + m->faces[n].index[1];
				mesh.dataIndices[(indexOffset+n*3)+2] = vertexOffset + m->faces[n].index[2];
			}
			vertexOffset += m->nvertices;
			indexOffset += m->nfaces * 3;
		}
		
		//add a faceset for the material group
		if (it->first >= 0 && indexOffset > lastMaterialIndex)
		{
			mesh.useMaterial(lastMaterialIndex, indexOffset, f->materials[it->first]->name);
			//printf("%s: %i->%i\n", f->materials[it->first]->name, lastMaterialIndex, indexOffset);
			lastMaterialIndex = indexOffset;
		}
	}
	printf("Grouped to %i facesets\n", (int)materialInstances.size());

	lib3ds_file_free(f);

	mesh.generateNormals();
	mesh.numPolygons = mesh.numIndices / 3;
	
	#if 0
	//http://openctm.sourceforge.net/media/DevelopersManual.pdf
	3DScontext context;

	// Create a new importer context
	context = ctmNewContext(3DS_IMPORT);

	// Load the Open3DS file
	ctmLoad(context, filename);
	3DSenum err = ctmGetError(context);
	if(err != 3DS_NONE)
	{
		printf("Error opening %s\n", filename);
		ctmFreeContext(context);
		mesh.error = true;
		return false;
	}

	// Access the mesh data
	mesh.numVertices = ctmGetInteger(context, 3DS_VERTEX_COUNT);
	int norms = ctmGetInteger(context, 3DS_HAS_NORMALS);
	int texcs = ctmGetInteger(context, 3DS_UV_MAP_COUNT);
	int other = ctmGetInteger(context, 3DS_ATTRIB_MAP_COUNT);
	mesh.numPolygons = ctmGetInteger(context, 3DS_TRIANGLE_COUNT);
	mesh.numIndices = 3 * mesh.numPolygons;

	mesh.sub[VERTICES] = (float*)ctmGetFloatArray(context, 3DS_VERTICES);
	if (norms)
		mesh.sub[NORMALS] = (float*)ctmGetFloatArray(context, 3DS_NORMALS);
	if (texcs)
		mesh.sub[TEXCOORDS] = (float*)ctmGetFloatArray(context, 3DS_UV_MAP_1);
	if (other)
		mesh.sub[TANGENTS] = (float*)ctmGetFloatArray(context, 3DS_ATTRIB_MAP_1);
		
	delete[] mesh.dataIndices;
	mesh.dataIndices = new unsigned int[mesh.numIndices];
	mesh.indexed = true;

	memcpy(mesh.dataIndices, ctmGetIntegerArray(context, 3DS_INDICES), mesh.numIndices * sizeof(unsigned int));
	
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
#endif
	return true;

}
