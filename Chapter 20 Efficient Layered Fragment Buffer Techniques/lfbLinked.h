/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef LFB_LINKED_H
#define LFB_LINKED_H

#define LFB_UNDERALLOCATE 0.5 //will reallocate when fragments falls below this amount allocated. must be < 1
#define LFB_OVERALLOCATE 1.2 //will allocate this amount of fragments when reallocating. must be >= 1

#include "lfb.h"

class ImageBuffer;

class LinkedLFB : public LFB
{
protected:
	ImageBuffer* alloc; //data for the atomic counter
	ImageBuffer* headPtrs; //per-pixel head pointers
	ImageBuffer* nextPtrs; //per-fragment next pointers
	ImageBuffer* data; //fragment data
	virtual bool resize(vec2i dim);
	virtual bool resizePool(int allocs); //resizes data/nextPtrs. useful virtual for linked pages
	virtual void initBuffers(); //zeroes buffers - is potentially called from count() as well as begin()
public:
	LinkedLFB();
	virtual ~LinkedLFB();
	virtual void setDefines(Shader& program);
	virtual bool setUniforms(Shader& program);
	virtual void begin();
	virtual bool count();
	virtual int end();
	virtual std::string getName();
};

#endif
