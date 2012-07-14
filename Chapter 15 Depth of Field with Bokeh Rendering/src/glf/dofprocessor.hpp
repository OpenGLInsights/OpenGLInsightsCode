#ifndef GLF_DOF_HPP
#define GLF_DOF_HPP

//-----------------------------------------------------------------------------
// Include
//-----------------------------------------------------------------------------
#include <glf/wrapper.hpp>
#include <glf/texture.hpp>
#include <glf/pass.hpp>

namespace glf
{
	//--------------------------------------------------------------------------
	class DOFProcessor
	{
	private:
					DOFProcessor(		const DOFProcessor&);
		DOFProcessor operator=(			const DOFProcessor&);
	public:
					DOFProcessor(		int _w, 
										int _h);

		// Load bokeh/aperture shape from a file
		void		BokehTexture(		const std::string& _filename);

		// Take position and color buffer and output DOF result into _target
		void		Draw(				const Texture2D& _colorTex, 
										const Texture2D& _positionTex, 
										const glm::mat4& _view,
										float 			_nearStart,
										float 			_nearEnd,
										float 			_farStart,
										float 			_farEnd,
										float 			_maxCoCRadius,
										float 			_maxBokehRadius,
										int 			_nSamples,
										float 			_intensityThreshold,
										float 			_cocThreshold,
										float			_bokehDepthCutoff,
										bool			_poissonFiltering,
										const RenderTarget& _target);
		int			GetDetectedBokehs(	);
	public:
		//----------------------------------------------------------------------
		struct ResetPass
		{
										ResetPass():program("DOF::Reset"){}
			GLint						bokehCountTexUnit;
			Program 					program;
		};
		//----------------------------------------------------------------------
		struct CoCPass
		{
										CoCPass():program("DOF::CoCPass"){}
			GLint 						positionTexUnit;
			GLint						farStartVar;		// Far start
			GLint						farEndVar;			// Far end
			GLint						viewMatVar;			// View matrix

			Program 					program;
		};
		//----------------------------------------------------------------------
		struct DetectionPass
		{
										DetectionPass():program("DOF::DetectionPass"){}
			GLint 						colorTexUnit;
			GLint 						blurDepthTexUnit;
			GLint 						cocThresholdVar;
			GLint 						lumThresholdVar;
			GLint 						maxCoCRadiusVar;
			GLint 						bokehCountTexUnit;
			GLint 						bokehColorTexUnit;
			GLint 						bokehPositionTexUnit;

			Program 					program;
		};
		//----------------------------------------------------------------------
		struct BlurSeparablePass
		{
										BlurSeparablePass():program("DOF::BlurSeparablePass"){}
			GLint 						colorTexUnit;
			GLint						blurDepthTexUnit;
			GLint						directionVar;
			GLint						maxCoCRadiusVar;

			Program 					program;
		};
		//----------------------------------------------------------------------
		struct BlurPoissonPass
		{
										BlurPoissonPass():program("DOF::BlurPoissonPass"){}
			GLint 						colorTexUnit;
			GLint 						rotationTexUnit;
			GLint						blurDepthTexUnit;
			GLint						nSamplesVar;
			GLint						maxCoCRadiusVar;

			Program 					program;
		};
		//----------------------------------------------------------------------
		struct SynchronizationPass
		{
										SynchronizationPass():program("DOF::SynchronisationPass"){}
			GLint 						indirectBufferTexUnit;
			
			Program 					program;
		};
		//----------------------------------------------------------------------
		struct RenderingPass
		{
										RenderingPass():program("DOF::RenderingPass"){}
			GLint 						bokehPositionTexUnit;
			GLint 						bokehColorTexUnit;
			GLint 						bokehShapeTexUnit;
			GLint 						blurDepthTexUnit;
			GLint						maxBokehRadiusVar;
			GLint						bokehDepthCutoffVar;

			Program 					program;
		};

	private:
		Texture2D						blurDepthTex;		// Store pixel blur / linear-depth
		Texture2D						detectionTex;		// Store color of pixels which are not bokeh
		Texture2D						blurTex;			// Store result of vertical blur
		Texture2D						bokehShapeTex;		// Store aperture/bokeh shape
		Texture2D						rotationTex;		// Store rotation for Poisson sampling
		
		Texture2D						bokehPositionTex;	// Store bokeh position
		Texture2D						bokehColorTex;		// Store bokeh color
		AtomicCounterBuffer				bokehCounterACB;	// Atomic bokeh counter
		GLuint							indirectBufferTexID;// Texture object for the indirect buffer

		GLuint							blurDepthFBO;		// Framebuffers
		GLuint							detectionFBO;		//
		GLuint							blurFBO;			//

		ResetPass 						resetPass;			// Reset bokeh counter
		CoCPass 						cocPass;			// Compute pixel blur and linear depth
		DetectionPass					detectionPass;		// Detect pixel which are bokeh
		BlurSeparablePass				blurSeparablePass;	// Blur pixel which are not bokeh (with a separable filter)
		BlurPoissonPass					blurPoissonPass;	// Blur pixel which are not bokeh (with a poisson filter)
		SynchronizationPass				synchronizationPass;// Synchronize bokeh counter with indirect buffer
		RenderingPass					renderingPass;		// Render bokehs
				
		VertexBuffer3F					pointVBO;			// Point VBO
		VertexArray						pointVAO;			// Point VAO
		IndirectArrayBuffer				pointIndirectBuffer;// Indirect buffer for instancing bokeh
	};
}

#endif
