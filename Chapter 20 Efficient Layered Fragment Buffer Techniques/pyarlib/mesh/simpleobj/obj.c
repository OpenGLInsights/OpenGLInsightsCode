
#include "obj.h"
#include "uthash.h"

#include <stdio.h>
#include <errno.h>

#ifdef _WIN32
#define strtok_r(s, d, t) strtok_s(s, d, t)
#define strtof(s, e) (float)strtod(s, e)
#pragma warning(disable: 4996)
#endif

//obj files should not have lines longer than this
#define OBJ_MAX_LINE_LEN 1024

//polygons will be capped at 8 vertices
#define OBJ_MAX_POLYGON 8

//a quick structure for dynamic reallocation
typedef struct _ReallocArray
{
	//the user edits these
	void* data; //should typecast to data type
	int size; //size is in blocks of blockSize bytes
	
	//these are for private use
	int reserved; //size in blockSize bytes
	int blockSize;
} ReallocArray;

//init the ReallocArray, zero size and one reserved data block
void initArray(ReallocArray* array, int blockSize)
{
	array->data = malloc(blockSize);
	array->size = 0;
	array->reserved = 1;
	array->blockSize = blockSize;
}

//after changing size, call this to make sure there is enough memory
void allocArray(ReallocArray* array)
{
	//no need to realloc
	if (array->reserved >= array->size)
		return;
	
	//printf("About to reallocate %i for %i\n", array->reserved, array->size);
	while (array->reserved < array->size)
		array->reserved <<= 1; //double size
	array->data = realloc(array->data, array->reserved * array->blockSize);
	//printf("\tReallocated %i\n", array->reserved);
}

//reallocates the array to the exact size (for final use after allocArray)
void exactAllocArray(ReallocArray* array)
{
	if (array->reserved == array->size)
		return;
	
	array->reserved = array->size;
	array->data = realloc(array->data, array->reserved * array->blockSize);
}

//free allocated array data
void freeArray(ReallocArray* array)
{
	free(array->data);
	array->data = NULL;
	array->size = 0;
	array->reserved = 0;
	array->blockSize = 0;
}

//stores vertex combination (position/texture/normal indices)
typedef struct _VertHashKey
{
	int v, t, n;
} VertHashKey;

//hash struct for finding unique vertex combinations
typedef struct _VertHash
{
	VertHashKey key;
	int index;
    UT_hash_handle hh;
} VertHash;

//hashes/maps material names to the materials index in the OBJMesh
typedef struct _MatHash
{
	char* name;
	int index;
    UT_hash_handle hh;
} MatHash;

//reads the next float from a tokenized string, with error handling
float readTokFloat(char** tmp, int* warn)
{
	char* e;
	char* t = strtok_r(NULL, " ", tmp);
	if (!t)
	{
		*warn = 1;
		return 0.0f;
	}
	float f = (float)strtof(t, &e);
	if (f == 0.0f && t == e)
		*warn = 1;
	return f;
}

//removes whitespace and newlines at end of string (replaces with \0)
void trimRight(char* str)
{
	int i = strlen(str);
	char* c;
	for (c = str + i - 1; c > str; --c)
		if (*c != ' ' && *c != '\r' && *c != '\n' && *c != '\t')
			break;
	++c;
	*c = '\0';
}

void setFaceSet(ReallocArray* facesets, int material, int smooth, int index)
{
	OBJFaceSet* edit = NULL;
	OBJFaceSet* last = NULL;
	if (facesets->size > 0)
		last = ((OBJFaceSet*)facesets->data) + (facesets->size-1);
	
	if (last && (last->material == material || material == -1) && last->smooth == smooth)
		return; //nothing to do
	
	if (last && last->indexStart == index) //no faces in set last set
	{
		if (material == -1 && smooth == -1) //unique combination to mark end of faceset
		{
			facesets->size -= 1; //end of unused faceset. remove
			return;
		}
		edit = last;
	}
	else
	{
		//update end index
		if (last)
			last->indexEnd = index;
			
		if (material == -1 && smooth == -1) //unique combination to mark end of faceset
			return;
	
		//append new faceset
		facesets->size += 1;
		allocArray(facesets);
		edit = ((OBJFaceSet*)facesets->data) + (facesets->size-1);
		edit->indexStart = index;
		edit->indexEnd = index; //unnecessary, but just in case
	}
	
	if (last && material == -1)
		edit->material = last->material; //use previous material (eg for changing smooth state)
	else
		edit->material = material; //simply update material. may be -1
	edit->smooth = smooth;
}

//a reference to the material (.mtl) filename may be in the .obj file
void parseMaterials(OBJMesh* mesh, const char* filename);

OBJMesh* objMeshLoad(const char* filename)
{
	int linenum = 0;
	int warning = 0;
	int fatalError = 0;
	
	//open the ascii file
	FILE* file = fopen(filename, "r");
	
	//check it actually opened
	if (!file)
	{
		perror(filename);
		return NULL;
	}
	
	//the mesh we're going to return
	OBJMesh* mesh = (OBJMesh*)malloc(sizeof(OBJMesh));
	memset(mesh, 0, sizeof(OBJMesh));
	
	//we don't know how much data the obj file has, so we start with one and keep realloc-ing double
	ReallocArray vertices, faceSets, vertexHash, positions, normals, texCoords, triangles;
	initArray(&vertexHash, sizeof(VertHash*)); //array of hash record pointers (for freeing)
	initArray(&positions, sizeof(float) * 3);
	initArray(&normals, sizeof(float) * 3);
	initArray(&texCoords, sizeof(float) * 2);
	initArray(&triangles, sizeof(unsigned int) * 3);
	initArray(&faceSets, sizeof(OBJFaceSet));
	vertices.size = 0; //vertices are allocated later (at first face)
	
	//obj indices start at 1. we'll use the zero element for "error", giving with zero data
	positions.size = 1;
	normals.size = 1;
	texCoords.size = 1;
	allocArray(&positions);
	allocArray(&normals);
	allocArray(&texCoords);
	memset(positions.data, 0, positions.blockSize);
	memset(normals.data, 0, normals.blockSize);
	memset(texCoords.data, 0, texCoords.blockSize);
	
	//vertex combinations v/t/n are not always the same index. different vertex
	//combinations are hashed and reused, saving memory
	//see uthash.h (http://uthash.sourceforge.net/)
	VertHash* vertexRecords = NULL;
	
	//materials are referenced by name. this hash maps a name to material index
	MatHash* materialRecords = NULL;
	
	//whether the mesh contains normals or texture coordinates, and hence
	//the vertex data stride, is decided at the first face line ("f ...")
	int reachedFirstFace = 0;
	
	//current shading state
	int smoothShaded = 1;
	
	//start reading, line by line
	char* tmpTok;
	char line[OBJ_MAX_LINE_LEN];
	while (fgets(line, OBJ_MAX_LINE_LEN, file) != NULL)
	{
		//split line by spaces
		strtok_r(line, " ", &tmpTok);
		
		//what data does this line give us, if any?
		if (line[0] == 'v')
		{
			//this line contains vertex data
			if (line[1] == '\0') //\0 as strtok replaced the space
			{
				//position data. allocate more memory if needed
				positions.size++;
				allocArray(&positions);
				
				//read data from string
				((float*)positions.data)[(positions.size-1)*3+0] = readTokFloat(&tmpTok, &warning);
				((float*)positions.data)[(positions.size-1)*3+1] = readTokFloat(&tmpTok, &warning);
				((float*)positions.data)[(positions.size-1)*3+2] = readTokFloat(&tmpTok, &warning);
			}
			else if (line[1] == 'n')
			{
				//normal data. allocate more memory if needed
				normals.size++;
				allocArray(&normals);
				
				//read data from string
				((float*)normals.data)[(normals.size-1)*3+0] = readTokFloat(&tmpTok, &warning);
				((float*)normals.data)[(normals.size-1)*3+1] = readTokFloat(&tmpTok, &warning);
				((float*)normals.data)[(normals.size-1)*3+2] = readTokFloat(&tmpTok, &warning);
			}
			else if (line[1] == 't')
			{
				//texture data. allocate more memory if needed
				texCoords.size++;
				allocArray(&texCoords);
				
				//read data from string
				((float*)texCoords.data)[(texCoords.size-1)*2+0] = readTokFloat(&tmpTok, &warning);
				((float*)texCoords.data)[(texCoords.size-1)*2+1] = readTokFloat(&tmpTok, &warning);
			}
		}
		else if (line[0] == 'f')
		{
			//this line contains face data. this may have many vertices
			//but we just want triangles. so we triangulate.
			//NOTE: ALL vertex data must be given before being referenced by a face
			//NOTE: At least one vt or vn must be specified before the first f, or the rest will be ignored
			if (!reachedFirstFace)
			{
				//must have previously specified vertex positions
				if (positions.size == 1)
				{
					fatalError = 1;
					break;
				}
				
				//calculate vertex stride
				mesh->hasNormals = (normals.size > 1) ? 1 : 0;
				mesh->hasTexCoords = (texCoords.size > 1) ? 1 : 0;
				mesh->normalOffset = 3 * sizeof(float);
				mesh->texcoordOffset = mesh->normalOffset + mesh->hasNormals * 3 * sizeof(float);
				mesh->stride = mesh->texcoordOffset + mesh->hasTexCoords * 2 * sizeof(float);
				initArray(&vertices, mesh->stride);
				reachedFirstFace = 1;
			}
			
			//start by extracting groups of vertex data (pos/tex/norm)
			int i = 0;
			char* vert[OBJ_MAX_POLYGON];
			while ((vert[i] = strtok_r(NULL, " ", &tmpTok)) != NULL && i < OBJ_MAX_POLYGON)
				++i;
			
			//triangulate using the "fan" method
			int triVert = 0; //triVert contains the current face's vertex index. may not equal v as vertices can be ignored. 
			int triangulate[2];
			for (int v = 0; v < i; ++v)
			{
				//split groups by "/" and store in inds[3]
				int t = 0;
				int inds[3];
				char* tok = strtok_r(vert[v], "/", &tmpTok);
				while (tok != NULL && t < 3)
				{
					inds[t++] = atoi(tok);
					tok = strtok_r(NULL, "/", &tmpTok);
				}
				while (t < 3)
					inds[t++] = 0;
				
				//set provided, yet unused indices as invalid. this
				//prevents unnecessary unique vertices being created
				if (!mesh->hasTexCoords)
					inds[1] = -1;
				if (!mesh->hasNormals)
					inds[2] = -1;
					
				//ignore vertex if position indices are out of bounds
				if (inds[0] < 0 || inds[0] >= positions.size)
				{
					warning = 1;
					continue;
				}
				
				//use zero for out of bound normals and texture coordinates
				if ((mesh->hasTexCoords && (inds[1] < 0 || inds[1] >= texCoords.size)) ||
					(mesh->hasNormals && (inds[2] < 0 || inds[2] >= normals.size)))
				{
					warning = 1;
				}
					
				//since vertices will be reused a lot, we need to hash the v/t/n combination
				int uniqueVertIndex;
				
				//check if the vertex already exists in hash
				VertHash h;
				VertHash* found = NULL;
				memset(&h, 0, sizeof(VertHash));
				h.key.v = inds[0];
				h.key.t = inds[1];
				h.key.n = inds[2];
				//printf("looking up %i/%i/%i\n", h.key.v, h.key.t, h.key.n);
				HASH_FIND(hh, vertexRecords, &h.key, sizeof(VertHashKey), found);
				//printf("done looking up\n");
				if (found)
				{
					//found. use that vertex
					//printf("vert %i/%i/%i already exists as %i\n", inds[0], inds[1], inds[2], found->index);
					uniqueVertIndex = found->index;
				}
				else
				{
					//printf("new vert %i/%i/%i\n", inds[0], inds[1], inds[2]);
				
					//not found. create a new vertex
					uniqueVertIndex = vertices.size++;
					allocArray(&vertices);
					
					//copy data for vertex
					memcpy(((float*)vertices.data) + uniqueVertIndex * mesh->stride / sizeof(float), 
						((float*)positions.data) + inds[0] * 3,
						sizeof(float) * 3);
					if (mesh->hasTexCoords)
						memcpy(((float*)vertices.data) + (uniqueVertIndex * mesh->stride + mesh->texcoordOffset) / sizeof(float),
						((float*)texCoords.data) + inds[1] * 2,
						sizeof(float) * 2);
					if (mesh->hasNormals)
						memcpy(((float*)vertices.data) + (uniqueVertIndex * mesh->stride + mesh->normalOffset) / sizeof(float),
						((float*)normals.data) + inds[2] * 3,
						sizeof(float) * 3);
					
					//add vertex to hash table
					vertexHash.size++;
					allocArray(&vertexHash);
					VertHash* newRecord = (VertHash*)malloc(sizeof(VertHash));
					((VertHash**)vertexHash.data)[vertexHash.size-1] = newRecord; //store pointer to quickly free hash records later
					
					h.index = uniqueVertIndex;
					*newRecord = h;
					
					HASH_ADD(hh, vertexRecords, key, sizeof(VertHashKey), newRecord);
				}
				
				if (triVert == 0)
				{
					//store the first vertex
					triangulate[0] = uniqueVertIndex;
				}
				else if (triVert > 1)
				{
					//printf("tri %i->%i->%i\n", triangulate[0], triangulate[1], uniqueVertIndex);
					
					//this is at least the 3rd vertex - we have a new triangle to add
					//always create triangles between the current, previous and first vertex
					triangles.size++;
					allocArray(&triangles);
				
					//read data from string
					((unsigned int*)triangles.data)[(triangles.size-1)*3+0] = triangulate[0];
					((unsigned int*)triangles.data)[(triangles.size-1)*3+1] = triangulate[1];
					((unsigned int*)triangles.data)[(triangles.size-1)*3+2] = uniqueVertIndex;
					
				}
				//store the last vertex
				triangulate[1] = uniqueVertIndex;
				++triVert;
			}
		}
		else if (strcmp(line, "usemtl") == 0)
		{
			//the material state has changed - create a new faceset
			char* mtlname = strtok_r(NULL, " ", &tmpTok);
			trimRight(mtlname);
			
			//find the corresponding material
			MatHash* matRecord;
			HASH_FIND_STR(materialRecords, mtlname, matRecord);
			if (matRecord)
			{
				//printf("material: '%s'\n", mtlname);
				setFaceSet(&faceSets, matRecord->index, smoothShaded, triangles.size * 3);
			}
			else
			{
				warning = 1;
				printf("Undefined material: '%s'\n", mtlname);
				setFaceSet(&faceSets, -1, -1, triangles.size * 3);
			}
		}
		else if (strcmp(line, "mtllib") == 0)
		{
			//load all materials from the given .mtl file
			if (mesh->numMaterials > 0)
			{
				fatalError = 1; //shouldn't specify multiple .mtl files
				break;
			}
			char* mtlname = strtok_r(NULL, " ", &tmpTok);
			trimRight(mtlname);
			if (mtlname)
				parseMaterials(mesh, mtlname);
				
			//add all materials to the hash
			MatHash* matRecord;
			for (int m = 0; m < mesh->numMaterials; ++m)
			{
				HASH_FIND_STR(materialRecords, mesh->materials[m].name, matRecord);
				if (matRecord)
				{
					warning = 1;
					printf("Multiple materials with name '%s'\n", mesh->materials[m].name);
					continue; //can't have multiple definitions of the same material
				}
				matRecord = (MatHash*)malloc(sizeof(MatHash));
				matRecord->name = mesh->materials[m].name;
				matRecord->index = m;
				HASH_ADD_KEYPTR(hh, materialRecords, matRecord->name, strlen(matRecord->name), matRecord);
			}
		}
		else if (line[0] == 's')
		{
			#if !IGNORE_SMOOTHING
			//smooth shading has been changed - create a new faceset
			char* smooth = strtok_r(NULL, " ", &tmpTok);
			trimRight(smooth);
			smoothShaded = atoi(smooth);
			if (strcmp(smooth, "on") == 0) smoothShaded = 1;
			if (strcmp(smooth, "off") == 0) smoothShaded = 0;
			setFaceSet(&faceSets, -1, smoothShaded, triangles.size * 3);
			#endif
		}
		//options o and g are ignored
		
		linenum++; //for warnings/errors
	}
	setFaceSet(&faceSets, -1, -1, triangles.size * 3); //update end of final faceset
	
	//fill the rest of the mesh structure
	exactAllocArray(&vertices);
	exactAllocArray(&triangles);
	exactAllocArray(&faceSets);
	mesh->vertices = (float*)vertices.data;
	mesh->indices = (unsigned int*)triangles.data;
	mesh->facesets = (OBJFaceSet*)faceSets.data;
	mesh->numVertices = vertices.size;
	mesh->numIndices = triangles.size * 3;
	mesh->numFacesets = faceSets.size;
	
	//cleanup
	//NOTE: vertices and indices are used in the returned mesh data and are not freed
	
	MatHash* matRecord;
	MatHash* matTmp;
	HASH_ITER(hh, materialRecords, matRecord, matTmp) {
	  HASH_DEL(materialRecords, matRecord);
	  free(matRecord);
	}
	
	HASH_CLEAR(hh, vertexRecords);
	for (int i = 0; i < vertexHash.size; ++i)
		free(((VertHash**)vertexHash.data)[i]);
	freeArray(&vertexHash);
	
	freeArray(&positions);
	freeArray(&normals);
	freeArray(&texCoords);
	
	fclose(file);
	
	if (fatalError != 0)
	{
		printf("Error: could not load mesh %s (line %i)\n", filename, linenum);
		objMeshFree(&mesh);
		return NULL;
	}
	if (warning != 0)
		printf("Warning: mesh %s contains errors\n", filename);
	
	return mesh;
}

void replaceString(char** dest, const char* src)
{
	if (*dest)
		free(*dest);
	*dest = (char*)malloc(strlen(src) + 1);
	strcpy(*dest, src);
}

void parseMaterials(OBJMesh* mesh, const char* filename)
{
	int warning = 0;
	int notImportantWarning = 0;

	//open/check etc
	FILE* file = fopen(filename, "r");
	if (!file)
	{
		perror(filename);
		return;
	}
	
	ReallocArray materials;
	initArray(&materials, sizeof(OBJMaterial));
	OBJMaterial* mat = NULL;
	
	//read line by line
	char* tmpTok;
	char line[OBJ_MAX_LINE_LEN];
	while (fgets(line, OBJ_MAX_LINE_LEN, file) != NULL)
	{
		//split line by spaces
		strtok_r(line, " ", &tmpTok);
		
		//extract data
		if (mat && line[0] == 'K') //colours (rgb)
		{
			float col[4];
			col[0] = readTokFloat(&tmpTok, &warning);
			col[1] = readTokFloat(&tmpTok, &warning);
			col[2] = readTokFloat(&tmpTok, &warning);
			col[3] = readTokFloat(&tmpTok, &notImportantWarning);
			if (line[1] == 'a')	memcpy(mat->ambient, col, sizeof(col));
			if (line[1] == 'd')	memcpy(mat->diffuse, col, sizeof(col));
			if (line[1] == 's')	memcpy(mat->specular, col, sizeof(col));
		}
		else if (mat && (line[0] == 'd' || strcmp(line, "Tr") == 0)) //transparency
			mat->transparency = readTokFloat(&tmpTok, &warning);
		else if (mat && strcmp(line, "Ns") == 0) //specular shininess
			mat->shininess = readTokFloat(&tmpTok, &warning);
		else if (mat && strcmp(line, "illum") == 0) //rendering mode (not very useful)
		{
			char* mode = strtok_r(NULL, " ", &tmpTok);
			if (mode)
				mat->mode = atoi(mode);
		}
		else if (mat && (strcmp(line, "map_Kd") == 0 || strcmp(line, "map_Ka") == 0)) //texture map
		{
			if (line[5] == 'a' && mat->texture)
				continue; //can't replace a Kd with a Ka
			char* texname = strtok_r(NULL, " ", &tmpTok);
			trimRight(texname);
			replaceString(&mat->texture, texname);
		}
		else if (mat && strcmp(line, "map_Ks") == 0) //specular map
		{
			char* texname = strtok_r(NULL, " ", &tmpTok);
			trimRight(texname);
			replaceString(&mat->texSpecular, texname);
		}
		else if (mat && (strcmp(line, "map_bump") == 0 || strcmp(line, "bump") == 0)) //normal map
		{
			char* texname = strtok_r(NULL, " ", &tmpTok);
			trimRight(texname);
			replaceString(&mat->texNormal, texname);
		}
		else if (strcmp(line, "newmtl") == 0) //new material - create "mat"
		{
			char* matname = strtok_r(NULL, " ", &tmpTok);
			if (!matname)
			{
				warning = 1;
				continue; //can't add with no name
			}
			
			materials.size++;
			allocArray(&materials);
			mat = &((OBJMaterial*)materials.data)[materials.size-1];
			memset(mat, 0, sizeof(OBJMaterial));
			mat->shininess = 50.0f; //default
			
			trimRight(matname);
			replaceString(&mat->name, matname);
		}
	}
	
	//update the mesh's material array with the new one
	exactAllocArray(&materials);
	if (mesh->materials)
		free(mesh->materials);
	mesh->materials = (OBJMaterial*)materials.data;
	mesh->numMaterials = materials.size;
}

void objMeshFree(OBJMesh** mesh)
{
	free((*mesh)->vertices);
	free((*mesh)->indices);
	free((*mesh)->materials);
	free((*mesh)->facesets);
	free(*mesh);
	*mesh = NULL;
}
