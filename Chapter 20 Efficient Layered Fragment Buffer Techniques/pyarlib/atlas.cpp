/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include "atlas.h"
#include "util.h"

#include "includegl.h"
#include "shader.h"
#include "shaderutil.h"

#include "imgpng.h"
#include "vbomesh.h"
#include "resources.h"

using namespace std;

EMBED(atlasBlitVert, atlasBlit.vert);
EMBED(atlasBlitFrag, atlasBlit.frag);

static Shader shader("atlasBlit.vert", "atlasBlit.frag");

bool TextureAtlas::Tex::setUniform(Shader* program, const std::string& name) const
{
	bool ok  = true;
	ok = ok && program->set(name + ".pos", npos);
	ok = ok && program->set(name + ".size", nsize);
	return ok;
}

TextureAtlas::TextureAtlas()
{
	dirty = false;
}
TextureAtlas::~TextureAtlas()
{
	atlas.release();
}
bool TextureAtlas::isDirty()
{
	return dirty;
}
void TextureAtlas::recursivePack(vec2i packOffset, vec2i packSize)
{
	//get the next biggest source that fits
	Source* s;
	while (true)
	{
		//return if we've run out of items to pack
		if (cur >= (int)sources.size())
			return;
		
		s = &sources[sources.size() - cur - 1];
	
		++cur;
	
		//check that it fits and has not already been packed
		if (!s->packed && s->size.x <= packSize.x && s->size.y <= packSize.y)
			break;
	}
	
	//place the texture
	s->packed = true;
	s->pos = packOffset;
	
	//we're left with an L shape. split to give the largest area
	//possible, then recurse into the biggest first
	int a1 = (packSize.x - s->size.x) * packSize.y;
	int a2 = (packSize.y - s->size.y) * packSize.x;
	
	int curBack = cur;
	if (a1 < a2)
	{
		recursivePack(packOffset + vec2i(s->size.x, 0), vec2i(packSize.x - s->size.x, packSize.y));
		cur = curBack;
		recursivePack(packOffset + vec2i(0, s->size.y), vec2i(s->size.x, packSize.y - s->size.y));
	}
	else
	{
		recursivePack(packOffset + vec2i(0, s->size.y), vec2i(packSize.x, packSize.y - s->size.y));
		cur = curBack;
		recursivePack(packOffset + vec2i(s->size.x, 0), vec2i(packSize.x - s->size.x, s->size.y));
	}
}
bool TextureAtlas::createPack(vec2i testSize)
{
	sort(sources.begin(), sources.end());
	
	cur = 0;
	
	//clear packed flags
	for (int i = 0; i < (int)sources.size(); ++i)
		sources[i].packed = false;
	
	recursivePack(vec2i(0), testSize);
	
	//see if everything was packed
	bool done = true;
	for (int i = 0; i < (int)sources.size(); ++i)
		done = done && sources[i].packed;
	
	return done;
}
void TextureAtlas::upload()
{
	FrameBuffer fbo;
	fbo.colour[0] = &atlas;
	fbo.resize(size);
	fbo.attach();
	fbo.bind();
	glClearColor(1,1,1,0);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_BLEND);

	vec3f quadData[] = {
		vec3f(0, 0, 0),
		vec3f(0, 1, 0),
		vec3f(1, 0, 0),
		vec3f(1, 1, 0)
		};
	VertexBuffer quad;
	quad.buffer(quadData, sizeof(quadData));

	//instead of loading the shader from disk, use the embedded text included with BINDATA()
	static bool setSource = false;
	if (!setSource)
	{
		Shader::include("atlasBlit.vert", RESOURCE(atlasBlitVert));
		Shader::include("atlasBlit.frag", RESOURCE(atlasBlitFrag));
		setSource = true;
	}

	shader.use();
	shader.attrib("osVert", quad, GL_FLOAT, sizeof(vec3f));
	texMap.clear();
	
	//render all the sources
	vec2f fsize(size);
	for (int i = 0; i < (int)sources.size(); ++i)
	{
		texMap[sources[i].name] = textures.size();
		
		Tex t;
		t.size = sources[i].size;
		t.npos = vec2f(sources[i].pos)/fsize;
		t.nsize = vec2f(t.size)/fsize;
		
		textures.push_back(t);
		
		shader.set("pos", t.npos);
		shader.set("size", t.nsize);
		glBindTexture(GL_TEXTURE_2D, sources[i].tex);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
	shader.unuse();
	fbo.unbind();
	fbo.colour[0] = NULL; //don't want fbo release()ing atlas
	fbo.release();
	quad.release();
	
	dirty = false;

	#if 0
	QI::ImagePNG test;
	test.readTexture(atlas);
	test.saveImage("atlas.png");
	printf("saved atlas.png for debugging\n");
	#endif
}
void TextureAtlas::cleanup(bool deleteSource)
{
	if (deleteSource)
	{
		for (int i = 0; i < (int)sources.size(); ++i)
		{
			glDeleteTextures(1, &sources[i].tex);
		}
	}
	sources.clear();
}
void TextureAtlas::add(std::string name, GLuint tex)
{
	assert(tex != 0);

	dirty = true;

	Source source;
	source.tex = tex;
	source.name = name;
	
	//FIXME: may want to pass in texture size instead
	//for now, packing a texture atlas is considered pre-processing and I don't care about speed
	glBindTexture(GL_TEXTURE_2D, tex);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &source.size.x);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &source.size.y);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	sources.push_back(source);
}
bool TextureAtlas::has(std::string name)
{
	return texMap.find(name) != texMap.end();
}
TextureAtlas::Tex TextureAtlas::get(std::string name)
{
	return textures[texMap[name]];
}
bool TextureAtlas::pack()
{
	//get the initial test size for packing
	int totalPixels = 0;
	for (int i = 0; i < (int)sources.size(); ++i)
	{
		totalPixels += sources[i].size.x * sources[i].size.y;
	}
	
	size = vec2i(32);
	
	//brute force attempt to pack with increasing powers of two textures
	/*
	bool success = false;
	int perfectWidth = 1 << (ceilLog2(totalPixels) / 2);
	for (int w = perfectWidth; w < TEXTURE_ATLAS_MAX_SIZE; w *= 2)
	{
		for (int h = 1; h >= 0; --h)
		{
			size = vec2i(w>>h, w);
			success = createPack(size);
			if (success)
				break;
		}
		if (success)
			break;
	}
	*/

	//binary search for best size
	int canary = 0;
	bool success = false;
	int perfectWidth = 1 << (ceilLog2(totalPixels) / 2);
	int w = perfectWidth;
	int s;
	for (s = w; s >= 4; s /= 2) //size must be a multiple of 4
	{
		size = vec2i(w);
		success = createPack(size);
		if (success)
			w -= s;
		else
			w += s;
		++canary;
	}
	if (!success) //make sure search ends on a success
	{
		w += s;
		size = vec2i(w);
		success = createPack(size);
		++canary;
	}
	
	//even if unsuccessful, still upload and just let the atlas be invalid, returning false
	//no need to make the user's day any worse
	upload();
	
	return success;
}
