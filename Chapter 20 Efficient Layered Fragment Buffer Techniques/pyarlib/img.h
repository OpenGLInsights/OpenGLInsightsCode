/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef QI_IMG_H
#define QI_IMG_H

#include <string>
#ifdef _WIN32
#include <memory>
#else
#include <tr1/memory>
#endif

namespace QI
{
	struct RCByteArray : public std::tr1::shared_ptr<unsigned char>
	{
		RCByteArray() : std::tr1::shared_ptr<unsigned char>() {}
		RCByteArray(unsigned char*& o) : std::tr1::shared_ptr<unsigned char>(o) {}
		operator unsigned char*() {return get();}
	};
	
	struct Image
	{
		bool repeat;
		bool mipmap;
		int nearest;
		int anisotropy;
		int channels;
		int width, height;
		RCByteArray data;
		Image();
		virtual ~Image();
		virtual bool loadImage(std::string filename);
		virtual bool saveImage(std::string filename);
		virtual void generateNoise() {}; //TODO
		unsigned int bufferTexture(); //returns new GL texture
		void bufferTexture(unsigned int object, unsigned long target = -1); //buffers to object
		void readTexture(unsigned int id);
	};
}

#endif
