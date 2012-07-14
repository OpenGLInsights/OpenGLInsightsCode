//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/geometry.hpp>

namespace glf
{
	/*
	//--------------------------------------------------------------------------
	void CreateInnerCubePos(	glf::VertexBuffer<glm::vec3>::Buffer& _cube)
	{
		_cube.Resize(24);
		glm::vec3* vertices = _cube.Lock();

		// +X : Right
		vertices[0]	    	= glm::vec3( 1.f, 1.f,-1.f);
		vertices[1]	    	= glm::vec3( 1.f,-1.f,-1.f);
		vertices[2] 		= glm::vec3( 1.f, 1.f, 1.f);
		vertices[3]	    	= glm::vec3( 1.f,-1.f, 1.f);

		// -X : Left
		vertices[4]	    	= glm::vec3(-1.f,-1.f,-1.f);
		vertices[5]	    	= glm::vec3(-1.f, 1.f,-1.f);
		vertices[6] 		= glm::vec3(-1.f, 1.f, 1.f);
		vertices[7]	    	= glm::vec3(-1.f,-1.f, 1.f);

		// +Y : Back
		vertices[8]			= glm::vec3(-1.f, 1.f,-1.f);
		vertices[9]			= glm::vec3( 1.f, 1.f,-1.f);
		vertices[10] 		= glm::vec3( 1.f, 1.f, 1.f);
		vertices[11]	    = glm::vec3(-1.f, 1.f, 1.f);

		// -Y : Front
		vertices[12]	    = glm::vec3( 1.f,-1.f,-1.f);
		vertices[13]	    = glm::vec3(-1.f,-1.f,-1.f);
		vertices[14]	    = glm::vec3(-1.f,-1.f, 1.f);
		vertices[15] 		= glm::vec3( 1.f,-1.f, 1.f);

		// +Z : Top
		vertices[16]	    = glm::vec3(-1.f, 1.f, 1.f);
		vertices[17] 		= glm::vec3( 1.f, 1.f, 1.f);
		vertices[18]		= glm::vec3( 1.f,-1.f, 1.f);
		vertices[19]		= glm::vec3(-1.f,-1.f, 1.f);

		// -Z : Bottom
		vertices[20]		= glm::vec3(-1.f,-1.f,-1.f);
		vertices[21]		= glm::vec3( 1.f,-1.f,-1.f);
		vertices[22] 		= glm::vec3( 1.f, 1.f,-1.f);
		vertices[23]		= glm::vec3(-1.f, 1.f,-1.f);

		_cube.Unlock();
	}
	//--------------------------------------------------------------------------
	void CreateInnerCubeTex(	glf::VertexBuffer<glm::vec3>::Buffer& _cube)
	{
		_cube.Resize(24);
		glm::vec3* vertices = _cube.Lock();

		// +X : Right
		vertices[0]	    	= glm::vec3(0.f,0.f,0.f);
		vertices[1]	    	= glm::vec3(1.f,0.f,0.f);
		vertices[2] 		= glm::vec3(1.f,1.f,0.f);
		vertices[3]	    	= glm::vec3(0.f,1.f,0.f);

		// -X : Left	
		vertices[4] 	   	= glm::vec3(0.f,0.f,0.f);
		vertices[5] 	   	= glm::vec3(1.f,0.f,0.f);
		vertices[6] 		= glm::vec3(1.f,1.f,0.f);
		vertices[7] 	   	= glm::vec3(0.f,1.f,0.f);

		// +Y : Back
		vertices[8]	    	= glm::vec3(0.f,0.f,0.f);
		vertices[9] 	   	= glm::vec3(1.f,0.f,0.f);
		vertices[10] 		= glm::vec3(1.f,1.f,0.f);
		vertices[11]    	= glm::vec3(0.f,1.f,0.f);

		// -Y : Front
		vertices[12]    	= glm::vec3(0.f,0.f,0.f);
		vertices[13]    	= glm::vec3(1.f,0.f,0.f);
		vertices[14] 		= glm::vec3(1.f,1.f,0.f);
		vertices[15]    	= glm::vec3(0.f,1.f,0.f);

		// +Z : Top
		vertices[16]    	= glm::vec3(0.f,0.f,0.f);
		vertices[17]    	= glm::vec3(1.f,0.f,0.f);
		vertices[18] 		= glm::vec3(1.f,1.f,0.f);
		vertices[19]    	= glm::vec3(0.f,1.f,0.f);

		// -Z : Bottom
		vertices[20]    	= glm::vec3(0.f,0.f,0.f);
		vertices[21]    	= glm::vec3(1.f,0.f,0.f);
		vertices[22] 		= glm::vec3(1.f,1.f,0.f);
		vertices[23]    	= glm::vec3(0.f,1.f,0.f);
	}
	*/
	//--------------------------------------------------------------------------
	void CreateCubePos(	glf::VertexBuffer3F& _cube)
	{
		_cube.Allocate(36,GL_STATIC_DRAW);
		glm::vec3* vertices = _cube.Lock();

		// +X : Right
		vertices[0]	    	= glm::vec3(1.f,-1.f,-1.f);
		vertices[1]	    	= glm::vec3(1.f, 1.f,-1.f);
		vertices[2] 		= glm::vec3(1.f, 1.f, 1.f);
		vertices[3]	    	= glm::vec3(1.f,-1.f,-1.f);
		vertices[4] 		= glm::vec3(1.f, 1.f, 1.f);
		vertices[5]		    = glm::vec3(1.f,-1.f, 1.f);

		// -X : Left
		vertices[6]	    	= glm::vec3(-1.f,-1.f,-1.f);
		vertices[7] 		= glm::vec3(-1.f, 1.f, 1.f);
		vertices[8]	    	= glm::vec3(-1.f, 1.f,-1.f);
		vertices[9]			= glm::vec3(-1.f,-1.f,-1.f);
		vertices[10]	    = glm::vec3(-1.f,-1.f, 1.f);
		vertices[11] 		= glm::vec3(-1.f, 1.f, 1.f);

		// +Y : Back
		vertices[12]		= glm::vec3(-1.f, 1.f,-1.f);
		vertices[13] 		= glm::vec3( 1.f, 1.f, 1.f);
		vertices[14]		= glm::vec3( 1.f, 1.f,-1.f);
		vertices[15]		= glm::vec3(-1.f, 1.f,-1.f);
		vertices[16]	    = glm::vec3(-1.f, 1.f, 1.f);
		vertices[17] 		= glm::vec3( 1.f, 1.f, 1.f);

		// -Y : Front
		vertices[18]	    = glm::vec3(-1.f,-1.f,-1.f);
		vertices[19]	    = glm::vec3( 1.f,-1.f,-1.f);
		vertices[20] 		= glm::vec3( 1.f,-1.f, 1.f);
		vertices[21]	    = glm::vec3(-1.f,-1.f,-1.f);
		vertices[22] 		= glm::vec3( 1.f,-1.f, 1.f);
		vertices[23]	    = glm::vec3(-1.f,-1.f, 1.f);

		// +Z : Top
		vertices[24]		= glm::vec3(-1.f,-1.f, 1.f);
		vertices[25]		= glm::vec3( 1.f,-1.f, 1.f);
		vertices[26] 		= glm::vec3( 1.f, 1.f, 1.f);
		vertices[27]		= glm::vec3(-1.f,-1.f, 1.f);
		vertices[28] 		= glm::vec3( 1.f, 1.f, 1.f);
		vertices[29]	    = glm::vec3(-1.f, 1.f, 1.f);

		// -Z : Bottom
		vertices[30]		= glm::vec3(-1.f,-1.f,-1.f);
		vertices[31] 		= glm::vec3( 1.f, 1.f,-1.f);
		vertices[32]		= glm::vec3( 1.f,-1.f,-1.f);
		vertices[33]		= glm::vec3(-1.f,-1.f,-1.f);
		vertices[34]		= glm::vec3(-1.f, 1.f,-1.f);
		vertices[35] 		= glm::vec3( 1.f, 1.f,-1.f);

		_cube.Unlock();
	}
	//--------------------------------------------------------------------------
	void CreateCubeTex(	glf::VertexBuffer3F& _cube)
	{
		_cube.Allocate(36,GL_STATIC_DRAW);

		// (U,V,Layer)
		glm::vec3* vertices = _cube.Lock();

		// +X : Right
		vertices[0]	    	= glm::vec3(1.f,0.f,0.f);
		vertices[1]	    	= glm::vec3(0.f,0.f,0.f);
		vertices[2]		    = glm::vec3(0.f,1.f,0.f);
		vertices[3]	    	= glm::vec3(1.f,0.f,0.f);
		vertices[4]		    = glm::vec3(0.f,1.f,0.f);
		vertices[5] 		= glm::vec3(1.f,1.f,0.f);

		// -X : Left
		vertices[6]	    	= glm::vec3(0.f,0.f,1.f);
		vertices[7]	    	= glm::vec3(1.f,1.f,1.f);
		vertices[8]		    = glm::vec3(1.f,0.f,1.f);
		vertices[9]	    	= glm::vec3(0.f,0.f,1.f);
		vertices[10]	    = glm::vec3(0.f,1.f,1.f);
		vertices[11]		= glm::vec3(1.f,1.f,1.f);

		// +Y : Back
		vertices[12]    	= glm::vec3(0.f,0.f,2.f);
		vertices[13]    	= glm::vec3(1.f,1.f,2.f);
		vertices[14] 		= glm::vec3(1.f,0.f,2.f);
		vertices[15]    	= glm::vec3(0.f,0.f,2.f);
		vertices[16] 		= glm::vec3(0.f,1.f,2.f);
		vertices[17]	    = glm::vec3(1.f,1.f,2.f);

		// -Y : Front
		vertices[18]	   	= glm::vec3(1.f,0.f,3.f);
		vertices[19]	   	= glm::vec3(0.f,0.f,3.f);
		vertices[20]	   	= glm::vec3(0.f,1.f,3.f);
		vertices[21]	   	= glm::vec3(1.f,0.f,3.f);
		vertices[22]	   	= glm::vec3(0.f,1.f,3.f);
		vertices[23]	   	= glm::vec3(1.f,1.f,3.f);

		// +Z : Top
		vertices[24]	   	= glm::vec3(0.f,1.f,4.f);
		vertices[25]	   	= glm::vec3(1.f,1.f,4.f);
		vertices[26] 		= glm::vec3(1.f,0.f,4.f);
		vertices[27]	   	= glm::vec3(0.f,1.f,4.f);
		vertices[28] 		= glm::vec3(1.f,0.f,4.f);
		vertices[29]	    = glm::vec3(0.f,0.f,4.f);

		// -Z : Bottom
		vertices[30]    	= glm::vec3(0.f,0.f,5.f);
		vertices[31]    	= glm::vec3(1.f,1.f,5.f);
		vertices[32] 		= glm::vec3(1.f,0.f,5.f);
		vertices[33]    	= glm::vec3(0.f,0.f,5.f);
		vertices[34] 		= glm::vec3(0.f,1.f,5.f);
		vertices[35]	    = glm::vec3(1.f,1.f,5.f);
		_cube.Unlock();
	}
	//--------------------------------------------------------------------------
	void CreateQuad(	glf::VertexBuffer3F& _quad)
	{
		_quad.Allocate(6,GL_STATIC_DRAW);
		glm::vec3* vertices = _quad.Lock();
		vertices[0] = glm::vec3(0,0,0);
		vertices[1] = glm::vec3(1,0,0);
		vertices[2] = glm::vec3(1,1,0);
		vertices[3] = glm::vec3(0,0,0);
		vertices[4] = glm::vec3(1,1,0);
		vertices[5] = glm::vec3(0,1,0);
		_quad.Unlock();
	}
	//--------------------------------------------------------------------------
	void CreateQuad(	glf::VertexBuffer4F& _quad)
	{
		_quad.Allocate(6,GL_STATIC_DRAW);
		glm::vec4* vertices = _quad.Lock();
		vertices[0] = glm::vec4(0,0,0,1);
		vertices[1] = glm::vec4(1,0,0,1);
		vertices[2] = glm::vec4(1,1,0,1);
		vertices[3] = glm::vec4(0,0,0,1);
		vertices[4] = glm::vec4(1,1,0,1);
		vertices[5] = glm::vec4(0,1,0,1);
		_quad.Unlock();
	}
	//--------------------------------------------------------------------------
	void CreateScreenTriangle(glf::VertexBuffer2F& _triangle)
	{
		_triangle.Allocate(3,GL_STATIC_DRAW);
		glm::vec2* vertices = _triangle.Lock();
		vertices[0] = glm::vec2(-2,-2);
		vertices[1] = glm::vec2(4,0);
		vertices[2] = glm::vec2(0,4);
		_triangle.Unlock();
	}
}
