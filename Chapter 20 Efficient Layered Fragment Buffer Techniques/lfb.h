/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef LFB_H
#define LFB_H

#include "pyarlib/vec.h"
#include "pyarlib/matrix.h"
#include "pyarlib/profile.h"
#include "pyarlib/shader.h"

#include <map>
#include <string>

//the ImageBuffer contains a buffer object mapped to a texture buffer.
//image buffers hold all LFB data and from experience are a little faster than texture image units
class ImageBuffer;

class LFB
{
private:

	//disable copying - destructor frees GL memory
	LFB(const LFB& copy) {}
	void operator=(const LFB& copy) {}
	
	//zeroed data to quickly zero buffers. this is resized to the maximum needed
	static ImageBuffer* zeroes;
	
protected:

	//states change with begin() count() and end() calls
	enum State {
		PRE_INIT, //begin hasn't been called
		FIRST_PASS, //between begin and count
		SECOND_PASS, //between count and end
		SORTING, //during end
		DRAWING, //after end (or before next begin call)
	};
	
	State state;
	
	int maxFrags; //max depth complexity
	
	int lfbDataType; //GL data type, eg GL_RGBA32F
	int lfbDataStride; //bytes per fragment

	//this is the 2D size of the LFB although the data is really 1D.
	//lfb.glsl has overloads for adding 1D fragment, even from a vertex shader
	vec2i size;
	
	int totalPixels; //size.x * size.y
	int totalFragments; //is not always accurate but will never be less than exact
	int allocFragments; //data allocated, for some implementations will allocate more to reduce allocation frequency
	
	virtual bool resize(vec2i dim); //resize buffers such as counts and head pointers
	void zeroBuffer(ImageBuffer* buffer); //coppies zeros to buffer
	
public:

	std::map<std::string, int> memory; //info only
	int getTotalPixels();

	//for debugging - to time algorithm steps. point this to an instance
	Profile* profile;

	LFB();
	virtual ~LFB();
	void setMaxFrags(int n); //preferably change this once as altering maxFrags will cause a recompile for the #define
	virtual void setDefines(Shader& program); //call before shader.use(). unless defines change, shader will not recompile
	virtual bool setUniforms(Shader& program) =0; //return false if out-of-memory or other error
	virtual void begin();
	virtual bool count(); //return true to render second pass
	virtual int end();
	virtual void sort(bool inMainVideoMem = false); //not implemented! useful for reading from sorted LFB many times
	virtual std::string getName() =0; //just for debugging
	std::string getMemoryInfo();
};

#endif
