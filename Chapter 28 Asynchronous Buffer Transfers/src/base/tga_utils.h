/*
Copyright (C) 2011 by Ladislav Hrabcak

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef __ASYNC_VBO_TRANSFERS_BASE_TGA_UTILS_H__
#define __ASYNC_VBO_TRANSFERS_BASE_TGA_UTILS_H__

namespace base {

#pragma pack(push,1)

struct tga_header
{
	char identsize;			//< size of ID field that follows 18 byte header (0 usually)
	char colormaptype;		//< type of colour map 0=none, 1=has palette
	char imagetype;			//< type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

	short colormapstart;	//< first colour map entry in palette
	short colormaplength;	//< number of colours in palette
	char colormapbits;		//< number of bits per palette entry 15,16,24,32

	short xstart;           //< image x origin
	short ystart;           //< image y origin
	short width;            //< image width in pixels
	short height;           //< image height in pixels
	char bits;              //< image bits per pixel 8,16,24,32
	char descriptor;        //< image descriptor bits (vh flip bits)

	tga_header(
		const short width,
		const short height,
		const char bits)
		: identsize(0)
		, colormaptype(0)
		, imagetype(2)
		, colormapstart(0)
        , colormaplength(0)
        , colormapbits(0)
        , xstart(0)
        , ystart(0)
        , width(width)
        , height(height)
        , bits(bits)
        , descriptor(0)
	{}
};

#pragma pack(pop)

} // end of namespace base

#endif // __ASYNC_VBO_TRANSFERS_BASE_TGA_UTILS_H__
