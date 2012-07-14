//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cassert>
#include <cstring>
#include <sstream>

namespace glf
{
	//-------------------------------------------------------------------------
	template<GLenum B, typename T>
	IBuffer<B,T>::IBuffer(int _count, GLenum _update):
	BufferType(B),
	update(_update),
	count(_count),
	lock(false)
	{
		glGenBuffers(1,&id);
		glBindBuffer(B,id);
		glBindBuffer(B,0);

		Allocate(count, update);
	}
	//-------------------------------------------------------------------------
	template<GLenum B, typename T>
	IBuffer<B,T>::IBuffer():
	BufferType(B),
	update(GL_DYNAMIC_DRAW),
	count(0),
	lock(false)
	{
		glGenBuffers(1,&id);
		glBindBuffer(B,id);
		glBindBuffer(B,0);
	}
	//-------------------------------------------------------------------------
	template<GLenum B, typename T>
	void IBuffer<B,T>::Allocate(int _count)
	{
		Allocate(_count, update);
	}
	//-------------------------------------------------------------------------
	template<GLenum B, typename T>
	void IBuffer<B,T>::Allocate(int _count, GLenum _update)
	{
		assert(_count>0);
		count = _count;
		update= _update;

		glBindBuffer(B,id);
		glBufferData(B,count*sizeof(T),NULL,update);

		glf::CheckError("Buffer::Allocate");
	}
	//-------------------------------------------------------------------------
	template<GLenum B, typename T>
	IBuffer<B,T>::~IBuffer() 
	{
		glDeleteBuffers(1,&id);
	}
	//------------------------------------------------------------------------
	template<GLenum B, typename T>
	T* IBuffer<B,T>::Lock(GLuint _access)
	{
		assert(!lock);

		glBindBuffer(B,id);
		void* p = glMapBuffer(B,_access);
		assert(p!=NULL);
		lock = true;

		glf::CheckError("Buffer::Lock");
		return (T*)p;
	}
	//-------------------------------------------------------------------------
	template<GLenum B, typename T>
	void IBuffer<B,T>::Unlock()
	{
		assert(lock);

		glBindBuffer(B,id);
		glUnmapBuffer(B);
		glf::CheckError("Buffer::Unlock");
		lock = false;
	}
	//-------------------------------------------------------------------------
	template<GLenum B, typename T>
	void IBuffer<B,T>::Fill(T* _data, int _count)
	{	
		assert(!lock);

		assert(_count<=count);
		glBindBuffer(B,id);
		glBufferData(B,_count*sizeof(T),(void*)_data,update);
	}
	//-------------------------------------------------------------------------
	template<typename T>
	void VertexArray::Add(		//typename const VertexBuffer<T>::Buffer& _buffer,
								const T& 	_buffer,
								GLint    	_location, 
								int      	_nComponents,
								GLenum   	_componentType,
								bool     	_normalize,
								int			_offset)
	{
		glBindVertexArray(id);
			glBindBuffer(GL_ARRAY_BUFFER, _buffer.id);
				glVertexAttribPointer(	_location, 
										_nComponents, 
										_componentType, 
										_normalize, 
										sizeof(typename T::DataType), 
										GLF_BUFFER_OFFSET(_offset));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glEnableVertexAttribArray(_location);
		glBindVertexArray(0);

		assert(glf::CheckError("VertexArray::Add"));
	}
	//-------------------------------------------------------------------------
}

