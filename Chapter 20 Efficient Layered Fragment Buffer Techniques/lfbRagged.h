/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef LFB_RAGGED_H
#define LFB_RAGGED_H

#include "lfb.h"

class ImageBuffer;

class RaggedLFB : public LFB
{
protected:
	int prefixSumsSize; //next power of two of totalPixels
	int prefixSumsHeight; //for the blend texture
	
	ImageBuffer* offsets;
	ImageBuffer* data;
	ImageBuffer* ids;
	
	virtual bool resize(vec2i dim);
	
	//blending requires swapping the current FBO in the first pass
	int backupFBOHandle;
	void backupFBO();
	void restoreFBO();
	
	void initBlending();
	void copyBlendResult();
	
	bool countUsingBlending;
	bool globalSort;
	
	unsigned int blendFBO;
	unsigned int blendTex;
public:
	RaggedLFB();
	void useBlending(bool enable);
	void useGlobalSort(bool enable);
	virtual ~RaggedLFB();
	virtual void setDefines(Shader& program);
	virtual bool setUniforms(Shader& program);
	virtual void begin();
	virtual bool count();
	virtual int end();
	virtual std::string getName();
};

#endif

