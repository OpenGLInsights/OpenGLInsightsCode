#ifndef GLF_HELPER_HPP
#define GLF_HELPER_HPP

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/utils.hpp>
#include <glf/bound.hpp>
#include <glf/memory.hpp>
#include <glf/buffer.hpp>
#include <glf/wrapper.hpp>
#include <vector>

namespace glf
{
	//--------------------------------------------------------------------------
	class Helper
	{
	public:
		typedef SmartPointer<Helper> Ptr;
		VertexBuffer3F	vbuffer;
		VertexBuffer3F	cbuffer;
		VertexArray		vao;
		glm::mat4 		transform;
		GLenum			type;
	private:
						Helper();
	public:
						~Helper();
		static Ptr		Create();
	};
	//--------------------------------------------------------------------------
	class HelperManager
	{
	public:
		typedef SmartPointer<HelperManager> Ptr;
		static Ptr		Create(				);
						HelperManager(		);
						~HelperManager(		);
		Helper::Ptr 	CreatePointSet(		const std::vector<glm::vec3>& _points,
											const glm::mat4& _t=glm::mat4(1.f));
		Helper::Ptr 	CreateReferential(	float _s=1.f, 
											const glm::mat4& _t=glm::mat4(1.f));
		Helper::Ptr 	CreateReferential(	const glm::vec3& _x, 
											const glm::vec3& _y, 
											const glm::vec3& _z, 
											float _s=1.f,
											const glm::mat4& _t=glm::mat4(1.f));
		Helper::Ptr 	CreateBound(		const BBox& _b,
											const glm::mat4& _t=glm::mat4(1.f));
		Helper::Ptr 	CreateBound(		const glm::vec3& c0,
											const glm::vec3& c1,
											const glm::vec3& c2,
											const glm::vec3& c3,
											const glm::vec3& c4,
											const glm::vec3& c5,
											const glm::vec3& c6,
											const glm::vec3& c7,
											const glm::mat4& _t=glm::mat4(1.f),
											const glm::vec3& _color=glm::vec3(1,1,0));
		Helper::Ptr 	CreateBound(		const glm::vec3& c0,
											const glm::vec3& c1,
											const glm::vec3& c2,
											const glm::vec3& c3,
											const glm::mat4& _t=glm::mat4(1.f));
		Helper::Ptr CreateTangentSpace(		VertexBuffer3F& _pvbo,
											VertexBuffer3F& _nvbo,
											VertexBuffer4F& _tvbo,
											IndexBuffer& _ibo,
											int _startIndex,
											int _countIndex,
											float _vectorSize);
		void 			Clear();

		std::vector<Helper::Ptr> helpers;
	};
	//--------------------------------------------------------------------------
	struct HelperRenderer
	{
	public:
		Program 		program;
		GLint			transformVar;
		GLint			modelVar;
		GLint			vbufferVar;
		GLint			cbufferVar;
	public:
		void 			Draw(	const glm::mat4& _projection,
								const glm::mat4& _view,
								const std::vector<Helper::Ptr>& _helpers);
						~HelperRenderer();
						HelperRenderer();
	private:
						HelperRenderer(const HelperRenderer&);
						HelperRenderer& operator=(const HelperRenderer&);
	
	};
}
#endif

