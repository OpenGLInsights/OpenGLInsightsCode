//-----------------------------------------------------------------------------
// Include
//-----------------------------------------------------------------------------
#include <glf/dofprocessor.hpp>
#include <glf/io/image.hpp>
#include <glf/debug.hpp>
#include <glf/rng.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp>

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
#define RUN_TIMINGS 0

namespace glf
{
	//-------------------------------------------------------------------------
	DOFProcessor::DOFProcessor(int _w, int _h)
	{
		// Resources initialization
		{
			// Load bokeh texture
			BokehTexture(directory::TextureDirectory + "HexagonalBokeh.png");

			blurDepthTex.Allocate(GL_RGBA32F,_w,_h);
			blurDepthTex.SetFiltering(GL_LINEAR,GL_LINEAR);
			blurDepthTex.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
			detectionTex.Allocate(GL_RGBA32F,_w,_h);
			detectionTex.SetFiltering(GL_LINEAR,GL_LINEAR);
			detectionTex.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
			blurTex.Allocate(GL_RGBA32F,_w,_h);
			blurTex.SetFiltering(GL_LINEAR,GL_LINEAR);
			blurTex.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
			rotationTex.Allocate(GL_RG16F,_w,_h);
			rotationTex.SetFiltering(GL_LINEAR,GL_LINEAR);
			rotationTex.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);

			glGenFramebuffers(1, &blurDepthFBO);
			glBindFramebuffer(GL_FRAMEBUFFER,blurDepthFBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, blurDepthTex.target, blurDepthTex.id, 0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glBindFramebuffer(GL_FRAMEBUFFER,0);
			glf::CheckFramebuffer(blurDepthFBO);

			glGenFramebuffers(1, &detectionFBO);
			glBindFramebuffer(GL_FRAMEBUFFER,detectionFBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, detectionTex.target, detectionTex.id, 0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glBindFramebuffer(GL_FRAMEBUFFER,0);
			glf::CheckFramebuffer(detectionFBO);

			glGenFramebuffers(1, &blurFBO);
			glBindFramebuffer(GL_FRAMEBUFFER,blurFBO);
			glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, blurTex.target, blurTex.id, 0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glBindFramebuffer(GL_FRAMEBUFFER,0);
			glf::CheckFramebuffer(blurFBO);

			// Create texture for counting bokeh
			// Texture size is set to the resolution in order to avoid overflow
			bokehPositionTex.Allocate(GL_RGBA32F,_w,_h);
			bokehPositionTex.SetFiltering(GL_NEAREST,GL_NEAREST);
			bokehColorTex.Allocate(GL_RGBA32F,_w,_h);
			bokehColorTex.SetFiltering(GL_NEAREST,GL_NEAREST);

			// Setup the indirect buffer
			pointIndirectBuffer.Allocate(1);
			DrawArraysIndirectCommand* indirectCmd = pointIndirectBuffer.Lock();
			indirectCmd[0].count 				= 1;
			indirectCmd[0].primCount 			= 0; // Number of instance to draw
			indirectCmd[0].first 				= 0;
			indirectCmd[0].reservedMustBeZero 	= 0;
			pointIndirectBuffer.Unlock();

			// Create the texture proxy for the indirect buffer
			glGenTextures(1, &indirectBufferTexID);
			glBindTexture(GL_TEXTURE_BUFFER, indirectBufferTexID);
			glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, pointIndirectBuffer.id);
			glBindTexture(GL_TEXTURE_BUFFER, 0);

			// Allocate atomic counter
			bokehCounterACB.Allocate(1,GL_DYNAMIC_DRAW);

			// Create point VBO and VAO
			pointVBO.Allocate(1,GL_STATIC_DRAW);
			glm::vec3* pvertices = pointVBO.Lock();
			pvertices[0] = glm::vec3(0,0,0);
			pointVBO.Unlock();
			pointVAO.Add(pointVBO,semantic::Position,3,GL_FLOAT);

			glf::CheckError("DofProcessor::Resources");
		}

		// CoC Pass
		{
			cocPass.program.Compile(ProgramOptions::CreateVSOptions().Append(LoadFile(directory::ShaderDirectory + "bokehcoc.vs")),
									LoadFile(directory::ShaderDirectory + "bokehcoc.fs"));

			cocPass.farStartVar			= cocPass.program["FarStart"].location;
			cocPass.farEndVar			= cocPass.program["FarEnd"].location;
			cocPass.viewMatVar			= cocPass.program["ViewMat"].location;
			cocPass.positionTexUnit		= cocPass.program["PositionTex"].unit;

			glProgramUniform1i(cocPass.program.id, cocPass.program["PositionTex"].location,cocPass.positionTexUnit);

			glf::CheckError("DofProcessor::BlurDepth");
		}

		// Detection Pass
		{
			detectionPass.program.Compile(	ProgramOptions::CreateVSOptions().Append(LoadFile(directory::ShaderDirectory + "bokehdetection.vs")),
											LoadFile(directory::ShaderDirectory + "bokehdetection.fs"));

			detectionPass.colorTexUnit		= detectionPass.program["ColorTex"].unit;
			detectionPass.blurDepthTexUnit	= detectionPass.program["BlurDepthTex"].unit;
			detectionPass.lumThresholdVar	= detectionPass.program["LumThreshold"].location;
			detectionPass.cocThresholdVar	= detectionPass.program["CoCThreshold"].location;
			detectionPass.maxCoCRadiusVar	= detectionPass.program["MaxCoCRadius"].location;
			detectionPass.bokehColorTexUnit	= detectionPass.program["BokehColorTex"].unit;
			detectionPass.bokehPositionTexUnit= detectionPass.program["BokehPositionTex"].unit;			

			glProgramUniform1i(detectionPass.program.id, detectionPass.program["BlurDepthTex"].location,detectionPass.blurDepthTexUnit);
			glProgramUniform1i(detectionPass.program.id, detectionPass.program["ColorTex"].location,detectionPass.colorTexUnit);
			glProgramUniform1i(detectionPass.program.id, detectionPass.program["BokehColorTex"].location,detectionPass.bokehColorTexUnit);
			glProgramUniform1i(detectionPass.program.id, detectionPass.program["BokehPositionTex"].location,detectionPass.bokehPositionTexUnit);

			glf::CheckError("DofProcessor::BlurDetection");
		}

		// Blur separable pass
		{
			blurSeparablePass.program.Compile(	ProgramOptions::CreateVSOptions().Append(LoadFile(directory::ShaderDirectory + "bokehblur.vs")),
												LoadFile(directory::ShaderDirectory + "bokehblur.fs"));

			blurSeparablePass.blurDepthTexUnit	= blurSeparablePass.program["BlurDepthTex"].unit;
			blurSeparablePass.colorTexUnit		= blurSeparablePass.program["ColorTex"].unit;
			blurSeparablePass.maxCoCRadiusVar	= blurSeparablePass.program["MaxCoCRadius"].location;
			blurSeparablePass.directionVar		= blurSeparablePass.program["Direction"].location;

			glProgramUniform1i(blurSeparablePass.program.id, blurSeparablePass.program["BlurDepthTex"].location,blurSeparablePass.blurDepthTexUnit);
			glProgramUniform1i(blurSeparablePass.program.id, blurSeparablePass.program["ColorTex"].location,blurSeparablePass.colorTexUnit);

			glf::CheckError("DofProcessor::BlurSeparable");
		}

		// Blur poisson pass
		{
			// Sampling point
			glm::vec2 Halton[32];
			Halton[0]       = glm::vec2(-0.353553, 0.612372);
			Halton[1]       = glm::vec2(-0.25, -0.433013);
			Halton[2]       = glm::vec2(0.663414, 0.55667);
			Halton[3]       = glm::vec2(-0.332232, 0.120922);
			Halton[4]       = glm::vec2(0.137281, -0.778559);
			Halton[5]       = glm::vec2(0.106337, 0.603069);
			Halton[6]       = glm::vec2(-0.879002, -0.319931);
			Halton[7]       = glm::vec2(0.191511, -0.160697);
			Halton[8]       = glm::vec2(0.729784, 0.172962);
			Halton[9]       = glm::vec2(-0.383621, 0.406614);
			Halton[10]      = glm::vec2(-0.258521, -0.86352);
			Halton[11]      = glm::vec2(0.258577, 0.34733);
			Halton[12]      = glm::vec2(-0.82355, 0.0962588);
			Halton[13]      = glm::vec2(0.261982, -0.607343);
			Halton[14]      = glm::vec2(-0.0562987, 0.966608);
			Halton[15]      = glm::vec2(-0.147695, -0.0971404);
			Halton[16]      = glm::vec2(0.651341, -0.327115);
			Halton[17]      = glm::vec2(0.47392, 0.238012);
			Halton[18]      = glm::vec2(-0.738474, 0.485702);
			Halton[19]      = glm::vec2(-0.0229837, -0.394616);
			Halton[20]      = glm::vec2(0.320861, 0.74384);
			Halton[21]      = glm::vec2(-0.633068, -0.0739953);
			Halton[22]      = glm::vec2(0.568478, -0.763598);
			Halton[23]      = glm::vec2(-0.0878153, 0.293323);
			Halton[24]      = glm::vec2(-0.528785, -0.560479);
			Halton[25]      = glm::vec2(0.570498, -0.13521);
			Halton[26]      = glm::vec2(0.915797, 0.0711813);
			Halton[27]      = glm::vec2(-0.264538, 0.385706);
			Halton[28]      = glm::vec2(-0.365725, -0.76485);
			Halton[29]      = glm::vec2(0.488794, 0.479406);
			Halton[30]      = glm::vec2(-0.948199, 0.263949);
			Halton[31]      = glm::vec2(0.0311802, -0.121049);

			// Create and fill rotation texture
			RNG rng;
			glm::vec2* rotations = new glm::vec2[_w * _h];
			for(int y=0;y<_h;++y)
			for(int x=0;x<_w;++x)
			{	
				float theta 		= 2.f * M_PI * rng.RandomFloat();
				rotations[x+y*_w] 	= glm::vec2(cos(theta),sin(theta));
			}
			rotationTex.Fill(GL_RG,GL_FLOAT,(unsigned char*)&rotations[0][0]);
			delete[] rotations;

			blurPoissonPass.program.Compile(	ProgramOptions::CreateVSOptions().Append(LoadFile(directory::ShaderDirectory + "bokehblurpoisson.vs")),
												LoadFile(directory::ShaderDirectory + "bokehblurpoisson.fs"));

			blurPoissonPass.blurDepthTexUnit	= blurPoissonPass.program["BlurDepthTex"].unit;
			blurPoissonPass.colorTexUnit		= blurPoissonPass.program["ColorTex"].unit;
			blurPoissonPass.rotationTexUnit		= blurPoissonPass.program["RotationTex"].unit;
			blurPoissonPass.maxCoCRadiusVar		= blurPoissonPass.program["MaxCoCRadius"].location;
			blurPoissonPass.nSamplesVar			= blurPoissonPass.program["NSamples"].location;

			glProgramUniform1i(blurPoissonPass.program.id,	blurPoissonPass.program["BlurDepthTex"].location,blurPoissonPass.blurDepthTexUnit);
			glProgramUniform1i(blurPoissonPass.program.id,	blurPoissonPass.program["ColorTex"].location,blurPoissonPass.colorTexUnit);
			glProgramUniform1i(blurPoissonPass.program.id,	blurPoissonPass.program["RotationTex"].location,blurPoissonPass.rotationTexUnit);
			glProgramUniform2fv(blurPoissonPass.program.id,	blurPoissonPass.program["Samples[0]"].location,32,&Halton[0][0]);

			glf::CheckError("DofProcessor::BlurPoisson");
		}

		// Synchronization Pass
		{
			synchronizationPass.program.Compile(LoadFile(directory::ShaderDirectory + "bokehsynchronization.vs"),
												LoadFile(directory::ShaderDirectory + "bokehsynchronization.fs"));

			synchronizationPass.indirectBufferTexUnit = synchronizationPass.program["IndirectBufferTex"].unit;
			glProgramUniform1i(synchronizationPass.program.id,synchronizationPass.program["IndirectBufferTex"].location,synchronizationPass.indirectBufferTexUnit );

			glf::CheckError("DofProcessor::Synchronization");
		}

		// Rendering pass
		{
			renderingPass.program.Compile(	ProgramOptions::CreateVSOptions().Append(LoadFile(directory::ShaderDirectory + "bokehrendering.vs")),
											LoadFile(directory::ShaderDirectory + "bokehrendering.gs"),
											LoadFile(directory::ShaderDirectory + "bokehrendering.fs"));

			renderingPass.blurDepthTexUnit		= renderingPass.program["BlurDepthTex"].unit;
			renderingPass.bokehPositionTexUnit	= renderingPass.program["BokehPositionTex"].unit;
			renderingPass.bokehColorTexUnit		= renderingPass.program["BokehColorTex"].unit;
			renderingPass.bokehShapeTexUnit		= renderingPass.program["BokehShapeTex"].unit;
			renderingPass.maxBokehRadiusVar		= renderingPass.program["MaxBokehRadius"].location;
			renderingPass.bokehDepthCutoffVar	= renderingPass.program["BokehDepthCutoff"].location;

			glProgramUniform2f(renderingPass.program.id, renderingPass.program["PixelScale"].location,1.f/_w, 1.f/_h);
			glProgramUniform1i(renderingPass.program.id, renderingPass.program["BokehPositionTex"].location,renderingPass.bokehPositionTexUnit);
			glProgramUniform1i(renderingPass.program.id, renderingPass.program["BokehShapeTex"].location,renderingPass.bokehShapeTexUnit);
			glProgramUniform1i(renderingPass.program.id, renderingPass.program["BokehColorTex"].location,renderingPass.bokehColorTexUnit);
			glProgramUniform1i(renderingPass.program.id, renderingPass.program["BlurDepthTex"].location,renderingPass.blurDepthTexUnit);

			glf::CheckError("DofProcessor::Rendering");
		}

		glf::CheckError("DOFProcessor::Create");
	}
	//-------------------------------------------------------------------------
	void DOFProcessor::BokehTexture(		const std::string& _filename)
	{
		io::LoadTexture(_filename,
						bokehShapeTex,
						true,
						true);

		bokehShapeTex.SetFiltering(GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR);
		bokehShapeTex.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
		glBindTexture(bokehShapeTex.target,bokehShapeTex.id);
		glGenerateMipmap(bokehShapeTex.target);
		glBindTexture(bokehShapeTex.target,0);
	}
	//-------------------------------------------------------------------------
	int	DOFProcessor::GetDetectedBokehs()
	{
		// Print number of bokeh drawn during the last frame 
		int nBokehs;
		DrawArraysIndirectCommand* indirectCmd = pointIndirectBuffer.Lock();
		nBokehs = indirectCmd[0].primCount;
		pointIndirectBuffer.Unlock();
		glf::CheckError("DOFProcessor::GetDetectedBokehs");
		return nBokehs;
	}
	//-------------------------------------------------------------------------
	void DOFProcessor::Draw(	const Texture2D& _colorTex, 
								const Texture2D& _positionTex, 
								const glm::mat4& _view,
								float			_nearStart,
								float			_nearEnd,
								float			_farStart,
								float			_farEnd,
								float 			_maxCoCRadius,
								float 			_maxBokehRadius,
								int				_nSamples,
								float			_lumThreshold,
								float			_cocThreshold,
								float			_bokehDepthCutoff,
								bool 			_poissonFiltering,
								const RenderTarget& _renderTarget)
	{
		glf::CheckError("DOFProcessor::DrawBegin");

		// Reset bokeh counter (draw a fake point)
		glf::manager::timings->StartSection(section::DofReset);
			glm::uint32* bokehCounterValue = (glm::uint32*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(glm::uint32),GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
			*bokehCounterValue = 0;
			glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
		glf::manager::timings->EndSection(section::DofReset);

		// Compute amount of blur and linear depth for each pixel
		glf::manager::timings->StartSection(section::DofBlurDepth);
		glUseProgram(cocPass.program.id);
			glBindFramebuffer(GL_FRAMEBUFFER,blurDepthFBO);
			glClear(GL_COLOR_BUFFER_BIT);
			glProgramUniform1f(cocPass.program.id,			cocPass.farStartVar,	_farStart);
			glProgramUniform1f(cocPass.program.id,			cocPass.farEndVar,		_farEnd);
			glProgramUniformMatrix4fv(cocPass.program.id,	cocPass.viewMatVar,		1, GL_FALSE, &_view[0][0]);
			_positionTex.Bind(cocPass.positionTexUnit);
			_renderTarget.Draw();
			glf::CheckError("DOFProcessor::DrawBLURDEPTH");
		glf::manager::timings->EndSection(section::DofBlurDepth);

		// Detect pixel which are bokeh and output color of pixels which are not bokeh
		glf::manager::timings->StartSection(section::DofDetection);
		glUseProgram(detectionPass.program.id);
			glBindFramebuffer(GL_FRAMEBUFFER,detectionFBO);
			glClear(GL_COLOR_BUFFER_BIT);
			glProgramUniform1f(detectionPass.program.id,detectionPass.cocThresholdVar,_cocThreshold);
			glProgramUniform1f(detectionPass.program.id,detectionPass.lumThresholdVar,_lumThreshold);
			glProgramUniform1f(detectionPass.program.id,detectionPass.maxCoCRadiusVar,_maxCoCRadius);

			glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER,0,bokehCounterACB.id);
			glActiveTexture(GL_TEXTURE0 + detectionPass.bokehPositionTexUnit);
			glBindImageTexture(detectionPass.bokehPositionTexUnit, bokehPositionTex.id,0,false,0,GL_WRITE_ONLY,GL_RGBA32F);
			glActiveTexture(GL_TEXTURE0 + detectionPass.bokehColorTexUnit);
			glBindImageTexture(detectionPass.bokehColorTexUnit, bokehColorTex.id,0,false,0,GL_WRITE_ONLY,GL_RGBA32F);

			blurDepthTex.Bind(detectionPass.blurDepthTexUnit);
			_colorTex.Bind(detectionPass.colorTexUnit);
			_renderTarget.Draw();
			glf::CheckError("DOFProcessor::DrawDETECTION");
		glf::manager::timings->EndSection(section::DofDetection);

		glf::manager::timings->StartSection(section::DofBlur);
		if(_poissonFiltering)
		{
		glUseProgram(blurPoissonPass.program.id);
			glBindFramebuffer(GL_FRAMEBUFFER,_renderTarget.framebuffer);
			glClear(GL_COLOR_BUFFER_BIT);
			glProgramUniform1f(blurPoissonPass.program.id,		blurPoissonPass.maxCoCRadiusVar,	_maxCoCRadius);
			glProgramUniform1i(blurPoissonPass.program.id,		blurPoissonPass.nSamplesVar,		_nSamples);
			blurDepthTex.Bind(blurPoissonPass.blurDepthTexUnit);
			detectionTex.Bind(blurPoissonPass.colorTexUnit);
			rotationTex.Bind(blurPoissonPass.rotationTexUnit);
			_renderTarget.Draw();
			glf::CheckError("DOFProcessor::DrawPOISSONBLUR");
		}
		else
		{
		// Vertical blur of pixels which are not bokehs
		glUseProgram(blurSeparablePass.program.id);
			glBindFramebuffer(GL_FRAMEBUFFER,blurFBO);
			glClear(GL_COLOR_BUFFER_BIT);
			glProgramUniform1f(blurSeparablePass.program.id,		blurSeparablePass.maxCoCRadiusVar,	_maxCoCRadius);
			glProgramUniform2f(blurSeparablePass.program.id,		blurSeparablePass.directionVar,		1,0);
			blurDepthTex.Bind(blurSeparablePass.blurDepthTexUnit);
			detectionTex.Bind(blurSeparablePass.colorTexUnit);
			_renderTarget.Draw();
			glf::CheckError("DOFProcessor::DrawVBLUR");

		// Horizontal blur of pixels which are not bokehs
			glBindFramebuffer(GL_FRAMEBUFFER,_renderTarget.framebuffer);
			glClear(GL_COLOR_BUFFER_BIT);
			glProgramUniform1f(blurSeparablePass.program.id,		blurSeparablePass.maxCoCRadiusVar,	_maxCoCRadius);
			glProgramUniform2f(blurSeparablePass.program.id,		blurSeparablePass.directionVar,		0,1);
			blurDepthTex.Bind(blurSeparablePass.blurDepthTexUnit);
			blurTex.Bind(blurSeparablePass.colorTexUnit);
			_renderTarget.Draw();
			glf::CheckError("DOFProcessor::DrawHBLUR");
		}
		glf::manager::timings->EndSection(section::DofBlur);

		// Synchronize bokeh count with indirect draw buffer (draw a dummy point)
		glf::manager::timings->StartSection(section::DofSynchronization);
		glUseProgram(synchronizationPass.program.id);
			glActiveTexture(GL_TEXTURE0 + synchronizationPass.indirectBufferTexUnit);
			glBindImageTexture(synchronizationPass.indirectBufferTexUnit, indirectBufferTexID,0,false,0,GL_WRITE_ONLY, GL_R32UI);
			pointVAO.Draw(GL_POINTS,1,0);
			glf::CheckError("DOFProcessor::DrawSYNCHRONIZATION");
		glf::manager::timings->EndSection(section::DofSynchronization);

		// Render bokeh as textured quad (with additive blending)
		glf::manager::timings->StartSection(section::DofRendering);
		glUseProgram(renderingPass.program.id);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
			#if 0
			int count, primCount,first,reservedMustBeZero;
			DrawArraysIndirectCommand* indirectCmd = pointIndirectBuffer.Lock();
				count              = indirectCmd[0].count;
				primCount          = indirectCmd[0].primCount;
				first              = indirectCmd[0].first;
				reservedMustBeZero = indirectCmd[0].reservedMustBeZero;
			pointIndirectBuffer.Unlock();
			std::stringstream out;
			out << "count : " << count << "  ";
			out << "primCount : " << primCount << "  ";
			out << "first : " << first << "  ";
			out << "reservedMustBeZero  : " << reservedMustBeZero;
			glf::Info("%s",out.str().c_str());
			#endif
			glProgramUniform1f(renderingPass.program.id,renderingPass.maxBokehRadiusVar,_maxBokehRadius);
			glProgramUniform1f(renderingPass.program.id,renderingPass.bokehDepthCutoffVar,_bokehDepthCutoff);
			bokehPositionTex.Bind(renderingPass.bokehPositionTexUnit);
			bokehColorTex.Bind(renderingPass.bokehColorTexUnit);
			bokehShapeTex.Bind(renderingPass.bokehShapeTexUnit);
			blurDepthTex.Bind(renderingPass.blurDepthTexUnit);			
			pointVAO.Draw(GL_POINTS,pointIndirectBuffer);
			glf::CheckError("DOFProcessor::DrawRENDERING");
		glf::manager::timings->EndSection(section::DofRendering);
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glf::CheckError("DOFProcessor::DrawEnd");
	}
}

