#ifndef GLF_CSM_HPP
#define GLF_CSM_HPP

//-----------------------------------------------------------------------------
// Include
//-----------------------------------------------------------------------------
#include <glf/camera.hpp>
#include <glf/texture.hpp>
#include <glf/scene.hpp>
#include <glf/helper.hpp>
#include <glf/pass.hpp>
#include <glf/gbuffer.hpp>

namespace glf
{
	//-------------------------------------------------------------------------
	class CSMLight
	{
	public:
					CSMLight(		int _w, 
									int _h,
									int _nCascades);
				   ~CSMLight();
		void		SetIntensity(	const glm::vec3& _intensity);
		void		SetDirection(	const glm::vec3& _direction);
	private:
 					CSMLight(		const CSMLight&);
 		CSMLight	operator=(		const CSMLight&);
	public:
		glm::vec3 					direction;	// Light direction points the direction of the light flux
		glm::mat4					view;		// View matrix (center on cam pos)
		glm::mat4					camView;	// Camera view matrix used to generated the current CSM
		glm::mat4*					projs;		// Proj matrices for each split
		glm::mat4*					viewprojs;	// ViewProj matrices for each split
		float*						nearPlanes;
		float*						farPlanes;
		glm::vec3		 			intensity;
		int							nCascades;
		TextureArray2D				depthTexs;
		TextureArray2D				tmpTexs;	// Store linear moment for VSM and EVSM
		TextureArray2D				momentTexs;	// Store linear moment for VSM and EVSM
		TextureArray2D				filterTexs;	// Store filtered linear moment for VSM and EVSM
		GLuint						tmpFBO;
		GLuint						depthFBO;
		GLuint						filterFBO;
	};
	//-------------------------------------------------------------------------
	class CSMBuilder
	{
	public:
					CSMBuilder(		);
		void		Draw(			CSMLight&						_light,
									const Camera&					_camera,
									float 							_cascadeAlpha,
									float 							_blendFactor,
									const SceneManager& 			_scene);
	private:
 					CSMBuilder(		const CSMBuilder&);
 		CSMBuilder	operator=(		const CSMBuilder&);
	public:

		struct RegularRenderer
		{
									RegularRenderer():program("CSMBuilder::RegularRenderer"){}
			Program 				program;
			GLint 					projVar;
			GLint 					viewVar;
			GLint 					modelVar;
			GLint 					nCascadesVar;
		};

		struct TerrainRenderer
		{
									TerrainRenderer():program("CSMBuilder::TerrainRenderer"){}
			Program 				program;
			GLint 					projVar;
			GLint 					viewVar;
			GLint 					nCascadesVar;

			GLint					heightTexUnit;
			GLint 					tileSizeVar;
			GLint 					tileCountVar;
			GLint 					tileOffsetVar;
			GLint 					projFactorVar;
			GLint 					tessFactorVar;
			GLint					heightFactorVar;
		};

		struct MomentFilter
		{
									MomentFilter():program("CSMBuilder::MomentFilter"){}
			Program 				program;
			GLint 					momentTexUnit;
			GLint 					directionVar;
		};

		int							maxCascades;
		RegularRenderer				regularRenderer;
		TerrainRenderer				terrainRenderer;
		MomentFilter				momentFilter;

		VertexBuffer2F				vbo;
		VertexArray					vao;
	};
	//-------------------------------------------------------------------------
	class CSMRenderer
	{
	public:
					CSMRenderer(	int _w, 
									int _h);
		void 		Draw(			const CSMLight&	_light,
									const GBuffer&	_gbuffer,
									const glm::vec3&_viewPos,
									float 			_blendFactor,
									float 			_bias,
									RenderTarget&	_target);
	private:
 					CSMRenderer(	const CSMRenderer&);
 		CSMRenderer	operator=(		const CSMRenderer&);
	public:
		GLint 						positionTexUnit;
		GLint 						diffuseTexUnit;
		GLint 						normalTexUnit;
		GLint 						shadowTexUnit;

		GLint						blendFactorVar;
		GLint						viewPosVar;
		GLint						lightDirVar;
		GLint 						lightViewProjsVar;
		GLint						lightIntensityVar;
		GLint						biasVar;
		GLint						nCascadesVar;

		Program 					program;
	};
}

#endif
