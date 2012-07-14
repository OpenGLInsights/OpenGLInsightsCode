#ifndef GLF_GEOMETRY_HPP
#define GLF_GEOMETRY_HPP

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/utils.hpp>
#include <glf/buffer.hpp>

namespace glf
{
	void 	CreateCubePos(		glf::VertexBuffer3F& _cube);
	void 	CreateCubeTex(		glf::VertexBuffer3F& _cube);
	void	CreateCube(			glf::VertexBuffer3F& _cube);
	void	CreateQuad(			glf::VertexBuffer3F& _quad);
	void	CreateQuad(			glf::VertexBuffer4F& _quad);
	void	CreateQuad(			glf::IndexBuffer& _quad);
	void	CreateScreenTriangle(glf::VertexBuffer2F& _triangle);
}
#endif

