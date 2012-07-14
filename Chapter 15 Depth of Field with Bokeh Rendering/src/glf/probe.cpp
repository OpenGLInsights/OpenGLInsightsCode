//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/probe.hpp>
#include <glf/debug.hpp>
#include <glf/utils.hpp>
#include <glm/gtx/transform.hpp>
#include <glf/window.hpp>
#include <glf/geometry.hpp>

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
#define DISPLAY_SH_COEFFICIENTS 0

namespace glf
{
	//--------------------------------------------------------------------------
	ProbeLight::ProbeLight(int _resolution) 
	{ 
		for(int i=0;i<9;++i)
			shCoeffs[i]=glm::vec3(0);

		cubeTex.Allocate(GL_RGBA32F, _resolution, true);
		cubeTex.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
		cubeTex.SetFiltering(GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR);
	}
	//--------------------------------------------------------------------------
	ProbeBuilder::ProbeBuilder(int _resolution):
	program("ProbeBuiler"),
	resolution(_resolution)
	{
		CreateScreenTriangle(vbo);
		vao.Add(vbo,semantic::Position,2,GL_FLOAT);

		const int outCoeffs0 = 0;
		const int outCoeffs1 = 1;
		const int outCoeffs2 = 2;
		const int outCoeffs3 = 3;
		const int outCoeffs4 = 4;
		const int outCoeffs5 = 5;
		const int outCoeffs6 = 6;
		ProgramOptions options = ProgramOptions::CreateVSOptions();
		options.AddDefine<int>("BUILDER",1);
		options.AddDefine<int>("OUT_COEFF0",outCoeffs0);
		options.AddDefine<int>("OUT_COEFF1",outCoeffs1);
		options.AddDefine<int>("OUT_COEFF2",outCoeffs2);
		options.AddDefine<int>("OUT_COEFF3",outCoeffs3);
		options.AddDefine<int>("OUT_COEFF4",outCoeffs4);
		options.AddDefine<int>("OUT_COEFF5",outCoeffs5);
		options.AddDefine<int>("OUT_COEFF6",outCoeffs6);
		program.Compile(	options.Append(LoadFile(directory::ShaderDirectory + "probe.vs")),
							options.Append(LoadFile(directory::ShaderDirectory + "probe.fs")));

		glm::mat4 transformations[6];
		transformations[0] = glm::rotate(-90.f,0.f,0.f,1.f) * glm::rotate(90.f,1.f,0.f,0.f); 				// Positive X
		transformations[1] = glm::rotate( 90.f,0.f,0.f,1.f) * glm::rotate(90.f,1.f,0.f,0.f); 				// Negative X
		transformations[2] = glm::rotate( 90.f,1.f,0.f,0.f);												// Positive Y
		transformations[3] = glm::rotate(180.f,0.f,0.f,1.f) * glm::rotate(90.f,1.f,0.f,0.f); 				// Negative Y
		transformations[4] = glm::rotate(180.f,1.f,0.f,0.f);												// Positive Z
		transformations[5] = glm::mat4(1);																	// Negative Z
		glProgramUniformMatrix4fv(program.id, program["Transformations[0]"].location, 6, GL_FALSE, &transformations[0][0][0]);
		cubeTexUnit = program["CubeTex"].unit;
		glProgramUniform1i(program.id, program["CubeTex"].location, cubeTexUnit);

		// Would be 16FP enough precise ?
		shTex.Allocate(GL_RGBA32F,_resolution,_resolution,7,true);
		shTex.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
		shTex.SetFiltering(GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR);

		glGenFramebuffers(1,&shFBO);
		glBindFramebuffer(GL_FRAMEBUFFER,shFBO);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + outCoeffs0, shTex.id, 0, 0);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + outCoeffs1, shTex.id, 0, 1);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + outCoeffs2, shTex.id, 0, 2);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + outCoeffs3, shTex.id, 0, 3);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + outCoeffs4, shTex.id, 0, 4);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + outCoeffs5, shTex.id, 0, 5);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + outCoeffs6, shTex.id, 0, 6);
		GLenum buffers[7] = {	GL_COLOR_ATTACHMENT0, 
								GL_COLOR_ATTACHMENT1,
								GL_COLOR_ATTACHMENT2,
								GL_COLOR_ATTACHMENT3,
								GL_COLOR_ATTACHMENT4,
								GL_COLOR_ATTACHMENT5,
								GL_COLOR_ATTACHMENT6 };
		glDrawBuffers(7,buffers);
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glf::CheckFramebuffer(shFBO);

		glf::CheckError("ProbeBuilder::ProbeBuilder");
	}
	//--------------------------------------------------------------------------
	void ProbeBuilder::Filter(	ProbeLight& 				_probe,
								int 						_level)
	{
		assert(_probe.cubeTex.size.x == shTex.size.x);
		assert(_probe.cubeTex.size.y == shTex.size.y);
		assert(_probe.cubeTex.levels == shTex.levels);

		glViewport(0,0,shTex.size.x,shTex.size.y);
		glBindFramebuffer(GL_FRAMEBUFFER,shFBO);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program.id);
		_probe.cubeTex.Bind(cubeTexUnit);
		vao.Draw(GL_TRIANGLES,vbo.count,0);

		// Reduce/Sum SH Coeffs
		glBindTexture(GL_TEXTURE_2D_ARRAY,shTex.id);
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		// Retrieve coefficients
		// We multiple by the number of pixels in order to compensate the 
		// average process of the mipmap (we want the sum, not the average)
		//
		// The last two Probe coeffs are stored into the first 6 w components
		int nMipmaps = glf::MipmapLevels(resolution);
		float factor = float(resolution*resolution);
		float coeffs[28];
		glGetTexImage(GL_TEXTURE_2D_ARRAY, nMipmaps-1, GL_RGBA, GL_FLOAT, coeffs);
		_probe.shCoeffs[0] = glm::vec3(coeffs[0], coeffs[1], coeffs[2])  * factor;
		_probe.shCoeffs[1] = glm::vec3(coeffs[4], coeffs[5], coeffs[6])  * factor;
		_probe.shCoeffs[2] = glm::vec3(coeffs[8], coeffs[9], coeffs[10]) * factor;
		_probe.shCoeffs[3] = glm::vec3(coeffs[12],coeffs[13],coeffs[14]) * factor;
		_probe.shCoeffs[4] = glm::vec3(coeffs[16],coeffs[17],coeffs[18]) * factor;
		_probe.shCoeffs[5] = glm::vec3(coeffs[20],coeffs[21],coeffs[22]) * factor;
		_probe.shCoeffs[6] = glm::vec3(coeffs[24],coeffs[25],coeffs[26]) * factor;
		_probe.shCoeffs[7] = glm::vec3(coeffs[3], coeffs[7], coeffs[11]) * factor;
		_probe.shCoeffs[8] = glm::vec3(coeffs[15],coeffs[19],coeffs[23]) * factor;

		#if DISPLAY_SH_COEFFICIENTS
		for(int i=0;i<9;++i)
			glf::Info("Coeffs %d : %f %f %f",i,_probe.coeffs[i].x,_probe.coeffs[i].y,_probe.coeffs[i].z);
		#endif

		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport(0,0,ctx::window.Size.x,ctx::window.Size.y);

		// Reduce/Filter cubemap
		glBindTexture(GL_TEXTURE_CUBE_MAP,_probe.cubeTex.id);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP,0);

		glf::CheckError("ProbeBuilder::Update");
	}
	//-------------------------------------------------------------------------
	ProbeRenderer::ProbeRenderer(int _w, int _h):
	program("ProbeRenderer")
	{
		ProgramOptions options = ProgramOptions::CreateVSOptions();
		options.AddDefine<int>("RENDERER",1);
		options.AddDefine<int>("DIFFUSE_REFLECTION",1);
		options.AddDefine<int>("LIGHTING_ONLY",ENABLE_LIGHTING_ONLY);
		options.Include(LoadFile(directory::ShaderDirectory + "brdf.fs"));
		program.Compile(options.Append(LoadFile(directory::ShaderDirectory + "probe.vs")),
						options.Append(LoadFile(directory::ShaderDirectory + "probe.fs")));

		shCoeffsVar			= program["SHCoeffs[0]"].location;
		normalTexUnit		= program["NormalTex"].unit;
		diffuseTexUnit		= program["DiffuseTex"].unit;
		glProgramUniform1i(program.id, program["NormalTex"].location, normalTexUnit);
		glProgramUniform1i(program.id, program["DiffuseTex"].location, diffuseTexUnit);

		// For all reflections
//		viewPosVar			= program["ViewPos"].location;
//		cubeTexUnit			= program["CubeTex"].unit;
//		positionTexUnit		= program["PositionTex"].unit;
//		glProgramUniform1i(program.id, program["CubeTex"].location, cubeTexUnit);
//		glProgramUniform1i(program.id, program["PositionTex"].location, positionTexUnit);

	}
	//-------------------------------------------------------------------------
	void ProbeRenderer::Draw(	const ProbeLight&	_probe,
								const GBuffer&		_gbuffer,
								const glm::vec3&	_viewPos,
								const RenderTarget& _renderTarget)
	{
		glUseProgram(program.id);

		// For all reflections
//		glProgramUniform3f(program.id, viewPosVar, _viewPos.x, _viewPos.y, _viewPos.z);
//		_probe.cubeTex.Bind(cubeTexUnit);
//		_gbuffer.positionTex.Bind(positionTexUnit);

		glProgramUniform3fv(program.id, shCoeffsVar, 9, (float*)(&_probe.shCoeffs[0]));
		_gbuffer.diffuseTex.Bind(diffuseTexUnit);
		_gbuffer.normalTex.Bind(normalTexUnit);
		_renderTarget.Draw();

		glf::CheckError("ProbeRenderer::Draw");
	}
}

