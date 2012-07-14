//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <glf/texture.hpp>

namespace glf
{
	namespace
	{
		// Translate GL inner format into "native" external format and type
		void ToFormat(GLenum _innerFormat, GLenum& _format, GLenum& _type)
		{
			switch(_innerFormat)
			{
				case GL_RGBA32F 			: _format = GL_RGBA; _type = GL_FLOAT; break;
				case GL_RGB32F  			: _format = GL_RGB;  _type = GL_FLOAT; break;
				case GL_RG32F   			: _format = GL_RG;   _type = GL_FLOAT; break;
				case GL_R32F    			: _format = GL_RED;  _type = GL_FLOAT; break;

				case GL_RGBA16F 			: _format = GL_RGBA; _type = GL_FLOAT; break;
				case GL_RGB16F  			: _format = GL_RGB;  _type = GL_FLOAT; break;
				case GL_RG16F   			: _format = GL_RG;   _type = GL_FLOAT; break;
				case GL_R16F    			: _format = GL_RED;  _type = GL_FLOAT; break;

				case GL_RGBA32UI			: _format = GL_RGBA_INTEGER; _type = GL_UNSIGNED_INT; break;
				case GL_RGB32UI 			: _format = GL_RGB_INTEGER;  _type = GL_UNSIGNED_INT; break;
				case GL_RG32UI  			: _format = GL_RG_INTEGER;   _type = GL_UNSIGNED_INT; break;
				case GL_R32UI   			: _format = GL_RED_INTEGER;  _type = GL_UNSIGNED_INT; break;

				case GL_RGBA16UI			: _format = GL_RGBA_INTEGER; _type = GL_UNSIGNED_SHORT; break;
				case GL_RGB16UI 			: _format = GL_RGB_INTEGER;  _type = GL_UNSIGNED_SHORT; break;
				case GL_RG16UI  			: _format = GL_RG_INTEGER;   _type = GL_UNSIGNED_SHORT; break;
				case GL_R16UI   			: _format = GL_RED_INTEGER;  _type = GL_UNSIGNED_SHORT; break;
	
				case GL_RGBA8 				: _format = GL_RGBA; _type = GL_UNSIGNED_BYTE; break;
				case GL_RGB8  				: _format = GL_RGB;  _type = GL_UNSIGNED_BYTE; break;
				case GL_RG8  				: _format = GL_RG;   _type = GL_UNSIGNED_BYTE; break;
				case GL_R8    				: _format = GL_RED;  _type = GL_UNSIGNED_BYTE; break;

				case GL_SRGB8_ALPHA8	 	: _format = GL_RGBA; _type = GL_UNSIGNED_BYTE; break;
				case GL_SRGB8  				: _format = GL_RGB;  _type = GL_UNSIGNED_BYTE; break;

				case GL_DEPTH_COMPONENT32F	: _format = GL_DEPTH_COMPONENT;  _type = GL_FLOAT; break;
				case GL_DEPTH32F_STENCIL8	: _format = GL_DEPTH_STENCIL;	 _type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV; break;

				case GL_COMPRESSED_RGB_S3TC_DXT1_EXT 	: _format = GL_RGB;  _type = GL_UNSIGNED_BYTE; break;
				case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 	: _format = GL_RGBA; _type = GL_UNSIGNED_BYTE; break;
				case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 	: _format = GL_RGBA; _type = GL_UNSIGNED_BYTE; break;
				case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 	: _format = GL_RGBA; _type = GL_UNSIGNED_BYTE; break;

				default 		: Error("Automatic conversion : Yet unsupported texture inner format"); assert(false); break;
			}
		}
		//-------------------------------------------------------------------------
		/*void ToInnerFormat(GLenum _format, GLenum _type, GLenum& _innerFormat)
		{
			switch(_format)
			{
				case GL_RGBA :
				{
					switch(_type)
					{
						case GL_FLOAT		:
						case GL_INTEGER		: _type = 
						case GL_UNSIGNED_INT:
						case GL_UNSIGNED_SHORT
						case GL_UNSIGNED_BYTE;
					}
				}
				// TODO ?
				case GL_RGBA32F 			: _format = GL_RGBA; _type = GL_FLOAT; break;
				case GL_RGB32F  			: _format = GL_RGB;  _type = GL_FLOAT; break;
				case GL_RG32F   			: _format = GL_RG;   _type = GL_FLOAT; break;
				case GL_R32F    			: _format = GL_RED;  _type = GL_FLOAT; break;

				case GL_RGBA16F 			: _format = GL_RGBA; _type = GL_FLOAT; break;
				case GL_RGB16F  			: _format = GL_RGB;  _type = GL_FLOAT; break;
				case GL_RG16F   			: _format = GL_RG;   _type = GL_FLOAT; break;
				case GL_R16F    			: _format = GL_RED;  _type = GL_FLOAT; break;

				case GL_RGBA32UI			: _format = GL_RGBA_INTEGER; _type = GL_UNSIGNED_INT; break;
				case GL_RGB32UI 			: _format = GL_RGB_INTEGER;  _type = GL_UNSIGNED_INT; break;
				case GL_RG32UI  			: _format = GL_RG_INTEGER;   _type = GL_UNSIGNED_INT; break;
				case GL_R32UI   			: _format = GL_RED_INTEGER;  _type = GL_UNSIGNED_INT; break;

				case GL_RGBA16UI			: _format = GL_RGBA_INTEGER; _type = GL_UNSIGNED_SHORT; break;
				case GL_RGB16UI 			: _format = GL_RGB_INTEGER;  _type = GL_UNSIGNED_SHORT; break;
				case GL_RG16UI  			: _format = GL_RG_INTEGER;   _type = GL_UNSIGNED_SHORT; break;
				case GL_R16UI   			: _format = GL_RED_INTEGER;  _type = GL_UNSIGNED_SHORT; break;
	
				case GL_RGBA8 				: _format = GL_RGBA; _type = GL_UNSIGNED_BYTE; break;
				case GL_RGB8  				: _format = GL_RGB;  _type = GL_UNSIGNED_BYTE; break;
				case GL_RG8  				: _format = GL_RG;   _type = GL_UNSIGNED_BYTE; break;
				case GL_R8    				: _format = GL_RED;  _type = GL_UNSIGNED_BYTE; break;

				case GL_SRGB8_ALPHA8	 	: _format = GL_RGBA; _type = GL_UNSIGNED_BYTE; break;
				case GL_SRGB8  				: _format = GL_RGB;  _type = GL_UNSIGNED_BYTE; break;

				case GL_DEPTH_COMPONENT32F	: _format = GL_DEPTH_COMPONENT;  _type = GL_FLOAT; break;
				case GL_DEPTH32F_STENCIL8	: _format = GL_DEPTH_STENCIL;	 _type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV; break;

				case GL_COMPRESSED_RGB_S3TC_DXT1_EXT 	: _format = GL_RGB;  _type = GL_UNSIGNED_BYTE; break;
				case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 	: _format = GL_RGBA; _type = GL_UNSIGNED_BYTE; break;
				case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 	: _format = GL_RGBA; _type = GL_UNSIGNED_BYTE; break;
				case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 	: _format = GL_RGBA; _type = GL_UNSIGNED_BYTE; break;

				default 		: Error("Automatic conversion : Yet unsupported texture inner format"); assert(false); break;
			}
		}*/
	}
	//-------------------------------------------------------------------------
	void InnerFormatSplitter(GLenum _innerFormat, GLenum& _format, GLenum& _type)
	{
		ToFormat(_innerFormat, _format, _type);
	}
	//-------------------------------------------------------------------------
	void InnerFormatMerger(GLenum _format, GLenum _type, GLenum& _innerFormat)
	{

	}
	//-------------------------------------------------------------------------
	Texture1D::Texture1D():
	target(GL_TEXTURE_1D),
	format(-1),
	size(0)
	{
		glGenTextures(1,&id);
		glBindTexture(GL_TEXTURE_1D,id);

		SetFiltering(GL_LINEAR, GL_LINEAR);
		SetWrapping(GL_REPEAT);

		glf::CheckError("Texture1D::Texture1D");
	}
	//-------------------------------------------------------------------------
	Texture1D::~Texture1D()
	{
		glDeleteTextures(1,&id);
	}
	//-------------------------------------------------------------------------
	void Texture1D::Allocate(GLenum _innerFormat, int _w, bool _allocateMipmap)
	{
		GLenum fillFormat, fillType;
		ToFormat(_innerFormat,fillFormat,fillType);

		format 	= _innerFormat;
		size 	= _w;
		levels  = _allocateMipmap?MipmapLevels(_w):1;

		glBindTexture(GL_TEXTURE_1D,id); 
		for(int l=0;l<levels;++l)
			glTexImage1D(GL_TEXTURE_1D,l,format,NextMipmapDimension(size,l),0,fillFormat,fillType,NULL);
	}
	//-------------------------------------------------------------------------
	void Texture1D::Fill(GLenum _format, GLenum _type, unsigned char* _data, int _level)
	{
		assert(_level>=0);
		assert(_level<levels);

		// Aligment of 3 is not valid
		// http://www.opengl.org/wiki/Common_Mistakes
		bool setAlignment = _format==GL_RGB || _format==GL_BGR;
		if(setAlignment) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_1D,id);
		glTexSubImage1D(GL_TEXTURE_1D,_level,0,size,_format,_type,_data); 
		if(setAlignment) glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}
	//-------------------------------------------------------------------------
	void Texture1D::Bind(GLint _textureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + _textureUnit);
		glBindTexture(GL_TEXTURE_1D,id);
	}
	//-------------------------------------------------------------------------
	void Texture1D::SetFiltering(GLenum _min, GLenum _mag)
	{
		glTextureParameteriEXT(id, GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, _min);
		glTextureParameteriEXT(id, GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, _mag);
	}
	//-------------------------------------------------------------------------
	void Texture1D::SetWrapping(GLenum _s)
	{
		glTextureParameteriEXT(id, GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, _s);
	}
	//-------------------------------------------------------------------------
	void Texture1D::SetCompare(	GLenum _mode,
								GLenum _func)
	{
		glBindTexture(GL_TEXTURE_1D,id);
		glTextureParameteriEXT(id, GL_TEXTURE_1D, GL_TEXTURE_COMPARE_MODE, _mode);
		glTextureParameteriEXT(id, GL_TEXTURE_1D, GL_TEXTURE_COMPARE_FUNC, _func);
	}
	//-------------------------------------------------------------------------
	void Texture1D::SetAnisotropy(	float _aniso)
	{
		glTextureParameterfEXT(id, GL_TEXTURE_1D, GL_TEXTURE_MAX_ANISOTROPY_EXT, _aniso);
	}


	//-------------------------------------------------------------------------
	Texture2D::Texture2D():
	target(GL_TEXTURE_2D),
	format(-1),
	size(0,0),
	compressed(false)
	{
		glGenTextures(1,&id);
		glBindTexture(GL_TEXTURE_2D,id);
		SetFiltering(GL_LINEAR, GL_LINEAR);
		SetWrapping(GL_REPEAT, GL_REPEAT);
	}
	//-------------------------------------------------------------------------
	Texture2D::~Texture2D()
	{
		glDeleteTextures(1,&id);
	}
	//-------------------------------------------------------------------------
	void Texture2D::Allocate(GLenum _innerFormat, int _w, int _h, bool _allocateMipmap, bool _compressed)
	{
		GLenum fillFormat, fillType;
		ToFormat(_innerFormat,fillFormat,fillType);

		format		= _innerFormat;
		size 		= glm::ivec2(_w,_h);
		levels		= _allocateMipmap?MipmapLevels(std::max(_w,_h)):1;
		compressed	= _compressed;

		glBindTexture(GL_TEXTURE_2D,id); 
		for(int l=0;l<levels;++l)
			if(!compressed)
				glTexImage2D(GL_TEXTURE_2D,l,format,NextMipmapDimension(size.x, l),NextMipmapDimension(size.y, l),0,fillFormat,fillType,NULL);
			else
				glCompressedTexImage2D(GL_TEXTURE_2D,l,format,NextMipmapDimension(size.x, l),NextMipmapDimension(size.y, l),0,0,NULL);
	}
	//-------------------------------------------------------------------------
	void Texture2D::Fill(GLenum _format, GLenum _type, unsigned char* _data, int _level)
	{
		assert(_level>=0);
		assert(_level<levels);
		assert(!compressed);

		// Aligment of 3 is not valid
		// http://www.opengl.org/wiki/Common_Mistakes
		bool setAlignment = _format==GL_RGB || _format==GL_BGR;
		if(setAlignment) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D,id);
		glTexSubImage2D(GL_TEXTURE_2D,_level,0,0,size.x,size.y,_format,_type,_data); 
		if(setAlignment) glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}
	//-------------------------------------------------------------------------
	void Texture2D::FillCompressed(GLenum _format, std::size_t _dataSize, unsigned char* _data, int _level)
	{
		assert(_level>=0);
		assert(_level<levels);
		assert(compressed);

		// Aligment of 3 is not valid
		// http://www.opengl.org/wiki/Common_Mistakes
		bool setAlignment = _format==GL_RGB || _format==GL_BGR;
		if(setAlignment) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D,id);
		glCompressedTexSubImage2D(GL_TEXTURE_2D,_level,0,0,size.x,size.y,_format,_dataSize,_data);
		if(setAlignment) glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}
	//-------------------------------------------------------------------------
	void Texture2D::Bind(GLint _textureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + _textureUnit);
		glBindTexture(GL_TEXTURE_2D,id);
	}
	//-------------------------------------------------------------------------
	void Texture2D::SetFiltering(GLenum _min, GLenum _mag)
	{
		glTextureParameteriEXT(id, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _min);
		glTextureParameteriEXT(id, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _mag);
	}
	//-------------------------------------------------------------------------
	void Texture2D::SetWrapping(GLenum _s, GLenum _t)
	{
		glTextureParameteriEXT(id, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _s);
		glTextureParameteriEXT(id, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _t);
	}
	//-------------------------------------------------------------------------
	void Texture2D::SetCompare(	GLenum _mode,
								GLenum _func)
	{
		glBindTexture(GL_TEXTURE_2D,id);
		glTextureParameteriEXT(id, GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, _mode);
		glTextureParameteriEXT(id, GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, _func);
	}
	//-------------------------------------------------------------------------
	void Texture2D::SetAnisotropy(	float _aniso)
	{
		glTextureParameterfEXT(id, GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, _aniso);
	}

	//-------------------------------------------------------------------------
	Texture3D::Texture3D():
	target(GL_TEXTURE_3D),
	format(-1),
	size(0,0,0),
	compressed(false)
	{
		glGenTextures(1,&id);
		glBindTexture(GL_TEXTURE_3D,id);
		SetFiltering(GL_LINEAR, GL_LINEAR);
		SetWrapping(GL_REPEAT, GL_REPEAT, GL_REPEAT);
	}
	//-------------------------------------------------------------------------
	Texture3D::~Texture3D()
	{
		glDeleteTextures(1,&id);
	}
	//-------------------------------------------------------------------------
	void Texture3D::Allocate(GLenum _innerFormat, int _w, int _h, int _d, bool _allocateMipmap, bool _compressed)
	{
		GLenum fillFormat, fillType;
		ToFormat(_innerFormat,fillFormat,fillType);

		format		= _innerFormat;
		size 		= glm::ivec3(_w,_h,_d);
		levels		= _allocateMipmap?MipmapLevels(std::max(_d,std::max(_w,_h))):1;
		compressed	= _compressed;

		glBindTexture(GL_TEXTURE_3D,id); 
		for(int l=0;l<levels;++l)
			if(!compressed)
				glTexImage3D(GL_TEXTURE_3D,l,format,NextMipmapDimension(size.x, l),NextMipmapDimension(size.y, l),NextMipmapDimension(size.z, l),0,fillFormat,fillType,NULL);
			else
				glCompressedTexImage3D(GL_TEXTURE_3D,l,format,NextMipmapDimension(size.x, l),NextMipmapDimension(size.y, l),NextMipmapDimension(size.z, l),0,0,NULL);
	}
	//-------------------------------------------------------------------------
	void Texture3D::Fill(GLenum _format, GLenum _type, unsigned char* _data, int _level)
	{
		assert(_level>=0);
		assert(_level<levels);
		assert(!compressed);

		// Aligment of 3 is not valid
		// http://www.opengl.org/wiki/Common_Mistakes
		bool setAlignment = _format==GL_RGB || _format==GL_BGR;
		if(setAlignment) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_3D,id);
		glTexSubImage3D(GL_TEXTURE_3D,_level,0,0,0,size.x,size.y,size.z,_format,_type,_data); 
		if(setAlignment) glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}
	//-------------------------------------------------------------------------
	void Texture3D::FillCompressed(GLenum _format, std::size_t _dataSize, unsigned char* _data, int _level)
	{
		assert(_level>=0);
		assert(_level<levels);
		assert(compressed);

		// Aligment of 3 is not valid
		// http://www.opengl.org/wiki/Common_Mistakes
		bool setAlignment = _format==GL_RGB || _format==GL_BGR;
		if(setAlignment) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_3D,id);
		glCompressedTexSubImage3D(GL_TEXTURE_3D,_level,0,0,0,size.x,size.y,size.z,_format,_dataSize,_data);
		if(setAlignment) glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}
	//-------------------------------------------------------------------------
	void Texture3D::Bind(GLint _textureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + _textureUnit);
		glBindTexture(GL_TEXTURE_3D,id);
	}
	//-------------------------------------------------------------------------
	void Texture3D::SetFiltering(GLenum _min, GLenum _mag)
	{
		glTextureParameteriEXT(id, GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, _min);
		glTextureParameteriEXT(id, GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, _mag);
	}
	//-------------------------------------------------------------------------
	void Texture3D::SetWrapping(GLenum _s, GLenum _t, GLenum _r)
	{
		glTextureParameteriEXT(id, GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, _s);
		glTextureParameteriEXT(id, GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, _t);
		glTextureParameteriEXT(id, GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, _r);
	}
	//-------------------------------------------------------------------------
	void Texture3D::SetCompare(	GLenum _mode,
								GLenum _func)
	{
		glBindTexture(GL_TEXTURE_3D,id);
		glTextureParameteriEXT(id, GL_TEXTURE_3D, GL_TEXTURE_COMPARE_MODE, _mode);
		glTextureParameteriEXT(id, GL_TEXTURE_3D, GL_TEXTURE_COMPARE_FUNC, _func);
	}
	//-------------------------------------------------------------------------
	void Texture3D::SetAnisotropy(	float _aniso)
	{
		glTextureParameterfEXT(id, GL_TEXTURE_3D, GL_TEXTURE_MAX_ANISOTROPY_EXT, _aniso);
	}

	//-------------------------------------------------------------------------
	TextureArray2D::TextureArray2D():
	target(GL_TEXTURE_2D_ARRAY),
	format(-1),
	size(0,0)
	{
		glGenTextures(1,&id);
		glBindTexture(GL_TEXTURE_2D_ARRAY,id);
		SetFiltering(GL_LINEAR, GL_LINEAR);
		SetWrapping(GL_REPEAT, GL_REPEAT);
	}
	//-------------------------------------------------------------------------
	TextureArray2D::~TextureArray2D()
	{
		glDeleteTextures(1,&id);
	}
	//-------------------------------------------------------------------------
	void TextureArray2D::Allocate(GLenum _innerFormat, int _w, int _h, int _layers, bool _allocateMipmap)
	{
		GLenum fillFormat, fillType;
		ToFormat(_innerFormat,fillFormat,fillType);

		format 	= _innerFormat;
		size 	= glm::ivec2(_w,_h);
		layers	= _layers;
		levels  = _allocateMipmap?MipmapLevels(std::max(_w,_h)):1;

		glBindTexture(GL_TEXTURE_2D_ARRAY,id);
		for(int l=0;l<levels;++l)
			glTexImage3D(GL_TEXTURE_2D_ARRAY,l,format,NextMipmapDimension(size.x, l),NextMipmapDimension(size.y, l),layers,0,fillFormat,fillType,NULL);
	}
	//-------------------------------------------------------------------------
	void TextureArray2D::Fill(int _layer, GLenum _format, GLenum _type, unsigned char* _data, int _level)
	{
		assert(_level>=0);
		assert(_level<levels);

		// Aligment of 3 is not valid
		// http://www.opengl.org/wiki/Common_Mistakes
		bool setAlignment = _format==GL_RGB || _format==GL_BGR;
		if(setAlignment) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D_ARRAY,id);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,_level,0,0,_layer, size.x, size.y,1,_format,_type,_data); // Prob to fill mipmap ?
		if(setAlignment) glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}
	//-------------------------------------------------------------------------
	void TextureArray2D::Bind(GLint _textureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + _textureUnit);
		glBindTexture(GL_TEXTURE_2D_ARRAY,id);
	}
	//-------------------------------------------------------------------------
	void TextureArray2D::SetFiltering(GLenum _min, GLenum _mag)
	{
		glTextureParameteriEXT(id, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, _min);
		glTextureParameteriEXT(id, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, _mag);
	}
	//-------------------------------------------------------------------------
	void TextureArray2D::SetWrapping(GLenum _s, GLenum _t)
	{
		glTextureParameteriEXT(id, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, _s);
		glTextureParameteriEXT(id, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, _t);
	}
	//-------------------------------------------------------------------------
	void TextureArray2D::SetCompare(	GLenum _mode,
										GLenum _func)
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY,id);
		glTextureParameteriEXT(id, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, _mode);
		glTextureParameteriEXT(id, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, _func);
	}

	//-------------------------------------------------------------------------
	TextureCube::TextureCube():
	target(GL_TEXTURE_CUBE_MAP),
	format(-1),
	size(0,0)
	{
		glGenTextures(1,&id);
		glBindTexture(GL_TEXTURE_CUBE_MAP,id);
		SetFiltering(GL_LINEAR, GL_LINEAR);
		SetWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	}
	//-------------------------------------------------------------------------
	TextureCube::~TextureCube()
	{
		glDeleteTextures(1,&id);
	}
	//-------------------------------------------------------------------------
	void TextureCube::Allocate(GLenum _innerFormat, int _w, bool _allocateMipmap)
	{
		GLenum fillFormat,fillType;
		ToFormat(_innerFormat,fillFormat,fillType);

		format 	= _innerFormat;
		size 	= glm::ivec2(_w,_w);
		levels  = _allocateMipmap?MipmapLevels(_w):1;

		glBindTexture(GL_TEXTURE_CUBE_MAP,id);
		for(int l=0;l<levels;++l)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,l,format,NextMipmapDimension(size.x, l),NextMipmapDimension(size.y, l),0,fillFormat,fillType,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,l,format,NextMipmapDimension(size.x, l),NextMipmapDimension(size.y, l),0,fillFormat,fillType,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,l,format,NextMipmapDimension(size.x, l),NextMipmapDimension(size.y, l),0,fillFormat,fillType,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,l,format,NextMipmapDimension(size.x, l),NextMipmapDimension(size.y, l),0,fillFormat,fillType,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,l,format,NextMipmapDimension(size.x, l),NextMipmapDimension(size.y, l),0,fillFormat,fillType,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,l,format,NextMipmapDimension(size.x, l),NextMipmapDimension(size.y, l),0,fillFormat,fillType,NULL);
		}
	}
	//-------------------------------------------------------------------------
	void TextureCube::Fill(GLenum _face, GLenum _format, GLenum _type, unsigned char* _data, int _level)
	{
		assert(_level>=0);
		assert(_level<levels);

		// Aligment of 3 is not valid
		// http://www.opengl.org/wiki/Common_Mistakes
		bool setAlignment = _format==GL_RGB || _format==GL_BGR;
		if(setAlignment) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_CUBE_MAP,id);
		glTexSubImage2D(_face,_level,0,0,size.x,size.y,_format,_type,_data); // Prob to fill mipmap ?
		if(setAlignment) glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}
	//-------------------------------------------------------------------------
	void TextureCube::Bind(GLint _textureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + _textureUnit);
		glBindTexture(GL_TEXTURE_CUBE_MAP,id);
	}
	//-------------------------------------------------------------------------
	void TextureCube::SetFiltering(GLenum _min, GLenum _mag)
	{
		glTextureParameteriEXT(id, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, _min);
		glTextureParameteriEXT(id, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, _mag);
	}
	//-------------------------------------------------------------------------
	void TextureCube::SetWrapping(GLenum _s, GLenum _t, GLenum _r)
	{
		glTextureParameteriEXT(id, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, _s);
		glTextureParameteriEXT(id, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, _t);
		glTextureParameteriEXT(id, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, _r);
	}
	//-------------------------------------------------------------------------
	#if ENABLE_SAMPLER_OBJECT
	Sampler::Sampler()
	{
		glGenSamplers(1,&id);
		texUnit 	= 0;
		wrapS 		= GL_REPEAT;//GL_CLAMP_TO_EDGE;
		wrapT 		= GL_REPEAT;//GL_CLAMP_TO_EDGE;
		wrapR 		= GL_REPEAT;//GL_CLAMP_TO_EDGE;
		minFilter	= GL_NEAREST_MIPMAP_LINEAR;
		magFilter	= GL_LINEAR;
		borderColor = glm::vec4(0,0,0,1);
		minLOD		= -1000.f;
		maxLOD		=  1000.f;
		lodBias		=  0.f;
		compareMode = GL_NONE;
		compareFunc = GL_LEQUAL;
	}
	//-------------------------------------------------------------------------
	Sampler::~Sampler()
	{
		glDeleteSamples(1,&id);
	}
	//-------------------------------------------------------------------------
	void Sampler::Bind(GLint _textureUnit) const
	{
		glBindSampler(_textureUnit,id);

		glSamplerParamterfv(id,GL_TEXTURE_WRAP_S,wrapS);
		glSamplerParamterfv(id,GL_TEXTURE_WRAP_T,wrapT);
		glSamplerParamterfv(id,GL_TEXTURE_WRAP_R,wrapR);

		glSamplerParamterfv(id,GL_TEXTURE_MIN_FILTER,wrapS);
		glSamplerParamterfv(id,GL_TEXTURE_MAG_FILTER,wrapS);
		glSamplerParamterfv(id,GL_TEXTURE_BORDER_COLOR,&(borderColor[0]));

		glSamplerParamterfv(id,GL_TEXTURE_MIN_LOD,minLOD);
		glSamplerParamterfv(id,GL_TEXTURE_MAX_LOD,maxLOD);
		glSamplerParamterfv(id,GL_TEXTURE_LOD_BIAS,lodBias);

		glSamplerParamterfv(id,GL_TEXTURE_COMPARE_MODE,compareMode);
		glSamplerParamterfv(id,GL_TEXTURE_COMPARE_FUNC,compareFunc);
	}
	#endif
}

