/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include "png_loader.h"
#include <zlib.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef WIN32
#pragma warning(disable: 4996) /* disable deprication warnings */
#endif

#define PNG_HEADER "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A"
#define DECOMPRESS_CHUNK_SIZE (1 << 21)

#ifndef ABS
#define ABS(x) ((x)<0?(-(x)):(x))
#endif

unsigned int endian_swap(unsigned int x)
{
	/* reverses byte order */
	return (x>>24) | ((x<<8) & 0x00FF0000) | ((x>>8) & 0x0000FF00) | (x<<24);
}

unsigned int fread_uint(FILE* file)
{
	unsigned char tmp[4];
	fread(tmp, 1, 4, file);
	return ((unsigned int)tmp[0] << 24)
		| ((unsigned int)tmp[1] << 16)
		| ((unsigned int)tmp[2] << 8)
		| (unsigned int)tmp[3];
}

int str_compare(const char* a, const char* b)
{
	/* compares string a and b. returns 0 if match, +/- 1 to indicate order */
	do
	{
		if (*a != *b)
			return *a > *b? 1 : -1;
		a++;
		b++;
	} while (*a != '\0' && *b != '\0');
	return 0;
}

int paeth(int a, int b, int c)
{
	/*
	performs the paeth PNG filter from pixels values:
		a = back
		b = up
		c = up and back
	*/
	int p = a + b - c;
	int pa = ABS(p - a);
	int pb = ABS(p - b);
	int pc = ABS(p - c);
	if (pa <= pb && pa <= pc) return a;
	else if (pb <= pc) return b;
	else return c;
}

Image* load_png(const char* filename)
{
	/* loads and returns a PNG image in the Image data structure */
	int ret = 0;
	int outLen;
	unsigned char* outData;
	Image* image;
	int reading = 1; /* indicates whether the IEND chunk has been reached */
	unsigned int i, j, k;
	unsigned char* cdata; /* compressed data */
	unsigned char* tmp; /* holds unfiltered data */
	unsigned char* data = NULL; /* end raw data */
	unsigned char* palette = NULL;
	unsigned char* pdata; /* temp data for palette substitution */
	unsigned int paletteSize = 0;
	unsigned int dataSize = 0;
	char headerCheck[9];
	char chunkType[5];
	unsigned int chunkSize, startRead;
	unsigned int width=0, height=0;
	unsigned char bitDepth;
	unsigned char colourType;
	unsigned char compressionMethod;
	unsigned char filterMethod;
	unsigned char interlaceMethod;
	unsigned int crc;
	unsigned int bytesPerPixel = 0;
	unsigned int scanFilter;
	unsigned char pback, pup, pupback, cbyte;
	unsigned int read;
	FILE* file;
	memset(headerCheck, '\0', sizeof(headerCheck));
	memset(chunkType, '\0', sizeof(chunkType));

	//printf("%s\n", zlibVersion());

	/* set up zlib inflate */
	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;

	//NOTE: a crash here is generally from a lib/dll mismatch
	if (inflateInit(&strm) != Z_OK) {printf("Error: Cannot decompress image data\n"); return NULL;}
	
	/* parse the png file */
	file = fopen(filename, "rb");
	if (file == NULL)
		{printf("Error: Could not open file %s\n", filename); inflateEnd(&strm); return NULL;}
	fread(headerCheck, 8, 1, file);
	if (str_compare(headerCheck, PNG_HEADER) != 0) /* make sure we are loading a png */
	{
		printf("Error: File not a PNG - %s\n", filename);
		inflateEnd(&strm);
		fclose(file);
		return NULL;
	}
	while (reading == 1 && ferror(file) == 0) /* break on finish or an error */
	{
		chunkSize = fread_uint(file);
		fread(chunkType, 4, 1, file);
		startRead = ftell(file);
		if (str_compare(chunkType, "IHDR") == 0) /* png header chunk */
		{
			width = fread_uint(file);
			height = fread_uint(file);
			fread(&bitDepth, 1, 1, file);
			fread(&colourType, 1, 1, file);
			fread(&compressionMethod, 1, 1, file);
			fread(&filterMethod, 1, 1, file);
			fread(&interlaceMethod, 1, 1, file);
			if (bitDepth != 8) {printf("Error: Unsupported PNG bit depth\n"); inflateEnd(&strm); fclose(file); return NULL;}
			if (compressionMethod != 0) {printf("Error: Unknown PNG compression\n"); inflateEnd(&strm); fclose(file); return NULL;}
			if (filterMethod != 0) {printf("Error: Unknown PNG filter method\n"); inflateEnd(&strm); fclose(file); return NULL;}
			if (interlaceMethod != 0) {printf("Error: PNG Loader does not support interlacing\n"); inflateEnd(&strm); fclose(file); return NULL;}
			if (colourType == 0) /* greyscale */
				bytesPerPixel = 1;
			else if (colourType == 2) /* rgb */
				bytesPerPixel = 3;
			else if (colourType == 3) /* palette */
				bytesPerPixel = 1;
			else if (colourType == 6) /* rgba */
				bytesPerPixel = 4;
			else
				{printf("Error: Unsupported PNG colour type (%i)\n", colourType); fclose(file); return NULL;}
		}
		else if (str_compare(chunkType, "IDAT") == 0)
		{
			/* read in whole data chunk */
			cdata = (unsigned char*)malloc(chunkSize);
			if (fread(cdata, 1, chunkSize, file) != chunkSize || ferror(file) != 0)
				{printf("Error: Missing image data\n"); free(data); inflateEnd(&strm); fclose(file); return NULL;}

			/* inflate compressed data */
			/* === using zlib === */
			outData = (unsigned char*)malloc(DECOMPRESS_CHUNK_SIZE);
			strm.avail_in = chunkSize;
			strm.next_in = cdata;
			do
			{
				/* decompress next chunk */
				strm.avail_out = DECOMPRESS_CHUNK_SIZE;
				strm.next_out = outData;
				ret = inflate(&strm, Z_NO_FLUSH);
				
				/* check for errors */
				if (ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR) /* buffer error is not fatal - just needs another chunk */
				{
					printf("Error: Problem inflating png data: Zlib Error %i\n", ret);
					inflateEnd(&strm);
					free(outData);
					free(data);
					fclose(file);
					return NULL;
				}

				/* increase data size and copy inflated chunk */
				outLen = DECOMPRESS_CHUNK_SIZE - strm.avail_out;
				data = (unsigned char*)realloc(data, dataSize + outLen);
				memcpy(data + dataSize, outData, outLen);
				dataSize += outLen;
			} while (ret != Z_STREAM_END && outLen != 0);

			/* cleanup */
			free(outData);
			free(cdata);
		}
		else if (str_compare(chunkType, "PLTE") == 0)
		{
			paletteSize = chunkSize;
			palette = (unsigned char*)malloc(paletteSize);
			if (fread(palette, 1, chunkSize, file) != paletteSize || ferror(file) != 0)
				{printf("Error: Missing palette data\n"); free(data); inflateEnd(&strm); fclose(file); return NULL;}
		}
		else if (str_compare(chunkType, "IEND") == 0)
			reading = 0; /* end reached, don't bother continuing */
		else
			fseek(file, chunkSize, SEEK_CUR); /* skip unknown chunks */

		read = ftell(file) - startRead;

		assert(read == chunkSize); /* check if the correct amount of data has been read */
		if (read != chunkSize)
			{printf("Error: Unknown error while parsing chunk %s in %s. %i unread.\n", chunkType, filename, chunkSize - read); free(data); inflateEnd(&strm); fclose(file); return NULL;}

		/* TODO: Check CRC */
		crc = fread_uint(file);
		crc = crc; /* stupid g++ and its warnings! */
	}

	if (ret != Z_STREAM_END)
		{printf("Error: Problem inflating png data: Zlib Error %i\n", ret); fclose(file); free(data); return NULL;}
	if (ferror(file) != 0)
		{printf("Error: File IO error\n"); fclose(file); free(data); return NULL;}
	if (width == 0 || height == 0)
		{printf("Error: Missing PNG header chunk\n"); fclose(file); free(data); return NULL;}
	if (dataSize != width * height * bytesPerPixel + height)
		{printf("Error: Image size and data mismatch\n"); fclose(file); free(data); return NULL;}
	
	inflateEnd(&strm);
	fclose(file);

	/* apply filtering to the image data */
	tmp = data;
	dataSize = width * height * bytesPerPixel;
	data = (unsigned char*)malloc(dataSize);
	for (i = 0; i < height; ++i)
	{
		pback = 0;
		scanFilter = tmp[i * (width * bytesPerPixel + 1)]; /* get the first byte of each scanline */
		for (j = 0; j < width; ++j)
		{
			for (k = 0; k < bytesPerPixel; ++k)
			{
				if (i == 0)	pup = 0;
				else pup = data[((height-1-(i-1)) * width + j) * bytesPerPixel + k];
				if (j == 0) pback = 0;
				else pback = data[((height-1-i) * width + j-1) * bytesPerPixel + k];
				if (i == 0 || j == 0) pupback = 0;
				else pupback = data[((height-1-(i-1)) * width + j-1) * bytesPerPixel + k];

				/* get the current byte from tmp */
				//cbyte = tmp[i * (width * bytesPerPixel + 1) + j * bytesPerPixel + k + 1];
				cbyte = tmp[bytesPerPixel * (i * width + j) + i + 1 + k];

				/* filter, then set the current byte in data */
				switch (scanFilter)
				{
				case 0: /* None */
					data[((height-i-1) * width + j) * bytesPerPixel + k] = cbyte;
					break;
				case 1: /* Sub */
					data[((height-i-1) * width + j) * bytesPerPixel + k] = cbyte + pback;
					break;
				case 2: /* Up */
					data[((height-i-1) * width + j) * bytesPerPixel + k] = cbyte + pup;
					break;
				case 3: /* Average */
					data[((height-i-1) * width + j) * bytesPerPixel + k] = cbyte + ((int)pback + (int)pup) / 2;
					break;
				case 4: /* Paeth */
					data[((height-i-1) * width + j) * bytesPerPixel + k] = cbyte + paeth(pback, pup, pupback);
					break;
				default:
					printf("Error: Unknown scanline filter\n");
					free(tmp);
					free(data);
					return NULL;
				}
			}
		}
	}
	free(tmp);
	
	/* if a palette is used, substitute target colours */
	if (colourType == 3)
	{
		if (palette == NULL)
			{printf("Error: Palette chunk not found\n"); free(tmp); free(data); return NULL;}
		bytesPerPixel = 3;
		pdata = (unsigned char*)malloc(width * height * bytesPerPixel);
		for (i = 0; i < dataSize; ++i)
		{
			assert(data[i]+2 < (int)paletteSize); /* over reading palette */
			assert(i*bytesPerPixel+2 < width * height * bytesPerPixel); /* over reading final data */
			pdata[i*bytesPerPixel+0] = palette[data[i]*3+0];
			pdata[i*bytesPerPixel+1] = palette[data[i]*3+1];
			pdata[i*bytesPerPixel+2] = palette[data[i]*3+2];
		}
		free(data);
		data = pdata;
		pdata = NULL;
	}

	/* construct and return image data structure */
	image = (Image*)malloc(sizeof(Image));
	image->width = width;
	image->height = height;
	image->channels = bytesPerPixel;
	image->data = data;
	return image;
}

void free_image(Image* img)
{
	/* frees the Image structure */
	if (!img) return;
	free(img->data);
	free(img);
}
