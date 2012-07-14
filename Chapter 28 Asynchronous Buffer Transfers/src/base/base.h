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

#ifndef __ASYNC_VBO_TRANSFERS_BASE_H__
#define __ASYNC_VBO_TRANSFERS_BASE_H__

#include "pixelfmt.h"
#include "hptimer.h"

#include <string>
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <vector>

#include <gl/glew.h>

#include <glm/glm.hpp>

#define TMP_STR_BUF_SIZE 64

namespace base {

	class app;

///
class source_location
{
public:

	source_location(
		const char *function_name,
		const char *file_name,
		const int line_number)
		: _function_name(function_name)
		, _file_name(file_name)
		, _line_number(line_number)
	{}

	std::string to_str() const {
		char line[TMP_STR_BUF_SIZE];
		return std::string(_function_name)+"@"+_file_name+'('+_itoa(_line_number,line,10)+") ";
	}

	const char* function_name() const { return _function_name; }
	const char* file_name() const { return _file_name; }
	int line_number() const { return _line_number; }

private:
	const char* const _function_name;
	const char* const _file_name;
	const int _line_number;

	source_location(const source_location&);
	void operator = (const source_location&);
};

///
class exception
{
public:
    exception(const char* text) : _text(text) {}
    exception(const std::string &text) : _text(text) {}
    exception() : _text("unknown exception") {}
	
	~exception() { _text+="\n"; }

    exception& operator << (const char *czstr) { _text+=czstr; return *this; }
    exception& operator << (const std::string& str) { _text+=str; return *this; }
    exception& operator << (char c) { _text+=c; return *this; }

    exception& operator << (const int i) { char tmp[TMP_STR_BUF_SIZE]; _text+=_itoa(i,tmp,10); return *this; }
    exception& operator << (const unsigned int i) { char tmp[TMP_STR_BUF_SIZE]; _text+=_ultoa(i,tmp,10); return *this; }
    exception& operator << (const unsigned long i) { char tmp[TMP_STR_BUF_SIZE]; _text+=_ultoa(i,tmp,10); return *this; }
    exception& operator << (const float f) { char tmp[TMP_STR_BUF_SIZE]; sprintf(tmp,"%f",f); _text+=tmp; return *this; }

    const std::string& text() const { return _text; }

protected:
    std::string _text;
};

/// pixel format descriptor
struct pfd {
	int _internal;			//< OpenGL internal format (GL_RGBA8)
	int _format;			//< OpenGL format (GL_RGBA)
	int _type;				//< OpenGL component type (GL_UNSIGNED_BYTE)
	unsigned int _size;		//< byte size of one pixel (4)
	int _components;		//< number of components (4)
	bool _rt;				//< true if it can be used as render target
	int _sRGB;				//< sRGB pixel format alternative
	const char *_txt;		//< readable description
	bool _compressed;		//< true if it is compressed format
};

/// 
extern const pfd ___pfds[];

/// return pointer to pixel format descriptor
inline const pfd* get_pfd(const pixelfmt pf) { assert(pf>PF_UNKNOWN && pf<PF_LAST); return ___pfds+pf; }

/// run app and init OpenGL stuff
void run_app_win(app *a, const bool initgl=true);

/// read whole file into given std::vector
void read_file(
	const base::source_location &loc,
	const char* filename,
	std::vector<unsigned char> &data);

///
GLuint create_and_compile_shader(
	const base::source_location &loc,
	const std::string &filename,
	const GLuint type);

///
GLuint create_program(
	const GLuint vs,
	const GLuint gs,
	const GLuint fs);

///
void link_program(
	const base::source_location &loc,
	const GLuint prg);

/// simple wrapper for glGetUniformLocation with error reporting
GLint get_uniform_location(
	const base::source_location &loc,
	const GLuint prg,
	const char *name);

///
GLuint create_texture(
	const int width, 
	const int height,
	const base::pixelfmt pf,
	const void *data,
	const unsigned buffer = 0);

/// The only supported format is TGA 32bit RGBA8 uncompressed
GLuint create_texture_from_file(
	const base::source_location &loc,
	const char *filename,
    bool srgb);

/// AMD attr0 workaround stuff
void set_attr0_vbo_amd_wa();
void clear_attr0_vbo_amd_wa();

///
glm::ivec2 get_mouse_pos();
void set_mouse_pos(const glm::ivec2 &pos);

///
void init_opengl_win(const bool create_shared_ctx = false);

void init_opengl_dbg_win();

void* get_window_handle();

void* get_window_hdc();

void* get_shared_context();

void swap_buffers();

void sleep_ms(const int time);

void make_current_shared_context();

struct config {
	config()
		: use_debug_context(false)
		, use_debug_sync(false)
		, use_pinned_memory(false)
		, use_nvidia_fast_download(false)
		, use_async_readback(false)
	{}

	bool use_debug_context : 1;
	bool use_debug_sync : 1;
	bool use_pinned_memory : 1;
	bool use_nvidia_fast_download : 1;
	bool use_async_readback : 1;
};

config& cfg();

} // end of namespace base

#define SRC_LOCATION base::source_location(__FUNCTION__,__FILE__,__LINE__)

#define SRC_LOCATION_STR base::source_location(__FUNCTION__,__FILE__,__LINE__).to_str()

#endif // __ASYNC_VBO_TRANSFERS_BASE_H__
