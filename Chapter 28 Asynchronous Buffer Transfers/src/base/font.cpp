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

#include "font.h"
#include "base.h"
#include "bmf.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::font::font() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::font::~font() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::font::load(const base::source_location &loc, const char *filename)
{
	base::read_file(loc,filename,_data);

    const unsigned char* buf=&_data[0];
	_BMFInfo=reinterpret_cast<const BMFInfo*>(buf);

    if(_BMFInfo->pID[0] != 'B'
		|| _BMFInfo->pID[1] != 'M'
		|| _BMFInfo->pID[2] != 'F'
		|| _BMFInfo->ucVersion != 3)
        throw base::exception(loc.to_str())
			<< "This is not the BMF file or it's a bad version \"" << filename << "\"!";

	const unsigned char* block_id=buf+sizeof(_BMFInfo);
	const unsigned char* const end=buf+_data.size();

	while(block_id<end) {
		switch(*block_id) {
		case 1:
			++block_id;
			_infoBlock = reinterpret_cast<const InfoBlock*>(block_id);
			block_id += _infoBlock->blockSize;
			break;
		case 2:
			++block_id;
			_commonBlock = reinterpret_cast<const CommonBlock*>(block_id);
			block_id += _commonBlock->blockSize;
			break;
		case 3:
			++block_id;
			_pagesBlock = reinterpret_cast<const PagesBlock*>(block_id);
			block_id += _pagesBlock->blockSize;
			break;
		case 4:
			++block_id;
			_charsBlock = reinterpret_cast<const CharsBlock*>(block_id);
			block_id += _charsBlock->blockSize;
			break;
		case 5:
			++block_id;
			_kerningPairsBlock = reinterpret_cast<const KerningPairsBlock*>(block_id);
			block_id += _kerningPairsBlock->blockSize;
			break;
		default:
			throw base::exception(loc.to_str())
				<< "Unknown BMF block inf font \"" << filename << "\"!";
		}

		if (_BMFInfo->ucVersion == 3)
			block_id += 4;
	}
	
	const CharInfo *e = _charsBlock->chars + (_charsBlock->blockSize - 4) / sizeof(CharInfo);

	for(const CharInfo *i = _charsBlock->chars; i !=e; ++i)
		_chars.insert(std::pair<unsigned int, const CharInfo*>(i->id, i));

	// load font texture page, we are supporting only one txture page!
	std::string texture_page(
		_pagesBlock->pageNames,
		_pagesBlock->pageNames + _pagesBlock->blockSize);

	texture_page="fonts/"+texture_page;

	_tex=base::create_texture_from_file(loc, texture_page.c_str(), true);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
