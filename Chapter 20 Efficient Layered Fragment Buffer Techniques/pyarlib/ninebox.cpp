/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <string>
#include <vector>
#include <set>
#include <map>

#include <assert.h>
#include <stdio.h>

#include "ninebox.h"
#include "gpu.h"
#include "texture.h"
#include "shader.h"
#include "util.h"
#include "resources.h"

EMBED(nineboxFrag, ninebox.frag);
EMBED(nineboxVert, ninebox.vert);

static Shader shader("ninebox.vert", "ninebox.frag");

using namespace QG;
using namespace std;

NineBoxImage::NineBoxImage()
{
	subCount = 0;
}
NineBoxImage::NineBoxImage(int offset, unsigned int count)
{
	subCount = count;
	inset = vec2i(offset);
}
void NineBoxImage::update(int tex, const TextureAtlas::Tex& sub)
{
	*dynamic_cast<TextureAtlas::Tex*>(this) = sub;

	assert(size.x > 0);
	assert(size.y > 0);

	nsize.y /= subCount;
	
	sourceTex = tex;
}

NineBox::NineBox()
{
	image = NULL;
}
NineBox::NineBox(NineBoxImage* image)
{
	zero();
	this->image = image;
}

void NineBox::zero()
{
	subIndex = 0;
	Z = 0.0f;
	border = 0;
	pos = vec2i(0.0f);
	size = vec2i(0.0f);
}

NineBoxVerts::Vert NineBox::getVertex(int i, int j)
{
	NineBoxVerts::Vert ret;
	ret.pos.z = 0.999 - Z;
	switch (i)
	{
	case 0:
		ret.pos.x = pos.x - border.left;
		ret.tex.x = texPos.x;
		break;
	case 1:
		ret.pos.x = pos.x;
		ret.tex.x = texPos.x + texInset.x;
		break;
	case 2:
		ret.pos.x = pos.x + size.x;
		ret.tex.x = texPos.x + texSize.x - texInset.x;
		break;
	case 3:
		ret.pos.x = pos.x + size.x + border.right;
		ret.tex.x = texPos.x + texSize.x;
		break;
	}
	switch (j)
	{
	case 0:
		ret.pos.y = pos.y - border.top;
		ret.tex.y = texPos.y + texSize.y;
		break;
	case 1:
		ret.pos.y = pos.y;
		ret.tex.y = texPos.y + texSize.y - texInset.y;
		break;
	case 2:
		ret.pos.y = pos.y + size.y;
		ret.tex.y = texPos.y + texInset.y;
		break;
	case 3:
		ret.pos.y = pos.y + size.y + border.bottom;
		ret.tex.y = texPos.y;
		break;
	}
	return ret;
}

void NineBox::generate(NineBoxVerts& geometry)
{
	assert(image);
	assert(image->subCount > 0);
	texPos = image->npos + vec2f(0.0f, subIndex * image->nsize.y);
	texSize = image->nsize;
	texInset = (texSize * vec2f(image->inset)) / vec2f(image->size);
	
	int v = 0;
	geometry.triStrip[v++] = getVertex(0, 0);
	for (int j = 0; j < 4-1; ++j)
	{
		for (int k = 0; k < 4; ++k)
		{
			int i = (j % 2 == 0) ? (k) : (4 - 1 - k);
			geometry.triStrip[v++] = getVertex(i, j);
			geometry.triStrip[v++] = getVertex(i, j + 1);
		}
	}
	geometry.triStrip[v++] = getVertex(3, 3);

	assert(v == sizeof(NineBoxVerts) / sizeof(NineBoxVerts::Vert));
}

NineBoxPool::NineBoxPool()
{
	globalDirty = false;
	atlas = NULL;
	nextID = 0;
	error = false;
	released = true;
}
NineBoxPool::~NineBoxPool()
{
	delete atlas;
}
int NineBoxPool::create(std::string id)
{
	if (!atlas)
	{
		//only print warning once
		static bool givenWarning = false;
		if (!givenWarning)
		{
			printf("Error: must load some textures before creating a ninebox\n");
			givenWarning = true;
		}
		return -1;
	}
	
	//make sure the image id exists
	if (images.find(id) == images.end())
	{
		printf("Could not create ninebox for %s\n", id.c_str());
		return -1;
	}

	while (instances.find(nextID) != instances.end())
		++nextID;

	int ret = nextID;
	++nextID;

	assert(ret < 10000); //just in case create() is accidentally called once a frame or something

	NineBox ninebox(&images[id]);
	instances[ret] = ninebox;
	//the increase in size of instances will trigger a refresh
	
	globalDirty = true;
	released = false;
	
	dirty.insert(ret);
	return ret;
}
bool NineBoxPool::load(std::string id, std::string fname, int offset, int count)
{
	if (!atlas)
	{
		atlas = new TextureAtlas();
		atlas->atlas.anisotropy = 4;
		atlas->atlas.mipmap = true;
		atlas->atlas.nearest = false;
		atlas->atlas.repeat = false;
	}

	GLuint tex = getTexture(fname.c_str());
	vec2i size(getLastImageWidth(), getLastImageHeight());
	
	if (tex == 0)
		return false;
	assert(size.x > 0 && size.y > 0);

	atlas->add(id, tex);
	
	NineBoxImage image(offset, count);
	image.size = size; //set via .update() but needed beforehand by NineBoxPool::getSize()
	images[id] = image;
	return true;
}
NineBox& NineBoxPool::operator[](const int i)
{
	dirty.insert(i);
	return instances[i];
}
void NineBoxPool::remove(int i)
{
	if (released)
		return;
	assert(instances.find(i) != instances.end());
	instances.erase(i);
	if (dirty.find(i) != dirty.end())
		dirty.erase(i);
	globalDirty = true;
	
	nextID = mymin(i, nextID);
}

void NineBoxPool::draw(const mat44& transform)
{
	if (error || instances.size() == 0)
		return;
	
	//if additional images have been added, the atlas will be dirty
	if (atlas->isDirty())
	{
		error = error || !atlas->pack();
		
		atlas->atlas.genMipmap();
		
		//since the atlas has changed, all ninebox images must be updated
		for (NineBoxImages::iterator it = images.begin(); it != images.end(); ++it)
			it->second.update(atlas->atlas, atlas->get(it->first));
	}
	
	//update geometry buffer as needed
	unsigned int nineboxGeomSize = sizeof(NineBoxVerts);
	if (geometry.size() / nineboxGeomSize != instances.size())
		globalDirty = true; //instances have been added or removed
	if (globalDirty)
	{
		globalDirty = false;
		
		//resize VBO
		geometry.resize(instances.size() * nineboxGeomSize);

		//sort based on depth
		std::vector<std::pair<float, NineBox*> > sortedOrder;
		sortedOrder.resize(instances.size());
		int index = 0;
		for (NineBoxInstances::iterator it = instances.begin(); it != instances.end(); ++it, ++index)
			sortedOrder[index] = std::pair<float, NineBox*>(it->second.Z, &it->second);
		std::sort(sortedOrder.begin(), sortedOrder.end());
		
		//everything is dirty
		NineBoxVerts* verts = (NineBoxVerts*)geometry.map(false, true);
		for (int i = 0; i < (int)sortedOrder.size(); ++i)
		{
			sortedOrder[i].second->geomIndex = i;
			sortedOrder[i].second->generate(verts[sortedOrder[i].second->geomIndex]);
		}
		geometry.unmap();
	}
	else if (dirty.size())
	{
		//buffer dirty boxes only
		NineBoxVerts verts;
		for (DirtySet::iterator it = dirty.begin(); it != dirty.end(); ++it)
		{
			instances[*it].generate(verts);
			int index = instances[*it].geomIndex;
			geometry.buffer(&verts, sizeof(NineBoxVerts), sizeof(NineBoxVerts) * index);
		}
	}
	dirty.clear();

	//instead of loading the shader from disk, use the embedded text included with BINDATA()
	static bool setSource = false;
	if (!setSource)
	{
		Shader::include("ninebox.frag", RESOURCE(nineboxFrag));
		Shader::include("ninebox.vert", RESOURCE(nineboxVert));
		setSource = true;
	}

	shader.use();

	if (!shader.error())
	{
		shader.attrib("osVert", geometry, GL_FLOAT, sizeof(vec3f), sizeof(NineBoxVerts::Vert), 0);
		shader.attrib("texCoord", geometry, GL_FLOAT, sizeof(vec2f), sizeof(NineBoxVerts::Vert), sizeof(vec3f));
		shader.set("transformMat", transform);
		glBindTexture(GL_TEXTURE_2D, *atlas);
		shader.set("tex", 0);

		shader.set("test", 0);
		int numVerts = geometry.size() / sizeof(NineBoxVerts::Vert);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, numVerts);
		
		#if 0
		shader.set("test", 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, numVerts);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		#endif
	
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	shader.unuse();
	CHECKERROR;
}
vec2i NineBoxPool::getSize(std::string id)
{
	if (!atlas)
	{
		//no ninebox images were added
		return vec2i(0);
	}
	vec2i size = images[id].size;
	size.y /= images[id].subCount;
	return size;
}
void NineBoxPool::release()
{
	if (atlas)
		atlas->cleanup(true); //delete loaded sources
	geometry.release();
	instances.clear();
	dirty.clear();
	images.clear();
	released = true;
}
