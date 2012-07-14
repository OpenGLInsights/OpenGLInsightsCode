/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>

#include <map>
#include <vector>
#include <string>
#include <set>

#include "shader.h"
#include "util.h"
#include "fileutil.h"

using namespace std;

Shader* Shader::active = NULL;

std::set<Shader*>* Shader::instances = NULL;
std::map<std::string, const char*> Shader::includeOverrides;

static GLuint lastLoc = -1;

//boring code. too long to expand
template <> bool Shader::set<vec2f>(const std::string& name, const vec2f& t) {preSet(name); glUniform2f(getLoc(name), t.x, t.y); return checkSet(name);}
template <> bool Shader::set<vec3f>(const std::string& name, const vec3f& t) {preSet(name); glUniform3f(getLoc(name), t.x, t.y, t.z); return checkSet(name);}
template <> bool Shader::set<vec4f>(const std::string& name, const vec4f& t) {preSet(name); glUniform4f(getLoc(name), t.x, t.y, t.z, t.w); return checkSet(name);}
template <> bool Shader::set<vec2i>(const std::string& name, const vec2i& t) {preSet(name); glUniform2i(getLoc(name), t.x, t.y); return checkSet(name);}
template <> bool Shader::set<vec3i>(const std::string& name, const vec3i& t) {preSet(name); glUniform3i(getLoc(name), t.x, t.y, t.z); return checkSet(name);}
template <> bool Shader::set<vec4i>(const std::string& name, const vec4i& t) {preSet(name); glUniform4i(getLoc(name), t.x, t.y, t.z, t.w); return checkSet(name);}
template <> bool Shader::set<int>(const std::string& name, const int& t) {preSet(name); glUniform1i(getLoc(name), t); return checkSet(name);}
template <> bool Shader::set<unsigned int>(const std::string& name, const unsigned int& t) {preSet(name); glUniform1i(getLoc(name), t); return checkSet(name);}
template <> bool Shader::set<float>(const std::string& name, const float& t) {preSet(name); glUniform1f(getLoc(name), t); return checkSet(name);}
template <> bool Shader::set<bool>(const std::string& name, const bool& t) {preSet(name); glUniform1i(getLoc(name), (int)t); return checkSet(name);}
template <> bool Shader::set<mat44>(const std::string& name, const mat44& t) {preSet(name); glUniformMatrix4fv(getLoc(name), 1, GL_FALSE, t.m); return checkSet(name);}
template <> bool Shader::set<mat33>(const std::string& name, const mat33& t) {preSet(name); glUniformMatrix3fv(getLoc(name), 1, GL_FALSE, t.m); return checkSet(name);}

bool _checkGLErrors(const char* file, int line, bool silent)
{
	bool ok = true;
    GLenum error, last;
	
	//get all errors
	error = glGetError();
    while (error != GL_NO_ERROR)
    {
		const char* p = file;
		while (*p != '\\' && *p != '\0') ++p;
		if (*p == '\\') file = p+1; //print only filename past the last "\"
		
		//print error
    	ok = false;
    	if (!silent)
			printf("glError 0x%x caught in %s at %i: %s\n", error, file, line, gluErrorString(error));
		
		//to stop an infinite loop
		last = error;
		error = glGetError();
		if (error == last)
		{
			printf("GL error in error function. Have you initialized opengl?\n");
			return false;
		}
    }
    return !ok;
}

bool _checkEnabledAttribs(const char* file, int line)
{
	bool found = false;
	int maxAttribs;
	//check if any attributes are enabled (eg glEnableVertexAttrib)
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);
	for (int i = 0; i < maxAttribs; ++i)
	{
		int enabled;
		glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
		if (enabled)
		{
			printf("ERROR in %s:%i: Vertex attrib %i has not been disabled somewhere.\n", file, line, i);
			found = true;
		}
	}
	
	//check if any client states are enabled (eg glEnableClientState)
	GLenum arrays[] = {
		GL_VERTEX_ARRAY,
		GL_COLOR_ARRAY,
		GL_TEXTURE_COORD_ARRAY,
		GL_SECONDARY_COLOR_ARRAY,
		GL_NORMAL_ARRAY,
		GL_INDEX_ARRAY,
		GL_EDGE_FLAG_ARRAY,
		GL_FOG_COORD_ARRAY
		};
	CHECKERROR;
	for (int i = 0; i < (int)(sizeof(arrays)/sizeof(GLenum)); ++i)
	{
		int enabled;
		printf("%i %i\n", i, arrays[i]);
		glGetIntegerv(arrays[i], &enabled);
		CHECKERROR;
		if (enabled)
		{
			printf("ERROR in %s:%i: Array attrib %i has not been disabled somewhere.\n", file, line, arrays[i]);
			found = true;
		}
	}
	CHECKERROR;
	return found;
}

Shader::Shader(string filename)
{
	pname = filename;
	vert = pname + ".vert";
	frag = pname + ".frag";
	geom = pname + ".geom";
	
	init();
}

Shader::Shader(string vert, string frag, string geom)
{
	int i = frag.rfind(".");
	if (i > 0)
		pname = frag.substr(0, i);
	else
		pname = frag;
	this->vert = vert;
	this->frag = frag;
	this->geom = geom;
	
	init();
}

Shader::~Shader()
{
	release();
	instances->erase(this);
}

void Shader::init()
{
	compileError = false;
	program = 0;
	dirty = true;
	
	if (!instances)
		instances = new std::set<Shader*>();
	instances->insert(this);
}

GLuint Shader::getLoc(const std::string& name)
{
	GLuint loc = glGetUniformLocation(program, name.c_str());
	if (loc == (GLuint)-1)
		variableError.insert(name);
	lastLoc = loc;
	return loc;
}

void Shader::preSet(const std::string& name)
{
	if (CHECKERROR) //error happened elsewhere
		printf("\tCaught while setting uniform %s for %s\n", name.c_str(), pname.c_str());
}

bool Shader::checkSet(const std::string& name)
{
	if (CHECKERROR_SILENT && !error())
	{
		printf("Error setting uniform. Is %s bound and %s the right type?\n", pname.c_str(), name.c_str());
		return false;
	}
	return lastLoc != (GLuint)-1;
}

const std::string& Shader::name() const
{
	return pname;
}

bool Shader::error()
{
	return compileError;
}

bool Shader::reload()
{
	//check the separate files exist
	if (!fileExists(vert.c_str()) && includeOverrides.find(vert) == includeOverrides.end())
		vert.clear();
	if (!fileExists(frag.c_str()) && includeOverrides.find(frag) == includeOverrides.end())
		frag.clear();
	if (!fileExists(geom.c_str()) && includeOverrides.find(geom) == includeOverrides.end())
		geom.clear();
		
	//cout << "Shader: " << pname << " = " << vert << " - " << frag << " - " << geom << endl;
	
	//sometimes it happens...
	if (vert.size() + frag.size() + geom.size() == 0)
	{
		compileError = true;
		printf("Error: No shaders found for \"%s\"\n", pname.c_str());
		return false;
	}
		
	//free current program if there is one
	release();

	//compile
	errorStr.clear();
	bool ok = true;
	ShaderBuild build;
	for (std::map<std::string, const char*>::iterator it = includeOverrides.begin(); it != includeOverrides.end(); ++it)
	{
		//add all overrides to the builder
		build.include(it->first, it->second);
	}
	ok = ok && build.compile(vert, GL_VERTEX_SHADER, &defines, &errorStr);
	ok = ok && build.compile(frag, GL_FRAGMENT_SHADER, &defines, &errorStr);
	ok = ok && build.compile(geom, GL_GEOMETRY_SHADER, &defines, &errorStr);
	
	//link, if compiled
	if (ok)
	{
		program = build.link(&errorStr);
		ok = ok && (program > 0);
	}
	
	if (!ok)
	{
		std::cout << errorStr << endl;

		ofstream errlog("shaders.log", ios::app);
		errlog.write(errorStr.c_str(), errorStr.size());
		errlog.close();
	}
		
	//always cleanup
	build.cleanup();
	
	//return pro
	compileError = (program == 0);
	return !compileError;
}

bool Shader::use()
{
	assert(attribs.size() == 0); //unuse() must be called, in order, after use()
	assert(!active); //must unuse() previous shader before use()
	
	if (dirty)
	{
		if (!compileError)
		{
			reload();
			if (error())
				return false;
			else
				dirty = false;
		}
		else
			return false; //already failed once, not going to try again
	}

	active = this;
	glUseProgram(program);
	return true;
}

void Shader::attrib(std::string name, GLuint buffer, GLenum type, int sizeOfElementInBytes, int stride, int offset)
{
	if (error())
		return;

	GLuint location = glGetAttribLocation(program, name.c_str());
	if (location == (GLuint)-1)
	{
		printf("Warning: no attribute %s in %s\n", name.c_str(), pname.c_str());
		variableError.insert(name);
		return;
	}

	int bpa = 0;
	switch (type)
	{
	case GL_BYTE:
	case GL_UNSIGNED_BYTE:
		bpa = 1;
		break;
	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
		bpa = 2;
		break;
	case GL_INT:
	case GL_UNSIGNED_INT:
	case GL_FLOAT:
		bpa = 4;
		break;
	case GL_DOUBLE:
		bpa = 8;
		break;
	default:
		printf("Error: invalid TYPE for %s in %s\n", name.c_str(), pname.c_str());
		return;
	}
	
	assert((sizeOfElementInBytes % bpa) == 0); //size must be a multiple of sizeof(TYPE)
	
	attribs.insert(location);
	glEnableVertexAttribArray(location);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(location, sizeOfElementInBytes / bpa, type, GL_FALSE, stride, (GLvoid*)(intptr_t)offset);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool Shader::define(std::string name, std::string value)
{
	if (defines.find(name) != defines.end())
		if (defines[name] == value)
			return false; //no change, no need to dirty

	dirty = true;
	defines[name] = value;
	return true;
}

bool Shader::define(std::string name, int value)
{
	return define(name, intToString(value));
}

void Shader::undef(std::string name)
{
	dirty = true;
	ShaderBuild::Defines::iterator f = defines.find(name);
	if (f != defines.end())
		defines.erase(f);
}

void Shader::undefAll()
{
	defines.clear();
}

int Shader::unique(std::string type, std::string name)
{
	std::map<std::string, UniqueType>::iterator it;
	it = uniques.find(type);
	if (it == uniques.end())
	{
		UniqueType newtype;
		newtype.next = 0;
		uniques[type] = newtype;
		it = uniques.find(type);
	}
	std::map<std::string, int>::iterator mapped;
	mapped = it->second.names.find(name);
	if (mapped == it->second.names.end())
	{
		it->second.names[name] = it->second.next++;
	}
	return it->second.names[name];
}

void Shader::include(std::string filename, const char* srcData)
{
	includeOverrides[filename] = srcData;
}

void Shader::unuse()
{
	for (std::set<GLuint>::iterator iter = attribs.begin(); iter != attribs.end(); ++iter)
	{
		glDisableVertexAttribArray(*iter);
	}
	attribs.clear();
	glUseProgram(0);
	active = NULL;
}

void Shader::release()
{
	//CHECKERROR;
	if (program > 0)
	{
		//printf("Releasing shader %i\n", program);
		glDeleteProgram(program);
	}
	init();
	//CHECKERROR;
}

void Shader::releaseAll()
{
	//printf("Releasing shaders\n");
	if (instances)
		for (std::set<Shader*>::iterator it = instances->begin(); it != instances->end(); ++it)
			(*it)->release();
	//printf("Done Releasing shaders\n");
}
