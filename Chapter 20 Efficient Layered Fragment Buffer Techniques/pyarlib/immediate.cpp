/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <stdio.h>
#include <assert.h>
#include <memory.h>

#include <vector>
#include <set>
#include <map>
#include <string>

#include "includegl.h"
#include "immediate.h"
#include "shader.h"
#include "resources.h"

EMBED(immediateVert, immediate.vert);
EMBED(immediateFrag, immediate.frag);

static Shader shader("immediate.vert", "immediate.frag");

Immediate::Immediate()
{
	pretransform = false;
	currentTexture = 0;
	begun = false;
	projection = mat44::identity();
	currentCol = vec4f(1.0f);
	currentCoord = vec2f(0.0f);
	currentType = POINTS;
	
	verts.push_back(Vert()); //first element is primitve restart
	
	indices.resize((int)NUM_PRIMITIVE_TYPES);
	
	primEnums.resize((int)NUM_PRIMITIVE_TYPES, 0);
	primEnums[POINTS] = GL_POINTS;
	primEnums[LINES] = GL_LINES;
	primEnums[LINE_LOOP] = GL_LINE_LOOP;
	primEnums[LINE_STRIP] = GL_LINE_STRIP;
	primEnums[POLYGON] = GL_POLYGON;
	primEnums[TRIANGLES] = GL_TRIANGLES;
	primEnums[TRIANGLE_STRIP] = GL_TRIANGLE_STRIP;
	primEnums[TRIANGLE_FAN] = GL_TRIANGLE_FAN;
	primEnums[QUADS] = GL_QUADS;
	primEnums[QUAD_STRIP] = GL_QUAD_STRIP;
}
void Immediate::enablePretransform(bool enable)
{
	pretransform = enable;
}
void Immediate::useTexture(unsigned int handle)
{
	if (!begun)
		currentTexture = handle;
}
void Immediate::begin(Primitive type)
{
	assert(type >= 0 && type < (int)indices.size());
	begun = true;
	currentType = type;
}
void Immediate::colour(vec3f c) {colour(vec4f(c, 1.0f));}
void Immediate::colour(float r, float g, float b) {colour(vec4f(r, g, b, 1.0f));}
void Immediate::colour(float r, float g, float b, float a) {colour(vec4f(r, g, b, a));}
void Immediate::colour(vec4f c)
{
	currentCol = c;
}
void Immediate::vertex(vec2f p) {vertex(vec3f(p, 0.0f));}
void Immediate::vertex(float x, float y) {vertex(vec3f(x, y, 0.0f));}
void Immediate::vertex(float x, float y, float z) {vertex(vec3f(x, y, z));}
void Immediate::vertex(vec3f p)
{
	#define TOBYTE(c) ((unsigned char)(c*255))
	
	static Vert v;
	//v.tex = currentCoord;
	v.col = 
		(TOBYTE(currentCol.w) << 24) +
		(TOBYTE(currentCol.z) << 16) +
		(TOBYTE(currentCol.y) << 8) +
		TOBYTE(currentCol.x);
	if (pretransform)
		p = *this * p;
	v.pos = p;
	indices[currentType].push_back(verts.size());
	verts.push_back(v);
	
	assert(verts.size() < 1000); //guessing flush() or clear() is never called
}
void Immediate::texture(float u, float v) {texture(vec2f(u, v));}
void Immediate::texture(vec2f c)
{
	currentCoord = c;
}
void Immediate::end(bool flushStraightAway)
{
	indices[currentType].push_back(0); //restart primitive
	begun = false;
	if (flushStraightAway)
		flush();
}
void Immediate::draw()
{
	//buffer vertices
	bufferVertices.resize(sizeof(Vert) * verts.size(), false);
	memcpy(bufferVertices.map(false, true), &verts[0], sizeof(Vert) * verts.size());
	bufferVertices.unmap();
	
	//count indices from all primitives to resize buffer
	int total = 0;
	for (int i = 0; i < NUM_PRIMITIVE_TYPES; ++i)
		total += indices[i].size();
	
	bufferIndices.resize(total * sizeof(unsigned int), false);
	
	//buffer all indices
	int offset = 0;
	unsigned int* indexDat = (unsigned int*)bufferIndices.map(false, true);
	for (int i = 0; i < NUM_PRIMITIVE_TYPES; ++i)
	{
		if (indices[i].size())
		{
			memcpy(indexDat + offset, &indices[i][0], sizeof(unsigned int) * indices[i].size());
		}
		offset += indices[i].size();
	}
	bufferIndices.unmap();
	
	if (bufferVertices.size() && bufferIndices.size())
	{
		//instead of loading the shader from disk, use the embedded text included with BINDATA()
		static bool setSource = false;
		if (!setSource)
		{
			Shader::include("immediate.vert", RESOURCE(immediateVert));
			Shader::include("immediate.frag", RESOURCE(immediateFrag));
			setSource = true;
		}
	
		//init shader and draw
		shader.use();
		shader.set("projectionMat", projection);

		if (pretransform)
			shader.set("modelviewMat", mat44::identity());
		else
			shader.set("modelviewMat", (mat44)*this);

		shader.attrib("osVert", bufferVertices, GL_FLOAT, sizeof(vec3f), sizeof(Vert), 0);
		//shader.attrib("texCoord", bufferVertices, GL_FLOAT, sizeof(vec2f), sizeof(Vert), sizeof(vec3f));
		shader.attrib("vertColour", bufferVertices, GL_UNSIGNED_BYTE, sizeof(unsigned int), sizeof(Vert), sizeof(vec3f));
	
		bufferIndices.bind();
		glEnable(GL_PRIMITIVE_RESTART);
		glPrimitiveRestartIndex(0);
	
		//set the material
		//glBindTexture(GL_TEXTURE_2D, currentTexture);
		//shader.set("enableTex", (bool)(currentTexture > 0));
	
		if (!shader.error())
		{

		//draw all primitive groups
		offset = 0;
		for (int i = 0; i < NUM_PRIMITIVE_TYPES; ++i)
		{
			//for each material in this primitive type
			if (indices[i].size())
			{
				//draw
				glDrawElements(primEnums[i], indices[i].size()-1, GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned int)));
				offset += indices[i].size();
			}
		}

		}

		//glBindTexture(GL_TEXTURE_2D, 0);
	
		glDisable(GL_PRIMITIVE_RESTART);
		bufferIndices.unbind();
	
		shader.unuse();
	}
}
void Immediate::clear()
{
	for (int i = 0; i < NUM_PRIMITIVE_TYPES; ++i)
		indices[i].clear();
	verts.clear();
	verts.push_back(Vert()); //first element is primitve restart
}
void Immediate::flush()
{
	draw();
	clear();
}
