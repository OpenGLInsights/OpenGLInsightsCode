/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef IMMEDIATE_H
#define IMMEDIATE_H

#include "vec.h"
#include "gpu.h"
#include "matstack.h"

//a quick and dirty class to do immediate mode rendering using GL >3.2
//If speed is an issue, do NOT use this class
//designed for almost a single instance, with push/pop, begin/end
//bottleneck is buffering data to the GPU
class Immediate : public MatrixStack
{
public:
	enum Primitive {
		POINTS,
		
		LINES,
		LINE_LOOP,
		LINE_STRIP,
		
		POLYGON,
		TRIANGLES,
		TRIANGLE_STRIP,
		TRIANGLE_FAN,
		
		QUADS,
		QUAD_STRIP,
		
		NUM_PRIMITIVE_TYPES
	};
private:
	struct Vert
	{
		vec3f pos;
		//vec2f tex;
		unsigned int col;
	};
	typedef std::vector<unsigned int> Group;
	std::vector<Group> indices;
	std::vector<Vert> verts; //vertex data
	std::vector<unsigned int> primEnums; //opengl enums
	
	//state stuff
	vec4f currentCol;
	vec2f currentCoord;
	Primitive currentType;
	VertexBuffer bufferVertices;
	IndexBuffer bufferIndices;
	unsigned int currentTexture;
	bool begun;
	bool pretransform;
public:
	Immediate();
	mat44 projection;
	void enablePretransform(bool enable);
	void useTexture(unsigned int handle = 0);
	void begin(Primitive type);
	void colour(vec3f c);
	void colour(vec4f c);
	void colour(float r, float g, float b);
	void colour(float r, float g, float b, float a);
	void vertex(vec2f p);
	void vertex(vec3f p);
	void texture(float u, float v, float w) {} //NOT IMPLEMENTED
	void texture(float u, float v);
	void texture(vec2f c);
	void vertex(float x, float y);
	void vertex(float x, float y, float z);
	void end(bool flushStraightAway = true);
	void draw();
	void clear();
	void flush(); //simply draw() then clear()
};

#endif 

