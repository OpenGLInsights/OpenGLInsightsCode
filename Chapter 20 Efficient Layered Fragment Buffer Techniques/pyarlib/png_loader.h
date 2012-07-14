/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

/* png_loader.h pknowles 20/03/09 v1.0 */

/*
Compile:
	Include png_loader.h
	Add png_loader.c and deflate.c to the makefile
Usage:
	Use load_png(filename) to load a png file.
		The png file must be RGB or RGBA. Palette and Interlacing are not supported
	The returned Image structure contains width, height in pixels and channels -
		3 for RGB and 4 for RGBA. The image data contains a pixel array stored 
		sequencially in Image.data starting from top-left reading left to right
		with 8 bits per channel.
	Use free_image to release the memory allocated to the Image structure by load_png.
Notes:
	This loader does not yet support:
		- Palette
		- Interlacing
		- Greyscale
		- Any PNG Ancillary chunks
*/


#ifndef PNG_LOADER_H
#define PNG_LOADER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Public Interface */
typedef struct Image
{
	unsigned int width;
	unsigned int height;
	unsigned int channels;
	unsigned char* data;
} Image;

Image* load_png(const char* filename); /* loads and returns a PNG image in the Image data structure. NULL on error */
void free_image(Image* img); /* frees an Image data structure */

/* Private Functions */
unsigned int fread_uint(FILE* file); /* reads a big endian unsigned int */
unsigned int endian_swap(unsigned int x); /* swaps byte order in integer */
int str_compare(const char* a, const char* b); /* compares string a and b. returns 0 if match, +/- 1 to indicate order */
int paeth(int a, int b, int c); /* performs the paeth PNG filter from pixels values a, b and c */

#endif
