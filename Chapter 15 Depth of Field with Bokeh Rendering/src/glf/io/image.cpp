//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <glf/io/image.hpp>
#include <glf/utils.hpp>
#include <string>
#include <cstring>
#include <iostream>
#include <IL/il.h>
#include <IL/ilu.h>

#ifdef ENABLE_OPEN_EXR
	#include <OpenEXR/ImfInputFile.h>
	#include <OpenEXR/ImfOutputFile.h>
	#include <OpenEXR/ImfChannelList.h>
	#include <OpenEXR/ImfFrameBuffer.h>
	#include <OpenEXR/ImfFloatAttribute.h>
	#include <OpenEXR/ImfChannelListAttribute.h>
	#include <OpenEXR/half.h>
#endif

namespace glf
{
	namespace io
	{
		//---------------------------------------------------------------------
		#ifdef ENABLE_OPEN_EXR
		void SaveTexture(	const std::string& _filename,
							float* _data,
							int _w,
							int _h,
							bool _verbose)
		{
			int xRes 	  = _w;
			int yRes 	  = _h;
			int xOffset   = 0;
			int yOffset   = 0;
			int nChannels = 4;

			Imf::Header header(xRes, yRes);
			Imath::Box2i dataWindow(Imath::V2i(xOffset, yOffset), Imath::V2i(xOffset + xRes - 1, yOffset + yRes - 1));
			header.dataWindow() = dataWindow;

			header.channels().insert("R", Imf::Channel (Imf::HALF));
			header.channels().insert("G", Imf::Channel (Imf::HALF));
			header.channels().insert("B", Imf::Channel (Imf::HALF));
			header.channels().insert("A", Imf::Channel (Imf::HALF));

			::half *hchannels   = new ::half[nChannels * xRes * yRes];
			for (int y = 0; y < yRes; ++y)
			for (int x = 0; x < xRes; ++x)
			for (int c = 0; c < nChannels; ++c)
			{
				int iSrc = c + x*nChannels + (yRes-1-y)*xRes*nChannels; 
				int iDst = c + x*nChannels + y*xRes*nChannels;
				hchannels[iDst] = _data[iSrc];
			}

			Imf::FrameBuffer fb;
			fb.insert("R", Imf::Slice(Imf::HALF, (char *)hchannels,						nChannels*sizeof(::half), nChannels*xRes*sizeof(::half)));
			fb.insert("G", Imf::Slice(Imf::HALF, (char *)hchannels+1*sizeof(::half),	nChannels*sizeof(::half), nChannels*xRes*sizeof(::half)));
			fb.insert("B", Imf::Slice(Imf::HALF, (char *)hchannels+2*sizeof(::half),	nChannels*sizeof(::half), nChannels*xRes*sizeof(::half)));
			fb.insert("A", Imf::Slice(Imf::HALF, (char *)hchannels+3*sizeof(::half),	nChannels*sizeof(::half), nChannels*xRes*sizeof(::half)));

			Imf::OutputFile file(_filename.c_str(), header);
			file.setFrameBuffer(fb);
			try 
			{
				file.writePixels(yRes);
			}
			catch (const std::exception &e) 
			{
				Error("Unable to write image file \"%s\": %s", _filename.c_str(), e.what());
				assert(false);
			}

			delete[] hchannels;
		}
		#endif
		//---------------------------------------------------------------------
		void LoadTexture(	const std::string& _filename,
							Texture2D& _texture,
							bool _srgb,
							bool _allocateMipmap,
							bool _verbose)
		{
			try
			{
				ilInit();
				
				ILuint imgH;
				ilGenImages(1, &imgH);
				ilBindImage(imgH);
				if(!ilLoadImage((const ILstring)_filename.c_str()))
				{
					Error("Load image error : file does not exist (%s)",_filename.c_str());
				}
				
				if(_verbose)
				{
					Info("Load image : %s",_filename.c_str());
				}

				// Convert all to RGBA. TODO Need improvement ...
				GLenum format;
				bool convert = false;
				ILenum target;
				switch(ilGetInteger(IL_IMAGE_FORMAT))
				{
					case IL_RGB  			: format = GL_RGBA; convert=true; target = IL_RGBA; break;
					case IL_RGBA 			: format = GL_RGBA; break;
					case IL_BGR  			: format = GL_RGBA; convert=true; target = IL_RGBA; break;
					case IL_BGRA 			: format = GL_RGBA; convert=true; target = IL_RGBA; break;
					case IL_LUMINANCE 		: format = GL_RGBA; convert=true; target = IL_RGBA; break;
					case IL_COLOUR_INDEX	: format = GL_RGBA; convert=true; target = IL_RGBA; break;
					case IL_ALPHA			: format = GL_RGBA; convert=true; target = IL_RGBA; break;
					case IL_LUMINANCE_ALPHA	: format = GL_RGBA; convert=true; target = IL_RGBA; break;
					default 				: Error("Load image error : unsupported format (%s)",_filename.c_str());
				}

				GLenum type;
				switch(ilGetInteger(IL_IMAGE_TYPE))
				{
					case IL_UNSIGNED_BYTE	: type = GL_UNSIGNED_BYTE; break;
					case IL_FLOAT			: type = GL_FLOAT; break;
					//case IL_BYTE			: type = GL_BYTE; break;
					//case IL_SHORT			: type = GL_SHORT; break;
					//case IL_UNSIGNED_SHORT: type = GL_UNSIGNED_SHORT; break;
					//case IL_INT			: type = GL_INTEGER; break;
					//case IL_UNSIGNED_INT	: type = GL_UNSIGNED_INT; break;
					//case IL_DOUBLE		: Error("Load image error : double data are not supported (%s)",_filename.c_str()); break;
					default 				: Error("Load image error : unsupported data (%s)",_filename.c_str());
				}

				if(convert)
					 ilConvertImage(target, ilGetInteger(IL_IMAGE_TYPE));

				// Flip image
				ILinfo ImageInfo;
				iluGetImageInfo(&ImageInfo);
				if( ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT )
				{
					iluFlipImage();
					if(_verbose) Info("Flip image");
				}

				switch(type)
				{
					case GL_UNSIGNED_BYTE	:
					{
						if(_srgb)
						{
							_texture.Allocate( GL_SRGB8_ALPHA8, ilGetInteger(IL_IMAGE_WIDTH),ilGetInteger(IL_IMAGE_HEIGHT),_allocateMipmap);
							if(_verbose) Info("Allocate texture - format:GL_SRGB8_ALPHA8, w:%d, h:%d, mipmap:%s",ilGetInteger(IL_IMAGE_WIDTH),ilGetInteger(IL_IMAGE_HEIGHT), (_allocateMipmap?"TRUE":"FALSE") );
						}
						else
						{
							_texture.Allocate( GL_RGBA8, ilGetInteger(IL_IMAGE_WIDTH),ilGetInteger(IL_IMAGE_HEIGHT),_allocateMipmap);
							if(_verbose) Info("Allocate texture - format:GL_RGBA8, w:%d, h:%d, mipmap:%s",ilGetInteger(IL_IMAGE_WIDTH),ilGetInteger(IL_IMAGE_HEIGHT), (_allocateMipmap?"TRUE":"FALSE") );
						}
					}
					break;
					case GL_FLOAT			:
					{
						if(_srgb)
							Warning("Try to convert to SRGB, but texture format is not compatible");
						_texture.Allocate( GL_RGBA32F, ilGetInteger(IL_IMAGE_WIDTH),ilGetInteger(IL_IMAGE_HEIGHT),_allocateMipmap);
						if(_verbose) Info("Allocate texture - format:GL_RGBA32F, w:%d, h:%d, mipmap:%s",ilGetInteger(IL_IMAGE_WIDTH),ilGetInteger(IL_IMAGE_HEIGHT), (_allocateMipmap?"TRUE":"FALSE") );
					}
					break;
					default 				:
					{
						Error("Load image error : unsupported data (%s)",_filename.c_str());
					}
				}
				_texture.Fill(format,type,ilGetData());

				ilDeleteImages(1, &imgH);
				ilShutDown();
			}
			catch (const std::exception &e) 
			{
				Error("Unable to read image file \"%s\": %s",_filename.c_str(),e.what());
			}
		}
		//----------------------------------------------------------------------
		void SaveTexture(	const std::string& _filename,
							Texture2D& _texture,
							bool _verbose)
		{
			#ifdef ENABLE_OPEN_EXR
			float* data = new float[_texture.size.x * _texture.size.y * 4];
			glBindTexture(_texture.target,_texture.id);
			glGetTexImage(_texture.target,0,GL_RGBA,GL_FLOAT,data);
			SaveTexture(_filename,data,_texture.size.x,_texture.size.y,_verbose);
			delete[] data;
			#else
			try
			{
				ilInit();
				
				ILuint imgH;
				ilGenImages(1, &imgH);
				ilBindImage(imgH);

				float* data = new float[_texture.size.x * _texture.size.y * 4];
				glBindTexture(_texture.target,_texture.id);
				glGetTexImage(_texture.target,0,GL_RGBA,GL_FLOAT,data);

				ILboolean result;
				result = ilTexImage(_texture.size.x, _texture.size.y, 1, 4, IL_RGBA, IL_FLOAT, data);
				assert(result);

				ilEnable(IL_FILE_OVERWRITE);
//				result = ilSave(IL_EXR, _filename.c_str());
//				result = ilSave(IL_TYPE_UNKNOWN, _filename.c_str());
				#if WIN32
				result = ilSaveImage(s2ws(_filename).c_str());			
				#else
				result = ilSaveImage(_filename.c_str());
				#endif

				if(!result)
				{
					ILenum errorID = ilGetError();
					if(errorID != IL_NO_ERROR)
					{
						Error("Save image error :  %s",_filename.c_str());
						Error("Error ID :  %d",errorID);
					}
				}
				assert(result);

				delete[] data;

				if(_verbose)
				{
					Info("Save image : %s",_filename.c_str());
				}

				ilDeleteImages(1, &imgH);
				ilShutDown();
			}
			catch (const std::exception &e) 
			{
				Error("Unable to write texture \"%s\": %s",_filename.c_str(),e.what());
			}
			#endif
		}
		//----------------------------------------------------------------------
		void SaveTexture(	const std::string& _filename,
							TextureArray2D& _texture,
							bool _verbose)
		{
			#ifdef ENABLE_OPEN_EXR
			float* data = new float[_texture.size.x * _texture.size.y * _texture.layers * 4];
			glBindTexture(_texture.target,_texture.id);
			glGetTexImage(_texture.target,0,GL_RGBA,GL_FLOAT,data);

			for(int l=0;l<_texture.layers;++l)
			{
				std::stringstream out;
				out << _filename << "." << l << ".exr";
				SaveTexture(out.str(),data+(l * _texture.size.x * _texture.size.y * 4),_texture.size.x,_texture.size.y,_verbose);
			}
			delete[] data;
			#endif
		}
		//----------------------------------------------------------------------
		// PPM saver code
		//std::ofstream out("out.ppm",std::ios::binary);
		//out << 'P' << '6' << ' ' << '1' << '0' << '2' << '4' << ' ' << '1' << '0' << '2' << '4' << ' ' << '2' << '5' << '5' << '\n';
		//for(int y=0;y<1024;++y)
		//for(int x=0;x<1024;++x)
		//{
		//	out.write((char*)&(data[x+y*1024]),sizeof(unsigned char));// << " " << data[x+y*1024] << " "<< data[x+y*1024] << "\n";
		//	out.write((char*)&(data[x+y*1024]),sizeof(unsigned char));
		//	out.write((char*)&(data[x+y*1024]),sizeof(unsigned char));
		//}
		//----------------------------------------------------------------------
	}
}
