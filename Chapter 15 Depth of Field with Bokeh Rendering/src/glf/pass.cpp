//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/pass.hpp>
#include <glf/geometry.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace glf
{
	//--------------------------------------------------------------------------
	RenderSurface::RenderSurface(	unsigned int _width, 
									unsigned int _height):

	frameSize(_width,_height)
	{

		CreateScreenTriangle(vbo);
		vao.Add(vbo,semantic::Position,2,GL_FLOAT);

		// Regular 2D textures
		ProgramOptions regularOptions = ProgramOptions::CreateVSOptions();
		regularOptions.AddDefine<int>("REGULAR",1);
		regularOptions.AddResolution("SCREEN",_width,_height);
		regularRenderer.program.Compile(regularOptions.Append(LoadFile(directory::ShaderDirectory + "surface.vs")),
										regularOptions.Append(LoadFile(directory::ShaderDirectory + "surface.fs")));

		regularRenderer.textureUnit		= regularRenderer.program["Texture"].unit;
		regularRenderer.levelVar		= regularRenderer.program["Level"].location;
		glProgramUniform1i(regularRenderer.program.id, regularRenderer.program["Texture"].location, regularRenderer.textureUnit);

		// Array 2D textures
		ProgramOptions arrayOptions = ProgramOptions::CreateVSOptions();
		arrayOptions.AddDefine<int>("ARRAY",1);
		arrayOptions.AddResolution("SCREEN",_width,_height);
		arrayRenderer.program.Compile(	arrayOptions.Append(LoadFile(directory::ShaderDirectory + "surface.vs")),
										arrayOptions.Append(LoadFile(directory::ShaderDirectory + "surface.fs")));

		arrayRenderer.textureUnit		= arrayRenderer.program["Texture"].unit;
		arrayRenderer.levelVar			= arrayRenderer.program["Level"].location;
		arrayRenderer.layerVar			= arrayRenderer.program["Layer"].location;
		glProgramUniform1i(arrayRenderer.program.id, arrayRenderer.program["Texture"].location, arrayRenderer.textureUnit);

		glf::CheckError("Surface::Surface");
	}
	//--------------------------------------------------------------------------
	void RenderSurface::Draw(		const Texture2D& _texture,
									int _level)
	{
		assert(_texture.size.x==frameSize.x);
		assert(_texture.size.y==frameSize.y);

		glUseProgram(regularRenderer.program.id);
		_texture.Bind(regularRenderer.textureUnit);
		glProgramUniform1f(regularRenderer.program.id, regularRenderer.levelVar, float(_level));
		vao.Draw(GL_TRIANGLES,3,0);
		glf::CheckError("Surface::Regular::Draw");
	}
	//--------------------------------------------------------------------------
	void RenderSurface::Draw(		const TextureArray2D& _texture,
									int _layer,
									int _level)
	{
//		assert(_texture.size.x==frameSize.x);
//		assert(_texture.size.y==frameSize.y);

		glUseProgram(arrayRenderer.program.id);
		_texture.Bind(arrayRenderer.textureUnit);
		glProgramUniform1f(arrayRenderer.program.id, arrayRenderer.levelVar, float(_level));
		glProgramUniform1f(arrayRenderer.program.id, arrayRenderer.layerVar, float(_layer));
		vao.Draw(GL_TRIANGLES,3,0);
		glf::CheckError("Surface::Array::Draw");
	}
	//--------------------------------------------------------------------------
	RenderTarget::RenderTarget(				unsigned int _width, 
											unsigned int _height)
	{
		CreateScreenTriangle(vbo);
		vao.Add(vbo,semantic::Position,2,GL_FLOAT);
		texture.Allocate(GL_RGBA32F,_width,_height,true);
		texture.SetFiltering(GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR);
		texture.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, texture.target, texture.id, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glf::CheckFramebuffer(framebuffer);
		glf::CheckError("RenderTarget::RenderTarget");
	}
	//--------------------------------------------------------------------------
	void RenderTarget::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
	}
	//--------------------------------------------------------------------------
	void RenderTarget::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
	//--------------------------------------------------------------------------
	RenderTarget::~RenderTarget()
	{
		glDeleteFramebuffers(1, &framebuffer);
	}
	//--------------------------------------------------------------------------
	void RenderTarget::Draw() const
	{
		vao.Draw(GL_TRIANGLES,3,0);
	}
	//--------------------------------------------------------------------------
	void RenderTarget::AttachDepthStencil(const Texture2D& _depthStencilTex)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
		glFramebufferTexture2D(	GL_FRAMEBUFFER,
								GL_DEPTH_STENCIL_ATTACHMENT, 
								_depthStencilTex.target, 
								_depthStencilTex.id, 0);
		glf::CheckFramebuffer(framebuffer);
		glf::CheckError("AccumulationBuffer::AttachStencil");
	}
	//--------------------------------------------------------------------------
}
