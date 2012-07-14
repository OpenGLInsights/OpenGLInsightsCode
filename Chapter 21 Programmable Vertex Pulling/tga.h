/*
 * tga.h
 *
 *  Created on: Jan 26, 2010
 *      Author: aqnuep
 *
 *  Note: Do not use this module in any product as it is a very rough
 *        TGA image loader made only for the demo program and is far
 *        from product quality
 */

#ifndef TGA_H_
#define TGA_H_

#include <GL/gl.h>

namespace demo {

struct TGAHeader {
	GLbyte IDLength;
	GLbyte CMapType;
	GLbyte ImageType;
	GLbyte dontCare[5];
	GLushort Xorg;
	GLushort Yorg;
	GLushort Width;
	GLushort Height;
	GLbyte Depth;
	GLbyte Desc;
};

class TGAImage {
public:
	TGAImage(const char* filename);
	~TGAImage();

	void makeSeamless();

	int width;
	int height;
	int depth;

	void* data;
};

} /* namespace demo */

#endif /* TGA_H_ */
