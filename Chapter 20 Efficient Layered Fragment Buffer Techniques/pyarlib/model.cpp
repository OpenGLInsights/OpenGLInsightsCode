/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <GL/glew.h>
#include <assert.h>
#include "model.h"
#include "shaderutil.h"
#include "util.h"

#ifdef _WIN32
typedef unsigned __int64 uint64;
#else
#include <inttypes.h>
typedef uint64_t uint64;
#endif

#define SHORT_TUPLE(a, b, c) ((uint64)a * ((uint64)1<<(16*2)) + (uint64)b * ((uint64)1<<(16*1)) + (uint64)c * ((uint64)1<<(16*0)))

using namespace std;

typedef map<uint64, unsigned int> VertexMap;

Model::Model(string filename, bool genTangents)
{
	name = filename;
	vertPtr = NULL;
	normPtr = NULL;
	texPtr = NULL;
	tanPtr = NULL;
	indexPtr = NULL;

	string basePath;
	int lastSlash = max((int)filename.rfind("/"), (int)filename.rfind("\\"));
	if (lastSlash > 0) basePath = filename.substr(0, lastSlash + 1);

	bool hasNormal = false;
	bool hasTexture = false;
	int numFaces = 0;
	ifstream ifile(filename.c_str());
	if (!ifile.is_open())
	{
		cout << "Cannot open " << filename << endl;
		return;
	}
		
	mats["default"] = Material();
	
	bool gotFirstBound = false;
	vec3f boundMax, boundMin;
	vector<vec3f> readverts;
	vector<vec3f> readnorms;
	vector<vec2f> readtexcs;
	vector<vec3f> verts;
	vector<vec3f> norms;
	vector<vec2f> texcs;
	vector<vec4f> tangents;
	VertexMap vertexMap;
	vector<TriangleIndex> indices;
	string token;
	while (ifile >> token)
	{
		if (!token.size() || token[0] == '#')
			continue;
		if (token.compare("v") == 0)
		{
			float a, b, c;
			ifile >> a >> b >> c;
			vec3f v(a, b, c);
			readverts.push_back(v);
			if (!gotFirstBound)
			{
				gotFirstBound = true;
				boundMax = boundMin = v;
			}
			boundMax = vmax(boundMax, v);
			boundMin = vmin(boundMin, v);
		}
		else if (token.compare("vn") == 0)
		{
			float a, b, c;
			ifile >> a >> b >> c;
			vec3f n(a, b, c);
			//n.normalize();
			readnorms.push_back(n);
			hasNormal = true;
		}
		else if (token.compare("vt") == 0)
		{
			float a, b;
			ifile >> a >> b;
			vec2f t(a, b);
			readtexcs.push_back(t);
			hasTexture = true;
		}
		else if (token.compare("f") == 0)
		{
			++numFaces;
			unsigned int f[3];
			unsigned int i = 0;
			unsigned int v, t, n;
			while (ifile.peek() != '\n' && ifile >> token)
			{
				int ai = 0;
				int iarray[3];
				int a = 0, b;
				while ((b = (int)token.find("/", a)) >= 0)
				{
					iarray[ai++] = max(1, atoi(token.substr(a, b).c_str()));
					a = b+1;
				}
				iarray[ai++] = max(1, atoi(token.substr(a).c_str()));

				if (ai != 3) cout << token << endl;
				assert(ai == 3);

				++i;
				v = iarray[0]-1;
				t = iarray[1]-1;
				n = iarray[2]-1;

				//cout << token << " " << v << " " << t << " " << n << endl;
				
				assert(v < readverts.size());
				if (hasTexture)
				{
					if (!(t < readtexcs.size()))
						cout << token << " " << t << endl;
					assert(t < readtexcs.size());
				}
				if (hasNormal)
				{
					assert(n < readnorms.size());
				}

				/*
				vector<string> sindices = splitStr(split[i], "/");
				assert((int)sindices.size() == 1 + hasNormal + hasTexture);

				v = toInt(sindices[0])-1;
				if (hasNormal)
					if (hasTexture)
					{
						t = toInt(sindices[1])-1;
						n = toInt(sindices[2])-1;
					}
					else
						n = toInt(sindices[1])-1;
				else if (hasTexture)
					t = toInt(sindices[1])-1;
					*/

				unsigned int newvertindex;
#if EFFICIENT_MEMORY
				uint64 vertcomb = SHORT_TUPLE(v, t, n);
				VertexMap::iterator found = vertexMap.find(vertcomb);
				if (found == vertexMap.end())
				{
					newvertindex = (unsigned int)verts.size();
					vertexMap[vertcomb] = newvertindex;
					verts.push_back(readverts[v]);
					if (hasNormal)
						norms.push_back(readnorms[n].unit());
					if (hasTexture)
						texcs.push_back(readtexcs[t]);
				}
				else
					newvertindex = found->second;
#else
					newvertindex = (unsigned int)verts.size();
					verts.push_back(readverts[v]);
					if (hasNormal)
						norms.push_back(readnorms[n].unit());
					if (hasTexture)
						texcs.push_back(readtexcs[t]);
#endif
				if (i-1 > 2)
					f[1] = f[2];
				f[min(i-1, 2U)] = newvertindex;
				if (i-1 >= 2)
					indices.push_back(TriangleIndex(f[0], f[1], f[2]));
			}
		}
		else if (token.compare("mtllib") == 0)
		{
			ifile >> token;
#if DEBUG_INFO
			cout << "mtllib= " << token << endl;
#endif
			parseMtl(basePath, token);
		}
		else if (token.compare("usemtl") == 0)
		{
			ifile >> token;
			if (mats.find(token) != mats.end())
			{
				fsets.push_back(FaceSet());
				fsets.back().mat = mats[token];
				fsets.back().offset = (unsigned int)indices.size();
			}
#if DEBUG_INFO
			else
				cout << "unknown mtl: " << token << endl;
#endif
		}
		ifile.ignore(9999, '\n');
	}
	ifile.close();

#if DEBUG_INFO
	cout << "Input: v=" << readverts.size() << " n=" << readnorms.size() << " vt=" << readtexcs.size() << " f=" << numFaces << endl;
#endif

	vertSize = (unsigned int)verts.size();
	normSize = (unsigned int)norms.size();
	texSize = (unsigned int)texcs.size();
	indexSize = (unsigned int)indices.size();

#if DEBUG_INFO
	cout << "Input: v=" << vertSize << " n=" << normSize << " vt=" << texSize << " f=" << indexSize << endl;
#endif
	
	if (hasNormal)
		assert(vertSize == normSize); //normals must be the same size as vertices
	if (hasTexture)
		assert(vertSize == texSize); //texture coordinates must be the same size as vertices

	//generate tangent vectors for height mapping
	if (hasNormal && hasTexture && genTangents)
	{
		/*
		Tangent Space calculations from:
			Lengyel, Eric. Computing Tangent Space Basis Vectors for an Arbitrary Mesh.
			Terathon Software 3D Graphics Library, 2001. http://www.terathon.com/code/tangent.html
		*/
		tangents.assign(verts.size(), vec4f(0.0));
		for (unsigned int f = 0; f < indices.size(); ++f)
		{
			TriangleIndex& t = indices[f];
			vec3f va = verts[t.b] - verts[t.a];
			vec3f vb = verts[t.c] - verts[t.a];
			vec2f ca = texcs[t.b] - texcs[t.a];
			vec2f cb = texcs[t.c] - texcs[t.a];
			float det = 1.0f / (ca.x*cb.y - cb.x*ca.y);
			vec3f B = (vb*ca.x - va*cb.x) * det;
			//vec3f T = (va*cb.y - vb*ca.y) * det;
			tangents[t.a] += norms[t.a].cross(B);
			tangents[t.b] += norms[t.b].cross(B);
			tangents[t.c] += norms[t.c].cross(B);
		}
		for (unsigned int v = 0; v < tangents.size(); ++v)
		{
			float length = vec3f(tangents[v]).size();
			if (length > 0.01)
				tangents[v] /= length;
			else
				tangents[v] = vec4f(1, 0, 0, 1);
			tangents[v].w = 1.0;
		}
			
		//OLD WAY:
		/*vector<vec3f> tans;
		vector<vec3f> btans;
		tans.assign(verts.size(), vec3f(0.0)); //create placeholder data
		btans.assign(verts.size(), vec3f(0.0));
		for (unsigned int f = 0; f < indices.size(); ++f)
		{
			TriangleIndex& t = indices[f];
			vec3f va = verts[t.b] - verts[t.a];
			vec3f vb = verts[t.c] - verts[t.a];
			vec2f ta = texcs[t.b] - texcs[t.a];
			vec2f tb = texcs[t.c] - texcs[t.a];
			float r = 1.0f / (ta.x * tb.y - tb.x * ta.y);
			vec3f sdir(tb.y*va.x - ta.y*vb.x, tb.y*va.y - ta.y*vb.y, tb.y*va.z - ta.y*vb.z);
			vec3f tdir(ta.x*vb.x - tb.x*va.x, ta.x*vb.y - tb.x*va.y, ta.x*vb.z - tb.x*va.z);
			sdir *= r;
			tdir *= r;
			//printf("%i: %f %f %f %f\n", t.a, sdir.x, sdir.y, sdir.z);
			tans[t.a] += sdir;
			tans[t.b] += sdir;
			tans[t.c] += sdir;
			btans[t.a] += tdir;
			btans[t.b] += tdir;
			btans[t.c] += tdir;
		}
		for (unsigned int v = 0; v < tangents.size(); ++v)
		{
			tangents[v] = tans[v] - norms[v] * norms[v].dot(tans[v]);
			tangents[v].w = norms[v].cross(tans[v]).dot(btans[v]) < 0.0f ? -1.0f : 1.0f;
			//printf("t%i: %f %f %f %f\n", v, tans[v].x, tans[v].y, tans[v].z);
			//printf("n%i: %f %f %f %f\n", v, norms[v].x, norms[v].y, norms[v].z);
			printf("a%i: %f %f %f %f\n", v, tangents[v].x, tangents[v].y, tangents[v].z, tangents[v].w);
		}*/
	}
	tanSize = (unsigned int)tangents.size();

	if (mats.size() > 0)
	{
		//handle objects with no assigned material
		if (fsets.size() == 0)
		{
			fsets.push_back(FaceSet());
			fsets.back().mat = mats.begin()->second;
			fsets.back().offset = 0;
		}
		for (int i = 0; i < (int)fsets.size()-1; ++i)
		{
			fsets[i].length = fsets[i+1].offset - fsets[i].offset;
		}
		fsets[fsets.size()-1].length = indexSize - fsets[fsets.size()-1].offset;
	}
	assert(fsets.size() > 0);


	vec3f center = (boundMax + boundMin) * 0.5f;
	vec3f translate = -vec3f(center.x, boundMin.y, center.z);//-center;
	vec3f size = boundMax - boundMin;
	float m = std::max(std::max(size.x, size.y), size.z);
	vec3f scale = vec3f(1.0f / m);

	stride = 3 + hasNormal * 3 + hasTexture * 2;
	offsetNorm = 3;
	offsetTex = offsetNorm + hasNormal * 3;
	float* interleaved = new float[vertSize * stride];
	for (unsigned int i = 0; i < vertSize; ++i)
	{
		*(vec3f*)(interleaved + (i * stride)) = (verts[i] + translate) * scale;
		if (hasNormal)
			*(vec3f*)(interleaved + (i * stride + offsetNorm)) = norms[i];
		if (hasTexture)
			*(vec2f*)(interleaved + (i * stride + offsetTex)) = texcs[i];
	}
/*
	for (unsigned int i = 0; i < indexSize; ++i)
	{
		assert(indices[i].a >= 0 && indices[i].a < vertSize &&
			indices[i].b >= 0 && indices[i].b < vertSize &&
			indices[i].c >= 0 && indices[i].c < vertSize);
			
	}
*/

	verticesVBO.buffer(interleaved, vertSize*stride*sizeof(float));
	indicesVBO.buffer(&indices[0], indexSize*3*sizeof(unsigned int));

	delete[] interleaved;

	if (tangents.size())
	{
		tanPtr = new float[tanSize * sizeof(float) * 4];
		memcpy(tanPtr, &tangents[0], tanSize * sizeof(float) * 4);
	}

/*
	vertPtr = new float[vertSize * sizeof(float) * 3];
	normPtr = new float[normSize * sizeof(float) * 3];
	texPtr = new float[texSize * sizeof(float) * 2];
	tanPtr = new float[tanSize * sizeof(float) * 4];
	indexPtr = new unsigned int[indexSize * sizeof(unsigned int) * 3];

	for (unsigned int i = 0; i < vertSize; ++i)
	{
		verts[i] = (verts[i] + translate) * scale;
		vertPtr[3*i+0] = verts[i].x;
		vertPtr[3*i+1] = verts[i].y;
		vertPtr[3*i+2] = verts[i].z;
	}
	for (unsigned int i = 0; i < normSize; ++i)
	{
		normPtr[3*i+0] = norms[i].x;
		normPtr[3*i+1] = norms[i].y;
		normPtr[3*i+2] = norms[i].z;
	}
	for (unsigned int i = 0; i < texSize; ++i)
	{
		texPtr[2*i+0] = texcs[i].x;
		texPtr[2*i+1] = texcs[i].y;
	}
	for (unsigned int i = 0; i < tanSize; ++i)
	{
		tanPtr[4*i+0] = tangents[i].x;
		tanPtr[4*i+1] = tangents[i].y;
		tanPtr[4*i+2] = tangents[i].z;
		tanPtr[4*i+3] = tangents[i].w;
	}
	for (unsigned int i = 0; i < indexSize; ++i)
	{
		indexPtr[3*i+0] = indices[i].a;
		indexPtr[3*i+1] = indices[i].b;
		indexPtr[3*i+2] = indices[i].c;
	}
*/
}
Model::~Model()
{
	delete[] vertPtr;
	delete[] normPtr;
	delete[] texPtr;
	delete[] tanPtr;
	delete[] indexPtr;
}
void Model::parseMtl(string base, string filename)
{
	ifstream ifile((base + filename).c_str());
	if (!ifile.is_open())
	{
#if DEBUG_INFO
		cout << "Cannot open " << base + filename << endl;
#endif
		return;
	}
	Material m;
	string name;
	string line;
	while (ifile.good())
	{
		getline(ifile, line);
		vector<string> split = splitStr(line, " ");
		if (split.size() == 0 || split[0][0] == '#')
			continue;
		if (split[0].compare("newmtl") == 0)
		{
			if (name.size() > 0)
				mats[name] = m;
			name = split[1];
		}
		else if (split[0].compare("Ka") == 0)
		{
			m.Ka[0] = toFloat(split[1]);
			m.Ka[1] = toFloat(split[2]);
			m.Ka[2] = toFloat(split[3]);
			if (split.size() > 4)
				m.Ka[3] = toFloat(split[4]);
			else
				m.Ka[3] = 1.0f;
		}
		else if (split[0].compare("Kd") == 0)
		{
			m.Kd[0] = toFloat(split[1]);
			m.Kd[1] = toFloat(split[2]);
			m.Kd[2] = toFloat(split[3]);
			if (split.size() > 4)
				m.Kd[3] = toFloat(split[4]);
			else
				m.Kd[3] = 1.0f;
		}
		else if (split[0].compare("Ks") == 0)
		{
			m.Ks[0] = toFloat(split[1]);
			m.Ks[1] = toFloat(split[2]);
			m.Ks[2] = toFloat(split[3]);
			if (split.size() > 4)
				m.Ks[3] = toFloat(split[4]);
			else
				m.Ks[3] = 1.0f;
		}
		else if (split[0].compare("map_Kd") == 0)
		{
			string texName = splitStr(line, " ", 1)[1];
			map<string, GLuint>::iterator found;
			found = textures.find(texName);
			if (found != textures.end())
				m.tex = found->second;
			else
			{
#if DEBUG_INFO
				cout << "unique map_Kd " << texName << endl;
#endif
				setTextureRepeat(true);
				m.tex = getTexture((base + texName).c_str());
				setTextureRepeat(false);
				textures[texName] = m.tex;
			}
		}
		else if (split[0].compare("Ns") == 0)
		{
			m.Ns = toFloat(split[1]);
			if (m.Ns <= 0.0f)
				m.Ns = 50.0f;
		}
	}
	if (name.size() > 0)
			mats[name] = m;
	ifile.close();
}

void Model::draw(GLuint vertLoc, GLuint normLoc, GLuint texLoc, vector<int> triangles)
{
	bool verts = vertLoc != (GLuint)-1;
	bool norms = normLoc != (GLuint)-1 && normSize;
	bool texcs = texLoc != (GLuint)-1 && texSize;
	bool useAttribs = verts;
	
	GLint program = 0;
	GLuint diffuseLoc = (GLuint)-1;
	GLuint offsetLoc = (GLuint)-1;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	if (program)
	{
		diffuseLoc = glGetUniformLocation(program, "materialDiffuse");
		offsetLoc = glGetUniformLocation(program, "primitiveIndexOffset");
	}

	if (useAttribs)
	{
		glEnableVertexAttribArray(vertLoc);
		if (norms) glEnableVertexAttribArray(normLoc);
		if (texcs) glEnableVertexAttribArray(texLoc);

		glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
		glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), 0);
		if (norms) glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offsetNorm * sizeof(float)));
		if (texcs) glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offsetTex * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else
	{
		glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_VERTEX_ARRAY);
		if (normSize) glEnableClientState(GL_NORMAL_ARRAY);
		if (texSize) glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
		glVertexPointer(3, GL_FLOAT, stride * sizeof(float), 0);
		if (normSize) glNormalPointer(GL_FLOAT, stride * sizeof(float), (void*)(offsetNorm * sizeof(float)));
		if (texSize) glTexCoordPointer(2, GL_FLOAT, stride * sizeof(float), (void*)(offsetTex * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO);

	unsigned int curTri = 0;
	for (unsigned int i = 0; i < fsets.size(); ++i)
	{
		//set texture
		if (texSize)
		{
			if (useAttribs && texcs)
				setActiveTexture(0, "tex", fsets[i].mat.tex);
			else
			{
				if (texcs && fsets[i].mat.tex)
				{
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, fsets[i].mat.tex);
				}
				else
				{
					glDisable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, 0);
				}
			}
		}

		//set material
		glMaterialfv(GL_FRONT, GL_AMBIENT, fsets[i].mat.Ka);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, fsets[i].mat.Kd);
		glMaterialfv(GL_FRONT, GL_SPECULAR, fsets[i].mat.Ks);
		glMaterialf(GL_FRONT, GL_SHININESS, fsets[i].mat.Ns);

		if (diffuseLoc != (GLuint)-1)
			glUniform4fv(diffuseLoc, 1, fsets[i].mat.Kd);
		if (offsetLoc != (GLuint)-1)
			glUniform1i(offsetLoc, fsets[i].offset);
		
		if (triangles.size())
		{
			while (curTri < triangles.size() && (unsigned int)triangles[curTri] >= fsets[i].offset && (unsigned int)triangles[curTri] < fsets[i].offset + fsets[i].length)
			{
				//printf("%i %i -> %i/%i = %i\n", fsets[i].offset, fsets[i].length, curTri, triangles.size(), triangles[curTri]);
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(triangles[curTri++] * 3 * sizeof(int)));
			}
		}
		else if (fsets[i].length > 0)
		{
			assert(fsets[i].length * 3 + fsets[i].offset * 3 <= indexSize * 3);
			glDrawElements(GL_TRIANGLES, fsets[i].length * 3, GL_UNSIGNED_INT, (void*)(fsets[i].offset * 3 * sizeof(unsigned int)));
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisable(GL_TEXTURE_2D);
	if (useAttribs)
	{
		glDisableVertexAttribArray(vertLoc);
		if (norms) glDisableVertexAttribArray(normLoc);
		if (texcs) glDisableVertexAttribArray(texLoc);
	}
	else
		glPopClientAttrib();
}
float Model::toFloat(string str)
{
	float ret;
	stringstream converter(str);
	converter >> ret;
	return ret;
}
unsigned int Model::toInt(string str)
{
	unsigned int ret;
	stringstream converter(str);
	converter >> ret;
	return ret;
}
vector<string> Model::splitStr(string str, string sep, int maxSplit)
{
	vector<string> ret;
	int l = 0, p = 0;
	while ((p = (int)str.find_first_of(sep, l)) >= 0)
	{
		if (p > 0 && p != l)
		{
			if (maxSplit-- == 0)
				break;
			ret.push_back(str.substr(l, p-l));
		}
		l = p + (int)sep.size();
	}
	if (l < (int)str.size())
		ret.push_back(str.substr(l));
	return ret;
}

const float* Model::getTangents()
{
	return tanPtr;
}

unsigned int Model::getTangentsSize()
{
	return tanSize;
}



/*

GLuint Model::loadTexture(string filename)
{
	int dataOffset;
	int numPixels;
	int width, height, channels;
	unsigned char* data;
	GLuint texReference = 0;
	GLuint format;

	ifstream file(filename.c_str(), ios::binary);

	//process header
	file.seekg(10, ios::beg);
	file.read((char*)&dataOffset, 4);

	
	file.seekg(18, ios::beg);
	file.read((char*)&width, 4);
	file.seekg(22, ios::beg);
	file.read((char*)&height, 4);
	file.seekg(28, ios::beg);
	file.read((char*)&channels, 4);
	channels /= 8;

	//read actual data
	numPixels = width * height;
	data = new unsigned char[numPixels * channels];
	file.seekg(dataOffset, ios::beg);
	file.read((char*)data, numPixels * channels * sizeof(unsigned char));

	//copy data to an opengl texture
	glGenTextures(1, &texReference);
	if (texReference == 0)
		return 0;
	if (channels == 3)
		format = GL_BGR_EXT;
	else if (channels == 4)
		format = GL_BGRA_EXT;
	else
		return 0;
	glBindTexture(GL_TEXTURE_2D, texReference);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		channels,
		width,
		height,
		0,
		format,
		GL_UNSIGNED_BYTE,
		data);
	delete[] data;
	return texReference;
}
*/
