/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <map>

#include "includegl.h"

#include "shaderutil.h"
#include "vbomesh.h"
#include "util.h"

using namespace std;

const int VBOMesh::size[dataTypes] = {
	3,
	3,
	2,
	3
};

void VBOMesh::paramPlane(const vec2f& point, vec3f& vertex, vec3f& normal, vec2f& texcoord, vec3f& tangent)
{
	vertex(point.x, point.y, 0.0f);
	normal(0.0f, 0.0f, 1.0f);
	texcoord = point;
	tangent(1.0f, 0.0f, 0.0f);
}

void VBOMesh::paramSphere(const vec2f& point, vec3f& vertex, vec3f& normal, vec2f& texcoord, vec3f& tangent)
{
	static const float scaleu = 2.0f * pi;
	static const float scalev = pi;
	float u = scaleu * point.x;
	float v = scalev * point.y;
	vertex.x = cos(u) * sin(v);
	vertex.y = sin(u) * sin(v);
	vertex.z = cos(v);
	normal = vertex;
	texcoord = point;
	tangent.x = -sin(u) * sin(v);
	tangent.y = cos(u) * sin(v);
	tangent.z = cos(v);
}

VBOMesh::VBOMesh()
{
	boundsMin = average = center = boundsMax = vec3f(0.0f);
	interleaved = false;
	indexed = false;
	buffered = false;
	error = false;
	stride = 0;
	strideFloats = 0;
	primitives = GL_TRIANGLES;
	numVertices = 0;
	numIndices = 0;
	for (int a = 0; a < dataTypes; ++a)
	{
		has[a] = false;
		offset[a] = 0;
		sub[a] = NULL;
	}
	data = NULL;
	dataIndices = NULL;
	vloc = nloc = txloc = tgloc = -1;
}
void VBOMesh::draw(bool autoAttribLocs)
{
	if (error)
		return;

	if (!interleaved)
		interleave();

	//assert(has[NORMALS]);
	
	GLint program = 0;
	if (autoAttribLocs)
	{
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);
		if (program > 0)
		{
			vloc = glGetAttribLocation(program, "osVert");
			nloc = glGetAttribLocation(program, "osNorm");
			tgloc = glGetAttribLocation(program, "osTangent");
			txloc = glGetAttribLocation(program, "texCoord");
		}
		else
		{
			vloc = nloc = tgloc = txloc = (GLuint)-1;
		}
	}
	
	bool locVerts = (vloc != (GLuint)-1);
	bool locNormals = (nloc != (GLuint)-1);
	bool locTextureCoords = (txloc != (GLuint)-1);
	bool locTangents = (tgloc != (GLuint)-1);

	#define VBO_PTR(p, off) ((GLvoid*)((intptr_t)(p) + (intptr_t)(off) * sizeof(float)))

	if (buffered)
		vertices.bind();
	else
		glBindBuffer(GL_ARRAY_BUFFER, 0); //just in case: other code *should* keep GL_ARRAY_BUFFER unbound anyway

	GLvoid* arrayPointers[4] = {
		VBO_PTR(buffered?0:data, offset[0]),
		VBO_PTR(buffered?0:data, offset[1]),
		VBO_PTR(buffered?0:data, offset[2]),
		VBO_PTR(buffered?0:data, offset[3])
	};
	
	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

	if (locVerts)
	{
		glEnableVertexAttribArray(vloc);
		glVertexAttribPointer(vloc, 3, GL_FLOAT, GL_FALSE, stride, arrayPointers[0]);
	}
	else
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, stride, arrayPointers[0]);
	}

	if (has[NORMALS])
	{
		if (locVerts && locNormals)
		{
			glEnableVertexAttribArray(nloc);
			glVertexAttribPointer(nloc, 3, GL_FLOAT, GL_FALSE, stride, arrayPointers[1]);
		}
		else
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, stride, arrayPointers[1]);
		}
	}

	if (has[TEXCOORDS])
	{
		if (locVerts && locTextureCoords)
		{
			glEnableVertexAttribArray(txloc);
			glVertexAttribPointer(txloc, 2, GL_FLOAT, GL_FALSE, stride, arrayPointers[2]);
		}
		else
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, stride, arrayPointers[2]);
		}
	}

	if (has[TANGENTS])
	{
		if (locVerts && locTangents)
		{
			glEnableVertexAttribArray(tgloc);
			glVertexAttribPointer(tgloc, 3, GL_FLOAT, GL_FALSE, stride, arrayPointers[3]);
		}
	}

	if (indexed)
	{
		if (buffered) indices.bind();
		
		if (program)
		{
			GLuint texturedLoc = glGetUniformLocation(program, "textured");
			if (texturedLoc)
				glUniform1i(texturedLoc, 0);
		}
		
		//iterate through facesets. all elements will be drawn
		int lastIndex = 0;
		for (Facesets::iterator it = facesets.begin(); it != facesets.end(); ++it)
		{
			const VBOMeshFaceset& f = it->second;
			VBOMeshMaterial* m = materials[f.material];
			
			//draw "unbound" material elements (at the start or between material ranges)
			if (lastIndex < f.startIndex)
				glDrawElements(primitives, f.startIndex - lastIndex, GL_UNSIGNED_INT, (unsigned int*)(buffered?0:dataIndices) + lastIndex);
			
			//bind material and draw "bound" material range
			m->bind();
			glDrawElements(primitives, f.endIndex - f.startIndex, GL_UNSIGNED_INT, (unsigned int*)(buffered?0:dataIndices) + f.startIndex);
			m->unbind();
			
			lastIndex = f.endIndex;
		}
		
		//finish off the range, if there is any left. this will be the only draw call if there are no facesets
		if (lastIndex < numIndices)
			glDrawElements(primitives, numIndices - lastIndex, GL_UNSIGNED_INT, (unsigned int*)(buffered?0:dataIndices) + lastIndex);
		
		if (buffered) indices.unbind();
	}
	else
	{
		glDrawArrays(primitives, 0, numVertices);
	}
	
	if (buffered)
		vertices.unbind();

	glPopClientAttrib();
}
void VBOMesh::useMaterial(int start, int end, std::string name)
{
	assert(start >= 0 && start < numIndices);
	assert(end >= 1 && end <= numIndices);
	VBOMeshFaceset f;
	f.startIndex = start;
	f.endIndex = end;
	f.material = materialNames[name];
	facesets[f.startIndex] = f;
}
void VBOMesh::addMaterial(VBOMeshMaterial* material, std::string name)
{
	materialNames[name] = (int)materials.size();
	materials.push_back(material);
}
void VBOMesh::bind(GLuint vloc, GLuint nloc, GLuint txloc, GLuint tgloc)
{
	this->vloc = vloc;
	this->nloc = nloc;
	this->txloc = txloc;
	this->tgloc = tgloc;
}
void VBOMesh::upload(bool freeLocal)
{
	if (error)
		return;

	if (!interleaved)
		interleave();

	if (!data || stride == 0)
	{
		printf("Error: uploading empty mesh\n");
		error = true;
		return;
	}

	vertices.buffer(data, numVertices * stride);
	if (indexed)
		indices.buffer(dataIndices, numIndices * sizeof(unsigned int));

	if (primitives == GL_TRIANGLES)
		numPolygons = numIndices / 3;

	buffered = true;
	if (freeLocal)
	{
		delete[] data;
		delete[] dataIndices;
		data = NULL;
		dataIndices = NULL;
	}
}
void VBOMesh::allocate()
{
	if (data)
		delete[] data;

	calcInternal();

	if (stride == 0)
		printf("Error: stride == 0\n");

	data = new float[numVertices * strideFloats];
}
void VBOMesh::calcInternal()
{
	//update offsets
	offset[VERTICES] = 0;
	for (int a = 1; a < dataTypes; ++a)
	{
		offset[a] = offset[a-1] + has[a-1] * size[a-1];
	}
	
	//update stride
	strideFloats = 
		has[VERTICES] * size[VERTICES] +
		has[NORMALS] * size[NORMALS] +
		has[TEXCOORDS] * size[TEXCOORDS] +
		has[TANGENTS] * size[TANGENTS];
	stride = strideFloats * sizeof(float);
}
void VBOMesh::interleave(bool freeSource)
{
	for (int a = 0; a < dataTypes; ++a)
	{
		has[a] = (sub[a] != NULL);
	}

	if (!has[VERTICES])
	{
		error = true;
		printf("Error: interleaving empty mesh\n");
		return;
	}
	
	allocate();
	//allocate calls calcInternal

	for (int a = 0; a < dataTypes; ++a)
		if (has[a])
			for (int v = 0; v < numVertices; ++v)
				memcpy(data + v * strideFloats + offset[a], sub[a] + v * size[a], size[a] * sizeof(float));
				//is memcpy the best way to do this?
	
	interleaved = true;

	if (freeSource)
	{
		for (int a = 1; a < dataTypes; ++a)
		{
			delete[] sub[a];
			sub[a] = NULL;
		}
	}
}
bool VBOMesh::computeInfo()
{
	if (error || numVertices == 0)
	{
		printf("Can't compute info on bad mesh\n");
		return false;
	}
	
	InterleavedEditor<vec3f> verts;
	if (sub[VERTICES])
	{
		verts.data = sub[VERTICES];
	}
	else if (interleaved && data)
	{
		verts.data = data;
		verts.offset = offset[VERTICES] * sizeof(float);
		verts.stride = stride;
	}
	else
	{
		printf("Can't compute info on mesh - no local data\n");
		return false;
	}
	
	boundsMin = verts[0];
	boundsMax = verts[0];
	average = vec3f(0.0f);
	for (int v = 0; v < numVertices; ++v)
	{
		const vec3f vert = verts[v];
		boundsMin.x = mymin(boundsMin.x, vert.x);
		boundsMin.y = mymin(boundsMin.y, vert.y);
		boundsMin.z = mymin(boundsMin.z, vert.z);
		boundsMax.x = mymax(boundsMax.x, vert.x);
		boundsMax.y = mymax(boundsMax.y, vert.y);
		boundsMax.z = mymax(boundsMax.z, vert.z);
		average += vert;
	}
	average /= (float)numVertices;
	center = (boundsMin + boundsMax) * 0.5f;
	return true;
}
void VBOMesh::transform(const mat44& m)
{
	if (error)
		return;
	mat44 nmat = m.inverse().transpose();

	InterleavedEditor<vec3f> verts;
	InterleavedEditor<vec3f> norms;
	if (interleaved)
	{
		verts.data = data;
		verts.offset = offset[VERTICES] * sizeof(float);
		verts.stride = stride;
		norms.data = data;
		norms.offset = offset[NORMALS] * sizeof(float);
		norms.stride = stride;
	}
	else if (sub[VERTICES])
	{
		verts.data = sub[VERTICES];
		norms.data = sub[NORMALS];
	}
	else
		return;
	
	for (int v = 0; v < numVertices; ++v)
	{
		verts[v] = vec3f(m * vec4f(verts[v], 1.0f));
		if (has[NORMALS])
			norms[v] = (nmat * norms[v]).unit();
	}
}
bool VBOMesh::validate()
{
	if (!dataIndices)
		return false;
	for (int i = 0; i < numIndices; ++i)
		if (dataIndices[i] >= (unsigned int)numVertices)
			return false;
	return true;
}

void VBOMesh::generateNormals()
{
	if (!sub[VERTICES] || !dataIndices)
	{
		printf("Cannot generate normals - incomplete data\n");
		return;
	}
	
	if (!sub[NORMALS])
	{
		sub[NORMALS] = new float[numVertices * 3];
		has[NORMALS] = true;
	}

	vec3f* verts = (vec3f*)sub[VERTICES];
	vec3f* norms = (vec3f*)sub[NORMALS];

	for (int v = 0; v < numVertices; ++v)
		norms[v] = vec3f(0.0f);
	for (int t = 0; t < numIndices / 3; ++t)
	{
		unsigned int t1 = dataIndices[t*3+0];
		unsigned int t2 = dataIndices[t*3+1];
		unsigned int t3 = dataIndices[t*3+2];
		const vec3f& a = verts[t1];
		const vec3f& b = verts[t2];
		const vec3f& c = verts[t3];
		vec3f u = b - a;
		vec3f v = c - a;
		vec3f n = u.cross(v);
		//if (n.size() > 0.0) n.normalize(); //removes weighting based on triangle area
		norms[t1] += n;
		norms[t2] += n;
		norms[t3] += n;
	}
	for (int v = 0; v < numVertices; ++v)
		norms[v].normalize();
}

void VBOMesh::averageVertices()
{
	if (!sub[VERTICES] || !dataIndices)
		return;
	
	vec3f* verts = (vec3f*)sub[VERTICES];
	
	vec4f* apos = new vec4f[numVertices];

	for (int v = 0; v < numVertices; ++v)
		apos[v] = vec4f(0.0f);
	for (int t = 0; t < numIndices / 3; ++t)
	{
		unsigned int t1 = dataIndices[t*3+0];
		unsigned int t2 = dataIndices[t*3+1];
		unsigned int t3 = dataIndices[t*3+2];
		#if 0 //smooth to adjacent triangle centers
		vec3f center = (verts[t1] + verts[t2] + verts[t3]) / 3.0f;
		apos[t1] += vec4f(center, 1.0);
		apos[t2] += vec4f(center, 1.0);
		apos[t3] += vec4f(center, 1.0);
		#else //smoth to adjacent triangle opposite edges
		apos[t1] += vec4f((verts[t2] + verts[t3]) * 0.5f, 1.0);
		apos[t2] += vec4f((verts[t1] + verts[t3]) * 0.5f, 1.0);
		apos[t3] += vec4f((verts[t1] + verts[t2]) * 0.5f, 1.0);
		#endif
	}
	for (int v = 0; v < numVertices; ++v)
	{
		//vec3f averageCenter = vec3f(apos[v]) / apos[v].w;
		//if (apos[v].w >= 3.0f)
		//	verts[v] = averageCenter;
		if (apos[v].w >= 3.0f)
			verts[v] = apos[v] / apos[v].w;
	}
	
	delete[] apos;
}

void VBOMesh::generateTangents()
{
	if (!sub[VERTICES] || !sub[NORMALS] || !sub[TEXCOORDS])
	{
		printf("Cannot generate tangents: v(%s) n(%s) t(%s)\n",
			sub[VERTICES]?"yes":"no",
			sub[NORMALS]?"yes":"no",
			sub[TEXCOORDS]?"yes":"no");
		return;
	}
	if (!sub[TANGENTS])
	{
		sub[TANGENTS] = new float[numVertices * 3];
		has[TANGENTS] = true;
	}

	vec3f* verts = (vec3f*)sub[VERTICES];
	vec3f* norms = (vec3f*)sub[NORMALS];
	vec2f* texcs = (vec2f*)sub[TEXCOORDS];
	vec3f* tangs = (vec3f*)sub[TANGENTS];

	for (int tIndex = 0; tIndex < numIndices / 3; ++tIndex)
	{
		vec3i tri = ((vec3i*)dataIndices)[tIndex];
		vec3f u = verts[tri.y] - verts[tri.x];
		vec3f v = verts[tri.z] - verts[tri.x];
		vec2f s = texcs[tri.y] - texcs[tri.x];
		vec2f t = texcs[tri.z] - texcs[tri.x];
		float det = 1.0f / (s.x*t.y - t.x*s.y);
		vec3f B = (v*s.x - u*t.x) * det;
		//vec3f T = (va*cb.y - vb*ca.y) * det;
		tangs[tri.x] += norms[tri.x].cross(B);
		tangs[tri.y] += norms[tri.y].cross(B);
		tangs[tri.z] += norms[tri.z].cross(B);
	}
	for (int v = 0; v < numVertices; ++v)
	{
		float len = tangs[v].size();
		if (len > 0.01)
			tangs[v] /= len;
		else
			tangs[v] = vec3f(1, 0, 0);
	}
}
bool VBOMesh::release()
{
	facesets.clear();
	materialNames.clear();
	for (int i = 0; i < (int)materials.size(); ++i)
		delete materials[i]; //FIXME: this assumes the material was created with new[]
	materials.clear();

	//FIXME: most of this is in the constructor. should really add an init() function
	for (int a = 0; a < dataTypes; ++a)
		delete[] sub[a];
	delete[] data;
	delete[] dataIndices;
	data = NULL;
	dataIndices = NULL;
	buffered = false;
	vertices.release();
	indices.release();
	
	boundsMin = average = center = boundsMax = vec3f(0.0f);
	interleaved = false;
	indexed = false;
	error = false;
	stride = 0;
	strideFloats = 0;
	primitives = GL_TRIANGLES;
	numVertices = 0;
	numIndices = 0;
	for (int a = 0; a < dataTypes; ++a)
	{
		has[a] = false;
		offset[a] = 0;
		sub[a] = NULL;
	}
	vloc = nloc = txloc = tgloc = -1;
	return true;
}
int VBOMesh::getStride()
{
	return stride;
}

/*
void VBOMesh::realloc(bool verts, bool norms, bool texcs, bool tangents)
{
	//sizes for each vertex attribute
	static const int size[4] = {3 * sizeof(float), 3 * sizeof(float), 2 * sizeof(float), 3 * sizeof(float)};

	//copy offsets
	int copy[5];
	copy[0] = 0;
	copy[1] = copy[0] + (has[VERTICES] ? 3 : 0);
	copy[2] = copy[1] + (has[NORMALS] ? 3 : 0);
	copy[3] = copy[2] + (has[TEXCOORDS] ? 2 : 0);
	copy[4] = copy[3] + (has[TANGENTS] ? 3 : 0);

	int oldVertSize = copy[4];

	//paste offsets
	int paste[5];
	paste[0] = 0;
	paste[1] = paste[0] + (verts ? 3 : 0);
	paste[2] = paste[1] + (norms ? 3 : 0);
	paste[3] = paste[2] + (texcs ? 2 : 0);
	paste[4] = paste[3] + (tangents ? 3 : 0);

	int newVertSize = paste[4];

	if (newVertSize == 0)
	{
		printf("Error: Cannot realloc with 0 vertex attributes\n");
		return;
	}

	//doeas each vertex attrib need copying?
	bool same = true;
	bool check[4];
	for (int a = 0; a < 4; ++a)
	{
		check[a] = ((copy[a+1]-copy[a] > 0) && (paste[a+1]-paste[a] > 0));
		if (check[a])
			same = false;
	}

	//new vertex data
	float* newData = new float[numVertices * newVertSize];

	if (oldVertSize > 0 && !same)
	{
		for (int i = 0; i < numVertices; ++i)
		{
			for (int a = 0; a < 4; ++a)
			{
				if (check[a])
					memcpy(newData+i*newVertSize+paste[a], data+i*oldVertSize+copy[a], size[a]);
			}
		}
	}

	delete[] data;
	data = newData;

	has[VERTICES] = verts;
	has[NORMALS] = norms;
	has[TEXCOORDS] = texcs;
	has[TANGENTS] = tangents;

	vertexSize = newVertSize;
}

bool VBOMesh::loadRaw(const char* file, bool verts, bool norms, bool texcs, bool tangents)
{
	//calculate entire vertex struct sizes
	int fileVertSize = (verts?3:0) + (norms?3:0) + (texcs?2:0) + (tangents?3:0);

	bool readIndices = (fileVertSize == 0);

	static const int size[4] = {3 * sizeof(float), 3 * sizeof(float), 2 * sizeof(float), 3 * sizeof(float)};

	//offsets for new data
	int paste[5];
	paste[0] = 0;
	paste[1] = paste[0] + ((has[VERTICES]||verts) ? 3 : 0);
	paste[2] = paste[1] + ((has[NORMALS]||norms) ? 3 : 0);
	paste[3] = paste[2] + ((has[TEXCOORDS]||texcs) ? 2 : 0);
	paste[4] = paste[3] + ((has[TANGENTS]||tangents) ? 3 : 0);

	int newVertSize = paste[4];

	for (int a = 1; a < 5; ++a)
		if (paste[a] == paste[a-1])
			paste[a-1] = -1;

	//which attributes need reading
	bool check[4] = {verts, norms, texcs, tangents};

	//open input file
	ifstream ifile(file, ios::binary);
	if (!ifile.is_open())
	{
		error = true;
		printf("Error: could not open %s\n", file);
		return false;
	}

	//find and check file size
	ifile.seekg(0, ios::end);
	int fileSize = ifile.tellg();
	ifile.seekg(0, ios::beg);

	if (readIndices)
	{
		int possibleIndices = fileSize / sizeof(unsigned int);
		if (possibleIndices == 0 || possibleIndices % 3 != 0)
		{
			error = true;
			printf("Error: File %s has %i indices. Not divisible by 3.\n", file, possibleIndices);
			return false;
		}
		numIndices = possibleIndices;
		delete[] dataIndices;
		dataIndices = new unsigned int[numIndices];
		ifile.read((char*)dataIndices, numIndices * sizeof(unsigned int));
		indexed = true;
		numPolygons = numIndices / 3;
	}
	else
	{
		int possibleVerts = (fileSize / sizeof(float)) / fileVertSize;
		if (numVertices > 0 && numVertices != possibleVerts)
		{
			error = true;
			printf("Error: File %s has %i vertices. Expecting %i.\n", file, possibleVerts, numVertices);
			return false;
		}
		numVertices = possibleVerts;
		if (!indexed)
			numPolygons = numVertices / 3;

		//allocate new data if needed
		realloc(has[VERTICES]||verts, has[NORMALS]||norms, has[TEXCOORDS]||texcs, has[TANGENTS]||tangents);

		//read data
		for (int i = 0; i < numVertices; ++i)
		{
			for (int a = 0; a < 4; ++a)
			{
				if (check[a])
					ifile.read((char*)(data + i * newVertSize + paste[a]), size[a]);
			}
		}
	}

	buffered = false; //need to rebuffer
	bool good = ifile.good();
	ifile.close();

	if (good)
		return true;
	else
	{
		error = true;
		printf("Error: Error while reading %s\n", file);
		return false;
	}
}
*/

#define MESH_INDEX_S(i, j, w, s) ((((i)*(w))+(j))*(s))
#define MESH_INDEX(i, j, w) (((i)*(w))+(j))

int generateTriangleStrip(vec2i tess, unsigned int*& indices)
{
	int num = (tess.x * 2) * (tess.y - 1);
	indices = new unsigned int[num];
	int o = 0;
	for (int i = 0; i < tess.y-1; ++i)
	{
		for (int j = 0; j < tess.x; ++j)
		{
			if (i % 2 == 0) //even row
			{
				indices[o++] = MESH_INDEX(i, j, tess.x);
				indices[o++] = MESH_INDEX(i+1, j, tess.x);
			}
			else
			{
				indices[o++] = MESH_INDEX(i, tess.x-j-1, tess.x);
				indices[o++] = MESH_INDEX(i+1, tess.x-j-1, tess.x);
			}
		}
	}
	assert(o == num);
	return num;
}

VBOMesh VBOMesh::grid(vec2i size, vec3f (*param)(const vec2f&)) //param returns vert
{
	VBOMesh mesh;
	vec2i tess = size + 1;
	mesh.numVertices = tess.x * tess.y;
	mesh.has[VERTICES] = true;
	mesh.allocate();
	mesh.numIndices = generateTriangleStrip(tess, mesh.dataIndices);
	mesh.numPolygons = mesh.numIndices-2;
	mesh.indexed = true;
	mesh.primitives = GL_TRIANGLE_STRIP;
	for (int i = 0; i < tess.y; ++i)
	{
		for (int j = 0; j < tess.x; ++j)
		{
			vec2f point(j/(float)(size.x), i/(float)(size.y));
			vec3f vert = param(point);
			memcpy(mesh.data + MESH_INDEX_S(i, j, tess.x, mesh.strideFloats), &vert, sizeof(vec3f));
		}
	}
	return mesh;
}
VBOMesh VBOMesh::grid(vec2i size, void (*param)(const vec2f&, vec3f&)) //out args vert
{
	VBOMesh mesh;
	vec2i tess = size + 1;
	mesh.numVertices = tess.x * tess.y;
	mesh.has[VERTICES] = true;
	mesh.allocate();
	mesh.numIndices = generateTriangleStrip(tess, mesh.dataIndices);
	mesh.numPolygons = mesh.numIndices-2;
	mesh.indexed = true;
	mesh.primitives = GL_TRIANGLE_STRIP;
	for (int i = 0; i < tess.y; ++i)
	{
		for (int j = 0; j < tess.x; ++j)
		{
			vec2f point(j/(float)(size.x), i/(float)(size.y));
			vec3f& vertex = *(vec3f*)(mesh.data + MESH_INDEX_S(i, j, tess.x, mesh.strideFloats));
			param(point, vertex);
		}
	}
	return mesh;
}
VBOMesh VBOMesh::grid(vec2i size, void (*param)(const vec2f&, vec3f&, vec3f&)) //out args vert, norm
{
	VBOMesh mesh;
	vec2i tess = size + 1;
	mesh.numVertices = tess.x * tess.y;
	mesh.has[VERTICES] = true;
	mesh.has[NORMALS] = true;
	mesh.allocate();
	mesh.numIndices = generateTriangleStrip(tess, mesh.dataIndices);
	mesh.numPolygons = mesh.numIndices-2;
	mesh.indexed = true;
	mesh.primitives = GL_TRIANGLE_STRIP;
	for (int i = 0; i < tess.y; ++i)
	{
		for (int j = 0; j < tess.x; ++j)
		{
			vec2f point(j/(float)(size.x), i/(float)(size.y));
			vec3f& vertex = *(vec3f*)(mesh.data + MESH_INDEX_S(i, j, tess.x, mesh.strideFloats) + 0);
			vec3f& normal = *(vec3f*)(mesh.data + MESH_INDEX_S(i, j, tess.x, mesh.strideFloats) + 3);
			param(point, vertex, normal);
		}
	}
	return mesh;
}
VBOMesh VBOMesh::grid(vec2i size, void (*param)(const vec2f&, vec3f&, vec3f&, vec2f&)) //out args vert, norm, tex
{
	VBOMesh mesh;
	vec2i tess = size + 1;
	mesh.numVertices = tess.x * tess.y;
	mesh.has[VERTICES] = true;
	mesh.has[NORMALS] = true;
	mesh.has[TEXCOORDS] = true;
	mesh.allocate();
	mesh.numIndices = generateTriangleStrip(tess, mesh.dataIndices);
	mesh.numPolygons = mesh.numIndices-2;
	mesh.indexed = true;
	mesh.primitives = GL_TRIANGLE_STRIP;
	for (int i = 0; i < tess.y; ++i)
	{
		for (int j = 0; j < tess.x; ++j)
		{
			vec2f point(j/(float)(size.x), i/(float)(size.y));
			vec3f& vertex = *(vec3f*)(mesh.data + MESH_INDEX_S(i, j, tess.x, mesh.strideFloats) + 0);
			vec3f& normal = *(vec3f*)(mesh.data + MESH_INDEX_S(i, j, tess.x, mesh.strideFloats) + 3);
			vec2f& texcoord = *(vec2f*)(mesh.data + MESH_INDEX_S(i, j, tess.x, mesh.strideFloats) + 6);
			param(point, vertex, normal, texcoord);
		}
	}
	return mesh;
}
VBOMesh VBOMesh::grid(vec2i size, void (*param)(const vec2f&, vec3f&, vec3f&, vec3f&)) //out args vert, norm, tangent
{
	VBOMesh mesh;
	vec2i tess = size + 1;
	mesh.numVertices = tess.x * tess.y;
	mesh.has[VERTICES] = true;
	mesh.has[NORMALS] = true;
	mesh.has[TANGENTS] = true;
	mesh.allocate();
	mesh.numIndices = generateTriangleStrip(tess, mesh.dataIndices);
	mesh.numPolygons = mesh.numIndices-2;
	mesh.indexed = true;
	mesh.primitives = GL_TRIANGLE_STRIP;
	for (int i = 0; i < tess.y; ++i)
	{
		for (int j = 0; j < tess.x; ++j)
		{
			vec2f point(j/(float)(size.x), i/(float)(size.y));
			vec3f& vertex = *(vec3f*)(mesh.data + MESH_INDEX_S(i, j, tess.x, mesh.strideFloats) + 0);
			vec3f& normal = *(vec3f*)(mesh.data + MESH_INDEX_S(i, j, tess.x, mesh.strideFloats) + 3);
			vec3f& tangent = *(vec3f*)(mesh.data + MESH_INDEX_S(i, j, tess.x, mesh.strideFloats) + 6);
			param(point, vertex, normal, tangent);
		}
	}
	return mesh;
}
VBOMesh VBOMesh::grid(vec2i size, void (*param)(const vec2f&, vec3f&, vec3f&, vec2f&, vec3f&)) //out args vert, norm, tex, tangent
{
	VBOMesh mesh;
	vec2i tess = size + 1;
	mesh.numVertices = tess.x * tess.y;
	mesh.has[VERTICES] = true;
	mesh.has[NORMALS] = true;
	mesh.has[TEXCOORDS] = true;
	mesh.has[TANGENTS] = true;
	mesh.allocate();
	mesh.numIndices = generateTriangleStrip(tess, mesh.dataIndices);
	mesh.numPolygons = mesh.numIndices-2;
	mesh.indexed = true;
	mesh.interleaved = true;
	mesh.primitives = GL_TRIANGLE_STRIP;
	for (int i = 0; i < tess.y; ++i)
	{
		for (int j = 0; j < tess.x; ++j)
		{
			vec2f point(j/(float)(size.x), i/(float)(size.y));
			vec3f& vertex = *(vec3f*)(mesh.data + MESH_INDEX_S(i, j, tess.x, mesh.strideFloats) + 0);
			vec3f& normal = *(vec3f*)(mesh.data + MESH_INDEX_S(i, j, tess.x, mesh.strideFloats) + 3);
			vec2f& texcoord = *(vec2f*)(mesh.data + MESH_INDEX_S(i, j, tess.x, mesh.strideFloats) + 6);
			vec3f& tangent = *(vec3f*)(mesh.data + MESH_INDEX_S(i, j, tess.x, mesh.strideFloats) + 8);
			param(point, vertex, normal, texcoord, tangent);
		}
	}
	return mesh;
}
VBOMesh VBOMesh::cube()
{
	VBOMesh mesh;
	return mesh;
}
VBOMesh VBOMesh::quad()
{
	VBOMesh mesh;
	return mesh;
}
