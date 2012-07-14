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

#ifndef __ASYNC_VBO_TRANSFERS_BMF_H__
#define __ASYNC_VBO_TRANSFERS_BMF_H__

/*
	Font renderer is using Angel Code Bitmap Font Generator binary file format
	http://www.angelcode.com/products/bmfont/
*/

namespace base {

#pragma pack(push, 1)

///
struct BMFInfo
{
	char pID[3];
	char ucVersion;
};

///
struct InfoBlock
{
	int blockSize;
	unsigned short fontSize;
	unsigned char reserved:4;
	unsigned char bold:1;
	unsigned char italic:1;
	unsigned char unicode:1;
	unsigned char smooth:1;
	unsigned char charSet;
	unsigned short stretchH;
	unsigned char aa;
	unsigned char paddingUp;
	unsigned char paddingRight;
	unsigned char paddingDown;
	unsigned char paddingLeft;
	unsigned char spacingHoriz;
	unsigned char spacingVert;
	unsigned char outline;         // Added with version 2
	char fontName[1];
};

///
struct CommonBlock
{
	int blockSize;
	unsigned short lineHeight;
	unsigned short base;
	unsigned short scaleW;
	unsigned short scaleH;
	unsigned short pages;
	unsigned char packed:1;
	unsigned char reserved:7;
	unsigned char alphaChnl;
	unsigned char redChnl;
	unsigned char greenChnl;
	unsigned char blueChnl;
};

///
struct PagesBlock
{
	int  blockSize;
	char pageNames[1];
};

///
struct CharInfo
{
	unsigned int id;
	unsigned short x;
	unsigned short y;
	unsigned short width;
	unsigned short height;
	short xoffset;
	short yoffset;
	short xadvance;
	unsigned char page;
	unsigned char chnl;
};

///
struct CharsBlock
{
	int blockSize;
	CharInfo chars[1];
};

///
struct kerningPair
{
	unsigned int first;
	unsigned int second;
	short amount;
};

///
struct KerningPairsBlock
{
	int blockSize;
	kerningPair kerningPairs[1];
};

#pragma pack(pop)

} // end of namspace base

#endif // __ASYNC_VBO_TRANSFERS_BMF_H__
