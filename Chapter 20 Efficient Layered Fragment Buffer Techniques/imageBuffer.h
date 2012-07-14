/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef IMAGE_BUFFER_H
#define IMAGE_BUFFER_H

//this class is really a buffer object handle with some utility functions.
//textureObject is bound to the real data in bufferObject
//and can be attached to a shader with bind().

class ImageBuffer
{
private:
	int currentSize;
	unsigned int format;
	unsigned int bufferObject;
	unsigned int textureObject;
	
	//disable copying - destructor frees GL memory
	ImageBuffer(const ImageBuffer& copy) {}
	void operator=(const ImageBuffer& copy) {}
	
	void release();
	void create(int bytes);
	bool allocate(int bytes);
public:
	bool ready; //set if the buffer has been allocated and no errors occured
	
	ImageBuffer(unsigned int format);
	~ImageBuffer();
	bool resize(int size); //size in bytes
	bool bind(int index, const char* name, unsigned int program, bool read = true, bool write = true);
	static void unbindAll();
	void* map(bool read, bool write);
	bool unmap();
	void copy(ImageBuffer& dest, int offsetFrom = 0, int offsetTo = 0, int size = -1);
	
	int size() {return currentSize;}
	unsigned int getFormat() {return format;}
	
	//class itself can be used as the buffer object handle
	operator unsigned int() {return bufferObject;}
	unsigned int getTexture() {return textureObject;}
};

#endif
