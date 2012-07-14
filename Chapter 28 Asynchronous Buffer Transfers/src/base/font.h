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

#ifndef __ASYNC_VBO_TRANSFERS_FONT_H__
#define __ASYNC_VBO_TRANSFERS_FONT_H__

#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <gl/glew.h>

namespace base {

class source_location;
class canvas;

struct BMFInfo;
struct InfoBlock;
struct CommonBlock;
struct PagesBlock;
struct CharInfo;
struct CharsBlock;
struct KerningPairsBlock;

class font
{
	friend canvas;

public:
	font();
	~font();
	
	void load(
		const source_location &loc,
		const char *filename);

protected:
	std::vector<unsigned char> _data;
    const BMFInfo* _BMFInfo;
    const InfoBlock* _infoBlock;
    const CommonBlock* _commonBlock;
    const PagesBlock* _pagesBlock;
    const CharsBlock* _charsBlock;
    const KerningPairsBlock* _kerningPairsBlock;

	typedef std::map<unsigned int,const CharInfo*> char_map_t;
	char_map_t _chars;
	GLuint _tex;

private:
	font(const font&);
	void operator=(const font&);
};

} // end of namspace base

#endif // __ASYNC_VBO_TRANSFERS_FONT_H__
