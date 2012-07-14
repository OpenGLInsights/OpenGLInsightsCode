/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#define TEXTURE_ATLAS_MAX_SIZE 8192

#include "vec.h"
#include "shader.h"
#include "gpu.h"

#include <vector>
#include <map>
#include <string>

/*
GLSL:

struct Tex
{
	vec2 pos; //normalized position within atlas
	vec2 size; //normalized size within atlas
}
*/

class TextureAtlas
{
public:
	struct Tex : public ShaderUniform
	{
		vec2f npos;
		vec2f nsize;
		vec2i size;
	protected:
		virtual bool setUniform(Shader* program, const std::string& name) const;
	};
	vec2i size;
protected:
	struct Source
	{
		std::string name;
	
		vec2i pos;
		vec2i size;
		unsigned int tex;
		bool packed;
		
		//for greedy packing using std::sort
		bool operator<(const Source& o) const {return size.x*size.y < o.size.x*o.size.y;}
	};
	vec2i packMin;
	bool dirty;
	int cur; //current source index in sources being packed
	std::vector<Tex> textures;
	std::vector<Source> sources;
	std::map<std::string, int> texMap;
	void recursivePack(vec2i packOffset, vec2i packSize);
	virtual bool createPack(vec2i testSize); //attempts to pack into a testSize sized atlas
	void upload(); //upload current packing in sources to a new texture
public:
	Texture2D atlas;
	TextureAtlas();
	virtual ~TextureAtlas();
	bool isDirty();
	void cleanup(bool deleteSource); //free sources, leaving just the atlas
	void add(std::string name, GLuint tex); //add a source texture
	bool has(std::string name);
	Tex get(std::string name);
	bool pack(); //packs the added sources into the atlas
	operator GLuint() const {return atlas;}
};

#endif
