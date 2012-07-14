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

#include "app_bw.h"

#include "base/base.h"
#include "base/frame_context.h"
#include "base/canvas.h"

#include <glm/gtc/matrix_transform.hpp>

GLuint buffer = 0;
GLuint texture = 0;
GLuint queries[3] = { 0, };

const base::pixelfmt pf = base::PF_RGBA8;
const int texture_size = 4096;
const base::pfd *pfdesc = base::get_pfd(pf);
const int buffer_size =  
	!pfdesc->_compressed ? 
		texture_size * texture_size * pfdesc->_size
		: (texture_size >> 2) * pfdesc->_size * (texture_size >> 2);
const int buffer_size_mb = buffer_size / 0x100000;
	
const int target = GL_PIXEL_UNPACK_BUFFER;
const int usage = GL_STREAM_DRAW;

char *ptr_data = new char[buffer_size];

#define GL_EXTERNAL_VIRTUAL_MEMORY_AMD 37216

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

app_bw::app_bw() : app() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

app_bw::~app_bw() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void app_bw::start(const bool)
{
	_frame_context.reset(new base::frame_context());
	_frame_context->create_buffers();

	// set projection matrix
	app::create_perspective_matrix(_frame_context.get());
	
	base::canvas::load_and_init_shaders(SRC_LOCATION);

	// init test stuff
	memset(ptr_data, 0x44, buffer_size);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	texture = create_texture(texture_size, texture_size, pf, 0);

	glGenQueries(3, queries);
	glGenBuffers(1, &buffer);

	glBindBuffer(target, buffer);
	glBufferData(target, buffer_size, 0, usage);
	glBindBuffer(target, 0);

	glFinish();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void app_bw::test_buffer_bandwidth(const int method, const bool log_to_screen)
{
	_timer.start();

	glQueryCounter(queries[0], GL_TIMESTAMP);

	glBindBuffer(target, buffer);

	if(method == 0) {
		char *ptr = (char*)glMapBufferRange(
			target,
			0,
			buffer_size,
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		memset(ptr, 0x44, buffer_size);

		glUnmapBuffer(target);
	}
	else if(method == 1) {
		char *ptr = (char*)glMapBuffer(target, GL_WRITE_ONLY);

		memset(ptr, 0x44, buffer_size);

		glUnmapBuffer(target);
	}
	else {
		glBufferSubData(target, 0, buffer_size, ptr_data);
	}

	glQueryCounter(queries[1], GL_TIMESTAMP);

	glBindTexture(GL_TEXTURE_2D, texture);
	if(!pfdesc->_compressed) {
		glTexSubImage2D(
			GL_TEXTURE_2D, 
			0,
			0, 0,
			texture_size,
			texture_size,
			get_pfd(pf)->_format,
			get_pfd(pf)->_type,
			0);
	}
	else {
		glCompressedTexSubImage2D(
			GL_TEXTURE_2D, 
			0,
			0,
			0,
			texture_size,
			texture_size,
			pfdesc->_internal,
			buffer_size,
			0);
	}
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glQueryCounter(queries[2], GL_TIMESTAMP);

	__int64 result[3];

	const double cpu_time = _timer.elapsed_time();

	glGetQueryObjecti64v(queries[2], GL_QUERY_RESULT, result + 2);
	glGetQueryObjecti64v(queries[1], GL_QUERY_RESULT, result + 1);
	glGetQueryObjecti64v(queries[0], GL_QUERY_RESULT, result);

	if(log_to_screen) {
		const double time = double(result[1] - result[0]) * (1.0 / 1000000000.0);
		printf(
			"CPU -> GPU transfer time for %dMB: %.3f ms, bandwidth: %.3f MB/s CPU time: %.3f\n",
			buffer_size_mb,
			1000.0 * time,
			double(buffer_size) / double(0x100000) / time,
			cpu_time);
	}

	if(log_to_screen) {
		const double time = double(result[2] - result[1]) * (1.0 / 1000000000.0);
		printf(
			"texture update time for %dMB %s: %.3f ms, bandwidth: %.3f MB/s\n",
			buffer_size_mb,
			base::get_pfd(pf)->_txt,
			1000.0 * time,
			double(buffer_size) / double(0x100000) / time);
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void app_bw::draw_frame()
{
	static int c = 0; 

	test_buffer_bandwidth(2, true);

	if(c++ > 2)
		throw base::exception("The End...\n");
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
