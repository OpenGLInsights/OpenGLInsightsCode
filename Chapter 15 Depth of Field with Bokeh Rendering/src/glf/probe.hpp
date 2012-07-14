#ifndef GLF_PROBE_HPP
#define GLF_PROBE_HPP

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glm/glm.hpp>
#include <glf/texture.hpp>
#include <glf/wrapper.hpp>
#include <glf/buffer.hpp>
#include <glf/pass.hpp>
#include <glf/gbuffer.hpp>

namespace glf
{
	//--------------------------------------------------------------------------
	class ProbeLight
	{	
	private:
					ProbeLight(			const ProbeLight&);
		ProbeLight	operator=(			const ProbeLight&);
	public:	
										ProbeLight(int _resolution);
		glm::vec3						shCoeffs[9];	// For diffusion component
		TextureCube						cubeTex;		// For specular component
	};
	//--------------------------------------------------------------------------
	class ProbeBuilder
	{
	private:
					ProbeBuilder(		const ProbeBuilder&);
		ProbeBuilder operator=(			const ProbeBuilder&);
	public:
					ProbeBuilder(		int _resolution);
		void 		Filter(				ProbeLight& 			_probe,
										int 					_level=0);
	private:

		GLuint							cubeTexUnit;
		GLuint							shFBO;
		TextureArray2D					shTex;
		Program							program;
		VertexBuffer2F					vbo;
		VertexArray						vao;
		int 							resolution;
	};
	//--------------------------------------------------------------------------
	class ProbeRenderer
	{
	private:
 					ProbeRenderer(		const ProbeRenderer&);
 		ProbeRenderer operator=(		const ProbeRenderer&);
	public:
					ProbeRenderer(		int _w, 
										int _h);
		void 		Draw(				const ProbeLight&	_probe,
										const GBuffer&		_gbuffer,
										const glm::vec3&	_viewPos,
										const RenderTarget& _renderTarget);

		GLint 							positionTexUnit;
		GLint 							diffuseTexUnit;
		GLint 							normalTexUnit;
		GLint 							cubeTexUnit;
		GLint							shCoeffsVar;
		GLint							viewPosVar;
		Program 						program;
	};
	//--------------------------------------------------------------------------
}
#endif

