#ifndef GLF_TEXTURE_HPP
#define GLF_TEXTURE_HPP

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/utils.hpp>

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
namespace glf
{
	//--------------------------------------------------------------------------
	class Texture1D
	{
	public:
		GLuint 				id;
		GLenum 				target;
		GLenum 				format;
		int 				size;
		int 				levels;

			 Texture1D(		);
			~Texture1D(		);
		void Allocate(		GLenum _innferFormat, 
							int _w, 
							bool _allocateMipmap=false);
		void Fill(			GLenum _format, 
							GLenum _type, 
							unsigned char* _data, 
							int _level=0);
		void SetFiltering(	GLenum _min, 
							GLenum _mag);
		void SetWrapping(	GLenum _s);
		void SetCompare(	GLenum _mode,
							GLenum _func);
		void SetAnisotropy( float  _aniso);
		void Bind(			GLint  _textureUnit) const;
	private:
			 Texture1D( 	const Texture1D&);
			 Texture1D& operator=(const Texture1D&);
	};
	//--------------------------------------------------------------------------
	class Texture2D
	{
	public:
		GLuint 				id;
		GLenum 				target;
		GLenum 				format;
		glm::ivec2 			size;
		int 				levels;
		bool 				compressed;

			 Texture2D(		);
			~Texture2D(		);
		void Allocate(		GLenum _innferFormat, 
							int _w, 
							int _h,
							bool _allocateMipmap=false,
							bool _compressed=false);
		void Fill(			GLenum _format, 
							GLenum _type, 
							unsigned char* _data, 
							int _level=0);
		void FillCompressed(GLenum _format, 
							std::size_t _dataSize, 
							unsigned char* _data, 
							int _level);
		void SetFiltering(	GLenum _min, 
							GLenum _mag);
		void SetWrapping(	GLenum _s, 
							GLenum _t);
		void SetCompare(	GLenum _mode,
							GLenum _func);
		void SetAnisotropy( float  _aniso);
		void Bind(			GLint  _textureUnit) const;
	private:
			 Texture2D( const Texture2D&);
			 Texture2D& operator=(const Texture2D&);
	};
	//--------------------------------------------------------------------------
	class Texture3D
	{
	public:
		GLuint 				id;
		GLenum 				target;
		GLenum 				format;
		glm::ivec3 			size;
		int 				levels;
		bool 				compressed;

			 Texture3D(		);
			~Texture3D(		);
		void Allocate(		GLenum _innferFormat, 
							int _w, 
							int _h,
							int _d,
							bool _allocateMipmap=false,
							bool _compressed=false);
		void Fill(			GLenum _format, 
							GLenum _type, 
							unsigned char* _data, 
							int _level=0);
		void FillCompressed(GLenum _format, 
							std::size_t _dataSize, 
							unsigned char* _data, 
							int _level);
		void SetFiltering(	GLenum _min, 
							GLenum _mag);
		void SetWrapping(	GLenum _s, 
							GLenum _t,
							GLenum _r);
		void SetCompare(	GLenum _mode,
							GLenum _func);
		void SetAnisotropy( float  _aniso);
		void Bind(			GLint  _textureUnit) const;
	private:
			 Texture3D( const Texture3D&);
			 Texture3D& operator=(const Texture3D&);
	};
	//--------------------------------------------------------------------------
	struct TextureArray2D
	{
		GLuint 				id;
		GLenum 				target;
		GLenum 				format;
		glm::ivec2 			size;
		int					layers;
		int 				levels;

			 TextureArray2D();
			~TextureArray2D(		);
		void Allocate(		GLenum _innferFormat, 
							int _w, 
							int _h,
							int _layers,
							bool _allocateMipmap=false);
		void Fill(			int _layer, 
							GLenum _format, 
							GLenum _type, 
							unsigned char* _data, 
							int _level=0);
		void SetFiltering(	GLenum _min, 
							GLenum _mag);
		void SetWrapping(	GLenum _s, 
							GLenum _t);
		void SetCompare(	GLenum _mode,
							GLenum _func);
		void Bind(			GLint _textureUnit) const;
	private:
			 TextureArray2D( const TextureArray2D&);
			 TextureArray2D& operator=(const TextureArray2D&);
	};
	//--------------------------------------------------------------------------
	class TextureCube
	{
	public:
		GLuint 				id;
		GLenum 				target;
		GLenum 				format;
		glm::ivec2	 		size;
		int 				levels;

			 TextureCube(	);
			~TextureCube(	);
		void Allocate(		GLenum _innferFormat, 
							int _w, 
							bool _allocateMipmap=false);
		void Fill(			GLenum _face, 
							GLenum _format, 
							GLenum _type, 
							unsigned char* _data, 
							int _level=0);
		void SetFiltering(	GLenum _min, 
							GLenum _mag);
		void SetWrapping(	GLenum _s, 
							GLenum _t,
							GLenum _r);
		void Bind(			GLint _textureUnit) const;
	private:
			 TextureCube( const TextureCube&);
			 TextureCube& operator=(const TextureCube&);
	};
	//--------------------------------------------------------------------------
	struct Sampler
	{
		//----------------------------------------------------------------------
		GLuint 				id;
		GLuint 				texUnit;
		GLenum 				wrapS;
		GLenum 				wrapT;
		GLenum 				wrapR;
		GLenum 				minFilter;
		GLenum 				magFilter;
		glm::vec4 			borderColor;
		GLfloat 			minLOD;
		GLfloat 			maxLOD;
		GLfloat 			lodBias;
		GLenum 				compareMode;
		GLenum 				compareFunc;
		//----------------------------------------------------------------------
			 Sampler(		);
			~Sampler(		);
		void Bind(			GLint _textureUnit) const;
	};

	//-------------------------------------------------------------------------
	// Decompose an inner format into a format and a type.
	// Format and type are de closest decomposition
	//-------------------------------------------------------------------------
	void InnerFormatSplitter(	GLenum _innerFormat, 
								GLenum& _format, 
								GLenum& _type);
}

#endif
