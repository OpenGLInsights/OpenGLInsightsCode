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

#ifndef __ASYNC_VBO_TRANSFERS_SCENE_H__
#define __ASYNC_VBO_TRANSFERS_SCENE_H__

#include <vector>

#include <glm/glm.hpp>

#include <gl/glew.h>

namespace base {
	struct frame_context;
	class source_location;
}

class scene
{
public:
	static const int BUILDING_SIDE_SIZE = 32;
	static const int MAX_BLOCK_COUNT = BUILDING_SIDE_SIZE * (BUILDING_SIDE_SIZE >> 1) * BUILDING_SIDE_SIZE * 2;

	scene();
	~scene();

	struct block;

	// heading is in degrees
	block* add_block(
		const int type,
		const glm::vec3 &pos,
		const glm::vec3 &size,
		const float heading);
	void upload_blocks_to_gpu(
		const base::source_location &loc,
		base::frame_context *ctx);
	int frustum_check(base::frame_context *fc,const bool dont_check=true);
	void create_frustum_planes(glm::vec4 *planes, const glm::mat4 &mvp);

	static void load_and_init_shaders(const base::source_location &loc);

	static void create_test_scene(scene *s);
	static void render_blocks(const base::frame_context *ctx);

public:

	struct block {
		glm::mat4 *_tm;
		unsigned int *_flags;
		glm::vec3 *_hw;

		block(
			glm::mat4 *tm,
			unsigned int *flags,
			glm::vec3 *hw)
			: _tm(tm)
			, _flags(flags)
			, _hw(hw)
		{}
	};

protected:

	static const int NumTypes = 4;

	enum Flags {
		TypeMask	= NumTypes-1,
		Visible		= 0x04,
	};

	std::vector<glm::mat4> _tms;			//< transformation matrixes for blocks
	std::vector<glm::mat4> _bboxes;			//< inverse transposed TM matrices
	std::vector<glm::vec3> _hws;			//< half width for every block
	std::vector<unsigned int> _flags;		//<

	typedef std::vector<block> blocks_t;
	blocks_t _blocks;

	static GLuint _prg;
	static GLint _prg_max_sides;
	static GLint _prg_tb_blocks;
	static GLint _prg_block_type;
	static GLint _prg_start_index;
	static GLint _prg_mvp;
};

#endif // __ASYNC_VBO_TRANSFERS_SCENE_H__
