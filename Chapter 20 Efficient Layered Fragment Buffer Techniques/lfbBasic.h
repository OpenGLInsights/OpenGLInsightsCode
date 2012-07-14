/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef LFB_BASIC_H
#define LFB_BASIC_H

#include "lfb.h"

#define LFB_DEFAULT_NUMLAYERS 16

class ImageBuffer;

class BasicLFB : public LFB
{
protected:
	//a query is used to count the total fragments as this is not provided with the basic LFB
	bool waitingForQuery;
	unsigned int query;
	
	//the Z dimension of the 3D array.
	//maxFrags is simply the size of the shader-local array and is not related to numLayers
	int numLayers;
	
	ImageBuffer* counts;
	ImageBuffer* data;
	
	virtual bool resize(vec2i dim);
public:
	BasicLFB();
	virtual ~BasicLFB();
	virtual void setDefines(Shader& program);
	virtual bool setUniforms(Shader& program);
	virtual void begin();
	virtual bool count();
	
	//NOTE: fragment count may be delayed as a query is used
	//(possibly a few frames out of date)
	virtual int end();
	virtual std::string getName();
	
	//attempts to change the allocated number of layers in the 3D array. reduced on memory error
	void setNumLayers(int n);
};
#endif
