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

#include "app.h"

#include "base.h"
#include "font.h"
#include "canvas.h"

#include "frame_context.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GL_EXTERNAL_VIRTUAL_MEMORY_AMD 37216

GLuint base::app::_prg = 0;
GLint base::app::_prg_mvp = -1;
GLint base::app::_prg_size = -1;
GLint base::app::_prg_tex = -1;

base::app* base::app::_app = 0;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::app::app()
	: _velocity_boost(false),_position(20, 20, 90)
	, _frame_number(0)
{ 
	assert(_app==0); _app=this; 

	// init mouse stuff
	base::set_mouse_pos(glm::ivec2(200));
	_last_mouse_pos = base::get_mouse_pos();
	_mouse_pos=glm::ivec2(0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::app::~app() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::start(const bool nogl)
{
	if(!nogl) {
		renderer_side_start();
	}

	_canvas.reset(new base::canvas());
	_canvas->set_pos(100, 100);
	_canvas->set_size(220, 38);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::renderer_side_start()
{
	_fnt_mono.reset(new base::font());
	_fnt_mono->load(SRC_LOCATION,"fonts/lucida_console_13.fnt");

	_fnt_arial.reset(new base::font());
	_fnt_arial->load(SRC_LOCATION,"fonts/arial_14.fnt");
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::begin_frame(base::frame_context*, const bool nogl)
{
	if(!nogl) {
		//glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.3f, 0.2f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	if(_active) {
		_mouse_pos += glm::vec2(base::get_mouse_pos() - _last_mouse_pos) * 0.2f;
		base::set_mouse_pos(glm::ivec2(200));
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::create_perspective_matrix(frame_context *fc)
{
	fc->_mprj=glm::perspective(
		45.0f,
		float(get_wnd_width()) / float(get_wnd_height()),
		0.1f,
		500.0f);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::update_camera(frame_context *fc)
{
	fc->_view = glm::rotate(
		glm::rotate(
			glm::mat4(1),
			-_mouse_pos.x,
			glm::vec3(0,1,0)),
		-_mouse_pos.y, glm::vec3(1,0,0));

	_position += glm::mat3(fc->_view) * _velocity * (_velocity_boost ? 3.0f : 1.0f);

	fc->_view[3] = glm::vec4(_position, 1);
	fc->_mvp = fc->_mprj * glm::inverse(fc->_view);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::end_frame(base::frame_context *ctx, const bool nogl)
{
	_canvas->fill_rect(
		ctx,
		glm::vec2(0),
		glm::vec2(220, 38),
		glm::vec4(0.2f, 0.2f, 0.2f, 1));

	char tmp[512];
	sprintf(tmp, "vel(%.2f,%.2f) visible: %i", _velocity.x, _velocity.z, 0);

	_canvas->draw_text(
		ctx,
		glm::vec2(0),
		tmp,
		glm::vec4(1, 1, 0, 1),
		_fnt_mono.get());
	
	sprintf(tmp, "app::name: %s", get_app_name());
	
	_canvas->draw_text(
		ctx,
		glm::vec2(0, 22),
		tmp,
		glm::vec4(1, 0.3, 0.3, 1),
		_fnt_arial.get());

	if(!nogl)
		canvas::render(ctx);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::stop()
{
	_canvas.reset(0);
	_fnt_mono.reset(0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::mouse_move(const int, const int) {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::key(const int key,const bool down)
{
	const float speed = 0.05f;

	if(key == 0x57 || key == 0x26) _velocity.z += down ? -speed : speed;
	if(key == 0x53 || key == 0x28) _velocity.z += down ? speed : -speed;

	if(key == 0x41 || key == 0x25) _velocity.x += down ? -speed : speed;
	if(key == 0x44 || key == 0x27) _velocity.x += down ? speed : -speed;

	if(key == 0x52) _velocity.y += down ? speed : -speed;
	if(key == 0x46) _velocity.y += down ? -speed : speed;

	if(key == 0x10) _velocity_boost = down;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::activated(const bool state)
{
	_active=state;
	if(state)
		base::set_mouse_pos(glm::ivec2(200));
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::init_screen_capture()
{
	// pre-allocate buffers for screen capture
	const int screen_size = 
		get_wnd_width() 
			* get_wnd_height() 
			* base::get_pfd(CAPTURE_PF)->_size;
	
	screen_buffer * const e = _screen_buffers_rb + RB_BUFFERS_SIZE;
	for(screen_buffer *i = _screen_buffers_rb; i != e; ++i) {
		glGenBuffers(1, &i->_buffer);

		if(base::cfg().use_pinned_memory) {
			i->_pinned_ptr = new char[screen_size + 0x1000];
			i->_pinned_ptr_aligned = reinterpret_cast<char*>(
				unsigned(i->_pinned_ptr + 0xfff) & (~0xfff));

			glBindBuffer(GL_EXTERNAL_VIRTUAL_MEMORY_AMD, i->_buffer);
			glBufferData(
				GL_EXTERNAL_VIRTUAL_MEMORY_AMD,
				screen_size,
				i->_pinned_ptr_aligned,
				GL_STREAM_READ);
			glBindBuffer(GL_EXTERNAL_VIRTUAL_MEMORY_AMD, 0);
		}
		else {
			glBindBuffer(GL_PIXEL_PACK_BUFFER, i->_buffer);
			glBufferData(GL_PIXEL_PACK_BUFFER, screen_size, 0, GL_STREAM_READ);
		}

		glGenBuffers(1, &i->_tmp_buffer);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, i->_tmp_buffer);
		glBufferData(GL_PIXEL_PACK_BUFFER, screen_size, 0, GL_STREAM_COPY);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		i->_tmp_texture = base::create_texture(
			get_wnd_width(),
			get_wnd_height(),
			CAPTURE_PF,
			0);

		glGenQueries(3, i->_queries);

		glGenBuffers(1, &i->_showtime_buffer);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, i->_showtime_buffer);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, screen_size, 0, GL_STREAM_DRAW);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
	
	_showtime_tex = base::create_texture(
			get_wnd_width(),
			get_wnd_height(),
			CAPTURE_PF,
			0);

	// init round buffer positions
	_rb_head = _rb_tail = 0;

	// create framebuffer
	glGenFramebuffers(1, &_fb);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::process_captured_screen()
{
	while(_rb_tail != _rb_head) {
		const int screen_size = 
			get_wnd_width() 
				* get_wnd_height() 
				* base::get_pfd(CAPTURE_PF)->_size;
		const int tmp_tail = (_rb_tail + 1) & RB_BUFFERS_MASK;
		screen_buffer *sc = _screen_buffers_rb + tmp_tail;

		GLenum res = glClientWaitSync(sc->_fence, 0, 0);
		if(res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED) { // AMD returns GL_CONDITION_SATISFIED only BUG?
			glDeleteSync(sc->_fence);

			_timer.start();

			if(base::cfg().use_pinned_memory) {
				// process data
				//memcpy(ptr_dst, sc->_pinned_ptr_aligned, screen_size);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, sc->_buffer);
			}
			else {
				glBindBuffer(GL_COPY_READ_BUFFER, sc->_buffer);
				/*glBindBuffer(GL_PIXEL_UNPACK_BUFFER, sc->_showtime_buffer);
				char *ptr_dst = reinterpret_cast<char*>(
					glMapBufferRange(
						GL_PIXEL_UNPACK_BUFFER,
						0,
						screen_size,
						GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT));*/
				char *ptr_src = reinterpret_cast<char*>(
					glMapBufferRange(
						GL_COPY_READ_BUFFER,
						0,
						screen_size,
						GL_MAP_READ_BIT));

				//memcpy(ptr_dst, ptr_src, screen_size);

				glUnmapBuffer(GL_COPY_READ_BUFFER);
				//glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
			}

			glBindTexture(GL_TEXTURE_2D, _showtime_tex);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				base::get_pfd(CAPTURE_PF)->_internal,
				get_wnd_width(),
				get_wnd_height(),
				0,
				base::get_pfd(CAPTURE_PF)->_format,
				base::get_pfd(CAPTURE_PF)->_type, 
				0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

			const double map_time = _timer.elapsed_time();

			__int64 result[3]={0,};
			glGetQueryObjecti64v(sc->_queries[2], GL_QUERY_RESULT, result + 2);
			glGetQueryObjecti64v(sc->_queries[1], GL_QUERY_RESULT, result + 1);
			glGetQueryObjecti64v(sc->_queries[0], GL_QUERY_RESULT, result);
			const double coef_n2m = 1.0 / 1000000.0;
			const double time0 = double(result[1] - result[0]) * coef_n2m;
			const double time1 = double(result[2] - result[1]) * coef_n2m;
			static int counter = 0;

			if((counter++ % 64) == 0) {
				printf("read %.2f ms / %.2f GB/s copy: %.2f ms / %.2f GB/s lag: %u  map: %.2f ms / %.2f GB/s\n",
					time0,
					(double(screen_size) / time0) * (1.0 / double(0x100000)),
					time1,
					time1 > 0.01 ? (double(screen_size) / time1) * (1.0 / double(0x100000)) : 0.0,
					_frame_number - sc->_frame_num,
					map_time,
					map_time > 0.01 ? (double(screen_size) / map_time) * (1.0 / double(0x100000)) : 0.0);
			}

			_rb_tail = tmp_tail;
		}
		else {
			break;
		}
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::capture_screen()
{
	const int screen_size = 
		get_wnd_width() 
			* get_wnd_height() 
			* base::get_pfd(CAPTURE_PF)->_size;
	const int tmp_head = (_rb_head + 1) & RB_BUFFERS_MASK;
	if(tmp_head == _rb_tail) {
		printf("we are too fast there is no free screen buffer!\n");
	}
	else {
		screen_buffer *sc = _screen_buffers_rb + tmp_head;

		glReadBuffer(GL_BACK);

		glBindTexture(GL_TEXTURE_2D, sc->_tmp_texture);
		glCopyTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			0, 0,
			0, 0,
			get_wnd_width(), get_wnd_height());
		glBindTexture(GL_TEXTURE_2D, 0);

		glQueryCounter(sc->_queries[0], GL_TIMESTAMP);

		glBindBuffer(
			GL_PIXEL_PACK_BUFFER,
			base::cfg().use_nvidia_fast_download ? sc->_tmp_buffer : sc->_buffer);
		glReadPixels(
			0,
			0,
			get_wnd_width(),
			get_wnd_height(),
			base::get_pfd(CAPTURE_PF)->_format,
			base::get_pfd(CAPTURE_PF)->_type,
			0);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		
		glQueryCounter(sc->_queries[1], GL_TIMESTAMP);

		if(base::cfg().use_nvidia_fast_download) {
			glBindBuffer(GL_COPY_READ_BUFFER, sc->_tmp_buffer);
			glBindBuffer(GL_COPY_WRITE_BUFFER, sc->_buffer);
			glCopyBufferSubData(
				GL_COPY_READ_BUFFER,
				GL_COPY_WRITE_BUFFER,
				0,
				0,
				screen_size);
		}
		
		glQueryCounter(sc->_queries[2], GL_TIMESTAMP);
		
		sc->_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		sc->_frame_num = _frame_number;
		sc->_stage = 0;

		// update ring buffer head
		_rb_head = tmp_head;
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::load_and_init_shaders(const base::source_location &loc)
{
	assert(_prg == 0);

	_prg=base::create_program(
		base::create_and_compile_shader(
			SRC_LOCATION, "shaders/showtime_v.glsl", GL_VERTEX_SHADER),
		0,
		base::create_and_compile_shader(
			SRC_LOCATION, "shaders/showtime_f.glsl", GL_FRAGMENT_SHADER));
	base::link_program(loc, _prg);

	_prg_mvp = get_uniform_location(loc, _prg, "mvp");
	_prg_size = get_uniform_location(loc, _prg, "size");
	_prg_tex = get_uniform_location(loc, _prg, "tex");
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::prepare_showtime(frame_context *ctx)
{
	ctx->_showtime_mvp = glm::mat4(1);
	ctx->_showtime_mvp[3] = glm::vec4(32, 26, 46.25, 1);

	ctx->_showtime_mvp = ctx->_mvp * ctx->_showtime_mvp;
	ctx->_showtime_tex = _showtime_tex;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::render_showtime(frame_context *ctx)
{
	glUseProgram(_prg);

	// this is only needed on AMD cards due to driver bug wich needs
	// to have attr0 array anabled
	base::set_attr0_vbo_amd_wa();

	glUniformMatrix4fv(_prg_mvp, 1, 0, glm::value_ptr(ctx->_showtime_mvp));
	glUniform2f(_prg_size, 16, 9);

	// bind canvas elements texture buffer
    glUniform1i(_prg_tex, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ctx->_showtime_tex);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);

	// AMD workaround
	base::clear_attr0_vbo_amd_wa();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::app::create_frame_context_pool(const bool premap)
{
	for(int i = 0; i < base::frame_context::POOL_SIZE; ++i) {
		base::frame_context *ctx = new base::frame_context();
		ctx->create_buffers();
		ctx->assign_buffer_range(i);
		if(premap) {
			ctx->map_scene();
			ctx->map_canvas();
		}
		_pool.push_front(ctx);
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
