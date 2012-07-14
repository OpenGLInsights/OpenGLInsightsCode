//-----------------------------------------------------------------------------
// Include
//-----------------------------------------------------------------------------
#include <glf/ssao.hpp>
#include <glf/rng.hpp>

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

namespace glf
{
	//-------------------------------------------------------------------------
	SSAO::SSAO(int _w, int _h)
	{
		// TODO : create a smaller rotation texture and use tilling
		// Create and fill rotation texture
		RNG rng;
		rotationTex.Allocate(GL_RG16F,_w,_h);
		glm::vec2* rotations = new glm::vec2[_w * _h];
		for(int y=0;y<_h;++y)
		for(int x=0;x<_w;++x)
		{	
			float theta 		= 2.f * M_PI * rng.RandomFloat();
			rotations[x+y*_w] 	= glm::vec2(cos(theta),sin(theta));
		}
		rotationTex.Fill(GL_RG,GL_FLOAT,(unsigned char*)rotations);
		delete[] rotations;

		// Halton sequence generated using: WONG, T.-T., LUK, W.-S., AND HENG, P.-A. 1997.
		// Sampling with hammersley and Halton points
		// http://www.cse.cuhk.edu.hk/~ttwong/papers/udpoint/udpoint.pdf
		glm::vec2 Halton[32];
		Halton[0]	= glm::vec2(-0.353553, 0.612372);
		Halton[1]	= glm::vec2(-0.25, -0.433013);
		Halton[2]	= glm::vec2(0.663414, 0.55667);
		Halton[3]	= glm::vec2(-0.332232, 0.120922);
		Halton[4]	= glm::vec2(0.137281, -0.778559);
		Halton[5]	= glm::vec2(0.106337, 0.603069);
		Halton[6]	= glm::vec2(-0.879002, -0.319931);
		Halton[7]	= glm::vec2(0.191511, -0.160697);
		Halton[8]	= glm::vec2(0.729784, 0.172962);
		Halton[9]	= glm::vec2(-0.383621, 0.406614);
		Halton[10]	= glm::vec2(-0.258521, -0.86352);
		Halton[11]	= glm::vec2(0.258577, 0.34733);
		Halton[12]	= glm::vec2(-0.82355, 0.0962588);
		Halton[13]	= glm::vec2(0.261982, -0.607343);
		Halton[14]	= glm::vec2(-0.0562987, 0.966608);
		Halton[15]	= glm::vec2(-0.147695, -0.0971404);
		Halton[16]	= glm::vec2(0.651341, -0.327115);
		Halton[17]	= glm::vec2(0.47392, 0.238012);
		Halton[18]	= glm::vec2(-0.738474, 0.485702);
		Halton[19]	= glm::vec2(-0.0229837, -0.394616);
		Halton[20]	= glm::vec2(0.320861, 0.74384);
		Halton[21]	= glm::vec2(-0.633068, -0.0739953);
		Halton[22]	= glm::vec2(0.568478, -0.763598);
		Halton[23]	= glm::vec2(-0.0878153, 0.293323);
		Halton[24]	= glm::vec2(-0.528785, -0.560479);
		Halton[25]	= glm::vec2(0.570498, -0.13521);
		Halton[26]	= glm::vec2(0.915797, 0.0711813);
		Halton[27]	= glm::vec2(-0.264538, 0.385706);
		Halton[28]	= glm::vec2(-0.365725, -0.76485);
		Halton[29]	= glm::vec2(0.488794, 0.479406);
		Halton[30]	= glm::vec2(-0.948199, 0.263949);
		Halton[31]	= glm::vec2(0.0311802, -0.121049);

		// Create SSAO Pass
		ProgramOptions ssaoOptions = ProgramOptions::CreateVSOptions();
		ssaoOptions.AddDefine<int>("SSAO_PASS",1);
		ssaoPass.program.Compile(	ssaoOptions.Append(LoadFile(directory::ShaderDirectory + "ssao.vs")),
									ssaoOptions.Append(LoadFile(directory::ShaderDirectory + "ssao.fs")));

		ssaoPass.betaVar			= ssaoPass.program["Beta"].location;
		ssaoPass.epsilonVar			= ssaoPass.program["Epsilon"].location;
		ssaoPass.kappaVar	 		= ssaoPass.program["Kappa"].location;
		ssaoPass.sigmaVar			= ssaoPass.program["Sigma"].location;
		ssaoPass.radiusVar			= ssaoPass.program["Radius"].location;
		ssaoPass.nSamplesVar		= ssaoPass.program["nSamples"].location;
		ssaoPass.viewMatVar			= ssaoPass.program["View"].location;
		ssaoPass.nearVar			= ssaoPass.program["Near"].location;

		ssaoPass.positionTexUnit	= ssaoPass.program["PositionTex"].unit;
		ssaoPass.normalTexUnit		= ssaoPass.program["NormalTex"].unit;
		ssaoPass.rotationTexUnit	= ssaoPass.program["RotationTex"].unit;

		glProgramUniform1i(ssaoPass.program.id, ssaoPass.program["PositionTex"].location,	ssaoPass.positionTexUnit);
		glProgramUniform1i(ssaoPass.program.id, ssaoPass.program["NormalTex"].location,		ssaoPass.normalTexUnit);
		glProgramUniform1i(ssaoPass.program.id, ssaoPass.program["RotationTex"].location,	ssaoPass.rotationTexUnit);
		glProgramUniform2fv(ssaoPass.program.id, ssaoPass.program["Samples[0]"].location,	32, &Halton[0][0]);

		// Create Bilatereal Pass
		ProgramOptions bilateralOptions = ProgramOptions::CreateVSOptions();
		bilateralOptions.AddDefine<int>("BILATERAL_PASS",1);
		bilateralPass.program.Compile(	bilateralOptions.Append(LoadFile(directory::ShaderDirectory + "ssao.vs")),
										bilateralOptions.Append(LoadFile(directory::ShaderDirectory + "ssao.fs")));

		bilateralPass.sigmaScreenVar= bilateralPass.program["SigmaScreen"].location;
		bilateralPass.sigmaDepthVar	= bilateralPass.program["SigmaDepth"].location;
		bilateralPass.nTapsVar		= bilateralPass.program["nTaps"].location;
		bilateralPass.viewMatVar	= bilateralPass.program["ViewMat"].location;
		bilateralPass.directionVar	= bilateralPass.program["Direction"].location;

		bilateralPass.inputTexUnit	= bilateralPass.program["InputTex"].unit;
		bilateralPass.positionTexUnit=bilateralPass.program["PositionTex"].unit;

		glProgramUniform1i(bilateralPass.program.id, bilateralPass.program["InputTex"].location,	bilateralPass.inputTexUnit);
		glProgramUniform1i(bilateralPass.program.id, bilateralPass.program["PositionTex"].location,	bilateralPass.positionTexUnit);

		glf::CheckError("SSAO::Create");
	}
	//-------------------------------------------------------------------------
	void SSAO::Draw(	const GBuffer&	_gbuffer,
						const glm::mat4& _view,
						float 			_near,
						float 			_beta,
						float 			_epsilon,
						float 			_kappa,
						float 			_sigma,
						float 			_radius,
						int 			_nSamples,
						const RenderTarget& _renderTarget)
	{
		glUseProgram(ssaoPass.program.id);
		glProgramUniform1f(ssaoPass.program.id,			ssaoPass.nearVar,		_near);
		glProgramUniform1f(ssaoPass.program.id,			ssaoPass.betaVar,		_beta);
		glProgramUniform1f(ssaoPass.program.id,			ssaoPass.epsilonVar,	_epsilon);
		glProgramUniform1f(ssaoPass.program.id,			ssaoPass.kappaVar,		_kappa);
		glProgramUniform1f(ssaoPass.program.id,			ssaoPass.sigmaVar,		_sigma);
		glProgramUniform1f(ssaoPass.program.id,			ssaoPass.radiusVar,		_radius);
		glProgramUniform1i(ssaoPass.program.id,			ssaoPass.nSamplesVar,	_nSamples);
		glProgramUniformMatrix4fv(ssaoPass.program.id, 	ssaoPass.viewMatVar,	1, GL_FALSE, &_view[0][0]);

		_gbuffer.positionTex.Bind(ssaoPass.positionTexUnit);
		_gbuffer.normalTex.Bind(ssaoPass.normalTexUnit);
		rotationTex.Bind(ssaoPass.rotationTexUnit);
		_renderTarget.Draw();

		glf::CheckError("SSAO::SSAODraw");
	}
	//-------------------------------------------------------------------------
	void SSAO::Draw(	const Texture2D& _inputTex,
						const Texture2D& _positionTex,
						const glm::mat4& _view,
						float 			 _sigmaScreen,
						float 			 _sigmaDepth,
						int 			 _nTaps,
						const glm::vec2& _direction,
						const RenderTarget& _renderTarget)
	{
		glUseProgram(bilateralPass.program.id);
		glProgramUniform1f(bilateralPass.program.id,			bilateralPass.sigmaScreenVar,	_sigmaScreen);
		glProgramUniform1f(bilateralPass.program.id,			bilateralPass.sigmaDepthVar,	_sigmaDepth);
		glProgramUniform1i(bilateralPass.program.id,			bilateralPass.nTapsVar,			_nTaps);
		glProgramUniform2f(bilateralPass.program.id,			bilateralPass.directionVar,		_direction.x,_direction.y);
		glProgramUniformMatrix4fv(bilateralPass.program.id, 	bilateralPass.viewMatVar,		1, GL_FALSE, &_view[0][0]);

		_inputTex.Bind(bilateralPass.inputTexUnit);
		_positionTex.Bind(bilateralPass.positionTexUnit);
		_renderTarget.Draw();

		glf::CheckError("SSAO::BilateralDraw");
	}
	//-------------------------------------------------------------------------
}

