/*
 * tga.cpp
 *
 *  Created on: Jan 26, 2010
 *      Author: aqnuep
 *
 *  Note: Do not use this module in any product as it is a very rough
 *        TGA image loader made only for the demo program and is far
 *        from product quality
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include "tga.h"

using namespace std;

namespace demo {

TGAImage::TGAImage(const char* filename) {

	data = NULL;

	ifstream file(filename, ios::in | ios::binary);
	if (!file) {
		cerr << "Unable to open file: " << filename << endl;
		return;
	}

	TGAHeader header;
	file.read((char*)&header, sizeof(header));

	if ((header.ImageType != 2) or (header.CMapType != 0)) {
		cerr << "Invalid file format: " << filename << endl;
		return;
	}

	if ((header.Depth != 32) and (header.Depth != 24)) {
		cerr << "Unsupported color depth: " << (int)header.Depth << " (" << filename << ")" << endl;
		return;
	}

	width = header.Width;
	height = header.Height;
	depth = header.Depth;

	int dataSize = (width * height * depth) / 8;
	data = malloc(dataSize);

	file.seekg(header.IDLength, ios_base::cur);
	file.read((char*)(data), dataSize);

}

TGAImage::~TGAImage() {
	if (data != NULL) free(data);
}

void TGAImage::makeSeamless() {
	unsigned char *image = (unsigned char*)data;
	unsigned char a[4], b[4];
	char Bpp = depth / 8;

	for (int i=0; i<width; i++) {
		memcpy(a, &image[i*Bpp], Bpp);
		memcpy(b, &image[(i+(height-1)*width)*Bpp], Bpp);
		for (int j=0; j<Bpp; j++)
			a[j] = ((int)a[j] + (int)b[j]) / 2;
		memcpy(&image[i*Bpp], a, Bpp);
		memcpy(&image[(i+(height-1)*width)*Bpp], a, Bpp);
	}

	for (int i=0; i<height; i++) {
		memcpy(a, &image[i*width*Bpp], Bpp);
		memcpy(b, &image[(i*width+width-1)*Bpp], Bpp);
		for (int j=0; j<Bpp; j++)
			a[j] = ((int)a[j] + (int)b[j]) / 2;
		memcpy(&image[i*width*Bpp], a, Bpp);
		memcpy(&image[(i*width+width-1)*Bpp], a, Bpp);
	}
}

} /* namespace demo */
