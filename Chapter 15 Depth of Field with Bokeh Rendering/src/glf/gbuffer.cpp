//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/gbuffer.hpp>
#include <glf/geometry.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace glf
{
	//--------------------------------------------------------------------------
	GBuffer::GBuffer(				unsigned int _width, 
									unsigned int _height)
	{
		// Initialize G-Buffer textures
		positionTex.Allocate(GL_RGBA32F,_width,_height);
		normalTex.Allocate(GL_RGBA16F,_width,_height);
		diffuseTex.Allocate(GL_RGBA16F,_width,_height);
		depthTex.Allocate(GL_DEPTH32F_STENCIL8,_width,_height);
		positionTex.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
		normalTex.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
		diffuseTex.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
		depthTex.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);

		// Initialize framebuffer
		int outPosition			= 0;
		int outDiffuseSpecular	= 1;
		int outNormalRoughness	= 2;
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);

		// Attach output textures
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0 + outPosition, positionTex.target, positionTex.id, 0);
		glf::CheckFramebuffer(framebuffer);

		glBindTexture(diffuseTex.target,diffuseTex.id);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0 + outDiffuseSpecular, diffuseTex.target, diffuseTex.id, 0);
		glf::CheckFramebuffer(framebuffer);
		
		glBindTexture(normalTex.target,normalTex.id);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0 + outNormalRoughness, normalTex.target, normalTex.id, 0);
		glf::CheckFramebuffer(framebuffer);

		glBindTexture(depthTex.target,depthTex.id);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT, depthTex.target, depthTex.id, 0);
		glf::CheckFramebuffer(framebuffer);

		GLenum drawBuffers[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
		glDrawBuffers(3,drawBuffers);
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// Program regular mesh
		ProgramOptions regularOptions = ProgramOptions::CreateVSOptions();
		regularOptions.AddDefine<int>("GBUFFER",				1);
		regularOptions.AddDefine<int>("OUT_POSITION",			outPosition);
		regularOptions.AddDefine<int>("OUT_DIFFUSE_SPECULAR",	outDiffuseSpecular);
		regularOptions.AddDefine<int>("OUT_NORMAL_ROUGHNESS",	outNormalRoughness);
		regularRenderer.program.Compile(regularOptions.Append(LoadFile(directory::ShaderDirectory + "meshregular.vs")),
										regularOptions.Append(LoadFile(directory::ShaderDirectory + "meshregular.fs")));

		regularRenderer.transformVar	= regularRenderer.program["Transform"].location;
		regularRenderer.modelVar		= regularRenderer.program["Model"].location;
		regularRenderer.diffuseTexUnit	= regularRenderer.program["DiffuseTex"].unit;
		regularRenderer.normalTexUnit	= regularRenderer.program["NormalTex"].unit;
		regularRenderer.roughnessVar	= regularRenderer.program["Roughness"].location;
		regularRenderer.specularityVar	= regularRenderer.program["Specularity"].location;

		glProgramUniform1i(regularRenderer.program.id, regularRenderer.program["DiffuseTex"].location, regularRenderer.diffuseTexUnit);
		glProgramUniform1i(regularRenderer.program.id, regularRenderer.program["NormalTex"].location,  regularRenderer.normalTexUnit);


		// Program terrain mesh
		ProgramOptions terrainOptions = ProgramOptions::CreateVSOptions();
		terrainOptions.AddDefine<int>("GBUFFER",				1);
		terrainOptions.AddDefine<int>("OUT_POSITION",			outPosition);
		terrainOptions.AddDefine<int>("OUT_DIFFUSE_SPECULAR",	outDiffuseSpecular);
		terrainOptions.AddDefine<int>("OUT_NORMAL_ROUGHNESS",	outNormalRoughness);
		terrainRenderer.program.Compile(terrainOptions.Append(LoadFile(directory::ShaderDirectory + "meshterrain.vs")),
										terrainOptions.Append(LoadFile(directory::ShaderDirectory + "meshterrain.cs")),
										terrainOptions.Append(LoadFile(directory::ShaderDirectory + "meshterrain.es")),
										terrainOptions.Append(LoadFile(directory::ShaderDirectory + "meshterrain.fs")));

		terrainRenderer.transformVar	= terrainRenderer.program["Transform"].location;
		terrainRenderer.diffuseTexUnit	= terrainRenderer.program["DiffuseTex"].unit;
		terrainRenderer.normalTexUnit	= terrainRenderer.program["NormalTex"].unit;
		terrainRenderer.heightTexUnit	= terrainRenderer.program["HeightTex"].unit;
		terrainRenderer.roughnessVar	= terrainRenderer.program["Roughness"].location;
		terrainRenderer.specularityVar	= terrainRenderer.program["Specularity"].location;

		terrainRenderer.tileSizeVar		= terrainRenderer.program["TileSize"].location;
		terrainRenderer.tileCountVar	= terrainRenderer.program["TileCount"].location;
		terrainRenderer.tileOffsetVar	= terrainRenderer.program["TileOffset"].location;
		terrainRenderer.projFactorVar	= terrainRenderer.program["ProjFactor"].location;
		terrainRenderer.tessFactorVar	= terrainRenderer.program["TessFactor"].location;
		terrainRenderer.heightFactorVar	= terrainRenderer.program["HeightFactor"].location;
		terrainRenderer.tileFactorVar	= terrainRenderer.program["TileFactor"].location;

		glProgramUniform1i(terrainRenderer.program.id, terrainRenderer.program["DiffuseTex"].location, terrainRenderer.diffuseTexUnit);
		glProgramUniform1i(terrainRenderer.program.id, terrainRenderer.program["NormalTex"].location,  terrainRenderer.normalTexUnit);
		glProgramUniform1i(terrainRenderer.program.id, terrainRenderer.program["HeightTex"].location,  terrainRenderer.heightTexUnit);

		glf::CheckError("GBuffer::GBuffer");
	}
	//--------------------------------------------------------------------------
	GBuffer::~GBuffer()
	{
		glDeleteFramebuffers(1,&framebuffer);
	}	
	//--------------------------------------------------------------------------
	void GBuffer::Draw(				const glm::mat4& _projection,
									const glm::mat4& _view,
									const SceneManager& _scene)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glm::mat4 transform = _projection * _view;

		int nMeshes = int(_scene.regularMeshes.size());
		if(nMeshes>0)
		{
			// Render at the same resolution than the original window
			// Draw all objects
			glUseProgram(regularRenderer.program.id);
			glProgramUniformMatrix4fv(regularRenderer.program.id, regularRenderer.transformVar,  1, GL_FALSE, &transform[0][0]);
			for(int i=0;i<nMeshes;++i)
			{
				const RegularMesh& mesh = _scene.regularMeshes[i];
				glProgramUniformMatrix4fv(regularRenderer.program.id, regularRenderer.modelVar,  1, GL_FALSE, &_scene.transformations[i][0][0]);
				glProgramUniform1f(regularRenderer.program.id, regularRenderer.roughnessVar,   mesh.roughness);
				glProgramUniform1f(regularRenderer.program.id, regularRenderer.specularityVar, mesh.specularity);

				mesh.diffuseTex->Bind(regularRenderer.diffuseTexUnit);
				mesh.normalTex->Bind(regularRenderer.normalTexUnit);
				mesh.Draw();
			}
			glf::CheckError("GBuffer::Draw::Regulars");
		}

		int nTerrains = int(_scene.terrainMeshes.size());
		if(nTerrains>0)
		{
			// Render at the same resolution than the original window
			// Draw all objects
			glUseProgram(terrainRenderer.program.id);
			glProgramUniformMatrix4fv(terrainRenderer.program.id, terrainRenderer.transformVar,  1, GL_FALSE, &transform[0][0]);
			for(int i=0;i<nTerrains;++i)
			{
				const TerrainMesh& mesh = _scene.terrainMeshes[i];
				glProgramUniform3f(terrainRenderer.program.id, terrainRenderer.tileOffsetVar,	mesh.tileOffset.x, mesh.tileOffset.y, mesh.tileOffset.z);
				glProgramUniform2i(terrainRenderer.program.id, terrainRenderer.tileCountVar,	mesh.tileCount.x, mesh.tileCount.y);
				glProgramUniform2f(terrainRenderer.program.id, terrainRenderer.tileSizeVar,		mesh.tileSize.x, mesh.tileSize.y);
				glProgramUniform1f(terrainRenderer.program.id, terrainRenderer.tessFactorVar,	mesh.tessFactor);
				glProgramUniform1f(terrainRenderer.program.id, terrainRenderer.heightFactorVar,	mesh.heightFactor);
				glProgramUniform1f(terrainRenderer.program.id, terrainRenderer.projFactorVar,	mesh.projFactor);
				glProgramUniform1f(terrainRenderer.program.id, terrainRenderer.roughnessVar,	mesh.roughness);
				glProgramUniform1f(terrainRenderer.program.id, terrainRenderer.specularityVar,	mesh.specularity);
				glProgramUniform1f(terrainRenderer.program.id, terrainRenderer.tileFactorVar,	mesh.tileFactor);

				mesh.diffuseTex->Bind(terrainRenderer.diffuseTexUnit);
				mesh.normalTex->Bind(terrainRenderer.normalTexUnit);
				mesh.heightTex->Bind(terrainRenderer.heightTexUnit);
				mesh.Draw();
			}
			glf::CheckError("GBuffer::Draw::Terrains");
		}

		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glf::CheckError("GBuffer::Draw");
	}
	//--------------------------------------------------------------------------
}
