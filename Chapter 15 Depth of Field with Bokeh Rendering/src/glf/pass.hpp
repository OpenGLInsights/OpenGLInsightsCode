#ifndef GLF_PASS_HPP
#define GLF_PASS_HPP

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/utils.hpp>
#include <glf/wrapper.hpp>
#include <glf/scene.hpp>

namespace glf
{
	//--------------------------------------------------------------------------
	struct RenderSurface
	{
	private:
					RenderSurface(		const RenderSurface&);
		RenderSurface&	operator=(		const RenderSurface&);
	public:

		struct RegularRenderer
		{
										RegularRenderer():program("Surface::Regular"){}
			GLint 						textureUnit;
			GLint 						levelVar;
			Program 					program;
		};

		struct ArrayRenderer
		{
										ArrayRenderer():program("Surface::Array"){}
			GLint 						textureUnit;
			GLint 						layerVar;
			GLint 						levelVar;
			Program 					program;
		};

		ArrayRenderer					arrayRenderer;
		RegularRenderer					regularRenderer;
		glm::vec2						frameSize;
		VertexBuffer2F					vbo;
		VertexArray						vao;

					RenderSurface(		unsigned int _width, 
										unsigned int _height);
		void 		Draw(				const Texture2D& _texture,
										int 			 _level=0);
		void 		Draw(				const TextureArray2D& _texture,
										int 			_layer,
										int 			_level=0);
	};
	//--------------------------------------------------------------------------
	struct RenderTarget
	{
	private:
					RenderTarget(		const RenderTarget&);
		RenderTarget& operator=(		const RenderTarget&);
	public:
		glm::vec2						frameSize;
		Texture2D						texture;
		GLuint							framebuffer;
		VertexBuffer2F					vbo;
		VertexArray						vao;

					RenderTarget(		unsigned int _width, 
										unsigned int _height);
				   ~RenderTarget(		);
		void		Bind(				) const;
		void		Unbind(				) const;
		void		Draw(				) const;
		void		AttachDepthStencil(	const Texture2D& _depthStencilTex);
	};
	//--------------------------------------------------------------------------
}
#endif

