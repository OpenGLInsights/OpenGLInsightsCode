//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <glf/font.hpp>
#include <glf/geometry.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace glf
{
	//-------------------------------------------------------------------------
	Font::Font()
	{
		fontName	= "Invalid";
		fontSize	= 0;
		charStrides	= NULL;
		charSizes	= NULL;
		nCharacters = 0;
	}
	//-------------------------------------------------------------------------
	Font::~Font()
	{
		delete[] charSizes;
		delete[] charStrides;
	}
	//-------------------------------------------------------------------------
	std::string Font::ToString() const
	{
		std::stringstream info;
		info << "Font description [Name:"<< fontName << ", Size:" << fontSize << "]\n";
		for(unsigned int i=0;i<nCharacters;++i)
			info << "\tChar:" << static_cast<char>(i) << " [Stride:"<< charStrides[i] << ", Size:(" << charSizes[i].x << "," << charSizes[i].y << ")]\n";
		return info.str();
	}
	//-------------------------------------------------------------------------
	FontRenderer::FontRenderer(int _w, int _h):
	program("FontRenderer")
	{
		std::string vsSource = "\n\
			#version 330\n\
			\n\
			uniform mat4  Projection;\n\
			uniform ivec4 TextGeometry[64]; // (Translate.x,Translate.y,Scale.x,Scale.y)\n\
			uniform vec2  TextTexture[64];  // (CharWidth,CharStride)\n\
			\n\
			layout(location=0) in  vec4 Position;\n\
			out vec2 TexCoord;\n\
			\n\
			void main()\n\
			{\n\
				ivec4 geom  = TextGeometry[gl_InstanceID];\n\
				vec2 tex    = TextTexture[gl_InstanceID];\n\
				gl_Position = Projection * (Position*vec4(geom.zw,1,1) + vec4(geom.xy,0,0));\n\
				TexCoord    = Position.xy * vec2(tex.x,1) + vec2(tex.y,0);\n\
			}";

		std::string fsSource = "\n\
			#version 330\n\
			uniform vec4		Color;\n\
			uniform sampler2D	FontTex;\n\
			\n\
			in  vec2 TexCoord;\n\
			out vec4 FragColor;\n\
			\n\
			void main()\n\
			{\n\
				FragColor = texture2D(FontTex,TexCoord).xxxw * vec4(Color.xyz,1.0);\n\
			}";

		CreateQuad(quadVBO);
		quadVAO.Add(quadVBO,semantic::Position,4,GL_FLOAT);
		glm::mat4 projection = glm::ortho(0.f,float(_w),0.f,float(_h));

		program.Compile(vsSource,fsSource);
		textGeometryVar = program["TextGeometry[0]"].location;
		textTextureVar  = program["TextTexture[0]"].location;
		colorVar 		= program["Color"].location;
		fontTexUnit		= program["FontTex"].unit;

		glProgramUniformMatrix4fv(program.id, program["Projection"].location, 1, GL_FALSE, &projection[0][0]);
		glProgramUniform1i(program.id, program["FontTex"].location, fontTexUnit);

		maxCharacters	= 64;
		textGeometry	= new glm::ivec4[maxCharacters];
		textTexture		= new glm::vec2[maxCharacters];
	}
	//-------------------------------------------------------------------------
	void FontRenderer::Reshape(int _w, int _h)
	{
		glm::mat4 projection = glm::ortho(0.f,float(_w),0.f,float(_h));
		glProgramUniformMatrix4fv(program.id, program["Projection"].location, 1, GL_FALSE, &projection[0][0]);
	}
	//-------------------------------------------------------------------------
	FontRenderer::~FontRenderer()
	{
		delete[] textGeometry;
		delete[] textTexture;
	}
	//-------------------------------------------------------------------------
	void FontRenderer::Draw(int _x, int _y, const Font& _font, const std::string& _text, const glm::vec4& _color)
	{
		int nCharacters = int(_text.size());
		assert(nCharacters<maxCharacters);

		int stride = 0;
		for(int i=0; i<nCharacters; ++i)
		{
			char currentChar = _text[i];
			// Translate.x,Translate.y,Scale.x,Scale.y
			textGeometry[i]  = glm::ivec4(_x + stride,_y,_font.CharWidth(currentChar),_font.CharHeight(currentChar));
			// CharWidth, Stride
			textTexture[i]   = glm::vec2(_font.CharWidth(currentChar)  / static_cast<float>(_font.FontTex.size.x),
										 _font.CharStride(currentChar) / static_cast<float>(_font.FontTex.size.x));
			stride 			+= _font.CharWidth(currentChar);
		}

		glUseProgram(program.id);
		glProgramUniform4iv(program.id, textGeometryVar,nCharacters, &textGeometry[0][0]);
		glProgramUniform2fv(program.id, textTextureVar, nCharacters, &textTexture[0][0]);
		glProgramUniform4fv(program.id, colorVar,1, &_color[0]);
		_font.FontTex.Bind(fontTexUnit);
		quadVAO.Draw(GL_TRIANGLES,6,0,nCharacters);
	}
	//-------------------------------------------------------------------------
	unsigned int FontRenderer::ComputeWidth(const Font& _font, const std::string& _message) const
	{
		unsigned int width 		= 0;
		const unsigned int size = int(_message.length());
		for(unsigned int i=0;i<size;++i)
			width += _font.CharWidth(_message[i]);
		return width;
	}
}

