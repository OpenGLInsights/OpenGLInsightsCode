/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include "material.h"
#include "shaderutil.h"

#include "img.h"

Material::Material()
{
	texColour = 0;
	texNormal = 0;
	texSpecular = 0;
	colour = vec4f(1.0f);
	imgColour = NULL;
	imgNormal = NULL;
	imgSpecular = NULL;
}
Material::~Material()
{
	if (texColour)
		glDeleteTextures(1, &texColour);
	if (texNormal)
		glDeleteTextures(1, &texNormal);
	if (texSpecular)
		glDeleteTextures(1, &texSpecular);
}
void Material::bind()
{
	if (imgColour)
	{
		texColour = imgColour->bufferTexture();
		delete imgColour;
		imgColour = NULL;
	}
	if (imgNormal)
	{
		texNormal = imgNormal->bufferTexture();
		delete imgNormal;
		imgNormal = NULL;
	}
	if (imgSpecular)
	{
		texSpecular = imgSpecular->bufferTexture();
		delete imgSpecular;
		imgSpecular = NULL;
	}

	int i = 0;
	if (texColour) setActiveTexture(i++, "texColour", texColour);
	if (texNormal) setActiveTexture(i++, "texNormal", texNormal);
	if (texSpecular) setActiveTexture(i++, "texSpecular", texSpecular);
	
	GLint program = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	GLuint locColour = glGetUniformLocation(program, "colourIn");
	GLuint locTextured = glGetUniformLocation(program, "textured");
	if (locColour)
		glUniform4f(locColour, colour.x, colour.y, colour.z, colour.w);
	if (locTextured)
		glUniform1i(locTextured, i);
}
void Material::unbind()
{
	GLint program = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	GLuint locTextured = glGetUniformLocation(program, "textured");
	if (locTextured)
		glUniform1i(locTextured, 0);
}
