//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/postprocessor.hpp>
#include <glf/window.hpp>
#include <glf/utils.hpp>
#include <glf/geometry.hpp>


namespace glf
{
	//--------------------------------------------------------------------------
	PostProcessor::ToneMapping::ToneMapping():
	program("ToneMapping")
	{
	
	}
	//--------------------------------------------------------------------------
	PostProcessor::PostProcessor(		unsigned int _width, 
										unsigned int _height)
	{
		// Tone Mapping
		{
			toneMapping.program.Compile(	ProgramOptions::CreateVSOptions().Append(LoadFile(directory::ShaderDirectory + "tonemap.vs")),
											LoadFile(directory::ShaderDirectory + "tonemap.fs") );
			toneMapping.colorTexUnit		= toneMapping.program["ColorTex"].unit;
			toneMapping.exposureVar			= toneMapping.program["Exposure"].location;
			glProgramUniform1i(toneMapping.program.id, toneMapping.program["ColorTex"].location, toneMapping.colorTexUnit);
		}
	}
	//--------------------------------------------------------------------------
	PostProcessor::~PostProcessor()
	{
	}
	//--------------------------------------------------------------------------
	void PostProcessor::Draw(			const Texture2D& _colorTex,
										float _toneExposure,
										const RenderTarget& _renderTarget)
	{
		glUseProgram(toneMapping.program.id);
		glProgramUniform1f(toneMapping.program.id, toneMapping.exposureVar, _toneExposure);
		_colorTex.Bind(toneMapping.colorTexUnit);
		_renderTarget.Draw();
	}
	//--------------------------------------------------------------------------
}

