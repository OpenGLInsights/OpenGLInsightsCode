#ifndef GLF_IO_IMAGE_HPP
#define GLF_IO_IMAGE_HPP

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/texture.hpp>
#include <string>

namespace glf
{
	namespace io
	{
		//----------------------------------------------------------------------
		void LoadTexture(	const std::string& _filename,
							Texture2D& _texture,
							bool _srgb,
							bool _allocateMipmap,
							bool _verbose=true);
		//----------------------------------------------------------------------
		void SaveTexture(	const std::string& _filename,
							Texture2D& _texture,
							bool _verbose=true);
		//----------------------------------------------------------------------
		void SaveTexture(	const std::string& _filename,
							TextureArray2D& _texture,
							bool _verbose=true);
		//----------------------------------------------------------------------
	}
}

#endif
