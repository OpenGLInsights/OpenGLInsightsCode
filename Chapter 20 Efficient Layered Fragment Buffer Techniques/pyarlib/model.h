/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

#include "includegl.h"

#include "vec.h"
#include "texture.h"
#include "gpu.h"

#define DEBUG_INFO 0
#define EFFICIENT_MEMORY 0

class Model
{
public:
	std::string name;
	Model(std::string filename, bool genTangents = false);
	~Model();
	void draw(GLuint vertLoc = -1, GLuint normLoc = -1, GLuint texLoc = -1, std::vector<int> triangles = std::vector<int>());
	const float* getTangents();
	unsigned int getTangentsSize();
	VertexBuffer verticesVBO;
	IndexBuffer indicesVBO;
	int stride;
	int offsetNorm;
	int offsetTex;
	unsigned int vertSize, normSize, texSize, tanSize, indexSize;
private:
	float* vertPtr;
	float* normPtr;
	float* tanPtr;
	float* texPtr;
	unsigned int* indexPtr;
	struct TriangleIndex
	{
		TriangleIndex() {}
		TriangleIndex(unsigned int a, unsigned int b, unsigned int c)
		{
			this->a = a; this->b = b; this->c = c;
		}
		unsigned int a, b, c;
	};
	struct Material
	{
		Material()
		{
			Ka[0] = 0.0; Ka[1] = 0.0; Ka[2] = 0.0; Ka[3] = 1.0;
			Kd[0] = 1.0; Kd[1] = 1.0; Kd[2] = 1.0; Kd[3] = 1.0;
			Ks[0] = 1.0; Ks[1] = 1.0; Ks[2] = 1.0; Ks[3] = 1.0;
			Ns = 50.0;
			tex = 0;
		}
		float Ka[4], Kd[4], Ks[4];
		float Ns;
		GLuint tex;
	};
	struct FaceSet
	{
		Material mat;
		unsigned int offset;
		unsigned int length;
	};
	void parseMtl(std::string base, std::string filename);
	GLuint loadTexture(std::string filename);
	vec3f toVec3f(std::string str);
	float toFloat(std::string str);
	unsigned int toInt(std::string str);
	std::vector<std::string> splitStr(std::string str, std::string sep = "", int maxSplit = -1);
	std::map<std::string, Material> mats;
	std::vector<FaceSet> fsets;
	std::map<std::string, GLuint> textures;
};
