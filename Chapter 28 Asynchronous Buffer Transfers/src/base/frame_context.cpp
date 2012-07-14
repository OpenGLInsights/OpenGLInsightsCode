/*
Copyright (C) 2011 by Ladislav Hrabcak

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "frame_context.h"
#include "base/base.h"
#include "base/pixelfmt.h"

#include "canvas.h"
#include "../scene.h"

#include <gl/glew.h>

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const int base::frame_context::POOL_SIZE = 4;

bool __buffers_created = false;
GLuint __scene_buffer = 0;
GLuint __canvas_buffer = 0;

void base::frame_context::create_buffers()
{
	GLuint tmp[3];

	first = true;

	if(!__buffers_created) {
		glGenBuffers(2, tmp);
		_canvas_vbo = __canvas_buffer = tmp[0];
		_scene_vbo = __scene_buffer = tmp[1];
		if(_mode == ModeBufferData)
			__buffers_created = true;
	}
	else {
		_canvas_vbo = __canvas_buffer;
		_scene_vbo = __scene_buffer;
	}

	glGenBuffers(1, tmp);
	_test_vbo = tmp[0];

	glGenTextures(2, tmp);
	_canvas_tb = tmp[0];
	_scene_tb = tmp[1];

	// canvas
	glBindBuffer(GL_TEXTURE_BUFFER, _canvas_vbo);
	glBufferData(
		GL_TEXTURE_BUFFER,
		base::canvas::ELEMENTS_VBO_SIZE,
		0,
		GL_STREAM_DRAW);
			
	//scene
	glBindBuffer(GL_TEXTURE_BUFFER, _scene_vbo);
	glBufferData(
		GL_TEXTURE_BUFFER,
		scene::MAX_BLOCK_COUNT * sizeof(glm::mat4),
		0,
		GL_STREAM_DRAW);

	// big vbo for testing
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _test_vbo);
	glBufferData(
		GL_PIXEL_UNPACK_BUFFER,
		TEST_VBO_SIZE,
		0,
		GL_STREAM_DRAW);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	glBindTexture(GL_TEXTURE_BUFFER, _canvas_tb);
	glTexBuffer(
		GL_TEXTURE_BUFFER,
		base::get_pfd(base::PF_RGBA32F)->_internal,
		_canvas_vbo);

	glBindTexture(GL_TEXTURE_BUFFER, _scene_tb);
	glTexBuffer(
		GL_TEXTURE_BUFFER,
		base::get_pfd(base::PF_RGBA32F)->_internal,
		_scene_vbo);

	glBindTexture(GL_TEXTURE_BUFFER, 0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::frame_context::frame_context()
	: _scene_data_ptr(0)
	, _scene_data_ptr_size(0)
	, _elements(0)
	, _elements_begin(0)
	, _fence(0)
	, _mode(ModeWrite)
{}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::frame_context::assign_buffer_range(const int) {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::frame_context::map_scene()
{
	glBindBuffer(GL_TEXTURE_BUFFER, _scene_vbo);

	if(_mode == ModeInvalidateBuffer) {
		_scene_data_ptr = reinterpret_cast<glm::mat4*>(
			glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
				scene::MAX_BLOCK_COUNT * sizeof(glm::mat4),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	}
	else if(_mode == ModeFlushExplicit) {
		_scene_data_ptr = reinterpret_cast<glm::mat4*>(
			glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
				scene::MAX_BLOCK_COUNT * sizeof(glm::mat4),
				GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT));
	}
	else if(_mode == ModeUnsynchronized) {
		_scene_data_ptr = reinterpret_cast<glm::mat4*>(
			glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
				scene::MAX_BLOCK_COUNT * sizeof(glm::mat4),
				GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT));
	}
	else if(_mode == ModeWrite) {
		_scene_data_ptr = reinterpret_cast<glm::mat4*>(
			glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
				scene::MAX_BLOCK_COUNT * sizeof(glm::mat4),
				GL_MAP_WRITE_BIT));
	}
	else if(_mode == ModeBufferData || _mode == ModeBufferSubData) {
		if(!_scene_data_ptr)
			_scene_data_ptr = new glm::mat4[scene::MAX_BLOCK_COUNT];
	}

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	_scene_data_ptr_size = 0;

	assert(_scene_data_ptr != 0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::frame_context::map_canvas()
{
	glBindBuffer(GL_TEXTURE_BUFFER, _canvas_vbo);

	if(_mode == ModeInvalidateBuffer) {
		_elements_begin = _elements = 
			reinterpret_cast<glm::vec4*>(glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
				base::canvas::ELEMENTS_VBO_SIZE,
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
	}
	else if(_mode == ModeFlushExplicit) {
		_elements_begin = _elements = 
			reinterpret_cast<glm::vec4*>(glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
				base::canvas::ELEMENTS_VBO_SIZE,
				GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT));
	}
	else if(_mode == ModeUnsynchronized) {
		_elements_begin = _elements = 
			reinterpret_cast<glm::vec4*>(glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
				base::canvas::ELEMENTS_VBO_SIZE,
				GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT));
	}
	else if(_mode == ModeWrite) {
		_elements_begin = _elements = 
			reinterpret_cast<glm::vec4*>(glMapBufferRange(
				GL_TEXTURE_BUFFER, 
				0, 
				base::canvas::ELEMENTS_VBO_SIZE,
				GL_MAP_WRITE_BIT));
	}
	else if(_mode == ModeBufferData || _mode == ModeBufferSubData) {
		if(!_elements_begin)
			_elements_begin = new glm::vec4[base::canvas::ELEMENTS_VBO_SIZE];
		_elements = _elements_begin;
	}

	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	assert(_elements_begin != 0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::frame_context::flush_scene_data()
{
	glBindBuffer(GL_TEXTURE_BUFFER, _scene_vbo);
	if(_mode != ModeBufferData && _mode != ModeBufferSubData) {
		if(_mode == ModeFlushExplicit && _scene_data_ptr_size > 0) {
			glFlushMappedBufferRange(GL_TEXTURE_BUFFER, 0, _scene_data_ptr_size);
		}
		glUnmapBuffer(GL_TEXTURE_BUFFER);
	}
	else if(_mode == ModeBufferData) {
		glBufferData(GL_TEXTURE_BUFFER, _scene_data_ptr_size, 0, GL_STREAM_DRAW);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, _scene_data_ptr_size, _scene_data_ptr);
	}
	else if(_mode == ModeBufferSubData) {
		glBufferSubData(GL_TEXTURE_BUFFER, 0, _scene_data_ptr_size, _scene_data_ptr);
	}
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::frame_context::flush_canvas_data()
{
	const int size = (_elements - _elements_begin) * sizeof(glm::vec4) * 3;

	glBindBuffer(GL_TEXTURE_BUFFER, _canvas_vbo);
	if(_mode != ModeBufferData && _mode != ModeBufferSubData) {
		if(_mode == ModeFlushExplicit && _elements - _elements_begin > 0) {
			glFlushMappedBufferRange(GL_TEXTURE_BUFFER, 0, size);
		}
		glUnmapBuffer(GL_TEXTURE_BUFFER);
	}
	else if(_mode == ModeBufferData) {
		glBufferData(GL_TEXTURE_BUFFER, size, 0, GL_STREAM_DRAW);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, size, _elements_begin);
	}
	else if(_mode == ModeBufferSubData) {
		glBufferSubData(GL_TEXTURE_BUFFER, 0, size, _elements_begin);
	}
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::frame_context::test_transfer()
{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _test_vbo);
	_test_vbo_ptr = reinterpret_cast<char*>(glMapBufferRange(
		GL_PIXEL_UNPACK_BUFFER,
		0,
		TEST_VBO_SIZE,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT));

	printf("test ptr: %p\n", _test_vbo_ptr);
	memset(_test_vbo_ptr, 0xCD, TEST_VBO_SIZE);

	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::frame_context::put_fence()
{
	if(_fence) glDeleteSync(_fence);

	_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bool base::frame_context::check_fence()
{
	if(_fence==0) return true;

	GLenum res = glClientWaitSync(_fence, 0, 500000);
	
	// AMD returns GL_CONDITION_SATISFIED only BUG?
	return res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
