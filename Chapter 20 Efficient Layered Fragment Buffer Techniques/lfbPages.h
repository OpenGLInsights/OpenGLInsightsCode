/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef LFB_PAGES_H
#define LFB_PAGES_H

#include "lfbLinked.h"

class ImageBuffer;

class PagesLFB : public LinkedLFB
{
protected:
	int pageSize;
	
	//semaphoers are used to stop two shaders allocating the same page
	//in addition, they force other shaders to wait until one shader has finished allocating a page
	//currently shaders do not know whether their page has been allocated until they receive a semaphore lock
	//this could be accomplished if a page index was stored with each page, however the basic linked list method would still be faster
	ImageBuffer* semaphores;
	
	//counts give offsets within each page. after gaining a semaphore lock, the page index is count % pageSize.
	//if this index is zero, the current shader must allocate a new page.
	ImageBuffer* counts;
	
	virtual bool resize(vec2i dim);
	virtual bool resizePool(int allocs); //a hook to increase allocs to allocs * pageSize
	virtual void initBuffers(); //zeroes counts
public:
	PagesLFB();
	int getPageSize() {return pageSize;}
	virtual ~PagesLFB();
	virtual void setDefines(Shader& program);
	virtual bool setUniforms(Shader& program);
	//NOTE: PagesLFB->end() will generally overestimate
	//fragment count as the exact number is unknown
	virtual std::string getName();
};

#endif
