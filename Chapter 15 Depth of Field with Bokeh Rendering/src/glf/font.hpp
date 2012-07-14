#ifndef GLF_FONT_HPP
#define GLF_FONT_HPP

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/wrapper.hpp>
#include <glf/buffer.hpp>
#include <glf/texture.hpp>

namespace glf
{
	//--------------------------------------------------------------------------
	class Font
	{
		public:
								Font(			);
								~Font(			);
			std::string 		ToString(		) const;
			inline unsigned int CharWidth(		char _c) const;
			inline unsigned int CharHeight(		char _c) const;
			inline glm::uvec2	CharSize(		char _c) const;
			inline unsigned int CharStride(			char _c) const;
			inline unsigned int FontSize(		) const;
			inline std::string	FontName(		) const;
			template<class SerializedFont>
			void 				Load();
		public:
			Texture2D 			FontTex;		// Texture which stores characters shape
		private:
								Font(			const Font& _copy);
			Font& 				operator=(		const Font& _copy);
		private:
			std::string							fontName;		// Font name
			unsigned int						fontSize;		// Font size
			unsigned int*						charStrides;	// Stride in texture to acces to a character
			glm::uvec2*							charSizes;		// Size of a character
			unsigned int						nCharacters;	// Number of a character
	};
	//--------------------------------------------------------------------------
	class FontRenderer
	{
		public:
								FontRenderer(	int _x, 
												int _y);
								~FontRenderer(	);
			void 				Draw(			int _x, 
												int _y, 
												const Font& _font,
												const std::string& _message, 
												const glm::vec4& _color);
			unsigned int 		ComputeWidth(	const Font& _font, 
												const std::string& _message) const;
			void				Reshape(		int _w,
												int _h);
		private:
								FontRenderer(	const FontRenderer& _copy);
			FontRenderer& 		operator=(		const FontRenderer& _copy);
		private:
			VertexBuffer4F						quadVBO;
			VertexArray							quadVAO;
			int									maxCharacters;
			glm::ivec4*							textGeometry;
			glm::vec2*							textTexture;
			Program								program;
			GLint								colorVar;
			GLint								textGeometryVar;
			GLint								textTextureVar;
			GLint								fontTexUnit;
	};
	//--------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------
// Includes inline definition
//-----------------------------------------------------------------------------
#include <glf/font.inl>

#endif
