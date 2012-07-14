#ifndef GLF_SSAO_HPP
#define GLF_SSAO_HPP

//------------------------------------------------------------------------------
// Include
//------------------------------------------------------------------------------
#include <glf/wrapper.hpp>
#include <glf/gbuffer.hpp>
#include <glf/pass.hpp>

namespace glf
{
	//--------------------------------------------------------------------------
	class SSAO
	{
	private:
					SSAO(			const SSAO&);
		SSAO		operator=(		const SSAO&);
	public:
					SSAO(			int _w, 
									int _h);
		void 		Draw(			const GBuffer&	_gbuffer,
									const glm::mat4& _view,
									float 			_near,
									float 			_beta,
									float 			_epsilon,
									float 			_kappa,
									float			_sigma,
									float			_radius,
									int 			_nSamples,
									const RenderTarget& _renderTarget);

		void 		Draw(			const Texture2D& _inputTex,
									const Texture2D& _positionTex,
									const glm::mat4& _viewMat,
									float 			 _sigmaScreen,
									float 			 _sigmaDepth,
									int 			 _nTaps,
									const glm::vec2& _direction,
									const RenderTarget& _renderTarget);
	public:
		struct SSAOPass
		{
									SSAOPass():program("SSAO::SSAOPass"){}
			GLint 					positionTexUnit;
			GLint 					normalTexUnit;
			GLint					rotationTexUnit;

			GLint					nearVar;
			GLint					betaVar;
			GLint					epsilonVar;
			GLint					kappaVar;
			GLint					sigmaVar;
			GLint					radiusVar;
			GLint					nSamplesVar;
			GLint					viewMatVar;

			Program 				program;
		};

		struct BilateralPass
		{
									BilateralPass():program("SSAO::BilateraPass"){}
			GLint 					positionTexUnit;
			GLint 					inputTexUnit;

			GLint					viewMatVar;
			GLint					sigmaScreenVar;
			GLint					sigmaDepthVar;
			GLint					nTapsVar;
			GLint					directionVar;

			Program 				program;
		};
		
		SSAOPass					ssaoPass;
		BilateralPass				bilateralPass;
		Texture2D					rotationTex;
	};
	//--------------------------------------------------------------------------
}

#endif
