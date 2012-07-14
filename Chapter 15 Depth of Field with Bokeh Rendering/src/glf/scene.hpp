#ifndef GLF_SCENE_HPP
#define GLF_SCENE_HPP

//------------------------------------------------------------------------------
// Include
//------------------------------------------------------------------------------
#include <glf/wrapper.hpp>
#include <glf/texture.hpp>
#include <glf/buffer.hpp>
#include <glf/memory.hpp>
#include <glf/bound.hpp>
#include <glf/terrain.hpp>
#include <vector>

namespace glf
{
	//--------------------------------------------------------------------------
	namespace semantic
	{
		extern GLint Position;
		extern GLint Normal;
		extern GLint TexCoord;
		extern GLint Tangent;
		extern GLint Bitangent;
	}
	//--------------------------------------------------------------------------
	struct ShadowMesh
	{
	public:
										ShadowMesh();
		IndexBuffer*					indices;
		unsigned int 					startIndices;
		unsigned int 					countIndices;
		GLenum							primitiveType;
		VertexArray*					primitive;
		void							Draw() const
		{
			primitive->Draw(primitiveType,*indices,countIndices,startIndices);
		}
	};
	//--------------------------------------------------------------------------
	struct RegularMesh
	{
	public:
										RegularMesh();
		Texture2D*						diffuseTex;
		Texture2D*						normalTex;
		float 							roughness;
		float 							specularity;
		IndexBuffer*					indices;
		unsigned int 					startIndices;
		unsigned int 					countIndices;
		GLenum							primitiveType;
		VertexArray*					primitive;
		void							Draw() const
		{
			primitive->Draw(primitiveType,*indices,countIndices,startIndices);
		}
	};
	//--------------------------------------------------------------------------
	class ResourceManager
	{
	public:
										ResourceManager();
									   ~ResourceManager();
		Texture2D*						CreateTexture2D();
		VertexBuffer2F*					CreateVBO2F();
		VertexBuffer3F*					CreateVBO3F();
		VertexBuffer4F*					CreateVBO4F();
		IndexBuffer*					CreateIBO();
		VertexArray*					CreateVAO();
		void							Clear();

	private:
		ResourceManager(const ResourceManager&);
		ResourceManager& operator=(const ResourceManager&);

	private:
		MemoryPool<Texture2D>			tex2D;
		MemoryPool<VertexBuffer2F>		vbo2F;
		MemoryPool<VertexBuffer3F>		vbo3F;
		MemoryPool<VertexBuffer4F>		vbo4F;
		MemoryPool<IndexBuffer>			ibo;
		MemoryPool<VertexArray>			vao;
	};
	//--------------------------------------------------------------------------
	class SceneManager
	{
	public:
		std::vector<TerrainMesh> 		terrainMeshes;
		std::vector<RegularMesh> 		regularMeshes;
		std::vector<ShadowMesh> 		shadowMeshes;
		std::vector<glm::mat4>			transformations;
		std::vector<BBox>				oBounds;	// Objects
		std::vector<BBox>				tBounds;	// Terrains
		BBox							wBound;		// Global
	};

	//--------------------------------------------------------------------------
	// Others functions
	//--------------------------------------------------------------------------
	// Compute the bounding box of a VBO (need CPU/GPU synchronisation)
	BBox ObjectBound(					VertexBuffer3F& _vbo,
										IndexBuffer& _ibo,
										int _first,
										int _count);

	// Compute the bounding box of a scene (only CPU)
	// Need all objects' bbox have been set
	BBox WorldBound(					const SceneManager& _scene);
}

#endif
