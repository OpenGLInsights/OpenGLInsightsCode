//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/terrain.hpp>
#include <glf/geometry.hpp>

namespace glf
{
	//--------------------------------------------------------------------------
	TerrainBuilder::TerrainBuilder()
	{
		CreateScreenTriangle(vbo);
		vao.Add(vbo,semantic::Position,2,GL_FLOAT);

		ProgramOptions options = ProgramOptions::CreateVSOptions();
		options.AddDefine<int>("NORMAL_BUILDER",1);
		normalBuilder.program.Compile(	options.Append(LoadFile(directory::ShaderDirectory + "terrainbuilder.vs")),
										options.Append(LoadFile(directory::ShaderDirectory + "terrainbuilder.fs")));
		normalBuilder.heightFactorVar= normalBuilder.program["HeightFactor"].location;
		normalBuilder.terrainSizeVar = normalBuilder.program["TerrainSize"].location;
		normalBuilder.heightTexUnit  = normalBuilder.program["HeightTex"].unit;
		glProgramUniform1i(normalBuilder.program.id, normalBuilder.program["HeightTex"].location, normalBuilder.heightTexUnit);

		glGenFramebuffers(1,&framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glf::CheckFramebuffer(framebuffer);

		glf::CheckError("TerrainBuilder::TerrainBuilder");
	}
	//--------------------------------------------------------------------------
	void TerrainBuilder::BuildNormals(	Texture2D* _heightTexture,
										Texture2D* _normalTexture,
										const glm::vec2& _terrainSize,
										float _heightFactor)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,_normalTexture->target,_normalTexture->id,0);
		glViewport(0,0,_heightTexture->size.x,_heightTexture->size.y);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(normalBuilder.program.id);
		glProgramUniform1f(normalBuilder.program.id, normalBuilder.heightFactorVar, _heightFactor);
		glProgramUniform2f(normalBuilder.program.id, normalBuilder.terrainSizeVar,  _terrainSize.x, _terrainSize.y);
		_heightTexture->Bind(normalBuilder.heightTexUnit);
		vao.Draw(GL_TRIANGLES,3,0);
		glBindFramebuffer(GL_FRAMEBUFFER,0);

		// Filter normals
		glBindTexture(_normalTexture->target,_normalTexture->id);
		glGenerateMipmap(_normalTexture->target);
		glBindTexture(_normalTexture->target,0);

		glf::CheckError("TerrainBuilder::BuildNormals");
	}
	//--------------------------------------------------------------------------
	void TerrainBuilder::BuildOcclusion(Texture2D* _heightTexture,
										Texture2D* _occlusionTexture,
										float _heightFactor)
	{

		glf::CheckError("TerrainBuilder::BuildOcclusions");
	}
	//--------------------------------------------------------------------------
	TerrainMesh::TerrainMesh(	const glm::vec2 _terrainSize,
								const glm::vec3 _terrainOffset,
								Texture2D* _diffuseTexture,
								Texture2D* _normalTexture,
								Texture2D* _heightTexture,
								float _tileFactor,
								float _roughness,
								float _specularity,
								int _tileResolution):
	tileOffset(_terrainOffset),
	terrainSize(_terrainSize),
	tileFactor(_tileFactor),
	diffuseTex(_diffuseTexture),
	normalTex(_normalTexture),
	heightTex(_heightTexture),
	roughness(_roughness),
	specularity(_specularity)
	{
		assert(glf::CheckError("TerrainMesh::TerrainMesh"));
		Tesselation(_tileResolution,1,16,20);
	}
	//--------------------------------------------------------------------------
	void TerrainMesh::Tesselation(	int   _tileResolution,
									float _heightFactor,
									float _tessFactor,
									float _projFactor)
	{
		// Set LOD parameters
		tileCount 		= glm::ivec2(diffuseTex->size.x/_tileResolution,diffuseTex->size.y/_tileResolution);
		tileSize  		= glm::vec2(terrainSize.x / float(tileCount.x),terrainSize.x / float(tileCount.y));
		heightFactor	= _heightFactor;
		tessFactor 		= _tessFactor;
		projFactor 		= _projFactor;
	}
	//--------------------------------------------------------------------------
	void TerrainMesh::Draw() const
	{
		// TODO : Add frustum culling 
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		primitive->Draw(GL_PATCHES, 4, 0, tileCount.x*tileCount.y);

		assert(glf::CheckError("TerrainMesh::Draw"));
	}
	//--------------------------------------------------------------------------
	BBox TerrainMesh::Bound() const
	{
		BBox bound;
		bound.pMin = tileOffset;
		bound.pMax = tileOffset + glm::vec3(terrainSize,heightFactor);
		return bound;
	}
}

