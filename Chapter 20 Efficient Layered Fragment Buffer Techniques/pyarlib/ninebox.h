/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef NINE_BOX_H
#define NINE_BOX_H

#include "vec.h"
#include "matrix.h"
#include "atlas.h"

struct TextureBuffer;

namespace QG
{

struct Dimensions
{
	int top;
	int right;
	int bottom;
	int left;
	Dimensions() {top=right=bottom=left=0;}
	Dimensions(int i) {top=right=bottom=left=i;}
	void operator=(int i) {top=right=bottom=left=i;}
};

//initially, all NineBoxImages will populate data based on sourceTex
//when TextureAtlas::pack() is called in NineBoxPool::create(), NineBoxImages structs
//will be updated to reference an area within the atlas texture
//to references subImages, add nsize.y to npos.y
struct NineBoxImage : public TextureAtlas::Tex
{
	unsigned int sourceTex; //just in case multiple atlases are used at some point
	unsigned int subCount; //number of vertically stacked "sub" images (eg checkbox checked/undhecked)
	vec2i inset; //border inset in pixels
	NineBoxImage();
	NineBoxImage(int offset, unsigned int count);
	void update(int tex, const TextureAtlas::Tex& sub);
};

struct NineBoxVerts
{
	struct Vert
	{
		vec3f pos;
		vec2f tex;
	};

	//degenerate first vertex, 24 for the ninebox strip, and degenerate last vertex
	Vert triStrip[26];
};

struct NineBoxPool;
struct NineBox
{
private:
	friend struct NineBoxPool;
	int geomIndex;
	vec2f texPos;
	vec2f texSize;
	vec2f texInset;
	NineBoxImage* image;
	NineBoxVerts::Vert getVertex(int i, int j);
public:
	Dimensions border; //physical border size (pixels)
	vec2i pos; //position in pixels (from top left)
	vec2i size; //size of center area (pixels)
	float Z; //z-index (currently uses sorting, but may change to depth buffer)

	//index into stack of sub-images (eg checkbox checked/undhecked)
	int subIndex;

	NineBox();
	NineBox(NineBoxImage* image);

	//zero everything. essentially a "visible = false" method
	void zero();
	
	//generate ninebox geometry for VBO
	void generate(NineBoxVerts& geometry);
};

struct NineBoxPool
{
private:
	//next unique instance ID. when an instance is removed, this is update to fill the gap
	int nextID;
	bool released; //causes remove() to silently do nothing

	//an atlas is used so all nineboxes can be drawn in one go from a single texture
	TextureAtlas* atlas;

	//images are indexed by their given ID (or filename if none given)
	typedef std::map<std::string, NineBoxImage> NineBoxImages;
	NineBoxImages images;

	//instances have unique handles so they can easily be updated via operator[]
	typedef std::map<int, NineBox> NineBoxInstances;
	NineBoxInstances instances;

	//when an instance is updated, the geometry must also be updated and uploaded to the geometry VBO
	typedef std::set<int> DirtySet;
	DirtySet dirty;
	bool globalDirty;

	//vertex and texture
	VertexBuffer geometry;

	//if there is a problem with the texture atlas, disable drawing
	bool error;

	NineBoxPool(const NineBoxPool& other) {} //private - NO COPYING
	void operator=(const NineBoxPool& other) {} //private - NO COPYING
public:
	NineBoxPool();
	virtual ~NineBoxPool();

	//create a ninebox instance
	int create(std::string id);

	//load a ninebox texture. "offset" is the border inset with "count" stacked copies
	bool load(std::string id, std::string fname, int offset, int count = 1);

	//update an instance. NOTE: instance becomes dirty and triggers a buffer upload
	NineBox& operator[](const int i);

	//removes an instance
	void remove(int i);

	//draws all instances
	void draw(const mat44& transform);

	//gets the size in pixels of a ninebox image
	vec2i getSize(std::string id);

	void release();
};

}

#endif
