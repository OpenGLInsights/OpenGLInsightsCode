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

#include "canvas.h"
#include "font.h"
#include "bmf.h"
#include "base.h"
#include "app.h"
#include "frame_context.h"

#include <glm/gtc/type_ptr.hpp>

GLuint base::canvas::_prg = 0;
GLint base::canvas::_prg_tex_fnt = -1;
GLint base::canvas::_prg_tb_elements = -1;
GLint base::canvas::_prg_screen_size = -1;
GLint base::canvas::_prg_clip = -1;
GLint base::canvas::_prg_start_index = -1;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::canvas::canvas()
	: _pos(0)
	, _size(app::get()->get_wnd_width(),app::get()->get_wnd_height())
{}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::canvas::~canvas() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::canvas::load_and_init_shaders(const base::source_location &loc)
{
	assert(_prg == 0);

	_prg=base::create_program(
		base::create_and_compile_shader(
			SRC_LOCATION,"shaders/canvas_v.glsl",GL_VERTEX_SHADER),
		0,
		base::create_and_compile_shader(
			SRC_LOCATION,"shaders/canvas_f.glsl",GL_FRAGMENT_SHADER));
	base::link_program(loc, _prg);

	_prg_tb_elements= get_uniform_location(loc, _prg, "tb_elements");
	_prg_tex_fnt = get_uniform_location(loc, _prg, "tex_font");
	_prg_screen_size = get_uniform_location(loc, _prg, "screen_size");
	_prg_clip = get_uniform_location(loc, _prg, "clip");
	_prg_start_index = get_uniform_location(loc, _prg, "start_index");
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::canvas::draw_text(
	base::frame_context *ctx,
	const glm::vec2 &position,
	const std::string &text,
	const glm::vec4 &color,
	const font *fnt)
{
	assert(
		fnt->_infoBlock != 0 
		&& !text.empty()
		&& fnt!=0);
	assert(ctx->_elements != 0 && ctx->_elements_begin != 0);

	glm::vec4* const batch_elements = ctx->_elements;

	// get font height and init screen position
	const float line_height = fnt->_commonBlock->lineHeight;
	glm::vec2 pos = position + _pos;

	const font::char_map_t &chars=fnt->_chars;
    
	const char *e = text.c_str() + text.length();
	for(const char *i = text.c_str(); i != e; ++i) {
		const unsigned int char_id = *i;

		// new line
		if(*i == 0x0a) {
			pos=glm::vec2(position.x + _pos.x, pos.y + line_height);
			continue;
		}

		// find 
		font::char_map_t::const_iterator it = chars.find(char_id);
		if(it == chars.end())
			it = chars.find(0x20);

		assert(it != chars.end());

		const CharInfo *char_info=it->second;

		// space
		if(*i == 0x20) {
			pos.x += char_info->xadvance;
			continue;
		}

		assert(ctx->_elements - ctx->_elements_begin < MAX_ELEMENTS_COUNT * 3);

		// add element to VBO

		// position and size in screen space
        glm::vec4 elem_pos = glm::vec4(
			pos+glm::vec2(
				char_info->xoffset,
				line_height - char_info->yoffset - char_info->height),
			glm::vec2(char_info->width, char_info->height));

		// check if the element is in canvas space
		if( elem_pos.x <= _pos.x + _size.x
			&& elem_pos.x + elem_pos.z >= _pos.x
			&& elem_pos.y + elem_pos.w <= _pos.y + _size.y
			&& elem_pos.y >= _pos.y ) {

			*ctx->_elements++ = elem_pos;

			// position and size in font texture space
			*ctx->_elements++ = glm::vec4(
				glm::vec2(char_info->x, char_info->y + char_info->height),
				glm::vec2(char_info->width, -char_info->height));

			// color
			*ctx->_elements++ = color;
		}

		// adjust position on screen
		pos.x += char_info->xadvance;
	}

	// create or update batch
	base::batches_t::iterator last = ctx->_batches.end()-1;
	if(ctx->_batches.empty() || (last->tex != fnt->_tex && last->tex != 0))
		ctx->_batches.push_back(
			base::batch(
				_pos,
				_size,
				batch_elements - ctx->_elements_begin,	// start index
				(ctx->_elements - batch_elements) / 3,	// number of elements in batch
				fnt->_tex));
	else {
		last->count += (ctx->_elements - batch_elements) / 3;
		last->tex = fnt->_tex;
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::canvas::fill_rect(
	base::frame_context *ctx,
	const glm::vec2 &pos,
	const glm::vec2 &size,
	const glm::vec4 &color)
{
	assert(ctx->_elements != 0 && ctx->_elements_begin != 0);

	const glm::vec4 * const ebegin = ctx->_elements;

	*ctx->_elements++ = glm::vec4(_pos + pos,size);
	*ctx->_elements++ = glm::vec4(0);
	*ctx->_elements++ = color;

	// create or update batch
	if(ctx->_batches.empty())
		ctx->_batches.push_back(base::batch(
			_pos, _size, ebegin - ctx->_elements_begin, 1, 0));
	else
		++(ctx->_batches.end() - 1)->count;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::canvas::render(base::frame_context *ctx)
{
	if(!ctx->_batches.empty()) {
		// this is only needed on AMD cards due to driver bug wich needs
		// to have attr0 array anabled
		base::set_attr0_vbo_amd_wa();

		glUseProgram(_prg);

		// bind canvas elements texture buffer
        glUniform1i(_prg_tb_elements, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, ctx->_canvas_tb);

		const glm::vec2 screen_size(
			app::get()->get_wnd_width(),
			app::get()->get_wnd_height());

		// set screen size
		glUniform2fv(_prg_screen_size, 1, glm::value_ptr(1.0f / screen_size));
		// set texture channel for batch
		glUniform1i(_prg_tex_fnt, 1);
		glActiveTexture(GL_TEXTURE1);

		glDisable(GL_DEPTH_TEST);

		// draw canvas batches
		base::batches_t::const_iterator e = ctx->_batches.end();
		for(base::batches_t::const_iterator i = ctx->_batches.begin(); i != e; ++i) {
			// set canvas clip region
			glUniform4fv(_prg_clip, 1, glm::value_ptr(glm::vec4(i->pos, i->pos + i->size)));
			glUniform1i(_prg_start_index, i->index);

			glBindTexture(GL_TEXTURE_2D, i->tex);
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, i->count);
		}

		ctx->_batches.clear();

		base::clear_attr0_vbo_amd_wa();
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
