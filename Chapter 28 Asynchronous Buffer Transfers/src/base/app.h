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

#ifndef __ASYNC_VBO_TRANSFERS_APP_H__
#define __ASYNC_VBO_TRANSFERS_APP_H__

#include "hptimer.h"
#include "pixelfmt.h"

#include <memory>
#include <assert.h>

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <list>

class scene;

namespace base {

class canvas;
class font;
struct frame_context;
class source_location;

class app
{
public:
	app();
	virtual ~app();

	virtual void start(const bool nogl=false);
	virtual void renderer_side_start();
	void begin_frame(base::frame_context *ctx, const bool nogl = false); 
	virtual void draw_frame() {}
	void end_frame(base::frame_context *ctx, const bool nogl = false);
	virtual void stop();
	
	// desired client area size / OpenGL frame buffer size
	virtual int get_wnd_width() const { return 1280; }
	virtual int get_wnd_height() const { return 720; }

	virtual const char* get_app_name() const { return "unknown"; }
	virtual const char* get_wnd_name() const { return "async vbo transfers scenario one"; }
	virtual const char* get_wnd_cls() const { return "async vbo transfers"; }

	static app* get() { assert(_app); return _app; }

	virtual void mouse_move(const int x, const int y);

	virtual void key(const int key,const bool down);

	virtual void activated(const bool state);

	void create_perspective_matrix(frame_context *fc);
	void update_camera(frame_context *fc);

	/// it is called in base::run_app_win...
	void inc_frame_number() { ++_frame_number; }

	void init_screen_capture();
	void process_captured_screen();
	void capture_screen();

	static void load_and_init_shaders(const base::source_location &loc);
	void prepare_showtime(frame_context *ctx);
	static void render_showtime(frame_context *ctx);

	base::frame_context* pop_frame_context_from_pool() {
		if(!_pool.empty()) {
			base::frame_context *ctx = _pool.back();
			_pool.pop_back();
			return ctx;
		}
		else
			return 0;
	}

	void push_frame_context_to_pool(frame_context *ctx) {
		_pool.push_front(ctx);
	}

	void create_frame_context_pool(const bool premap = false);

protected:
	static app *_app;

	std::auto_ptr<base::font> _fnt_mono;
	std::auto_ptr<base::font> _fnt_arial;
	std::auto_ptr<base::canvas> _canvas;

	glm::ivec2 _last_mouse_pos;
	glm::vec2 _mouse_pos;

	glm::vec3 _velocity;
	glm::vec3 _position;

	base::hptimer _timer;

	int _frame_number;

	// capture stuff
	static const int RB_BUFFERS_SIZE = 4;
	static const int RB_BUFFERS_MASK = RB_BUFFERS_SIZE - 1;
	static const base::pixelfmt CAPTURE_PF = base::PF_RGBA8;
	struct screen_buffer {
		char *_pinned_ptr;
		char *_pinned_ptr_aligned;
		unsigned _buffer;
		unsigned _tmp_buffer;
		unsigned _tmp_texture;
		int _frame_num;
		GLsync _fence;
		unsigned _queries[3];
		int _stage;
		unsigned _showtime_buffer;
	};
	int _rb_head, _rb_tail;
	screen_buffer _screen_buffers_rb[RB_BUFFERS_SIZE];
	unsigned _fb;
	unsigned _showtime_tex;

	bool _velocity_boost : 1;
	bool _active : 1;

	/// frame context pool
	std::list<base::frame_context*> _pool;

	// showtime stuff
	static GLuint _prg;
	static GLint _prg_mvp;
	static GLint _prg_size;
	static GLint _prg_tex;

private:
	app(const app&);
	void operator =(const app&);
};

} // end of namespace base

#endif // __ASYNC_VBO_TRANSFERS_APP_H__
