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

#ifndef __ASYNC_VBO_TRANSFERS_BASE_PIXELFMT_H__
#define __ASYNC_VBO_TRANSFERS_BASE_PIXELFMT_H__

namespace base {

enum pixelfmt {
    PF_UNKNOWN = 0,

	PF_R8,
	PF_RG8,
	PF_RGB8,
	PF_RGBA8,

	PF_R8I,
	PF_RG8I,
	PF_RGB8I,
	PF_RGBA8I,

	PF_R8UI,
	PF_RG8UI,
	PF_RGB8UI,
	PF_RGBA8UI,

	PF_R16,
	PF_RG16,
	PF_RGB16,
	PF_RGBA16,

	PF_R16I,
	PF_RG16I,
	PF_RGB16I,
	PF_RGBA16I,

	PF_R16UI,
	PF_RG16UI,
	PF_RGB16UI,
	PF_RGBA16UI,

	PF_R32I,
	PF_RG32I,
	PF_RGB32I,
	PF_RGBA32I,

	PF_R32UI,
	PF_RG32UI,
	PF_RGB32UI,
	PF_RGBA32UI,

	PF_BGR8,
	PF_BGRA8,

	PF_BGR8I,
	PF_BGRA8I,

	PF_BGR8UI,
	PF_BGRA8UI,

	PF_BGR16,
	PF_BGRA16,

	PF_BGR16I,
	PF_BGRA16I,

	PF_BGR16UI,
	PF_BGRA16UI,

	PF_BGR32I,
	PF_BGRA32I,

	PF_BGR32UI,
	PF_BGRA32UI,

	// FLOAT

	PF_R32F,
	PF_RG32F,
	PF_RGB32F,
	PF_RGBA32F,

	// HALF FLOAT with source 16F

	PF_R16F,
	PF_RG16F,
	PF_RGB16F,
	PF_RGBA16F,

    // HALF FLOAT with source 32F

	PF_R16F_FLOAT,
	PF_RG16F_FLOAT,
	PF_RGB16F_FLOAT,
	PF_RGBA16F_FLOAT,

	// DEPTH

	PF_DEPTH16,
	PF_DEPTH24,
	PF_DEPTH24_STENCIL8,
	PF_DEPTH32,
	PF_DEPTH32F,
	PF_DEPTH32F_S8,

	// COMPRESSED

	PF_DXT1,
	PF_DXT3,
	PF_DXT5,

	// SIGNED NORMALIZED

	PF_R8_SNORM,
	PF_RG8_SNORM,
	PF_RGB8_SNORM,
	PF_RGBA8_SNORM,

	PF_R16_SNORM,
	PF_RG16_SNORM,
	PF_RGB16_SNORM,
	PF_RGBA16_SNORM,

	PF_DXT1_SRGB,
	PF_DXT3_SRGB,
	PF_DXT5_SRGB,

	PF_RGBA8_SRGB,
	PF_BGRA8_SRGB,

	PF_RGB565,
	PF_R11_G11_B10F,

	PF_LAST,
};

} // end of namespace base

#endif // __ASYNC_VBO_TRANSFERS_BASE_PIXELFMT_H__
