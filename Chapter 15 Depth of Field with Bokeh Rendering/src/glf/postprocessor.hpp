#ifndef GLF_POSTPROCESSOR_HPP
#define GLF_POSTPROCESSOR_HPP

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/buffer.hpp>
#include <glf/wrapper.hpp>
#include <glf/texture.hpp>
#include <glf/pass.hpp>

namespace glf
{
	//--------------------------------------------------------------------------
	class PostProcessor
	{
	private:
					PostProcessor(		const PostProcessor&);
		PostProcessor operator=(		const PostProcessor&);

	public:
					PostProcessor(		unsigned int _width, 
										unsigned int _height);
					~PostProcessor(		);
		void 		Draw(				const Texture2D& 	_colorTex,
										float 				_toneExposure,
										const RenderTarget& _renderTarget);

	private:
		struct ToneMapping
		{
										ToneMapping();
			GLint 						luminanceTexUnit;
			GLint 						colorTexUnit;
			GLint 						exposureVar;
			GLint 						keyValueVar;
			Program						program;
		};

		ToneMapping						toneMapping;
	};
	//--------------------------------------------------------------------------
}
#endif

