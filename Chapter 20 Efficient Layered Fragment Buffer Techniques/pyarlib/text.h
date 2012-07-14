/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef TEXT_H
#define TEXT_H

#include "vec.h"
#include "matrix.h"

#include <vector>

struct FT_FaceRec_;
struct FT_GlyphSlotRec_;
struct FT_LibraryRec_;
typedef struct FT_FaceRec_* FT_Face;
typedef struct FT_GlyphSlotRec_* FT_GlyphSlot;
typedef struct FT_LibraryRec_* FT_Library;

class Text;
class FontManager;

struct GPUBuffer;
struct VertexBuffer;

//font glyph - used while drawing glyphs from gl texture
struct GlyphInfo
{
	vec2i size;
	vec2i pos;
};

//text char - used to render/position strings of glyphs
struct CharInfo
{
	float glyph;
	vec2f pos;
};

class Font
{
	friend class FontManager;
	friend class Text;
	FT_Face face;
	FT_GlyphSlot slot;
	vec2i glyphsSize;
	vec2i glyphsDim;
	vec2i texSize;
	GlyphInfo* glyphInfo; //used local when constructing string
	GPUBuffer* bufferedInfo; //used GPU side while drawing
	void begin();
	void end();
	std::string name;
private:
	bool hasRasterized;
	Font(std::string file, int pt);
	~Font();
public:
	vec2i dpi;
	unsigned int tex;
	void rasterize();
	static Font* get(std::string file, int pt); //stupid shortcut to FontManager::get
	std::vector<CharInfo> makeString(const std::string& str, vec2i* bmin = NULL, vec2i* bmax = NULL);
};

class Text
{
	std::vector<CharInfo> charString;
	int charsVisible;
	VertexBuffer* chars;
public:
	Font* font;
	
	//NOTE: +y is up, +x is right
	//bounds min is bottom left
	//bounds max is top right
	//make sure to flip y when working with 0,0 = top/left (eg in quickqui)
	vec2i boundsMin;
	vec2i boundsMax;
	vec2i size;
	
	vec3f colour;

	std::string text;
	Text(std::string fontName, int pt = 11);
	Text(Font* font);
	void setFont(std::string fontName, int pt = 11);
	void setFont(Font* font);
	void release();
	void operator=(std::string str);
	void textf(const char* fmt, ...);
	void textf(const char* fmt, va_list& ap);
	void draw(vec2i offset = vec2i(0));
	void draw(const mat44& projection);

private:
	Text& operator=(const Text& other);
};

class FontManager
{
	friend class Font;
private:
	FT_Library library;
	typedef std::map<std::string, Font*> FontMap;
	typedef std::map<std::string, std::string> FileMap;
	FontMap fonts;
	FileMap files;
	FontManager();
	~FontManager();
	const std::string& getFontFile(std::string file);
	static FontManager* singleton;
	static FontManager& getSingleton();
	Font* get(std::string file, int pt);
};

#endif
