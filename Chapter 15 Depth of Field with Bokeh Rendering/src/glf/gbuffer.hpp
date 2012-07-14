#ifndef GLF_GBUFFER_HPP
#define GLF_GBUFFER_HPP

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/utils.hpp>
#include <glf/wrapper.hpp>
#include <glf/scene.hpp>

namespace glf
{
	//--------------------------------------------------------------------------
	struct GBuffer
	{
	private:
					GBuffer(			const GBuffer&);
		GBuffer&	operator=(			const GBuffer&);
	public:
					GBuffer(			unsigned int _width, 
										unsigned int _height);
				   ~GBuffer(			);
		void 		Draw(				const glm::mat4& _projection,
										const glm::mat4& _view,
										const SceneManager& _scene);

		// Regular mesh renderer
		struct RegularRenderer
		{
										RegularRenderer():program("GBuffer::Regular"){}
			Program 					program;
			GLint 	 					diffuseTexUnit;
			GLint 	 					normalTexUnit;
			GLint	 					roughnessVar;
			GLint	 					specularityVar;
			GLint	 					transformVar;
			GLint	 					modelVar;
		};

		// Terrain mesh renderer
		struct TerrainRenderer
		{
										TerrainRenderer():program("GBuffer::Terrain"){}
			Program 					program;
			GLint						diffuseTexUnit;
			GLint						normalTexUnit;
			GLint						heightTexUnit;
			GLint	 					roughnessVar;
			GLint	 					specularityVar;
			GLint 						transformVar;

			GLint 						tileSizeVar;
			GLint 						tileCountVar;
			GLint 						tileOffsetVar;
			GLint 						projFactorVar;
			GLint 						tessFactorVar;
			GLint						heightFactorVar;
			GLint						tileFactorVar;
		};

		// Resources
		RegularRenderer					regularRenderer;
		TerrainRenderer					terrainRenderer;
		Texture2D 						positionTex;	// Position buffer (could be reconstruct from depth)
		Texture2D  						normalTex;		// RGB : World space normal buffer / A : roughness
		Texture2D 						diffuseTex;		// RGB : albedo / A : specularity
		Texture2D  						depthTex; 		// Depth/Stencil buffer
		GLuint	 						framebuffer;
	};
	//--------------------------------------------------------------------------
}
#endif

