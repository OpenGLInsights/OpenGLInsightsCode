//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/helper.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

namespace glf
{
	//--------------------------------------------------------------------------
	Helper::Helper():
	transform(1.f),
	type(GL_LINES)
	{
		vao.Add(vbuffer,semantic::Position,3,GL_FLOAT);
		vao.Add(cbuffer,semantic::Color,3,GL_FLOAT);
	}
	//--------------------------------------------------------------------------
	Helper::~Helper()
	{

	}
	//--------------------------------------------------------------------------
	Helper::Ptr Helper::Create()
	{
		return Helper::Ptr(new Helper());
	}
	//--------------------------------------------------------------------------
	HelperRenderer::HelperRenderer():
	program("HelperRenderer")
	{
		program.Compile(ProgramOptions::CreateVSOptions().Append(LoadFile(directory::ShaderDirectory + "helper.vs")),
						LoadFile(directory::ShaderDirectory + "helper.fs"));

		transformVar	= program["Transform"].location;
		modelVar		= program["Model"].location;

		vbufferVar		= program["Position"].location;
		cbufferVar		= program["Color"].location;
	}
	//--------------------------------------------------------------------------
	void HelperRenderer::Draw(		const glm::mat4& _projection,
									const glm::mat4& _view,
									const std::vector<Helper::Ptr>& _helpers)
	{
		// Render lighting pass
		glUseProgram(program.id);
		glf::CheckError("Check program");
		glm::mat4 transform = _projection * _view;
		glProgramUniformMatrix4fv(program.id, transformVar,  1, GL_FALSE, &transform[0][0]);

		// Render at the same resolution than the original window
		// Draw all helpers
		for(unsigned int i=0;i<_helpers.size();++i)
		{
			glProgramUniformMatrix4fv(program.id, 	modelVar,  1, 	GL_FALSE, &_helpers[i]->transform[0][0]);
			_helpers[i]->vao.Draw(_helpers[i]->type,_helpers[i]->vbuffer.count);
		}
	}
	//--------------------------------------------------------------------------
	HelperRenderer::~HelperRenderer()
	{

	}
	//--------------------------------------------------------------------------
	HelperManager::Ptr HelperManager::Create()
	{
		return Ptr(new HelperManager());
	}
	//--------------------------------------------------------------------------
	HelperManager::HelperManager()
	{

	}
	//--------------------------------------------------------------------------
	HelperManager::~HelperManager()
	{
		Clear();
	}
	//--------------------------------------------------------------------------
	void HelperManager::Clear()
	{
		helpers.clear();
	}
	//--------------------------------------------------------------------------
	Helper::Ptr HelperManager::CreatePointSet(	const std::vector<glm::vec3>& _points,
												const glm::mat4& _t)
	{
		Helper::Ptr ref = Helper::Create();
		ref->vbuffer.Allocate(int(_points.size()),GL_STATIC_DRAW);
		ref->cbuffer.Allocate(int(_points.size()),GL_STATIC_DRAW);
		ref->type  = GL_POINTS;
		ref->transform = _t;

		glm::vec3* v = ref->vbuffer.Lock();
		glm::vec3* c = ref->cbuffer.Lock();
		for(unsigned int i=0;i<_points.size();++i)
		{
			v[i] = _points[i];
			c[i] = glm::vec3(1,1,0);
		}

		ref->vbuffer.Unlock();
		ref->cbuffer.Unlock();

		helpers.push_back(ref);
		return ref;
	}
	//--------------------------------------------------------------------------
	Helper::Ptr HelperManager::CreateReferential(	float _s, 
													const glm::mat4& _t)
	{
		Helper::Ptr ref = Helper::Create();
		ref->vbuffer.Allocate(6,GL_STATIC_DRAW);
		ref->cbuffer.Allocate(6,GL_STATIC_DRAW);
		ref->type  = GL_LINES;
		ref->transform = _t;

		glm::vec3* v = ref->vbuffer.Lock();
		glm::vec3* c = ref->cbuffer.Lock();
		v[0] = glm::vec3(0,0,0);
		v[1] = glm::vec3(_s,0,0);

		c[0] = glm::vec3(1,0,0);
		c[1] = glm::vec3(1,0,0);

		v[2] = glm::vec3(0,0,0);
		v[3] = glm::vec3(0,_s,0);

		c[2] = glm::vec3(0,1,0);
		c[3] = glm::vec3(0,1,0);

		v[4] = glm::vec3(0,0,0);
		v[5] = glm::vec3(0,0,_s);

		c[4] = glm::vec3(0,0,1);
		c[5] = glm::vec3(0,0,1);
		ref->vbuffer.Unlock();
		ref->cbuffer.Unlock();

		helpers.push_back(ref);
		return ref;
	}
	//--------------------------------------------------------------------------
	Helper::Ptr HelperManager::CreateReferential(	const glm::vec3& _x, 
													const glm::vec3& _y, 
													const glm::vec3& _z, 
													float _s,
													const glm::mat4& _t)
	{
		Helper::Ptr ref = Helper::Create();
		ref->vbuffer.Allocate(6,GL_STATIC_DRAW);
		ref->cbuffer.Allocate(6,GL_STATIC_DRAW);
		ref->type  = GL_LINES;
		ref->transform = _t;

		glm::vec3* v = ref->vbuffer.Lock();
		glm::vec3* c = ref->cbuffer.Lock();
		v[0] = glm::vec3(0,0,0);
		v[1] = _s*_x;

		c[0] = glm::vec3(1,0,0);
		c[1] = glm::vec3(1,0,0);

		v[2] = glm::vec3(0,0,0);
		v[3] = _s*_y;

		c[2] = glm::vec3(0,1,0);
		c[3] = glm::vec3(0,1,0);

		v[4] = glm::vec3(0,0,0);
		v[5] = _s*_z;

		c[4] = glm::vec3(0,0,1);
		c[5] = glm::vec3(0,0,1);
		ref->vbuffer.Unlock();
		ref->cbuffer.Unlock();

		helpers.push_back(ref);
		return ref;
	}
	//--------------------------------------------------------------------------
	Helper::Ptr HelperManager::CreateBound(	const BBox& _b,
											const glm::mat4& _t)
	{
		glm::vec3 c0,c1,c2,c3,c4,c5,c6,c7;
		_b.ToCorners(c0,c1,c2,c3,c4,c5,c6,c7);

		Helper::Ptr ref = Helper::Create();
		ref->vbuffer.Allocate(24,GL_STATIC_DRAW);
		ref->cbuffer.Allocate(24,GL_STATIC_DRAW);
		ref->type  = GL_LINES;
		ref->transform = _t;
		
		glm::vec3* v = ref->vbuffer.Lock();
		v[0] = c0;
		v[1] = c1;
		v[2] = c1;
		v[3] = c2;
		v[4] = c2;
		v[5] = c3;
		v[6] = c3;
		v[7] = c0;

		v[8] = c4;
		v[9] = c5;
		v[10] = c5;
		v[11] = c6;
		v[12] = c6;
		v[13] = c7;
		v[14] = c7;
		v[15] = c4;

		v[16] = c0;
		v[17] = c4;
		v[18] = c1;
		v[19] = c5;
		v[20] = c2;
		v[21] = c6;
		v[22] = c3;
		v[23] = c7;
		ref->vbuffer.Unlock();

		glm::vec3* c = ref->cbuffer.Lock();
		for(int i=0;i<24;++i)
			c[i] = glm::vec3(1,1,0);
		ref->cbuffer.Unlock();

		helpers.push_back(ref);
		return ref;
	}
	//--------------------------------------------------------------------------
	Helper::Ptr HelperManager::CreateBound(	const glm::vec3& c0,
											const glm::vec3& c1,
											const glm::vec3& c2,
											const glm::vec3& c3,
											const glm::vec3& c4,
											const glm::vec3& c5,
											const glm::vec3& c6,
											const glm::vec3& c7,
											const glm::mat4& _t,
											const glm::vec3& _color)
	{
		Helper::Ptr ref = Helper::Create();
		ref->vbuffer.Allocate(24,GL_STATIC_DRAW);
		ref->cbuffer.Allocate(24,GL_STATIC_DRAW);
		ref->type  = GL_LINES;
		ref->transform = _t;
		
		glm::vec3* v = ref->vbuffer.Lock();
		v[0] = c0;
		v[1] = c1;
		v[2] = c1;
		v[3] = c2;
		v[4] = c2;
		v[5] = c3;
		v[6] = c3;
		v[7] = c0;

		v[8] = c4;
		v[9] = c5;
		v[10] = c5;
		v[11] = c6;
		v[12] = c6;
		v[13] = c7;
		v[14] = c7;
		v[15] = c4;

		v[16] = c0;
		v[17] = c4;
		v[18] = c1;
		v[19] = c5;
		v[20] = c2;
		v[21] = c6;
		v[22] = c3;
		v[23] = c7;
		ref->vbuffer.Unlock();

		glm::vec3* c = ref->cbuffer.Lock();
		for(int i=0;i<24;++i)
			c[i] = _color;
		ref->cbuffer.Unlock();

		helpers.push_back(ref);
		return ref;
	}
	//--------------------------------------------------------------------------
	Helper::Ptr HelperManager::CreateBound(	const glm::vec3& c0,
											const glm::vec3& c1,
											const glm::vec3& c2,
											const glm::vec3& c3,
											const glm::mat4& _t)
	{
		Helper::Ptr ref = Helper::Create();
		ref->vbuffer.Allocate(8,GL_STATIC_DRAW);
		ref->cbuffer.Allocate(8,GL_STATIC_DRAW);
		ref->type      = GL_LINES;
		ref->transform = _t;
		
		glm::vec3* v = ref->vbuffer.Lock();
		v[0] = c0;
		v[1] = c1;
		v[2] = c1;
		v[3] = c2;
		v[4] = c2;
		v[5] = c3;
		v[6] = c3;
		v[7] = c0;
		ref->vbuffer.Unlock();

		glm::vec3* c = ref->cbuffer.Lock();
		for(int i=0;i<8;++i)
			c[i] = glm::vec3(1,0,1);
		ref->cbuffer.Unlock();

		helpers.push_back(ref);
		return ref;
	}
	//--------------------------------------------------------------------------
	Helper::Ptr HelperManager::CreateTangentSpace(	VertexBuffer3F& _pvbo,
													VertexBuffer3F& _nvbo,
													VertexBuffer4F& _tvbo,
													IndexBuffer& _ibo,
													int _startIndex,
													int _countIndex,
													float _vectorSize)
	{
		Helper::Ptr ref = Helper::Create();
		ref->vbuffer.Allocate(6*_countIndex,GL_STATIC_DRAW);
		ref->cbuffer.Allocate(6*_countIndex,GL_STATIC_DRAW);
		ref->type  = GL_LINES;
		ref->transform = glm::mat4(1.f);

		glm::vec3* hvertices = ref->vbuffer.Lock();
		glm::vec3* hcolors   = ref->cbuffer.Lock();

		glm::vec3* vertices = _pvbo.Lock();
		glm::vec4* tangents = _tvbo.Lock();
		glm::vec3* normals  = _nvbo.Lock();
		unsigned int* indices= _ibo.Lock();
		int current = 0;
		for(int i=_startIndex;i<_startIndex+_countIndex;++i)
		{
			int index = indices[i];

			// Tangent
			hvertices[current+0] = vertices[index];
			hvertices[current+1] = vertices[index] + glm::vec3(tangents[index]) * _vectorSize;

			// Bitangent
			glm::vec3 bitangent = glm::cross(glm::vec3(tangents[index]),normals[index]) * glm::sign(tangents[index].w);
			hvertices[current+2] = vertices[index];
			hvertices[current+3] = vertices[index] + bitangent * _vectorSize;

			// Normal
			hvertices[current+4] = vertices[index];
			hvertices[current+5] = vertices[index] + normals[index] * _vectorSize;

			hcolors[current+0] = glm::vec3(1.f,0.f,0.f);
			hcolors[current+1] = glm::vec3(1.f,0.f,0.f);
			hcolors[current+2] = glm::vec3(0.f,1.f,0.f);
			hcolors[current+3] = glm::vec3(0.f,1.f,0.f);
			hcolors[current+4] = glm::vec3(0.f,0.f,1.f);
			hcolors[current+5] = glm::vec3(0.f,0.f,1.f);

			current += 6;
		}
		_ibo.Unlock();
		_nvbo.Unlock();
		_tvbo.Unlock();
		_pvbo.Unlock();

		ref->cbuffer.Unlock();
		ref->vbuffer.Unlock();

		helpers.push_back(ref);
		return ref;
	}
}
