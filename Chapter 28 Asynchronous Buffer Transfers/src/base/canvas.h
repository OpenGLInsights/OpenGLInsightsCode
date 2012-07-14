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

#ifndef __ASYNC_VBO_TRANSFERS_CANVAS_H__
#define __ASYNC_VBO_TRANSFERS_CANVAS_H__

#include <gl/glew.h>

#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace glm {
	typedef lowp_ivec2 svec2;	//< short vec2
}

namespace base {

class source_location;
class font;
struct frame_context;

class canvas
{
public:
	static const unsigned int MAX_ELEMENTS_COUNT=8192;
	static const unsigned int ELEMENTS_VBO_SIZE=MAX_ELEMENTS_COUNT*4*sizeof(glm::vec4);
	static const unsigned int ELEMENTS_VBO_SIZE2=MAX_ELEMENTS_COUNT*4;

	canvas();

	~canvas();
		
	void draw_text(
		base::frame_context *ctx,
		const glm::vec2 &position,
		const std::string &text,
		const glm::vec4 &color,
		const font *fnt);

	void fill_rect(
		base::frame_context *ctx,
		const glm::vec2 &pos,
		const glm::vec2 &size,
		const glm::vec4 &color);


	static void render(base::frame_context *ctx);

	void set_pos(const float x,const float y) { _pos=glm::vec2(x,y); }
	void set_size(const float w,const float h) { _size=glm::vec2(w,h); }

	static void load_and_init_shaders(const base::source_location &loc);

protected:
	static GLuint _prg;
	static GLint _prg_tex_fnt;
	static GLint _prg_tb_elements;
	static GLint _prg_screen_size;
	static GLint _prg_clip;
	static GLint _prg_start_index;

	/// canvas position and size on screen
	glm::vec2 _pos;
	glm::vec2 _size;

private:
	canvas(const canvas&);
	void operator=(const canvas&);
};

} // end of namespace base

#endif // __ASYNC_VBO_TRANSFERS_CANVAS_H__
