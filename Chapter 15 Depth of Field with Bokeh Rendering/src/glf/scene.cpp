//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <glf/scene.hpp>

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
#define DEFAULT_POOL_SIZE				1024

namespace glf
{
	//--------------------------------------------------------------------------
	ResourceManager::ResourceManager():
	tex2D(DEFAULT_POOL_SIZE),
	vbo2F(DEFAULT_POOL_SIZE),
	vbo3F(DEFAULT_POOL_SIZE),
	vbo4F(DEFAULT_POOL_SIZE),
	ibo(DEFAULT_POOL_SIZE),
	vao(DEFAULT_POOL_SIZE)
	{

	}
	//--------------------------------------------------------------------------
	ResourceManager::~ResourceManager()
	{
		Clear();
	}
	//--------------------------------------------------------------------------
	Texture2D* ResourceManager::CreateTexture2D()
	{
		return tex2D.Allocate();
	}
	//--------------------------------------------------------------------------
	VertexBuffer2F* ResourceManager::CreateVBO2F()
	{
		return vbo2F.Allocate();
	}
	//--------------------------------------------------------------------------
	VertexBuffer3F* ResourceManager::CreateVBO3F()
	{
		return vbo3F.Allocate();
	}
	//--------------------------------------------------------------------------
	VertexBuffer4F* ResourceManager::CreateVBO4F()
	{
		return vbo4F.Allocate();
	}
	//--------------------------------------------------------------------------
	IndexBuffer* ResourceManager::CreateIBO()
	{
		return ibo.Allocate();
	}
	//--------------------------------------------------------------------------
	VertexArray* ResourceManager::CreateVAO()
	{
		return vao.Allocate();
	}
	//--------------------------------------------------------------------------
	void ResourceManager::Clear()
	{
		tex2D.DesallocateAll();
		vbo2F.DesallocateAll();
		vbo3F.DesallocateAll();
		vbo4F.DesallocateAll();
		ibo.DesallocateAll();
		vao.DesallocateAll();
	}
	//--------------------------------------------------------------------------
	RegularMesh::RegularMesh():
	diffuseTex(NULL),
	normalTex(NULL),
	roughness(1),
	specularity(0),
	indices(NULL),
	startIndices(0),
	countIndices(0),
	primitiveType(GL_TRIANGLES),
	primitive(NULL)
	{

	}
	//--------------------------------------------------------------------------
	ShadowMesh::ShadowMesh():
	indices(NULL),
	startIndices(0),
	countIndices(0),
	primitiveType(GL_TRIANGLES),
	primitive(NULL)
	{

	}
	//--------------------------------------------------------------------------
	BBox ObjectBound(	VertexBuffer3F& _vbo,
						IndexBuffer& _ibo,
						int _first,
						int _count)
	{
		glf::Info("first     : %d",_first);
		glf::Info("count     : %d",_count);
		glf::Info("ibo count : %d",_ibo.count);
		assert(_first+_count<=_ibo.count);

		BBox bbox;
		glm::vec3* pptr    = _vbo.Lock();
		unsigned int* iptr = _ibo.Lock();
		for(int i=_first;i<_first+_count;++i)
		{
			bbox.Add(pptr[iptr[i]]);
		}
		_ibo.Unlock();
		_vbo.Unlock();
		return bbox;
	}
	//--------------------------------------------------------------------------
	BBox WorldBound(const SceneManager& _scene)
	{
		BBox bbox;
		int nOBounds = int(_scene.oBounds.size());
		int nTBounds = int(_scene.tBounds.size());
		for(int i=0;i<nOBounds;++i)
			bbox.Add(_scene.oBounds[i]);
		for(int i=0;i<nTBounds;++i)
			bbox.Add(_scene.tBounds[i]);
		return bbox;
	}
}

