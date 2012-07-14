//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

namespace glf
{
	//-------------------------------------------------------------------------
	template<class SerializedFont>
	void Font::Load()
	{
		SerializedFont font;
		fontName 		= font.FontName;
		fontSize 		= font.FontSize;
		charStrides 	= new unsigned int[font.NumberOfCharacters];
		charSizes 		= new glm::uvec2[font.NumberOfCharacters];
		nCharacters = font.NumberOfCharacters;

		for(unsigned int i=0; i<nCharacters; ++i)
		{
			charSizes[i].x	= font.CharWidth[i];
			charSizes[i].y	= font.CharHeight[i];
			charStrides[i] 	= font.CharStride[i];
		}

		// Create texture from character pixel
		FontTex.Allocate(GL_RGBA8,font.Width,font.Height);
		FontTex.SetFiltering(GL_LINEAR,GL_LINEAR);
		FontTex.SetWrapping(GL_REPEAT,GL_REPEAT);
		FontTex.Fill(GL_RGBA,GL_UNSIGNED_BYTE,font.Pixels);
	}
	//--------------------------------------------------------------------------
	inline unsigned int Font::CharWidth(char _c) const
	{
		assert(charSizes!=NULL);
		return charSizes[static_cast<unsigned int>(_c)].x;
	}
	//--------------------------------------------------------------------------
	inline unsigned int Font::CharHeight(char _c) const
	{
		assert(charSizes!=NULL);
		return charSizes[static_cast<unsigned int>(_c)].y;
	}
	//--------------------------------------------------------------------------
	inline glm::uvec2 Font::CharSize(char _c) const
	{
		assert(charSizes!=NULL);
		return charSizes[static_cast<unsigned int>(_c)];
	}
	//--------------------------------------------------------------------------
	inline unsigned int Font::FontSize() const
	{
		return fontSize;
	}
	//--------------------------------------------------------------------------
	inline std::string Font::FontName() const
	{
		return fontName;
	}
	//--------------------------------------------------------------------------
	inline unsigned int Font::CharStride(char _c) const
	{
		assert(charStrides!=NULL);
		return charStrides[static_cast<unsigned int>(_c)];
	}
}
