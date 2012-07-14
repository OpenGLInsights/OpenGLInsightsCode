//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/io/model.hpp>
#include <glf/io/image.hpp>
#include <glf/utils.hpp>
#include <glf/debug.hpp>
//------------------------------------------------------------------------------
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cfloat>
#include <climits>
#include <ctype.h>
#include <algorithm>
#include <limits>
#include <vector>
#include <map>
#include <fstream>
#include <cassert>

//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
#define MAX_ANISOSTROPY					16.f

// OBJ loader
namespace
{
	//--------------------------------------------------------------------------
	// Copyright (c) 2007 dhpoware. All Rights Reserved.
	//
	// Permission is hereby granted, free of charge, to any person obtaining a
	// copy of this software and associated documentation files (the "Software"),
	// to deal in the Software without restriction, including without limitation
	// the rights to use, copy, modify, merge, publish, distribute, sublicense,
	// and/or sell copies of the Software, and to permit persons to whom the
	// Software is furnished to do so, subject to the following conditions:
	//
	// The above copyright notice and this permission notice shall be included in
	// all copies or substantial portions of the Software.
	//
	// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
	// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
	// IN THE SOFTWARE.
	//--------------------------------------------------------------------------
	//
	// The methods normalize() and scale() are based on source code from
	// http://www.mvps.org/directx/articles/scalemesh9.htm.
	//
	// The addVertex() method is based on source code from the Direct3D MeshFromOBJ
	// sample found in the DirectX SDK.
	//
	// The generateTangents() method is based on public source code from
	// http://www.terathon.com/code/tangent.php.
	//
	// The importGeometryFirstPass(), importGeometrySecondPass(), and
	// importMaterials() methods are based on source code from Nate Robins' OpenGL
	// Tutors programs (http://www.xmission.com/~nate/tutors.html).
	//
	//--------------------------------------------------------------------------
	class ModelOBJ
	{
	public:
		struct Material
		{
			float ambient[4];
			float diffuse[4];
			float specular[4];
			float shininess;        // [0 = min shininess, 1 = max shininess]
			float alpha;            // [0 = fully transparent, 1 = fully opaque]

			std::string name;
			std::string colorMapFilename;
			std::string specularMapFilename;
			std::string bumpMapFilename;
		};

		struct Vertex
		{
			float position[3];
			float texCoord[2];
			float normal[3];
			float tangent[4];
			float bitangent[3];
		};

		struct Mesh
		{
			int startIndex;
			int triangleCount;
			const Material *pMaterial;
		};

		ModelOBJ();
		~ModelOBJ();

		void destroy();
		bool import(const char *pszFilename, bool rebuildNormals = false, bool rebuildTangents = false);
		void normalize(float scaleTo = 1.0f, bool center = true);
		void reverseWinding();

		void getCenter(float &x, float &y, float &z) const;
		float getWidth() const;
		float getHeight() const;
		float getLength() const;
		float getRadius() const;

		const int *getIndexBuffer() const;
		int getIndexSize() const;

		const Material &getMaterial(int i) const;
		const Mesh &getMesh(int i) const;

		int getNumberOfIndices() const;
		int getNumberOfMaterials() const;
		int getNumberOfMeshes() const;
		int getNumberOfTriangles() const;
		int getNumberOfVertices() const;

		const std::string &getPath() const;

		const Vertex &getVertex(int i) const;
		const Vertex *getVertexBuffer() const;
		int getVertexSize() const;

		bool hasNormals() const;
		bool hasPositions() const;
		bool hasTangents() const;
		bool hasTextureCoords() const;

	private:
		void addTrianglePos(int index, int material,
			int v0, int v1, int v2);
		void addTrianglePosNormal(int index, int material,
			int v0, int v1, int v2,
			int vn0, int vn1, int vn2);
		void addTrianglePosTexCoord(int index, int material,
			int v0, int v1, int v2,
			int vt0, int vt1, int vt2);
		void addTrianglePosTexCoordNormal(int index, int material,
			int v0, int v1, int v2,
			int vt0, int vt1, int vt2,
			int vn0, int vn1, int vn2);
		int addVertex(int hash, const Vertex *pVertex);
		void bounds(float center[3], float &width, float &height,
			float &length, float &radius) const;
		void buildMeshes();
		void generateNormals();
		void generateTangents();
		void importGeometryFirstPass(FILE *pFile);
		void importGeometrySecondPass(FILE *pFile);
		bool importMaterials(const char *pszFilename);
		void scale(float scaleFactor, float offset[3]);

		bool m_hasPositions;
		bool m_hasTextureCoords;
		bool m_hasNormals;
		bool m_hasTangents;

		int m_numberOfVertexCoords;
		int m_numberOfTextureCoords;
		int m_numberOfNormals;
		int m_numberOfTriangles;
		int m_numberOfMaterials;
		int m_numberOfMeshes;

		float m_center[3];
		float m_width;
		float m_height;
		float m_length;
		float m_radius;

		std::string m_directoryPath;

		std::vector<Mesh> m_meshes;
		std::vector<Material> m_materials;
		std::vector<Vertex> m_vertexBuffer;
		std::vector<int> m_indexBuffer;
		std::vector<int> m_attributeBuffer;
		std::vector<float> m_vertexCoords;
		std::vector<float> m_textureCoords;
		std::vector<float> m_normals;

		std::map<std::string, int> m_materialCache;
		std::map<int, std::vector<int> > m_vertexCache;
	};
	//--------------------------------------------------------------------------
	bool MeshCompFunc(const ModelOBJ::Mesh &lhs, const ModelOBJ::Mesh &rhs)
	{
		return lhs.pMaterial->alpha > rhs.pMaterial->alpha;
	}
	//--------------------------------------------------------------------------
	inline void ModelOBJ::getCenter(float &x, float &y, float &z) const
	{ x = m_center[0]; y = m_center[1]; z = m_center[2]; }
	//--------------------------------------------------------------------------
	inline float ModelOBJ::getWidth() const
	{ return m_width; }
	//--------------------------------------------------------------------------
	inline float ModelOBJ::getHeight() const
	{ return m_height; }
	//--------------------------------------------------------------------------
	inline float ModelOBJ::getLength() const
	{ return m_length; }
	//--------------------------------------------------------------------------
	inline float ModelOBJ::getRadius() const
	{ return m_radius; }
	//--------------------------------------------------------------------------
	inline const int *ModelOBJ::getIndexBuffer() const
	{ return &m_indexBuffer[0]; }
	//--------------------------------------------------------------------------
	inline int ModelOBJ::getIndexSize() const
	{ return static_cast<int>(sizeof(int)); }
	//--------------------------------------------------------------------------
	inline const ModelOBJ::Material &ModelOBJ::getMaterial(int i) const
	{ return m_materials[i]; }
	//--------------------------------------------------------------------------
	inline const ModelOBJ::Mesh &ModelOBJ::getMesh(int i) const
	{ return m_meshes[i]; }
	//--------------------------------------------------------------------------
	inline int ModelOBJ::getNumberOfIndices() const
	{ return m_numberOfTriangles * 3; }
	//--------------------------------------------------------------------------
	inline int ModelOBJ::getNumberOfMaterials() const
	{ return m_numberOfMaterials; }
	//--------------------------------------------------------------------------
	inline int ModelOBJ::getNumberOfMeshes() const
	{ return m_numberOfMeshes; }
	//--------------------------------------------------------------------------
	inline int ModelOBJ::getNumberOfTriangles() const
	{ return m_numberOfTriangles; }
	//--------------------------------------------------------------------------
	inline int ModelOBJ::getNumberOfVertices() const
	{ return static_cast<int>(m_vertexBuffer.size()); }
	//--------------------------------------------------------------------------
	inline const std::string &ModelOBJ::getPath() const
	{ return m_directoryPath; }
	//--------------------------------------------------------------------------
	inline const ModelOBJ::Vertex &ModelOBJ::getVertex(int i) const
	{ return m_vertexBuffer[i]; }
	//--------------------------------------------------------------------------
	inline const ModelOBJ::Vertex *ModelOBJ::getVertexBuffer() const
	{ return &m_vertexBuffer[0]; }
	//--------------------------------------------------------------------------
	inline int ModelOBJ::getVertexSize() const
	{ return static_cast<int>(sizeof(Vertex)); }
	//--------------------------------------------------------------------------
	inline bool ModelOBJ::hasNormals() const
	{ return m_hasNormals; }
	//--------------------------------------------------------------------------
	inline bool ModelOBJ::hasPositions() const
	{ return m_hasPositions; }
	//--------------------------------------------------------------------------
	inline bool ModelOBJ::hasTangents() const
	{ return m_hasTangents; }
	//--------------------------------------------------------------------------
	inline bool ModelOBJ::hasTextureCoords() const
	{ return m_hasTextureCoords; }
	//--------------------------------------------------------------------------
	ModelOBJ::ModelOBJ()
	{
		m_hasPositions = false;
		m_hasNormals = false;
		m_hasTextureCoords = false;
		m_hasTangents = false;

		m_numberOfVertexCoords = 0;
		m_numberOfTextureCoords = 0;
		m_numberOfNormals = 0;
		m_numberOfTriangles = 0;
		m_numberOfMaterials = 0;
		m_numberOfMeshes = 0;

		m_center[0] = m_center[1] = m_center[2] = 0.0f;
		m_width = m_height = m_length = m_radius = 0.0f;
	}
	//--------------------------------------------------------------------------
	ModelOBJ::~ModelOBJ()
	{
		destroy();
	}
	//--------------------------------------------------------------------------
	void ModelOBJ::bounds(float center[3], float &width, float &height,
		                  float &length, float &radius) const
	{
		float xMax = std::numeric_limits<float>::min();
		float yMax = std::numeric_limits<float>::min();
		float zMax = std::numeric_limits<float>::min();

		float xMin = std::numeric_limits<float>::max();
		float yMin = std::numeric_limits<float>::max();
		float zMin = std::numeric_limits<float>::max();

		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;

		int numVerts = static_cast<int>(m_vertexBuffer.size());

		for (int i = 0; i < numVerts; ++i)
		{
		    x = m_vertexBuffer[i].position[0];
		    y = m_vertexBuffer[i].position[1];
		    z = m_vertexBuffer[i].position[2];

		    if (x < xMin)
		        xMin = x;

		    if (x > xMax)
		        xMax = x;

		    if (y < yMin)
		        yMin = y;

		    if (y > yMax)
		        yMax = y;

		    if (z < zMin)
		        zMin = z;

		    if (z > zMax)
		        zMax = z;
		}

		center[0] = (xMin + xMax) / 2.0f;
		center[1] = (yMin + yMax) / 2.0f;
		center[2] = (zMin + zMax) / 2.0f;

		width = xMax - xMin;
		height = yMax - yMin;
		length = zMax - zMin;

		radius = std::max(std::max(width, height), length);
	}
	//--------------------------------------------------------------------------
	void ModelOBJ::destroy()
	{
		m_hasPositions = false;
		m_hasTextureCoords = false;
		m_hasNormals = false;
		m_hasTangents = false;

		m_numberOfVertexCoords = 0;
		m_numberOfTextureCoords = 0;
		m_numberOfNormals = 0;
		m_numberOfTriangles = 0;
		m_numberOfMaterials = 0;
		m_numberOfMeshes = 0;

		m_center[0] = m_center[1] = m_center[2] = 0.0f;
		m_width = m_height = m_length = m_radius = 0.0f;

		m_directoryPath.clear();

		m_meshes.clear();
		m_materials.clear();
		m_vertexBuffer.clear();
		m_indexBuffer.clear();
		m_attributeBuffer.clear();

		m_vertexCoords.clear();
		m_textureCoords.clear();
		m_normals.clear();

		m_materialCache.clear();
		m_vertexCache.clear();
	}
	//--------------------------------------------------------------------------
	bool ModelOBJ::import(const char *pszFilename, bool rebuildNormals, bool rebuildTangents)
	{
		FILE *pFile = fopen(pszFilename, "r");

		if (!pFile)
		    return false;

		// Extract the directory the OBJ file is in from the file name.
		// This directory path will be used to load the OBJ's associated MTL file.

		m_directoryPath.clear();

		std::string filename = pszFilename;
		std::string::size_type offset = filename.find_last_of('\\');

		if (offset != std::string::npos)
		{
		    m_directoryPath = filename.substr(0, ++offset);
		}
		else
		{
		    offset = filename.find_last_of('/');

		    if (offset != std::string::npos)
		        m_directoryPath = filename.substr(0, ++offset);
		}

		// Import the OBJ file.

		importGeometryFirstPass(pFile);
		rewind(pFile);
		importGeometrySecondPass(pFile);
		fclose(pFile);

		// Perform post import tasks.

		buildMeshes();
		bounds(m_center, m_width, m_height, m_length, m_radius);

		// Build vertex normals if required.

		if (rebuildNormals)
		{
		    generateNormals();
		}
		else
		{
		    if (!hasNormals())
		        generateNormals();
		}

		// Build tangents is required.
		if(rebuildTangents)
		{
			generateTangents();
		}
		else
		{
			for (int i = 0; i < m_numberOfMaterials; ++i)
			{
				if (!m_materials[i].bumpMapFilename.empty())
				{
					generateTangents();
					break;
				}
			}
		}

		return true;
	}
	//--------------------------------------------------------------------------
	void ModelOBJ::normalize(float scaleTo, bool center)
	{
		float width = 0.0f;
		float height = 0.0f;
		float length = 0.0f;
		float radius = 0.0f;
		float centerPos[3] = {0.0f};

		bounds(centerPos, width, height, length, radius);

		float scalingFactor = scaleTo / radius;
		float offset[3] = {0.0f};

		if (center)
		{
		    offset[0] = -centerPos[0];
		    offset[1] = -centerPos[1];
		    offset[2] = -centerPos[2];
		}
		else
		{
		    offset[0] = 0.0f;
		    offset[1] = 0.0f;
		    offset[2] = 0.0f;
		}

		scale(scalingFactor, offset);
		bounds(m_center, m_width, m_height, m_length, m_radius);
	}
	//--------------------------------------------------------------------------
	void ModelOBJ::reverseWinding()
	{
		int swap = 0;

		// Reverse face winding.
		for (int i = 0; i < static_cast<int>(m_indexBuffer.size()); i += 3)
		{
		    swap = m_indexBuffer[i + 1];
		    m_indexBuffer[i + 1] = m_indexBuffer[i + 2];
		    m_indexBuffer[i + 2] = swap;
		}

		float *pNormal = 0;
		float *pTangent = 0;

		// Invert normals and tangents.
		for (int i = 0; i < static_cast<int>(m_vertexBuffer.size()); ++i)
		{
		    pNormal = m_vertexBuffer[i].normal;
		    pNormal[0] = -pNormal[0];
		    pNormal[1] = -pNormal[1];
		    pNormal[2] = -pNormal[2];

		    pTangent = m_vertexBuffer[i].tangent;
		    pTangent[0] = -pTangent[0];
		    pTangent[1] = -pTangent[1];
		    pTangent[2] = -pTangent[2];
		}
	}
	//--------------------------------------------------------------------------
	void ModelOBJ::scale(float scaleFactor, float offset[3])
	{
		float *pPosition = 0;

		for (int i = 0; i < static_cast<int>(m_vertexBuffer.size()); ++i)
		{
		    pPosition = m_vertexBuffer[i].position;

		    pPosition[0] += offset[0];
		    pPosition[1] += offset[1];
		    pPosition[2] += offset[2];

		    pPosition[0] *= scaleFactor;
		    pPosition[1] *= scaleFactor;
		    pPosition[2] *= scaleFactor;
		}
	}
	//--------------------------------------------------------------------------
	void ModelOBJ::addTrianglePos(int index, int material, int v0, int v1, int v2)
	{
		Vertex vertex =
		{
		    {0.0f, 0.0f, 0.0f},
		    {0.0f, 0.0f},
		    {0.0f, 0.0f, 0.0f}
		};

		m_attributeBuffer[index] = material;

		vertex.position[0] = m_vertexCoords[v0 * 3];
		vertex.position[1] = m_vertexCoords[v0 * 3 + 1];
		vertex.position[2] = m_vertexCoords[v0 * 3 + 2];
		m_indexBuffer[index * 3] = addVertex(v0, &vertex);

		vertex.position[0] = m_vertexCoords[v1 * 3];
		vertex.position[1] = m_vertexCoords[v1 * 3 + 1];
		vertex.position[2] = m_vertexCoords[v1 * 3 + 2];
		m_indexBuffer[index * 3 + 1] = addVertex(v1, &vertex);

		vertex.position[0] = m_vertexCoords[v2 * 3];
		vertex.position[1] = m_vertexCoords[v2 * 3 + 1];
		vertex.position[2] = m_vertexCoords[v2 * 3 + 2];
		m_indexBuffer[index * 3 + 2] = addVertex(v2, &vertex);
	}
	//--------------------------------------------------------------------------
	void ModelOBJ::addTrianglePosNormal(int index, int material, int v0, int v1,
		                                int v2, int vn0, int vn1, int vn2)
	{
		Vertex vertex =
		{
		    {0.0f, 0.0f, 0.0f},
		    {0.0f, 0.0f},
		    {0.0f, 0.0f, 0.0f},
		    {0.0f, 0.0f, 0.0f}
		};

		m_attributeBuffer[index] = material;

		vertex.position[0] = m_vertexCoords[v0 * 3];
		vertex.position[1] = m_vertexCoords[v0 * 3 + 1];
		vertex.position[2] = m_vertexCoords[v0 * 3 + 2];
		vertex.normal[0] = m_normals[vn0 * 3];
		vertex.normal[1] = m_normals[vn0 * 3 + 1];
		vertex.normal[2] = m_normals[vn0 * 3 + 2];
		m_indexBuffer[index * 3] = addVertex(v0, &vertex);

		vertex.position[0] = m_vertexCoords[v1 * 3];
		vertex.position[1] = m_vertexCoords[v1 * 3 + 1];
		vertex.position[2] = m_vertexCoords[v1 * 3 + 2];
		vertex.normal[0] = m_normals[vn1 * 3];
		vertex.normal[1] = m_normals[vn1 * 3 + 1];
		vertex.normal[2] = m_normals[vn1 * 3 + 2];
		m_indexBuffer[index * 3 + 1] = addVertex(v1, &vertex);

		vertex.position[0] = m_vertexCoords[v2 * 3];
		vertex.position[1] = m_vertexCoords[v2 * 3 + 1];
		vertex.position[2] = m_vertexCoords[v2 * 3 + 2];
		vertex.normal[0] = m_normals[vn2 * 3];
		vertex.normal[1] = m_normals[vn2 * 3 + 1];
		vertex.normal[2] = m_normals[vn2 * 3 + 2];
		m_indexBuffer[index * 3 + 2] = addVertex(v2, &vertex);
	}
	//--------------------------------------------------------------------------
	void ModelOBJ::addTrianglePosTexCoord(int index, int material, int v0, int v1,
		                                  int v2, int vt0, int vt1, int vt2)
	{
		Vertex vertex =
		{
		    {0.0f, 0.0f, 0.0f},
		    {0.0f, 0.0f},
		    {0.0f, 0.0f, 0.0f},
		    {0.0f, 0.0f, 0.0f}
		};

		m_attributeBuffer[index] = material;

		vertex.position[0] = m_vertexCoords[v0 * 3];
		vertex.position[1] = m_vertexCoords[v0 * 3 + 1];
		vertex.position[2] = m_vertexCoords[v0 * 3 + 2];
		vertex.texCoord[0] = m_textureCoords[vt0 * 2];
		vertex.texCoord[1] = m_textureCoords[vt0 * 2 + 1];
		m_indexBuffer[index * 3] = addVertex(v0, &vertex);

		vertex.position[0] = m_vertexCoords[v1 * 3];
		vertex.position[1] = m_vertexCoords[v1 * 3 + 1];
		vertex.position[2] = m_vertexCoords[v1 * 3 + 2];
		vertex.texCoord[0] = m_textureCoords[vt1 * 2];
		vertex.texCoord[1] = m_textureCoords[vt1 * 2 + 1];
		m_indexBuffer[index * 3 + 1] = addVertex(v1, &vertex);

		vertex.position[0] = m_vertexCoords[v2 * 3];
		vertex.position[1] = m_vertexCoords[v2 * 3 + 1];
		vertex.position[2] = m_vertexCoords[v2 * 3 + 2];
		vertex.texCoord[0] = m_textureCoords[vt2 * 2];
		vertex.texCoord[1] = m_textureCoords[vt2 * 2 + 1];
		m_indexBuffer[index * 3 + 2] = addVertex(v2, &vertex);
	}
	//--------------------------------------------------------------------------
	void ModelOBJ::addTrianglePosTexCoordNormal(int index, int material, int v0,
		                                        int v1, int v2, int vt0, int vt1,
		                                        int vt2, int vn0, int vn1, int vn2)
	{
		Vertex vertex =
		{
		    {0.0f, 0.0f, 0.0f},
		    {0.0f, 0.0f},
		    {0.0f, 0.0f, 0.0f},
		    {0.0f, 0.0f, 0.0f}
		};

		m_attributeBuffer[index] = material;

		vertex.position[0] = m_vertexCoords[v0 * 3];
		vertex.position[1] = m_vertexCoords[v0 * 3 + 1];
		vertex.position[2] = m_vertexCoords[v0 * 3 + 2];
		vertex.texCoord[0] = m_textureCoords[vt0 * 2];
		vertex.texCoord[1] = m_textureCoords[vt0 * 2 + 1];
		vertex.normal[0] = m_normals[vn0 * 3];
		vertex.normal[1] = m_normals[vn0 * 3 + 1];
		vertex.normal[2] = m_normals[vn0 * 3 + 2];
		m_indexBuffer[index * 3] = addVertex(v0, &vertex);

		vertex.position[0] = m_vertexCoords[v1 * 3];
		vertex.position[1] = m_vertexCoords[v1 * 3 + 1];
		vertex.position[2] = m_vertexCoords[v1 * 3 + 2];
		vertex.texCoord[0] = m_textureCoords[vt1 * 2];
		vertex.texCoord[1] = m_textureCoords[vt1 * 2 + 1];
		vertex.normal[0] = m_normals[vn1 * 3];
		vertex.normal[1] = m_normals[vn1 * 3 + 1];
		vertex.normal[2] = m_normals[vn1 * 3 + 2];
		m_indexBuffer[index * 3 + 1] = addVertex(v1, &vertex);

		vertex.position[0] = m_vertexCoords[v2 * 3];
		vertex.position[1] = m_vertexCoords[v2 * 3 + 1];
		vertex.position[2] = m_vertexCoords[v2 * 3 + 2];
		vertex.texCoord[0] = m_textureCoords[vt2 * 2];
		vertex.texCoord[1] = m_textureCoords[vt2 * 2 + 1];
		vertex.normal[0] = m_normals[vn2 * 3];
		vertex.normal[1] = m_normals[vn2 * 3 + 1];
		vertex.normal[2] = m_normals[vn2 * 3 + 2];
		m_indexBuffer[index * 3 + 2] = addVertex(v2, &vertex);
	}
	//--------------------------------------------------------------------------
	int ModelOBJ::addVertex(int hash, const Vertex *pVertex)
	{
		int index = -1;
		std::map<int, std::vector<int> >::const_iterator iter = m_vertexCache.find(hash);

		if (iter == m_vertexCache.end())
		{
		    // Vertex hash doesn't exist in the cache.

		    index = static_cast<int>(m_vertexBuffer.size());
		    m_vertexBuffer.push_back(*pVertex);
		    m_vertexCache.insert(std::make_pair(hash, std::vector<int>(1, index)));
		}
		else
		{
		    // One or more vertices have been hashed to this entry in the cache.

		    const std::vector<int> &vertices = iter->second;
		    const Vertex *pCachedVertex = 0;
		    bool found = false;

		    for (std::vector<int>::const_iterator i = vertices.begin(); i != vertices.end(); ++i)
		    {
		        index = *i;
		        pCachedVertex = &m_vertexBuffer[index];

		        if (memcmp(pCachedVertex, pVertex, sizeof(Vertex)) == 0)
		        {
		            found = true;
		            break;
		        }
		    }

		    if (!found)
		    {
		        index = static_cast<int>(m_vertexBuffer.size());
		        m_vertexBuffer.push_back(*pVertex);
		        m_vertexCache[hash].push_back(index);
		    }
		}

		return index;
	}
	//--------------------------------------------------------------------------
	void ModelOBJ::buildMeshes()
	{
		// Group the model's triangles based on material type.

		Mesh *pMesh = 0;
		int materialId = -1;
		int numMeshes = 0;

		// Count the number of meshes.
		for (int i = 0; i < static_cast<int>(m_attributeBuffer.size()); ++i)
		{
		    if (m_attributeBuffer[i] != materialId)
		    {
		        materialId = m_attributeBuffer[i];
		        ++numMeshes;
		    }
		}

		// Allocate memory for the meshes and reset counters.
		m_numberOfMeshes = numMeshes;
		m_meshes.resize(m_numberOfMeshes);
		numMeshes = 0;
		materialId = -1;

		// Build the meshes. One mesh for each unique material.
		for (int i = 0; i < static_cast<int>(m_attributeBuffer.size()); ++i)
		{
		    if (m_attributeBuffer[i] != materialId)
		    {
		        materialId = m_attributeBuffer[i];
		        pMesh = &m_meshes[numMeshes++];            
		        pMesh->pMaterial = &m_materials[materialId];
		        pMesh->startIndex = i * 3;
		        ++pMesh->triangleCount;
		    }
		    else
		    {
		        ++pMesh->triangleCount;
		    }
		}

		// Sort the meshes based on its material alpha. Fully opaque meshes
		// towards the front and fully transparent towards the back.
		std::sort(m_meshes.begin(), m_meshes.end(), MeshCompFunc);
	}
	//--------------------------------------------------------------------------
	void ModelOBJ::generateNormals()
	{
		const int *pTriangle = 0;
		Vertex *pVertex0 = 0;
		Vertex *pVertex1 = 0;
		Vertex *pVertex2 = 0;
		float edge1[3] = {0.0f, 0.0f, 0.0f};
		float edge2[3] = {0.0f, 0.0f, 0.0f};
		float normal[3] = {0.0f, 0.0f, 0.0f};
		float length = 0.0f;
		int totalVertices = getNumberOfVertices();
		int totalTriangles = getNumberOfTriangles();

		// Initialize all the vertex normals.
		for (int i = 0; i < totalVertices; ++i)
		{
		    pVertex0 = &m_vertexBuffer[i];
		    pVertex0->normal[0] = 0.0f;
		    pVertex0->normal[1] = 0.0f;
		    pVertex0->normal[2] = 0.0f;
		}

		// Calculate the vertex normals.
		for (int i = 0; i < totalTriangles; ++i)
		{
		    pTriangle = &m_indexBuffer[i * 3];

		    pVertex0 = &m_vertexBuffer[pTriangle[0]];
		    pVertex1 = &m_vertexBuffer[pTriangle[1]];
		    pVertex2 = &m_vertexBuffer[pTriangle[2]];

		    // Calculate triangle face normal.

		    edge1[0] = pVertex1->position[0] - pVertex0->position[0];
		    edge1[1] = pVertex1->position[1] - pVertex0->position[1];
		    edge1[2] = pVertex1->position[2] - pVertex0->position[2];

		    edge2[0] = pVertex2->position[0] - pVertex0->position[0];
		    edge2[1] = pVertex2->position[1] - pVertex0->position[1];
		    edge2[2] = pVertex2->position[2] - pVertex0->position[2];

		    normal[0] = (edge1[1] * edge2[2]) - (edge1[2] * edge2[1]);
		    normal[1] = (edge1[2] * edge2[0]) - (edge1[0] * edge2[2]);
		    normal[2] = (edge1[0] * edge2[1]) - (edge1[1] * edge2[0]);

		    // Accumulate the normals.

		    pVertex0->normal[0] += normal[0];
		    pVertex0->normal[1] += normal[1];
		    pVertex0->normal[2] += normal[2];

		    pVertex1->normal[0] += normal[0];
		    pVertex1->normal[1] += normal[1];
		    pVertex1->normal[2] += normal[2];

		    pVertex2->normal[0] += normal[0];
		    pVertex2->normal[1] += normal[1];
		    pVertex2->normal[2] += normal[2];

			#if ENABLE_CHECK_MODEL_LOADING
			if( isnan(normal[0]) || isnan(normal[1]) || isnan(normal[2]) )
				glf::Error("Normal NaN");
			if( isinf(normal[0]) || isinf(normal[1]) || isinf(normal[2]) )
				glf::Error("Normal Inf");
			#endif
		}

		// Normalize the vertex normals.
		for (int i = 0; i < totalVertices; ++i)
		{
		    pVertex0 = &m_vertexBuffer[i];

			float nLength = sqrtf(	pVertex0->normal[0] * pVertex0->normal[0] +
									pVertex0->normal[1] * pVertex0->normal[1] +
									pVertex0->normal[2] * pVertex0->normal[2]);

			// Set a default normal to avoir issue
			if(nLength<1e-6f)
			{
				pVertex0->normal[0] = 0;
				pVertex0->normal[1] = 0;
				pVertex0->normal[2] = 1;
				nLength 			= 1;
			}

			length = 1.0f / nLength;

			#if ENABLE_CHECK_MODEL_LOADING
			if( isnan(length) )
				glf::Error("Normal Length NaN");
			if( isinf(length) )
				glf::Error("Normal Length Inf");
			#endif

		    pVertex0->normal[0] *= length;
		    pVertex0->normal[1] *= length;
		    pVertex0->normal[2] *= length;

		}

		m_hasNormals = true;
	}
	//--------------------------------------------------------------------------
	void ModelOBJ::generateTangents()
	{
		const int *pTriangle = 0;
		Vertex *pVertex0 = 0;
		Vertex *pVertex1 = 0;
		Vertex *pVertex2 = 0;
		float edge1[3] = {0.0f, 0.0f, 0.0f};
		float edge2[3] = {0.0f, 0.0f, 0.0f};
		float texEdge1[2] = {0.0f, 0.0f};
		float texEdge2[2] = {0.0f, 0.0f};
		float tangent[3] = {0.0f, 0.0f, 0.0f};
		float bitangent[3] = {0.0f, 0.0f, 0.0f};
		float det = 0.0f;
		float nDotT = 0.0f;
		float bDotB = 0.0f;
		float length = 0.0f;
		int totalVertices = getNumberOfVertices();
		int totalTriangles = getNumberOfTriangles();

		// Initialize all the vertex tangents and bitangents.
		for (int i = 0; i < totalVertices; ++i)
		{
		    pVertex0 = &m_vertexBuffer[i];

		    pVertex0->tangent[0] = 0.0f;
		    pVertex0->tangent[1] = 0.0f;
		    pVertex0->tangent[2] = 0.0f;
		    pVertex0->tangent[3] = 0.0f;

		    pVertex0->bitangent[0] = 0.0f;
		    pVertex0->bitangent[1] = 0.0f;
		    pVertex0->bitangent[2] = 0.0f;
		}

		// Calculate the vertex tangents and bitangents.
		for (int i = 0; i < totalTriangles; ++i)
		{
		    pTriangle = &m_indexBuffer[i * 3];

		    pVertex0 = &m_vertexBuffer[pTriangle[0]];
		    pVertex1 = &m_vertexBuffer[pTriangle[1]];
		    pVertex2 = &m_vertexBuffer[pTriangle[2]];

		    // Calculate the triangle face tangent and bitangent.

		    edge1[0] = pVertex1->position[0] - pVertex0->position[0];
		    edge1[1] = pVertex1->position[1] - pVertex0->position[1];
		    edge1[2] = pVertex1->position[2] - pVertex0->position[2];

		    edge2[0] = pVertex2->position[0] - pVertex0->position[0];
		    edge2[1] = pVertex2->position[1] - pVertex0->position[1];
		    edge2[2] = pVertex2->position[2] - pVertex0->position[2];

		    texEdge1[0] = pVertex1->texCoord[0] - pVertex0->texCoord[0];
		    texEdge1[1] = pVertex1->texCoord[1] - pVertex0->texCoord[1];

		    texEdge2[0] = pVertex2->texCoord[0] - pVertex0->texCoord[0];
		    texEdge2[1] = pVertex2->texCoord[1] - pVertex0->texCoord[1];

		    det = texEdge1[0] * texEdge2[1] - texEdge2[0] * texEdge1[1];

			// Set default tangent and bitangent to avoid issue is det near to zero
		    if (fabs(det) < 1e-6f)
		    {
		        tangent[0] = 1.0f;
		        tangent[1] = 0.0f;
		        tangent[2] = 0.0f;

		        bitangent[0] = 0.0f;
		        bitangent[1] = 1.0f;
		        bitangent[2] = 0.0f;

				if(	fabs(tangent[0] * pVertex0->normal[0] + 
						 tangent[1] * pVertex0->normal[1] +
						 tangent[2] * pVertex0->normal[2]) > 0.8f )
				{
					tangent[0] = 0.0f;
					tangent[1] = 1.0f;
					tangent[2] = 0.0f;

					bitangent[0] = 0.0f;
					bitangent[1] = 0.0f;
					bitangent[2] = 1.0f;
				}
		    }
		    else
		    {
		        det = 1.0f / det;

		        tangent[0] = (texEdge2[1] * edge1[0] - texEdge1[1] * edge2[0]) * det;
		        tangent[1] = (texEdge2[1] * edge1[1] - texEdge1[1] * edge2[1]) * det;
		        tangent[2] = (texEdge2[1] * edge1[2] - texEdge1[1] * edge2[2]) * det;

		        bitangent[0] = (-texEdge2[0] * edge1[0] + texEdge1[0] * edge2[0]) * det;
		        bitangent[1] = (-texEdge2[0] * edge1[1] + texEdge1[0] * edge2[1]) * det;
		        bitangent[2] = (-texEdge2[0] * edge1[2] + texEdge1[0] * edge2[2]) * det;
		    }

			// Check normal tangent and bitangent
			#if ENABLE_CHECK_MODEL_LOADING
			if( isnan(tangent[0]) || isnan(tangent[1]) || isnan(tangent[2]) )
				glf::Error("Tangent NaN");
			if( isinf(tangent[0]) || isinf(tangent[1]) || isinf(tangent[2]) )
				glf::Error("Tangent Inf");
			if( isnan(bitangent[0]) || isnan(bitangent[1]) || isnan(bitangent[2]) )
				glf::Error("Bitangent NaN");
			if( isinf(bitangent[0]) || isinf(bitangent[1]) || isinf(bitangent[2]) )
				glf::Error("Bitangent Inf");
			#endif

		    // Accumulate the tangents and bitangents.
		    pVertex0->tangent[0] += tangent[0];
		    pVertex0->tangent[1] += tangent[1];
		    pVertex0->tangent[2] += tangent[2];
		    pVertex0->bitangent[0] += bitangent[0];
		    pVertex0->bitangent[1] += bitangent[1];
		    pVertex0->bitangent[2] += bitangent[2];

		    pVertex1->tangent[0] += tangent[0];
		    pVertex1->tangent[1] += tangent[1];
		    pVertex1->tangent[2] += tangent[2];
		    pVertex1->bitangent[0] += bitangent[0];
		    pVertex1->bitangent[1] += bitangent[1];
		    pVertex1->bitangent[2] += bitangent[2];

		    pVertex2->tangent[0] += tangent[0];
		    pVertex2->tangent[1] += tangent[1];
		    pVertex2->tangent[2] += tangent[2];
		    pVertex2->bitangent[0] += bitangent[0];
		    pVertex2->bitangent[1] += bitangent[1];
		    pVertex2->bitangent[2] += bitangent[2];
		}

		// Orthogonalize and normalize the vertex tangents.
		for (int i = 0; i < totalVertices; ++i)
		{
		    pVertex0 = &m_vertexBuffer[i];

		    // Gram-Schmidt orthogonalize tangent with normal.

		    nDotT = pVertex0->normal[0] * pVertex0->tangent[0] +
		            pVertex0->normal[1] * pVertex0->tangent[1] +
		            pVertex0->normal[2] * pVertex0->tangent[2];

		    pVertex0->tangent[0] -= pVertex0->normal[0] * nDotT;
		    pVertex0->tangent[1] -= pVertex0->normal[1] * nDotT;
		    pVertex0->tangent[2] -= pVertex0->normal[2] * nDotT;

			// Check tangent
			float lenTangent = sqrtf(	pVertex0->tangent[0] * pVertex0->tangent[0] +
										pVertex0->tangent[1] * pVertex0->tangent[1] +
										pVertex0->tangent[2] * pVertex0->tangent[2]);
			float lenBitangent = sqrtf(	pVertex0->bitangent[0] * pVertex0->bitangent[0] +
										pVertex0->bitangent[1] * pVertex0->bitangent[1] +
										pVertex0->bitangent[2] * pVertex0->bitangent[2]);

			// Try to correct tangent if there is an issue
			if(lenTangent < 1e-6f && lenBitangent < 1e-6f)
			{
				pVertex0->tangent[0] = 1.0f;
				pVertex0->tangent[1] = 0.0f;
				pVertex0->tangent[2] = 0.0f;

				if(	fabs(pVertex0->tangent[0] * pVertex0->normal[0] + 
						 pVertex0->tangent[1] * pVertex0->normal[1] +
						 pVertex0->tangent[2] * pVertex0->normal[2]) > 0.8f )
				{
					pVertex0->tangent[0] = 0.0f;
					pVertex0->tangent[1] = 1.0f;
					pVertex0->tangent[2] = 0.0f;
				}
			}
			else if (lenTangent < 1e-6f)
			{
				if(lenBitangent)
				{
					pVertex0->tangent[0] = 	(pVertex0->bitangent[1] * pVertex0->normal[2]) - 
											(pVertex0->bitangent[2] * pVertex0->normal[1]);
					pVertex0->tangent[1] = 	(pVertex0->bitangent[2] * pVertex0->normal[0]) -
											(pVertex0->bitangent[0] * pVertex0->normal[2]);
					pVertex0->tangent[2] = 	(pVertex0->bitangent[0] * pVertex0->normal[1]) - 
											(pVertex0->bitangent[1] * pVertex0->normal[0]);
				}
			}

			// Normalize the tangent.
			length = 1.0f / sqrtf(	pVertex0->tangent[0] * pVertex0->tangent[0] +
									pVertex0->tangent[1] * pVertex0->tangent[1] +
									pVertex0->tangent[2] * pVertex0->tangent[2]);

		    pVertex0->tangent[0] *= length;
		    pVertex0->tangent[1] *= length;
		    pVertex0->tangent[2] *= length;

		    // Calculate the handedness of the local tangent space.
		    // The bitangent vector is the cross product between the triangle face
		    // normal vector and the calculated tangent vector. The resulting
		    // bitangent vector should be the same as the bitangent vector
		    // calculated from the set of linear equations above. If they point in
		    // different directions then we need to invert the cross product
		    // calculated bitangent vector. We store this scalar multiplier in the
		    // tangent vector's 'w' component so that the correct bitangent vector
		    // can be generated in the normal mapping shader's vertex shader.
		    //
		    // Normal maps have a left handed coordinate system with the origin
		    // located at the top left of the normal map texture. The x coordinates
		    // run horizontally from left to right. The y coordinates run
		    // vertically from top to bottom. The z coordinates run out of the
		    // normal map texture towards the viewer. Our handedness calculations
		    // must take this fact into account as well so that the normal mapping
		    // shader's vertex shader will generate the correct bitangent vectors.
		    // Some normal map authoring tools such as Crazybump
		    // (http://www.crazybump.com/) includes options to allow you to control
		    // the orientation of the normal map normal's y-axis.

		    bitangent[0] = (pVertex0->normal[1] * pVertex0->tangent[2]) - 
		                   (pVertex0->normal[2] * pVertex0->tangent[1]);
		    bitangent[1] = (pVertex0->normal[2] * pVertex0->tangent[0]) -
		                   (pVertex0->normal[0] * pVertex0->tangent[2]);
		    bitangent[2] = (pVertex0->normal[0] * pVertex0->tangent[1]) - 
		                   (pVertex0->normal[1] * pVertex0->tangent[0]);

		    bDotB = bitangent[0] * pVertex0->bitangent[0] + 
		            bitangent[1] * pVertex0->bitangent[1] + 
		            bitangent[2] * pVertex0->bitangent[2];

		    pVertex0->tangent[3] = (bDotB < 0.0f) ? 1.0f : -1.0f;

		    pVertex0->bitangent[0] = bitangent[0];
		    pVertex0->bitangent[1] = bitangent[1];
		    pVertex0->bitangent[2] = bitangent[2];

			// Check normal, tangent, and bitangent
			#if ENABLE_CHECK_MODEL_LOADING
			if( isnan(pVertex0->normal[0]) || isnan(pVertex0->normal[1]) || isnan(pVertex0->normal[2]) )
				glf::Error("Normal NaN");
			if( isinf(pVertex0->normal[0]) || isinf(pVertex0->normal[1]) || isinf(pVertex0->normal[2]) )
				glf::Error("Normal Inf");
			if( isnan(pVertex0->tangent[0]) || isnan(pVertex0->tangent[1]) || isnan(pVertex0->tangent[2]) )
				glf::Error("Tangent NaN");
			if( isinf(pVertex0->tangent[0]) || isinf(pVertex0->tangent[1]) || isinf(pVertex0->tangent[2]) )
				glf::Error("Tangent Inf");
			if( isnan(pVertex0->bitangent[0]) || isnan(pVertex0->bitangent[1]) || isnan(pVertex0->bitangent[2]) )
				glf::Error("Bitangent NaN");
			if( isinf(pVertex0->bitangent[0]) || isinf(pVertex0->bitangent[1]) || isinf(pVertex0->bitangent[2]) )
				glf::Error("Bitangent Inf");
			#endif
		}

		m_hasTangents = true;
	}
	//--------------------------------------------------------------------------
	void ModelOBJ::importGeometryFirstPass(FILE *pFile)
	{
		m_hasTextureCoords = false;
		m_hasNormals = false;

		m_numberOfVertexCoords = 0;
		m_numberOfTextureCoords = 0;
		m_numberOfNormals = 0;
		m_numberOfTriangles = 0;

		int v = 0;
		int vt = 0;
		int vn = 0;
		char buffer[256] = {0};
		std::string name;

		while (fscanf(pFile, "%256s", buffer) != EOF)
		{
		    switch (buffer[0])
		    {
		    case 'f':   // v, v//vn, v/vt, v/vt/vn.
		        fscanf(pFile, "%256s", buffer);

		        if (strstr(buffer, "//")) // v//vn
		        {
		            sscanf(buffer, "%d//%d", &v, &vn);
		            fscanf(pFile, "%256d//%256d", &v, &vn);
		            fscanf(pFile, "%256d//%256d", &v, &vn);
		            ++m_numberOfTriangles;

		            while (fscanf(pFile, "%256d//%256d", &v, &vn) > 0)
		                ++m_numberOfTriangles;
		        }
		        else if (sscanf(buffer, "%d/%d/%d", &v, &vt, &vn) == 3) // v/vt/vn
		        {
		            fscanf(pFile, "%256d/%256d/%256d", &v, &vt, &vn);
		            fscanf(pFile, "%256d/%256d/%256d", &v, &vt, &vn);
		            ++m_numberOfTriangles;

		            while (fscanf(pFile, "%256d/%256d/%256d", &v, &vt, &vn) > 0)
		                ++m_numberOfTriangles;
		        }
		        else if (sscanf(buffer, "%d/%d", &v, &vt) == 2) // v/vt
		        {
		            fscanf(pFile, "%256d/%256d", &v, &vt);
		            fscanf(pFile, "%256d/%256d", &v, &vt);
		            ++m_numberOfTriangles;

		            while (fscanf(pFile, "%256d/%256d", &v, &vt) > 0)
		                ++m_numberOfTriangles;
		        }
		        else // v
		        {
		            fscanf(pFile, "%256d", &v);
		            fscanf(pFile, "%256d", &v);
		            ++m_numberOfTriangles;

		            while (fscanf(pFile, "%256d", &v) > 0)
		                ++m_numberOfTriangles;
		        }
		        break;

		    case 'm':   // mtllib
		        fgets(buffer, sizeof(buffer), pFile);
		        sscanf(buffer, "%s %s", buffer, buffer);
		        name = m_directoryPath;
		        name += buffer;
		        importMaterials(name.c_str());
		        break;

		    case 'v':   // v, vt, or vn
		        switch (buffer[1])
		        {
		        case '\0':
		            fgets(buffer, sizeof(buffer), pFile);
		            ++m_numberOfVertexCoords;
		            break;

		        case 'n':
		            fgets(buffer, sizeof(buffer), pFile);
		            ++m_numberOfNormals;
		            break;

		        case 't':
		            fgets(buffer, sizeof(buffer), pFile);
		            ++m_numberOfTextureCoords;

		        default:
		            break;
		        }
		        break;

		    default:
		        fgets(buffer, sizeof(buffer), pFile);
		        break;
		    }
		}

		m_hasPositions = m_numberOfVertexCoords > 0;
		m_hasNormals = m_numberOfNormals > 0;
		m_hasTextureCoords = m_numberOfTextureCoords > 0;

		// Allocate memory for the OBJ model data.
		m_vertexCoords.resize(m_numberOfVertexCoords * 3);
		m_textureCoords.resize(m_numberOfTextureCoords * 2);
		m_normals.resize(m_numberOfNormals * 3);
		m_indexBuffer.resize(m_numberOfTriangles * 3);
		m_attributeBuffer.resize(m_numberOfTriangles);

		// Define a default material if no materials were loaded.
		if (m_numberOfMaterials == 0)
		{
		    Material defaultMaterial =
		    {
		        {0.2f, 0.2f, 0.2f, 1.0f},
		        {0.8f, 0.8f, 0.8f, 1.0f},
		        {0.0f, 0.0f, 0.0f, 1.0f},
		        0.0f,
		        1.0f,
		        std::string("default"),
		        std::string(),
		        std::string()
		    };

		    m_materials.push_back(defaultMaterial);
		    m_materialCache[defaultMaterial.name] = 0;
		}
	}
	//--------------------------------------------------------------------------
	void ModelOBJ::importGeometrySecondPass(FILE *pFile)
	{
		int v[3] = {0};
		int vt[3] = {0};
		int vn[3] = {0};
		int numVertices = 0;
		int numTexCoords = 0;
		int numNormals = 0;
		int numTriangles = 0;
		int activeMaterial = 0;
		char buffer[256] = {0};
		std::map<std::string, int>::const_iterator iter;

		while (fscanf(pFile, "%256s", buffer) != EOF)
		{
		    switch (buffer[0])
		    {
		    case 'f': // v, v//vn, v/vt, or v/vt/vn.
		        v[0]  = v[1]  = v[2]  = 0;
		        vt[0] = vt[1] = vt[2] = 0;
		        vn[0] = vn[1] = vn[2] = 0;

		        fscanf(pFile, "%256s", buffer);

		        if (strstr(buffer, "//")) // v//vn
		        {
		            sscanf(buffer, "%d//%d", &v[0], &vn[0]);
		            fscanf(pFile, "%256d//%256d", &v[1], &vn[1]);
		            fscanf(pFile, "%256d//%256d", &v[2], &vn[2]);

		            v[0] = (v[0] < 0) ? v[0] + numVertices - 1 : v[0] - 1;
		            v[1] = (v[1] < 0) ? v[1] + numVertices - 1 : v[1] - 1;
		            v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;

		            vn[0] = (vn[0] < 0) ? vn[0] + numNormals - 1 : vn[0] - 1;
		            vn[1] = (vn[1] < 0) ? vn[1] + numNormals - 1 : vn[1] - 1;
		            vn[2] = (vn[2] < 0) ? vn[2] + numNormals - 1 : vn[2] - 1;

		            addTrianglePosNormal(numTriangles++, activeMaterial,
		                v[0], v[1], v[2], vn[0], vn[1], vn[2]);

		            v[1] = v[2];
		            vn[1] = vn[2];

		            while (fscanf(pFile, "%256d//%256d", &v[2], &vn[2]) > 0)
		            {
		                v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;
		                vn[2] = (vn[2] < 0) ? vn[2] + numNormals - 1 : vn[2] - 1;

		                addTrianglePosNormal(numTriangles++, activeMaterial,
		                    v[0], v[1], v[2], vn[0], vn[1], vn[2]);

		                v[1] = v[2];
		                vn[1] = vn[2];
		            }
		        }
		        else if (sscanf(buffer, "%d/%d/%d", &v[0], &vt[0], &vn[0]) == 3) // v/vt/vn
		        {
		            fscanf(pFile, "%256d/%256d/%256d", &v[1], &vt[1], &vn[1]);
		            fscanf(pFile, "%256d/%256d/%256d", &v[2], &vt[2], &vn[2]);

		            v[0] = (v[0] < 0) ? v[0] + numVertices - 1 : v[0] - 1;
		            v[1] = (v[1] < 0) ? v[1] + numVertices - 1 : v[1] - 1;
		            v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;

		            vt[0] = (vt[0] < 0) ? vt[0] + numTexCoords - 1 : vt[0] - 1;
		            vt[1] = (vt[1] < 0) ? vt[1] + numTexCoords - 1 : vt[1] - 1;
		            vt[2] = (vt[2] < 0) ? vt[2] + numTexCoords - 1 : vt[2] - 1;

		            vn[0] = (vn[0] < 0) ? vn[0] + numNormals - 1 : vn[0] - 1;
		            vn[1] = (vn[1] < 0) ? vn[1] + numNormals - 1 : vn[1] - 1;
		            vn[2] = (vn[2] < 0) ? vn[2] + numNormals - 1 : vn[2] - 1;

		            addTrianglePosTexCoordNormal(numTriangles++, activeMaterial,
		                v[0], v[1], v[2], vt[0], vt[1], vt[2], vn[0], vn[1], vn[2]);

		            v[1] = v[2];
		            vt[1] = vt[2];
		            vn[1] = vn[2];

		            while (fscanf(pFile, "%256d/%256d/%256d", &v[2], &vt[2], &vn[2]) > 0)
		            {
		                v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;
		                vt[2] = (vt[2] < 0) ? vt[2] + numTexCoords - 1 : vt[2] - 1;
		                vn[2] = (vn[2] < 0) ? vn[2] + numNormals - 1 : vn[2] - 1;

		                addTrianglePosTexCoordNormal(numTriangles++, activeMaterial,
		                    v[0], v[1], v[2], vt[0], vt[1], vt[2], vn[0], vn[1], vn[2]);

		                v[1] = v[2];
		                vt[1] = vt[2];
		                vn[1] = vn[2];
		            }
		        }
		        else if (sscanf(buffer, "%d/%d", &v[0], &vt[0]) == 2) // v/vt
		        {
		            fscanf(pFile, "%256d/%256d", &v[1], &vt[1]);
		            fscanf(pFile, "%256d/%256d", &v[2], &vt[2]);

		            v[0] = (v[0] < 0) ? v[0] + numVertices - 1 : v[0] - 1;
		            v[1] = (v[1] < 0) ? v[1] + numVertices - 1 : v[1] - 1;
		            v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;

		            vt[0] = (vt[0] < 0) ? vt[0] + numTexCoords - 1 : vt[0] - 1;
		            vt[1] = (vt[1] < 0) ? vt[1] + numTexCoords - 1 : vt[1] - 1;
		            vt[2] = (vt[2] < 0) ? vt[2] + numTexCoords - 1 : vt[2] - 1;

		            addTrianglePosTexCoord(numTriangles++, activeMaterial,
		                v[0], v[1], v[2], vt[0], vt[1], vt[2]);

		            v[1] = v[2];
		            vt[1] = vt[2];

		            while (fscanf(pFile, "%256d/%256d", &v[2], &vt[2]) > 0)
		            {
		                v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;
		                vt[2] = (vt[2] < 0) ? vt[2] + numTexCoords - 1 : vt[2] - 1;

		                addTrianglePosTexCoord(numTriangles++, activeMaterial,
		                    v[0], v[1], v[2], vt[0], vt[1], vt[2]);

		                v[1] = v[2];
		                vt[1] = vt[2];
		            }
		        }
		        else // v
		        {
		            sscanf(buffer, "%d", &v[0]);
		            fscanf(pFile, "%256d", &v[1]);
		            fscanf(pFile, "%256d", &v[2]);

		            v[0] = (v[0] < 0) ? v[0] + numVertices - 1 : v[0] - 1;
		            v[1] = (v[1] < 0) ? v[1] + numVertices - 1 : v[1] - 1;
		            v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;

		            addTrianglePos(numTriangles++, activeMaterial, v[0], v[1], v[2]);

		            v[1] = v[2];

		            while (fscanf(pFile, "%256d", &v[2]) > 0)
		            {
		                v[2] = (v[2] < 0) ? v[2] + numVertices - 1 : v[2] - 1;

		                addTrianglePos(numTriangles++, activeMaterial, v[0], v[1], v[2]);

		                v[1] = v[2];
		            }
		        }
		        break;

		    case 'u': // usemtl
		        fgets(buffer, sizeof(buffer), pFile);
		        sscanf(buffer, "%s %s", buffer, buffer);
		        iter = m_materialCache.find(buffer);
		        activeMaterial = (iter == m_materialCache.end()) ? 0 : iter->second;
		        break;

		    case 'v': // v, vn, or vt.
		        switch (buffer[1])
		        {
		        case '\0': // v
		            fscanf(pFile, "%256f %256f %256f",
		                &m_vertexCoords[3 * numVertices],
		                &m_vertexCoords[3 * numVertices + 1],
		                &m_vertexCoords[3 * numVertices + 2]);
		            ++numVertices;
		            break;

		        case 'n': // vn
		            fscanf(pFile, "%256f %256f %256f",
		                &m_normals[3 * numNormals],
		                &m_normals[3 * numNormals + 1],
		                &m_normals[3 * numNormals + 2]);
		            ++numNormals;
		            break;

		        case 't': // vt
		            fscanf(pFile, "%256f %256f",
		                &m_textureCoords[2 * numTexCoords],
		                &m_textureCoords[2 * numTexCoords + 1]);
		            ++numTexCoords;
		            break;

		        default:
		            break;
		        }
		        break;

		    default:
		        fgets(buffer, sizeof(buffer), pFile);
		        break;
		    }
		}
	}
	//--------------------------------------------------------------------------
	bool ModelOBJ::importMaterials(const char *pszFilename)
	{
		FILE *pFile = fopen(pszFilename, "r");

		if (!pFile)
		    return false;

		Material *pMaterial = 0;
		int illum = 0;
		int numMaterials = 0;
		char buffer[256] = {0};

		// Count the number of materials in the MTL file.
		while (fscanf(pFile, "%256s", buffer) != EOF)
		{
		    switch (buffer[0])
		    {
		    case 'n': // newmtl
		        ++numMaterials;
		        fgets(buffer, sizeof(buffer), pFile);
		        sscanf(buffer, "%s %s", buffer, buffer);
		        break;

		    default:
		        fgets(buffer, sizeof(buffer), pFile);
		        break;
		    }
		}

		rewind(pFile);

		m_numberOfMaterials = numMaterials;
		numMaterials = 0;
		m_materials.resize(m_numberOfMaterials);

		// Load the materials in the MTL file.
		while (fscanf(pFile, "%256s", buffer) != EOF)
		{
		    switch (buffer[0])
		    {
		    case 'N': // Ns
		        fscanf(pFile, "%256f", &pMaterial->shininess);
		        // Wavefront .MTL file shininess is from [0,1000].
		        break;

		    case 'K': // Ka, Kd, or Ks
		        switch (buffer[1])
		        {
		        case 'a': // Ka
		            fscanf(pFile, "%256f %256f %256f",
		                &pMaterial->ambient[0],
		                &pMaterial->ambient[1],
		                &pMaterial->ambient[2]);
		            pMaterial->ambient[3] = 1.0f;
		            break;

		        case 'd': // Kd
		            fscanf(pFile, "%256f %256f %256f",
		                &pMaterial->diffuse[0],
		                &pMaterial->diffuse[1],
		                &pMaterial->diffuse[2]);
		            pMaterial->diffuse[3] = 1.0f;
		            break;

		        case 's': // Ks
		            fscanf(pFile, "%256f %256f %256f",
		                &pMaterial->specular[0],
		                &pMaterial->specular[1],
		                &pMaterial->specular[2]);
		            pMaterial->specular[3] = 1.0f;
		            break;

		        default:
		            fgets(buffer, sizeof(buffer), pFile);
		            break;
		        }
		        break;

		    case 'T': // Tr
		        switch (buffer[1])
		        {
		        case 'r': // Tr
		            fscanf(pFile, "%256f", &pMaterial->alpha);
		            pMaterial->alpha = 1.0f - pMaterial->alpha;
		            break;

		        default:
		            fgets(buffer, sizeof(buffer), pFile);
		            break;
		        }
		        break;

		    case 'd':
		        fscanf(pFile, "%256f", &pMaterial->alpha);
		        break;

		    case 'i': // illum
		        fscanf(pFile, "%256d", &illum);

		        if (illum == 1)
		        {
		            pMaterial->specular[0] = 0.0f;
		            pMaterial->specular[1] = 0.0f;
		            pMaterial->specular[2] = 0.0f;
		            pMaterial->specular[3] = 1.0f;
		        }
		        break;

		    case 'm': // map_Kd, map_Ks, map_bump
		        if (strstr(buffer, "map_Kd") != 0 || strstr(buffer, "map_kD") != 0)
		        {
		            fgets(buffer, sizeof(buffer), pFile);
		            sscanf(buffer, "%s %s", buffer, buffer);
		            pMaterial->colorMapFilename = buffer;
		        }
		        else if (strstr(buffer, "map_Ks") != 0 || strstr(buffer, "map_kS") != 0 )
		        {
		            fgets(buffer, sizeof(buffer), pFile);
		            sscanf(buffer, "%s %s", buffer, buffer);
		            pMaterial->specularMapFilename = buffer;
		        }
		        else if (strstr(buffer, "map_bump") != 0)
		        {
		            fgets(buffer, sizeof(buffer), pFile);
		            sscanf(buffer, "%s %s", buffer, buffer);
		            pMaterial->bumpMapFilename = buffer;
		        }
		        else
		        {
		            fgets(buffer, sizeof(buffer), pFile);
		        }
		        break;

		    case 'n': // newmtl
		        fgets(buffer, sizeof(buffer), pFile);
		        sscanf(buffer, "%s %s", buffer, buffer);

		        pMaterial = &m_materials[numMaterials];
		        pMaterial->ambient[0] = 0.2f;
		        pMaterial->ambient[1] = 0.2f;
		        pMaterial->ambient[2] = 0.2f;
		        pMaterial->ambient[3] = 1.0f;
		        pMaterial->diffuse[0] = 0.8f;
		        pMaterial->diffuse[1] = 0.8f;
		        pMaterial->diffuse[2] = 0.8f;
		        pMaterial->diffuse[3] = 1.0f;
		        pMaterial->specular[0] = 0.0f;
		        pMaterial->specular[1] = 0.0f;
		        pMaterial->specular[2] = 0.0f;
		        pMaterial->specular[3] = 1.0f;
		        pMaterial->shininess = 0.0f;
		        pMaterial->alpha = 1.0f;
		        pMaterial->name = buffer;
		        pMaterial->colorMapFilename.clear();
		        pMaterial->specularMapFilename.clear();
		        pMaterial->bumpMapFilename.clear();

		        m_materialCache[pMaterial->name] = numMaterials;
		        ++numMaterials;
		        break;

		    default:
		        fgets(buffer, sizeof(buffer), pFile);
		        break;
		    }
		}

		fclose(pFile);
		return true;
	}
	//--------------------------------------------------------------------------
}


namespace glf
{
	//--------------------------------------------------------------------------
	namespace io
	{
		namespace
		{
			//------------------------------------------------------------------
			typedef std::map<std::string,glf::Texture2D*> TextureDB;
			//------------------------------------------------------------------
			std::string ValidFilename(	const std::string& _inFolder, 
										const std::string& _inFile, 
										const std::string& _default)
			{
				std::string extension;
				if(glf::GetExtension(_inFile,extension))
					return _inFolder+_inFile;
				else
					return _default;
			}
			//------------------------------------------------------------------
			bool FindTexture(	const std::string& _filename, 
								const TextureDB& _textureDB,
								Texture2D*& _texture)
			{
				TextureDB::const_iterator it = _textureDB.find(_filename);
				if(it != _textureDB.end())
				{
					_texture = it->second;
					return true;
				}
				else
				{
					_texture = NULL;
					return false;
				}
			}
			//------------------------------------------------------------------
			Texture2D* GetDiffuseTex(	const std::string& _folder,
										const std::string& _filename, 
										TextureDB& _textureDB,
										ResourceManager& _resourceManager)
			{
				std::string filename = ValidFilename(_folder,_filename,"defaultdiffuse");
				Texture2D* texture   = NULL;
				if(!FindTexture(filename,_textureDB,texture))
				{
					texture = _resourceManager.CreateTexture2D();
					LoadTexture(filename,*texture,true,true);

					texture->SetFiltering(GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR);
					texture->SetAnisotropy(MAX_ANISOSTROPY);
					glBindTexture(texture->target,texture->id);
					glGenerateMipmap(GL_TEXTURE_2D);
					
					_textureDB[filename] = texture;
				}
				return texture;
			}
			//------------------------------------------------------------------
			Texture2D* GetSpecularTex(	const std::string& _folder,
										const std::string& _filename, 
										TextureDB& _textureDB,
										ResourceManager& _resourceManager)
			{
				std::string filename = ValidFilename(_folder,_filename,"");
				Texture2D* texture   = NULL;
				if(!FindTexture(filename,_textureDB,texture))
				{
					texture = _resourceManager.CreateTexture2D();
					LoadTexture(filename,*texture,true,true);

					texture->SetFiltering(GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR);
					texture->SetAnisotropy(MAX_ANISOSTROPY);
					glBindTexture(texture->target,texture->id);
					glGenerateMipmap(GL_TEXTURE_2D);
					
					_textureDB[filename] = texture;
				}
				return texture;
			}
			//------------------------------------------------------------------
			Texture2D* GetNormalTex(	const std::string& _folder,
										const std::string& _filename,
										TextureDB& _textureDB,
										ResourceManager& _resourceManager)
			{
				std::string filename = ValidFilename(_folder,_filename,"defaultnormal");
				Texture2D* texture   = NULL;
				if(!FindTexture(filename,_textureDB,texture))
				{
					texture = _resourceManager.CreateTexture2D();
					LoadTexture(filename,*texture,false,true);

					texture->SetFiltering(GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR);
					texture->SetAnisotropy(MAX_ANISOSTROPY);
					glBindTexture(texture->target,texture->id);
					glGenerateMipmap(GL_TEXTURE_2D);
					
					_textureDB[filename] = texture;
				}
				return texture;
			}
			//------------------------------------------------------------------
			void InitializeDB(	TextureDB& _textureDB,
								ResourceManager& _resourceManager)
			{
				// Create default textures
				Texture2D* diffuseTex  = _resourceManager.CreateTexture2D();
				//Texture2D* specularTex = _resourceManager.CreateTexture2D();
				Texture2D* normalTex   = _resourceManager.CreateTexture2D();

				unsigned char defaultColor[] = {255,255,255};
				diffuseTex->Allocate(GL_SRGB8_ALPHA8,1,1);
				diffuseTex->Fill(GL_RGB,GL_UNSIGNED_BYTE,defaultColor);
				diffuseTex->SetFiltering(GL_LINEAR,GL_LINEAR);

				//unsigned char defaultSpecular[] = {0,0,0};
				//specularTex->Allocate(GL_SRGB8_ALPHA8,1,1);
				//specularTex->Fill(GL_RGB,GL_UNSIGNED_BYTE,defaultSpecular);
				//specularTex->SetFiltering(GL_LINEAR,GL_LINEAR);

				unsigned char defaultNormal[] = {128,128,255};
				normalTex->Allocate(GL_RGBA8,1,1);
				normalTex->Fill(GL_RGB,GL_UNSIGNED_BYTE,defaultNormal);
				normalTex->SetFiltering(GL_LINEAR,GL_LINEAR);

				_textureDB["defaultdiffuse"]  = diffuseTex;
				//_textureDB["defaultspecular"] = specularTex;
				_textureDB["defaultnormal"]   = normalTex;
			}
		}
		//----------------------------------------------------------------------
		void LoadModel(		const std::string& _folder,
							const std::string& _filename,
							const glm::mat4& _transform,
							ResourceManager& _resourceManager,
							SceneManager& _scene,
							bool _verbose)
		{
			// Load objects
			ModelOBJ loader;
			bool loadOK = loader.import((_folder+_filename).c_str(), true, true);
			if(!loadOK)
			{
				glf::Error("Load model error (Folder: %s, Filename: %s)",_folder.c_str(),_filename.c_str());
				exit(-1);
			}

			int nObjects = loader.getNumberOfMeshes();
			if(_verbose)
			{
				glf::Info("Folder          : %s",_folder.c_str());
				glf::Info("Filename        : %s",_filename.c_str());
				glf::Info("nObjects        : %d",nObjects);
				glf::Info("hasNormals      : %d",loader.hasNormals());
				glf::Info("hasPositions    : %d",loader.hasPositions());
				glf::Info("hasTangents     : %d",loader.hasTangents());
				glf::Info("hasTextureCoords: %d",loader.hasTextureCoords());
				glf::Info("nVertices       : %d",loader.getNumberOfVertices());
				glf::Info("nIndices        : %d",loader.getNumberOfIndices());
				glf::Info("nTriangles      : %d",loader.getNumberOfTriangles());
			}
			assert(nObjects>0);
			assert(loader.hasTextureCoords());
			assert(loader.hasNormals());
			assert(loader.hasTangents());

			// Create VBO
			glf::VertexBuffer3F* vb = _resourceManager.CreateVBO3F();
			glf::VertexBuffer3F* nb = _resourceManager.CreateVBO3F();
			glf::VertexBuffer4F* tb = _resourceManager.CreateVBO4F();
			glf::VertexBuffer2F* ub = _resourceManager.CreateVBO2F();

			int nVertices = loader.getNumberOfVertices();
			vb->Allocate(nVertices,GL_STATIC_DRAW);
			nb->Allocate(nVertices,GL_STATIC_DRAW);
			tb->Allocate(nVertices,GL_STATIC_DRAW);
			ub->Allocate(nVertices,GL_STATIC_DRAW);

			glm::mat3 rotTransform = glm::mat3(_transform);
			const ModelOBJ::Vertex* vSource = loader.getVertexBuffer();
			glm::vec3* vptr = vb->Lock();
			glm::vec3* nptr = nb->Lock();
			glm::vec4* tptr = tb->Lock();
			glm::vec2* uptr = ub->Lock();
			for(int i=0;i<nVertices;++i)
			{
				vptr[i].x = vSource[i].position[0];
				vptr[i].y = vSource[i].position[1];
				vptr[i].z = vSource[i].position[2];
				vptr[i]   = glm::vec3(_transform * glm::vec4(vptr[i],1.f));

				nptr[i].x = vSource[i].normal[0];
				nptr[i].y = vSource[i].normal[1];
				nptr[i].z = vSource[i].normal[2];
				nptr[i]   = glm::normalize(rotTransform * nptr[i]);

				tptr[i].x = vSource[i].tangent[0];
				tptr[i].y = vSource[i].tangent[1];
				tptr[i].z = vSource[i].tangent[2];
				tptr[i].w = 0; 						// For removing translation
				tptr[i]   = glm::normalize(_transform * tptr[i]);

				glm::vec3 bitangent;
				bitangent.x = vSource[i].bitangent[0];
				bitangent.y = vSource[i].bitangent[1];
				bitangent.z = vSource[i].bitangent[2];
				bitangent   = glm::normalize(rotTransform * bitangent);

				uptr[i].x = vSource[i].texCoord[0];
				uptr[i].y = vSource[i].texCoord[1];

				// Compute the referential's handedness and store its sign 
				// into w component of the tangent vector
				tptr[i].w = glm::dot(bitangent,glm::normalize(glm::cross(nptr[i],glm::vec3(tptr[i]))));
				//glf::Info("Sign : %f",tptr[i].w);
			}
			ub->Unlock();
			tb->Unlock();
			nb->Unlock();
			vb->Unlock();

			// Create IBO
			glf::IndexBuffer* ib = _resourceManager.CreateIBO();
			int nIndices = loader.getNumberOfIndices();
			ib->Allocate(nIndices);
			const int* iSource = loader.getIndexBuffer();
			unsigned int* iptr = ib->Lock();
			for(int i=0;i<nIndices;++i)
				iptr[i] = iSource[i];
			ib->Unlock();

			// Create VAOs
			glf::VertexArray* regularVAO = _resourceManager.CreateVAO();
			regularVAO->Add(*vb,semantic::Position, 3,GL_FLOAT,false,0);
			regularVAO->Add(*nb,semantic::Normal,   3,GL_FLOAT,false,0);
			regularVAO->Add(*tb,semantic::Tangent,  4,GL_FLOAT,false,0);
			regularVAO->Add(*ub,semantic::TexCoord, 2,GL_FLOAT,false,0);

			glf::VertexArray* shadowVAO  = _resourceManager.CreateVAO();
			shadowVAO->Add(*vb,semantic::Position,  3,GL_FLOAT,false,0);

			// Create objets and load textures
			TextureDB textureDB;
			InitializeDB(textureDB,_resourceManager);
			for(int i=0;i<nObjects;++i)
			{
				const ModelOBJ::Mesh& mesh	= loader.getMesh(i);

				// Load textures
				glf::Texture2D* diffuseTex  = GetDiffuseTex(_folder,mesh.pMaterial->colorMapFilename,textureDB,_resourceManager);
				//glf::Texture2D* specularTex = GetSpecularTex(_folder,mesh.pMaterial->specularMapFilename,textureDB,_resourceManager);
				#if ENABLE_LOAD_NORMAL_MAP
				glf::Texture2D* normalTex   = GetNormalTex(_folder,mesh.pMaterial->bumpMapFilename,textureDB,_resourceManager);
				#else
				glf::Texture2D* normalTex   = GetNormalTex(_folder,"",textureDB,_resourceManager);
				#endif

				// Create and add regular mesh
				RegularMesh rmesh;
				rmesh.diffuseTex   = diffuseTex;
				//rmesh.specularTex  = specularTex;
				rmesh.normalTex    = normalTex;
				rmesh.roughness    = 1.f / mesh.pMaterial->shininess; // (Has to be specified as roughness into MTL file)
				rmesh.specularity  = 0.3333f * (mesh.pMaterial->specular[0]+mesh.pMaterial->specular[1]+mesh.pMaterial->specular[2]);
				rmesh.indices      = ib;
				rmesh.startIndices = mesh.startIndex;
				rmesh.countIndices = mesh.triangleCount*3;
				rmesh.primitiveType= GL_TRIANGLES;
				rmesh.primitive    = regularVAO;
				_scene.regularMeshes.push_back(rmesh);

				// Create and add shadow mesh
				ShadowMesh smesh;
				smesh.indices      = ib;
				smesh.startIndices = mesh.startIndex;
				smesh.countIndices = mesh.triangleCount*3;
				smesh.primitiveType= GL_TRIANGLES;
				smesh.primitive    = shadowVAO;
				_scene.shadowMeshes.push_back(smesh);

				glm::mat4 identity(1);
				_scene.transformations.push_back(identity);

				BBox obound = ObjectBound(*vb,*ib,rmesh.startIndices,rmesh.countIndices);
				_scene.oBounds.push_back(obound);

				#if ENABLE_OBJECT_TBN_HELPERS
					glf::manager::helpers->CreateTangentSpace(*vb,*nb,*tb,*ib,rmesh.startIndices,rmesh.countIndices,0.1f);
				#endif

				if(_verbose)
				{
					glf::Info("----------------------------------------------");
					glf::Info("MeshID       : %d",i);
					glf::Info("startIndex   : %d",mesh.startIndex);
					glf::Info("triangleCount: %d",mesh.triangleCount);

					glf::Info("Ambient   : %f,%f,%f,%f",
								mesh.pMaterial->ambient[0],
								mesh.pMaterial->ambient[1],
								mesh.pMaterial->ambient[2],
								mesh.pMaterial->ambient[3]);

					glf::Info("Diffuse   : %f,%f,%f,%f",
								mesh.pMaterial->diffuse[0],
								mesh.pMaterial->diffuse[1],
								mesh.pMaterial->diffuse[2],
								mesh.pMaterial->diffuse[3]);

					glf::Info("Specular  : %f,%f,%f,%f",
								mesh.pMaterial->specular[0],
								mesh.pMaterial->specular[1],
								mesh.pMaterial->specular[2],
								mesh.pMaterial->specular[3]);

					glf::Info("Shininess : %f",mesh.pMaterial->shininess);
					glf::Info("Alpha     : %f",mesh.pMaterial->alpha);
					glf::Info("Name      : %s",mesh.pMaterial->name.c_str());
					glf::Info("Color     : %s",mesh.pMaterial->colorMapFilename.c_str());
					glf::Info("Specular  : %s",mesh.pMaterial->specularMapFilename.c_str());
					glf::Info("Bump      : %s",mesh.pMaterial->bumpMapFilename.c_str());

					glf::Info("Bound     : (%f,%f,%f) (%f,%f,%f)",
											obound.pMin.x,
											obound.pMin.y,
											obound.pMin.z,
											obound.pMax.x,
											obound.pMax.y,
											obound.pMax.z);
				}
			}

			loader.destroy();
		}
		//----------------------------------------------------------------------
		void LoadTerrain(	const std::string& _folder,
							const std::string& _diffuseTex,
							const std::string& _heightTex,
							float _roughness,
							float _specularity,
							const glm::vec2& _terrainSize,
							const glm::vec3& _terrainOffset,
							int   _tileResolution,
							float _heightFactor,
							float _tessFactor,
							float _projFactor,
							float _tileFactor,
							ResourceManager& _resourceManager,
							SceneManager& _scene,
							bool _verbose)
		{
			TextureDB textureDB;
			InitializeDB(textureDB,_resourceManager);

			glf::VertexBuffer2F* terrainVBO = _resourceManager.CreateVBO2F();
			terrainVBO->Allocate(4,GL_STATIC_DRAW);
			glm::vec2* vertices = terrainVBO->Lock();
			vertices[0] = glm::vec2(0,0);
			vertices[1] = glm::vec2(1,0);
			vertices[2] = glm::vec2(1,1);
			vertices[3] = glm::vec2(0,1);
			terrainVBO->Unlock();

			glf::VertexArray* terrainVAO = _resourceManager.CreateVAO();
			terrainVAO->Add(*terrainVBO,glf::semantic::Position,2,GL_FLOAT);

			glf::Texture2D* diffuseTex = GetDiffuseTex(_folder,_diffuseTex,textureDB,_resourceManager);
			glf::Texture2D* heightTex  = GetDiffuseTex(_folder,_heightTex,textureDB,_resourceManager);
			glf::Texture2D* normalTex  = _resourceManager.CreateTexture2D();
			normalTex->Allocate(GL_RGBA8, heightTex->size.x, heightTex->size.y,true);
			normalTex->SetFiltering(GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR);
			normalTex->SetAnisotropy(MAX_ANISOSTROPY);

			TerrainMesh mesh(_terrainSize,_terrainOffset,diffuseTex,normalTex,heightTex,_tileFactor,_roughness,_specularity,_tileResolution);
			mesh.primitive  = terrainVAO;
			mesh.Tesselation(_tileResolution,_heightFactor,_tessFactor,_projFactor);
			_scene.terrainMeshes.push_back(mesh);
			_scene.tBounds.push_back(mesh.Bound());
		}
	}
}
