//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/buffer.hpp>

namespace glf
{
	//--------------------------------------------------------------------------
	namespace semantic
	{
		GLint Position 	= 0;
		GLint Normal 	= 1;
		GLint TexCoord 	= 2;
		GLint Tangent 	= 3;
		GLint Color	 	= 4;
		GLint Bitangent	= 5;
	};
	//--------------------------------------------------------------------------
	VertexArray::VertexArray()
	{
		glGenVertexArrays(1, &id);
	}
	//--------------------------------------------------------------------------
	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &id);
	}
	//--------------------------------------------------------------------------
	void VertexArray::Draw( 	GLenum _primitiveType,
								const IndexBuffer& _buffer) const
	{
		glBindVertexArray(id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer.id);
		glDrawElements(_primitiveType, _buffer.count, GL_UNSIGNED_INT, GLF_BUFFER_OFFSET(0) );
		glBindVertexArray(0);
	}
	//--------------------------------------------------------------------------
	void VertexArray::Draw( 	GLenum _primitiveType,
								const IndexBuffer& _buffer,
								int _count,
								int _first) const
	{
		glBindVertexArray(id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer.id);
		glDrawElements(_primitiveType, _count, GL_UNSIGNED_INT, GLF_BUFFER_OFFSET(_first*sizeof(unsigned int)) );
		glBindVertexArray(0);
	}
	//--------------------------------------------------------------------------
	void VertexArray::Draw(		GLenum _primitiveType, 
								int _count,
								int _first) const
	{
		glBindVertexArray(id);
		glDrawArrays(_primitiveType, _first, _count);
		glBindVertexArray(0);
	}
	//--------------------------------------------------------------------------
	void VertexArray::Draw(		GLenum _primitiveType, 
								int _count,
								int _first,
								int _primCount) const
	{
		glBindVertexArray(id);
		glDrawArraysInstanced(_primitiveType,_first,_count,_primCount);
		glBindVertexArray(0);
	}
	//--------------------------------------------------------------------------
	void VertexArray::Draw(		GLenum _primitiveType, 
								const IndirectArrayBuffer& _indirectBuffer) const
	{
		glBindVertexArray(id);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER,_indirectBuffer.id);
			glDrawArraysIndirect(_primitiveType,NULL);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER,0);
		glBindVertexArray(0);
	}
	//--------------------------------------------------------------------------
}

