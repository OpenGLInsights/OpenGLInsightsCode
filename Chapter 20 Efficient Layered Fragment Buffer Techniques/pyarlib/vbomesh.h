/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef VBOMESH_H
#define VBOMESH_H

#include "matrix.h"
#include "gpu.h"
#include "loader.h"

template<typename T>
struct InterleavedEditor
{
	int offset, stride;
	void* data;
	InterleavedEditor() {data=NULL;offset=0;stride=sizeof(T);}
	InterleavedEditor(int offset, int stride, void* data) : offset(offset), stride(stride), data(data) {}
	T& operator[](intptr_t index) {return *(T*)((char*)data+(offset+stride*index));} //wtf void pointer warning. HACK: cast to char
};

struct VBOMeshFaceset
{
	int startIndex;
	int endIndex;
	int material;
};

struct VBOMeshMaterial
{
	virtual void bind() =0;
	virtual void unbind() =0;
};

struct VBOMesh : public Loader<VBOMesh>
{
	enum VertexDataType
	{
		VERTICES,
		NORMALS,
		TEXCOORDS,
		TANGENTS,
		DATA_TYPES
	};

	static const int dataTypes = DATA_TYPES;

	static const int size[dataTypes];

	bool indexed; //drawArrays/drawElements
	bool error;
	bool buffered;
	bool interleaved; //cannot be draw()n without interleave()ing
	GLenum primitives;

	int offset[dataTypes];
	bool has[dataTypes];
	float* sub[dataTypes];

	int numVertices;
	int numIndices;
	int numPolygons; //for stats only
	int stride; //in bytes
	int strideFloats; //in floats

	VertexBuffer vertices;
	IndexBuffer indices;
	float* data;
	unsigned int* dataIndices;
	
	typedef std::map<int, VBOMeshFaceset> Facesets;
	Facesets facesets;
	std::vector<VBOMeshMaterial*> materials;
	std::map<std::string, int> materialNames;

	GLuint vloc, nloc, txloc, tgloc;
	
	vec3f boundsMin, average, center, boundsMax; //call computeInfo() to populate

	VBOMesh();

	//if no args provided, will use old glEnableClientState method
	void useMaterial(int start, int end, std::string name);
	void addMaterial(VBOMeshMaterial* material, std::string name);
	void bind(GLuint vloc = -1, GLuint nloc = -1, GLuint txloc = -1, GLuint tgloc = -1);
	void draw(bool autoAttribLocs = true); //if not uploaded, will call upload(true).
	void upload(bool freeLocal = true);
	void allocate(); //called during interleave to allocate float* data
	void calcInternal();
	void interleave(bool freeSource = true);
	bool computeInfo();
	void transform(const mat44& m);
	bool validate();

	void averageVertices();
	void generateNormals();
	void generateTangents();
	void realloc(bool verts, bool norms, bool texcs, bool tangents);
	bool inject(float* data, bool verts = true, bool norms = false, bool texcs = false, bool tangents = false);
	bool release();
	int getStride();

	static void paramPlane(const vec2f& point, vec3f& vertex, vec3f& normal, vec2f& texcoord, vec3f& tangent);
	static void paramSphere(const vec2f& point, vec3f& vertex, vec3f& normal, vec2f& texcoord, vec3f& tangent);

	static VBOMesh grid(vec2i size, vec3f (*param)(const vec2f&)); //param returns vert
	static VBOMesh grid(vec2i size, void (*param)(const vec2f&, vec3f&)); //out args vert
	static VBOMesh grid(vec2i size, void (*param)(const vec2f&, vec3f&, vec3f&)); //out args vert, norm
	static VBOMesh grid(vec2i size, void (*param)(const vec2f&, vec3f&, vec3f&, vec2f&)); //out args vert, norm, tex
	static VBOMesh grid(vec2i size, void (*param)(const vec2f&, vec3f&, vec3f&, vec3f&)); //out args vert, norm, tangent
	static VBOMesh grid(vec2i size, void (*param)(const vec2f&, vec3f&, vec3f&, vec2f&, vec3f&) = paramPlane); //out args vert, norm, tex, tangent
	static VBOMesh cube();
	static VBOMesh quad();
};

#endif
