/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#ifndef PYARLIB_MATERIAL_H
#define PYARLIB_MATERIAL_H

#include "vbomesh.h"
#include "vec.h"

namespace QI {
	struct Image;
};

//TODO: use Shader::uniq to bind textures without conflicts

struct Material : VBOMeshMaterial
{
	GLuint texColour;
	GLuint texNormal;
	GLuint texSpecular;
	vec4f colour;
	
	//textures will be uploaded on first bind()
	QI::Image* imgColour;
	QI::Image* imgNormal;
	QI::Image* imgSpecular;
	
	Material();
	~Material();
	virtual void bind();
	virtual void unbind();
	
private:
	//no copying!
	Material(const Material& other) {}
	void operator=(const Material& other) {}
};

#endif
