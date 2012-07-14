/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef SHADER_BUILD_H
#define SHADER_BUILD_H

struct ShaderBuild
{
	//files are broken into "bits" based on #includes.
	//eg <bit before #include><the #included file><bit after #include>
	//this way, a file never needs to be loaded twice and a merged file can be
	//passed to glShaderSource in separate bits. see ::compile()
	typedef std::vector<int> FileBits;
	std::map<std::string, FileBits> parseCache; //so files aren't loaded twice
	
	//this contains the data for a file bit, including the source file name
	//the start line number is included for a #line command which GLSL reads.
	//This allows error messages to be parsed so the correct source file, line etc can be printed
	struct FileBit
	{
		std::vector<std::string> path; //include tree path
		std::string filename;
		int startLine;
		std::string bit;
	};
	
	//this holds references to the file bits needed to reconstruct
	//the shader with the #include bits. ::object is the compiled shader
	struct ShaderSource
	{
		FileBits source; //indices to FileBits
		std::string name;
		GLuint object;
	};
	
	//map of #define values to substitute
	typedef std::map<std::string, std::string> Defines;
	Defines defines;

	//map of objects, eg GL_VERTEX_SHADER = <loaded/compiled shader>
	typedef std::map<GLenum, ShaderSource> ObjectMap;
	ObjectMap objects;
	
	//this is the list of actual filebits
	std::vector<FileBit> allfilebits;
	
	//used to print source code when errors are received
	typedef std::vector<std::string> Lines;
	std::map<std::string, Lines> allfiles;
	
	std::map<std::string, const char*> includeOverrides;
	
	//the final compiled program. returned by ::link
	GLuint program;
	
	//returns a line from a source file, if available
	bool getFileLine(std::string file, int line, std::string& source);
	
	//extracts file index and line number from log if possible. if errorStr is NULL, prints to stdout
	void parseLog(std::string* errorStr, std::string log, int baseFile, bool readLineNumbers);
	
	//parses a shader source file, recursively loading #includes and replacing #defines
	bool parse(std::string file, FileBits& sourceBits, Defines& defs, std::vector<std::string> path = std::vector<std::string>());
	
	//files that would normally be read from disk can be overridden with .include(<filename to override>, <file data>)
	//NOTE: the data pointer must remain valid until all compile() calls complete
	void include(std::string filename, const char* data);
	
	//if defs is not passed to ::compile(), the local ::defines is used instead. ::define() adds to the local ::defines
	void define(std::string def, std::string val);
	
	//compiles a shader, returning true on success
	bool compile(std::string file, GLenum type, Defines* defs = NULL, std::string* errStr = NULL);
	
	//links all compiled shaders and returns the program.
	GLuint link(std::string* errStr = NULL);
	
	//cleans up all variables, deletes shader objects etc.
	void cleanup();
};

#endif
