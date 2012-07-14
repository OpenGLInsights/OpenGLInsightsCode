#ifndef GLF_TERRAIN_HPP
#define GLF_TERRAIN_HPP

//-----------------------------------------------------------------------------
// Include
//-----------------------------------------------------------------------------
#include <glf/buffer.hpp>
#include <glf/bound.hpp>
#include <glf/wrapper.hpp>
#include <glf/texture.hpp>
#include <glm/glm.hpp>

namespace glf
{
	//--------------------------------------------------------------------------
	class TerrainBuilder
	{
	public:
				TerrainBuilder(				);
		void	BuildNormals(				Texture2D* _heightTexture,
											Texture2D* _normalTexture,
											const glm::vec2& _terrainSize,
											float _heightFactor=1.f);
		void	BuildOcclusion(				Texture2D* _heightTexture,
											Texture2D* _occlusionTexture,
											float _heightFactor=1.f);
	private:
		struct NormalBuilder
		{
											NormalBuilder():program("TerrainBuilder::Normal"){}
			GLint							heightFactorVar;
			GLint							terrainSizeVar;
			GLint							heightTexUnit;
			Program							program;
		};

		glf::VertexBuffer2F					vbo;
		glf::VertexArray					vao;
		GLuint								framebuffer;
		NormalBuilder						normalBuilder;
	};
	//--------------------------------------------------------------------------
	class TerrainMesh
	{
	public:
				TerrainMesh(				const glm::vec2 _terrainSize,
											const glm::vec3 _terrainOffset,
											Texture2D* _diffuseTexture,
											Texture2D* _normalTexture,
											Texture2D* _heightTexture,
											float _tileFactor,
											float _roughness,
											float _specularity,
											int _tileResolution=32);
		void 	Draw(						) const;
		void	Tesselation(				int   _tileResolution,
											float _heightFactor,
											float _tessFactor,
											float _projFactor);
		BBox	Bound(						) const;
	public:
		glf::VertexArray*					primitive;

		glm::vec2							tileSize;		// Tile size
		glm::ivec2							tileCount;		// Number of tiles for the terrain
		glm::vec3							tileOffset; 	// World space offset
		glm::vec2							terrainSize;	// Terrain size (tileCount * tileSize)

		float								heightFactor;	// Height of the heightfield
		float								tessFactor;		// Tesselation factor ]0,32]
		float								projFactor;		// For correcting projection estimation
		float								tileFactor;		// Factor for tilling diffuse texture

		glf::Texture2D*						diffuseTex;
		glf::Texture2D*						normalTex;
		glf::Texture2D*						heightTex;

		float								roughness;
		float								specularity;
	};
}

#endif
