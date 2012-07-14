/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <assert.h>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <set>

#include <stdarg.h>

#include "text.h"

#include "fileutil.h"
#include "util.h"
#include "gpu.h"
#include "shader.h"
#include "shaderutil.h"
#include "imgpng.h"
#include "resources.h"

EMBED(textFrag, text.frag);
EMBED(textVert, text.vert);
EMBED(textGeom, text.geom);

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SFNT_NAMES_H
#include <GL/gl.h>

using namespace std;

static Shader shader("text.vert", "text.frag", "text.geom");

Font::Font(std::string file, int pt)
{
	bufferedInfo = new GPUBuffer(GL_UNIFORM_BUFFER);
	glyphInfo = NULL;	
	hasRasterized = false;
	
	dpi = vec2i(90, 88);
	face = NULL;

	const std::string& buffer = FontManager::getSingleton().getFontFile(file);
	int error = FT_New_Memory_Face(FontManager::getSingleton().library, (FT_Byte*)&buffer[0], buffer.size(), 0, &face);
	if (error == FT_Err_Unknown_File_Format)
	{
		cout << "Error: Unknown font format \"" << file << "\"." << endl;
	}
	else if (error)
	{
		cout << "Error loading font \"" << file << "\"." << endl;
	}

	if (!error)
	{
		slot = face->glyph;
	
		FT_Set_Char_Size(face, pt<<6, pt<<6, dpi.x, dpi.y);
		//error = FT_Set_Pixel_Sizes(face, 18, 18);
		//error = FT_Select_Charmap(face, FT_ENCODING_BIG5);
	
		/*
		int numNames = FT_Get_Sfnt_Name_Count(face);
		if (numNames > 1)
		{
			FT_SfntName tmpName;
			FT_Get_Sfnt_Name(face, 1, &tmpName);
			name = string((char*)tmpName.string, tmpName.string_len);
		}
		else
			name = "UnnamedFont";
		*/
	
		if (face->family_name)
			name = face->family_name;
		else
			name = "UnnamedFont";
	
		//cout << "Loaded " << pt << "pt " << name << endl;
	}
	else
		face = NULL;
}
Font::~Font()
{
	delete bufferedInfo;
	delete[] glyphInfo;
	FT_Done_Face(face);
}
void Font::rasterize()
{
	int error;
	if (!face || hasRasterized)
		return;
	hasRasterized = true;
	
	float anisotropy = 0.0; //4.0
	int totalGlyphs = 256;
	
	//create texture for glyphs
	float maxAnisotropy; 
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	anisotropy = std::min(maxAnisotropy, anisotropy);
	
	//printf("anisotropy: %i\n", (int)anisotropy);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	if (anisotropy > 1.0)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	if (anisotropy > 1.0)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	//I don't know how to predict glyph sizes, so render first to work out the total
	vec2i glyphMax(0);
	for (int i = 0; i < totalGlyphs; ++i)
	{
		int index = FT_Get_Char_Index(face, i);
		error = FT_Load_Glyph(face, index, FT_LOAD_DEFAULT);
		error = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
		FT_Bitmap& bitmap = slot->bitmap;
		if (bitmap.width > glyphMax.x) glyphMax.x = bitmap.width;
		if (bitmap.rows > glyphMax.y) glyphMax.y = bitmap.rows;
	}
	if (glyphMax.x * glyphMax.y == 0)
	{
		cout << "Warning: Font " << name << " has size zero\n";
		glyphMax = vec2i(1);
	}
	
	//now work out total texture size etc
	int totalTexels = glyphMax.x * glyphMax.y * totalGlyphs;
	int squareWidth = ceilSqrt(totalTexels);
	glyphsDim.x = ceil(squareWidth, glyphMax.x);
	glyphsDim.y = ceil(totalGlyphs, glyphsDim.x);
	assert(glyphsDim.x * glyphsDim.y >= totalGlyphs);
	assert(glyphMax.x > 0 && glyphMax.y > 0);
	glyphsSize = glyphMax;
	texSize = glyphsDim * glyphsSize;
	texSize.x = ceil(texSize.x, 4)*4; //texture must be 4-byte aligned
	assert(texSize.x < 4096 && texSize.y < 4096);
	
	//allocate char info array (used for reading texture)
	delete[] glyphInfo;
	glyphInfo = new GlyphInfo[totalGlyphs];
	
	//resize texture
	unsigned char* zeroData = new unsigned char[texSize.x * texSize.y];
	memset(zeroData, 0, texSize.x * texSize.y);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, texSize.x, texSize.y, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, zeroData);
	delete[] zeroData;
		
	//render and upload glyphs
	vec2i tmpSize = glyphsSize;
	tmpSize.x = ceil(tmpSize.x, 4)*4;
	unsigned char* tmpData = new unsigned char[tmpSize.x * tmpSize.y];
	for (int i = 0; i < totalGlyphs; ++i)
	{
		//calc position within texture
		vec2i texOffset;
		texOffset.x = i % glyphsDim.x;
		texOffset.y = i / glyphsDim.x;
		texOffset *= glyphsSize;
		
		//render glyph
		int index = FT_Get_Char_Index(face, i);
		error = FT_Load_Glyph(face, index, FT_LOAD_DEFAULT);
		error = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
		//TODO: use FT_RENDER_MODE_LCD for better antialiasing
		
		//get glyph info
		FT_Bitmap& bitmap = slot->bitmap;
		assert(bitmap.pixel_mode == FT_PIXEL_MODE_GRAY);
		assert(bitmap.num_grays == 256);
		assert((int)abs(bitmap.pitch) == bitmap.width);
		vec2i glyphSize(bitmap.width, bitmap.rows);
		vec2i glyphPos(slot->bitmap_left, slot->bitmap_top);
		glyphInfo[i].pos = glyphPos;
		glyphInfo[i].size = glyphSize;
		if (glyphSize.x * glyphSize.y > 0)
		{
			glyphSize.x = ceil(glyphSize.x, 4)*4;
			assert(tmpSize.x >= glyphSize.x && tmpSize.y >= glyphSize.y);
			
			//glyphs need to be flipped for opengl
			for (int y = 0; y < glyphSize.y; ++y)
				for (int x = 0; x < glyphSize.x; ++x)
					tmpData[y*glyphSize.x+x] = x < bitmap.width ? bitmap.buffer[(glyphSize.y-y-1)*(bitmap.width)+x] : 0;
			
			//upload glyph. since we are writing left to right, the possible overwrite
			//of 3 pixel columns of zeroes outside the glyph box is ok - it will be overwritten with
			//the next glyph.
			glTexSubImage2D(GL_TEXTURE_2D, 0, texOffset.x, texOffset.y, glyphSize.x, glyphSize.y, GL_LUMINANCE, GL_UNSIGNED_BYTE, tmpData);
		}
	}
	delete[] tmpData;
	
	if (anisotropy > 1.0)
		glGenerateMipmap(GL_TEXTURE_2D);
		
	glBindTexture(GL_TEXTURE_2D, 0);
	
	bufferedInfo->buffer(glyphInfo, sizeof(GlyphInfo) * totalGlyphs);
	bufferedInfo->createImage(GL_RGBA32UI);
	
	//cout << "Rasterized " << totalGlyphs << " for " << name << " at " << glyphsSize.x << "x" << glyphsSize.y << " = " << texSize.x << "x" << texSize.y << endl;
	
	#if 0
	QI::ImagePNG saver;
	saver.readTexture(tex);
	saver.saveImage("font.png");
	#endif
}
Font* Font::get(std::string file, int pt)
{
	return FontManager::getSingleton().get(file, pt);
}
std::vector<CharInfo> Font::makeString(const std::string& str, vec2i* bmin, vec2i* bmax)
{
	std::vector<CharInfo> ret;

	if (!face)
		return ret;

	//FIXME: I assume this function could be optimized
	vec4i bbox(0);
	
	bool tab = false;
	int last = 0;
	if (!face)
		return ret;
	vec2i pen(0);
	//vec2i pen2(0); //TODO: kerning error might occur for large kerning
	//see: http://www.freetype.org/freetype2/docs/tutorial/step2.html
	int line = 0;
	bool useKerning = FT_HAS_KERNING(face) != 0;
	for (int i = 0; i < (int)str.size(); ++i)
	{
		char c = str[i];
		if (c == '\n')
		{
			pen.y -= face->size->metrics.height >> 6;
			pen.x = 0;
			last = 0;
			++line;
			continue;
		}
		if (c == '\t')
		{
			c = ' ';
			tab = true;
		}
	
		int index = FT_Get_Char_Index(face, c);
		if (useKerning && last && index)
		{
			FT_Vector kerning;
			FT_Get_Kerning(face, last, index, FT_KERNING_DEFAULT, &kerning);
			pen.x += kerning.x >> 6;
			//printf("%c %c %i\n", str[i-1], str[i], kerning.x >> 6);
		}
		last = index;
		
		//add to string
		CharInfo info;
		info.glyph = c;
		info.pos = pen;
		//printf("%i %i %i\n", info.glyph, info.pos.x, info.pos.y);
		ret.push_back(info);
		
		FT_Load_Glyph(face, index, FT_LOAD_DEFAULT);
		if (tab)
		{
			pen.x += slot->advance.x >> 4;
			tab = false;
		}
		else
			pen.x += slot->advance.x >> 6;
		
		//bounding box stuff
		//FIXME: this confusion of different unit types is annoying. no idea if this is right
		FT_Glyph_Metrics met = slot->metrics;
		if (line == 0)
			bbox.w = max((int)bbox.w, (int)met.horiBearingY >> 6);
		bbox.z = max(bbox.z, pen.x);
		bbox.y = min(bbox.y, pen.y + (int)((met.horiBearingY - met.height) >> 6));
	}
	
	if (bmin)
	{
		bmin->x = bbox.x;
		bmin->y = bbox.y;
	}
	if (bmax)
	{
		bmax->x = bbox.z;
		bmax->y = bbox.w;
	}
	return ret;
}

Text::Text(std::string fontName, int pt)
{
	setFont(fontName, pt);
}
Text::Text(Font* font)
{
	setFont(font);
}
void Text::setFont(std::string fontName, int pt)
{
	font = Font::get(fontName, pt);
	chars = NULL;
	boundsMin = vec2i(0);
	boundsMax = vec2i(0);
	colour = vec3f(1.0);
	*this = text;
}
void Text::setFont(Font* font)
{
	this->font = font;
	chars = NULL;
	boundsMin = vec2i(0);
	boundsMax = vec2i(0);
	colour = vec3f(1.0);
	*this = text;
}
void Text::release()
{
	if (chars)
		chars->release();
	delete chars;
	chars = NULL;
}
void Text::operator=(std::string str)
{
	if (str.size() == 0 && text.size() == 0)
		return; //stops init during constructor
		
	text = str;
	charString = font->makeString(text, &boundsMin, &boundsMax);
	charsVisible = (int)charString.size();
	size = boundsMax - boundsMin;
}
void Text::textf(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	textf(fmt, ap);
}
void Text::textf(const char* fmt, va_list& ap)
{
	#define MAX_TEXT_LENGTH 4096

	//expand fmt
	char tmp[MAX_TEXT_LENGTH];
	tmp[0] = '\0';
	if (fmt == NULL)
		return;
	int l = vsprintf(tmp, fmt, ap);
	assert(l < MAX_TEXT_LENGTH);
	va_end(ap);
	
	//pass it on to get processed properly
	this->operator=(string(tmp));
}
void Text::draw(vec2i offset)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int w = viewport[2] - viewport[0];
	int h = viewport[3] - viewport[1];
	mat44 transform = mat44::identity();
	transform *= mat44::translate(-1.0f, 1.0f, 0.0f);
	transform *= mat44::scale(2.0f/w, 2.0f/h, 1.0f);
	transform *= mat44::translate((float)offset.x, (float)offset.y, 0.0f);
	draw(transform);
}
void Text::draw(const mat44& projection)
{
	if (!charsVisible)
		return; //string is empty

	/*
	CharInfo* test = (CharInfo*)chars->map();
	for (int i = 0; i < text.size(); ++i)
		printf("%i %i %i\n", test[i].glyph, test[i].pos.x, test[i].pos.y);
	chars->unmap();
	*/
	
	//make sure chars is created and has enough memory
	if (!chars)
		chars = new VertexBuffer();
		
	//upload text string to GPU
	int datSize = charString.size() * sizeof(CharInfo);
	if (datSize)
	{
		const CharInfo& first = *charString.begin(); //lol
		chars->buffer((void*)&first, datSize);
		charString.clear();
	}

	if (!font->hasRasterized)
		font->rasterize();

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//instead of loading the shader from disk, use the embedded text included with BINDATA()
	static bool setSource = false;
	if (!setSource)
	{
		Shader::include("text.frag", RESOURCE(textFrag));
		Shader::include("text.vert", RESOURCE(textVert));
		Shader::include("text.geom", RESOURCE(textGeom));
		setSource = true;
	}
	
	shader.use();
	if (!shader.error())
	{
		shader.set("projectionMat", projection);
		shader.set("glyphsDim", font->glyphsDim);
		shader.set("glyphsSize", font->glyphsSize);
		shader.set("texSize", font->texSize);
		shader.set("colour", colour);
	
		//set font texture
		setActiveTexture(0, "font", font->tex);

		GLuint binding = 0;
		GLuint block = glGetUniformBlockIndex(shader, "GlyphInfo");
		glUniformBlockBinding(shader, block, binding);
		glBindBufferRange(GL_UNIFORM_BUFFER, binding, *font->bufferedInfo, 0, sizeof(GlyphInfo) * 256);
	
		//set vertex attributes for rendering
		//FFS GL_INT still doesn't work
		shader.attrib("intdata", chars->object, GL_FLOAT, sizeof(float)*3);
		glDrawArrays(GL_POINTS, 0, charsVisible);
	}
	shader.unuse();
	glPopAttrib();
}
Text& Text::operator=(const Text& other)
{
	//TODO
	return *this;
}

FontManager* FontManager::singleton;

FontManager::FontManager()
{
	int error = FT_Init_FreeType(&library);
	if ( error )
	{
		cout << "Error: Could not initialize freetype." << endl;
	}
}
FontManager::~FontManager()
{
	//cleanup font size instances
	for (FontMap::iterator it = fonts.begin(); it != fonts.end(); ++it)
		delete it->second;
	
	 FT_Done_FreeType(library);
}
const std::string& FontManager::getFontFile(std::string file)
{
	FileMap::iterator it = files.find(file);
	if (it != files.end())
		return it->second;
	else
	{
		files[file] = std::string();
		readFile(files[file], file.c_str());
		return files[file];
	}
}
FontManager& FontManager::getSingleton()
{
	if (!singleton)
		singleton = new FontManager();
	return *singleton;
}
Font* FontManager::get(std::string file, int pt)
{
	Font* ret;
	stringstream hash;
	hash << file << pt;
	FontMap::iterator it = fonts.find(hash.str());
	if (it != fonts.end())
		ret = it->second;
	else
		ret = fonts[hash.str()] = new Font(file, pt);
	return ret;
}


