/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>

#include "vec.h"
#include "matrix.h"

#include "includegl.h"

#include "shaderbuild.h"

#ifdef CHECKERROR
#undef CHECKERROR
#endif
#define CHECKERROR _checkGLErrors(__FILE__, __LINE__, false)
#define CHECKERROR_SILENT _checkGLErrors(__FILE__, __LINE__, true)

#define CHECKATTRIBS _checkEnabledAttribs(__FILE__, __LINE__)

bool _checkGLErrors(const char* file, int line, bool silent = false);
bool _checkEnabledAttribs(const char* file, int line);

class Shader;

//To allow additional uniform types, extend from this struct.
//This way, myshader.set("uniformName", myclass) can be used for a custom type
//return false on an error to report it
struct ShaderUniform
{
protected:
	friend class Shader;
	virtual bool setUniform(Shader* program, const std::string& name) const =0;
};


//the main shader class.
//handles loading and compiling shaders from source
//also has the "set" interface for setting uniform variables
//shaders are not cached
class Shader
{
private:
	struct UniqueType
	{
		int next;
		std::map<std::string, int> names;
	};
	std::string pname; //combined shader program name
	std::string vert;
	std::string frag;
	std::string geom;
	std::set<GLuint> attribs;
	static std::set<Shader*>* instances;
	static std::map<std::string, const char*> includeOverrides;
	std::map<std::string, UniqueType> uniques;
	ShaderBuild::Defines defines;
	GLuint program;
	bool dirty; //need to reload() before use()?
	bool compileError;
	inline GLuint getLoc(const std::string& name);
	void preSet(const std::string& name);
	bool checkSet(const std::string& name);
	void init(); //called from constructor
	
	void operator=(const Shader& other) {}; //private - NO COPYING!
public:
	static Shader* active;

	Shader(std::string filename); //constructs shader searching for filename.frag, filename.vert etc
	Shader(std::string vert, std::string frag, std::string geom = ""); //explicit files
	~Shader();
	const std::string& name() const;
	
	std::string errorStr; //contains concatenated errors from compiling shaders, if there were any
	std::set<std::string> variableError; //uniforms/attributes with invalid locations
	bool error(); //returns compileError
	
	//recompile the shader. called automatically on first use() or use() following define/undef
	bool reload();
	
	//IMPORTANT: call unuse on THIS object as attrib and other cleanups are done
	bool use();
	void unuse();
	
	//NOTE: size is in BYTES, just like stride and offset
	void attrib(std::string name, GLuint buffer, GLenum type, int sizeOfElementInBytes, int stride = 0, int offset = 0);
	
	//NOTE: modifying defines requires recompile so don't use once a frame!!! returns true on change
	bool define(std::string name, std::string value);
	bool define(std::string name, int value);
	void undef(std::string name);
	void undefAll();
	
	//textures and image units require attach points. this is a helper function to get a next available, unused index
	int unique(std::string type, std::string name);
	
	//files that would normally be read from disk can be overridden with .include(<filename to override>, <file data>)
	//NOTE: do not free srcData before the shader is compiled (or recompiled, possibly triggered by .define())
	static void include(std::string filename, const char* srcData);
	
	//set a variable name. this clas has vec and mat overrides. for others, use the ShaderUniform interface
	//bool set(const std::string& name, const ShaderUniform*& t);
	template <typename T> bool set(const std::string& name, const T& t);
	
	//this class can simply be treated as the opengl shader object
	operator const GLuint&() {return program;}
	
	void release();
	static void releaseAll(); //calls .release() on all instances
};

//template <typename T> bool Shader::set(const std::string& name, const T& t)

template <> bool Shader::set<vec2f>(const std::string& name, const vec2f& t);
template <> bool Shader::set<vec3f>(const std::string& name, const vec3f& t);
template <> bool Shader::set<vec4f>(const std::string& name, const vec4f& t);
template <> bool Shader::set<vec2i>(const std::string& name, const vec2i& t);
template <> bool Shader::set<vec3i>(const std::string& name, const vec3i& t);
template <> bool Shader::set<vec4i>(const std::string& name, const vec4i& t);
template <> bool Shader::set<int>(const std::string& name, const int& t);
template <> bool Shader::set<unsigned int>(const std::string& name, const unsigned int& t);
template <> bool Shader::set<float>(const std::string& name, const float& t);
template <> bool Shader::set<bool>(const std::string& name, const bool& t);
template <> bool Shader::set<mat44>(const std::string& name, const mat44& t);
template <> bool Shader::set<mat33>(const std::string& name, const mat33& t);
//template <> bool Shader::set<ShaderUniform>(const std::string& name, ShaderUniform const & t);

template <typename T> 
bool Shader::set(const std::string& name, const T& t)
{
	preSet(name);

	//intended for ShaderUniform children
	if (dynamic_cast<const ShaderUniform*>(&t)->setUniform(this, name))
	{
		if (checkSet(name))
			return true;
	}
	else
	{
		printf("Error setting uniform %s in %s\n", name.c_str(), pname.c_str());
	}
	return false;
}

#endif
