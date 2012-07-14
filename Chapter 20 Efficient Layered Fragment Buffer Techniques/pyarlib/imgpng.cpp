/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include "imgpng.h"

#include <assert.h>
#include <png.h>

#ifndef png_infopp_NULL
#define png_infopp_NULL (png_infopp)NULL
#endif
#ifndef png_voidp_NULL
#define png_voidp_NULL (png_infopp)NULL
#endif

#ifdef _WIN32
#pragma warning (disable:4996) //fopen deprecation etc
#endif

namespace QI
{

void checkLibpngVersion()
{
	static bool printedVersionError = false;
	if (strcmp(PNG_LIBPNG_VER_STRING, png_libpng_ver) != 0 && !printedVersionError)
	{
		printf("Warning: libpng dll mismatch. compiled with %s but dll is %s\n", PNG_LIBPNG_VER_STRING, png_libpng_ver);
		printedVersionError = true;
	}
}

void pngError(png_structp png_ptr, const char* msg)
{
	printf("PNG Error: %s\n", msg);
}
void pngWarning(png_structp png_ptr, const char* msg)
{
	printf("PNG Warning: %s\n", msg);
}

ImagePNG::ImagePNG() : Image()
{
}
ImagePNG::ImagePNG(std::string filename)
{
	if (!loadImage(filename))
		printf("Error loading %s\n", filename.c_str());
}
bool ImagePNG::loadImage(std::string filename)
{
	checkLibpngVersion();

	const char* file_name = filename.c_str();

	int bit_depth;
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	FILE *fp;

	if ((fp = fopen(file_name, "rb")) == NULL)
		return (false);

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		this, pngError, pngWarning);

	if (png_ptr == NULL)
	{
		fclose(fp);
		return (false);
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return (false);
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		fclose(fp);
		return (false);
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, png_voidp_NULL);
	/* At this point you have read the entire image */

	int nwidth = png_get_image_width(png_ptr, info_ptr);
	int nheight = png_get_image_height(png_ptr, info_ptr);
	int nchannels = png_get_channels(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	if (nwidth != width || nheight != height || nchannels != channels)
	{
		width = nwidth;
		height = nheight;
		channels = nchannels;
		unsigned char* imageData = new unsigned char[width*height*channels];
		data = RCByteArray(imageData);
	}

	assert(bit_depth == 8);

	png_bytepp row_pointers;
	row_pointers = png_get_rows(png_ptr, info_ptr);
	for (int i = 0; i < height; ++i)
		memcpy(data.get()+(height - i - 1)*width*channels, row_pointers[i], width*channels*sizeof(unsigned char));

	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	fclose(fp);
	return (true);
}
bool ImagePNG::saveImage(std::string filename)
{
	checkLibpngVersion();

	const char* file_name = filename.c_str();

	int bit_depth = 8;
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;

	if (width == 0 || height == 0 || data.get() == NULL)
	{
		printf("Error: Cannot save incomplete image\n");
		return 0;
	}

	volatile int color_type;
	switch (channels)
	{
	case 1: color_type = PNG_COLOR_TYPE_GRAY; break;
	case 2: color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
	case 3: color_type = PNG_COLOR_TYPE_RGB; break;
	case 4: color_type = PNG_COLOR_TYPE_RGB_ALPHA; break;
	default:
		printf("Error writing PNG: Invalid colour type with %i channels\n", channels);
		return false;
	}

	fp = fopen(file_name, "wb");
	if (fp == NULL)
		return (false);

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
		this, pngError, pngWarning);

	if (png_ptr == NULL)
	{
		fclose(fp);
		return (false);
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(fp);
		png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
		return (false);
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fclose(fp);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return (false);
	}
		
	png_set_IHDR(png_ptr, info_ptr, width, height,
		bit_depth, color_type, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_bytep* row_pointers = new png_bytep[height];
	for (int i=0; i<height; i++)
		row_pointers[i] = ((unsigned char*)data.get()) + width*channels*(height-i-1);
	png_set_rows(png_ptr, info_ptr, row_pointers);

	png_init_io(png_ptr, fp);

	#if 0
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, png_voidp_NULL);
	#else
	png_write_info(png_ptr, info_ptr); //if it crashes here, it's probably something to do with a pnglib binary mismatch
	png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, NULL);
	#endif

	png_destroy_write_struct(&png_ptr, &info_ptr);
	delete[] row_pointers;
	fclose(fp);
	return (true);
}

}
