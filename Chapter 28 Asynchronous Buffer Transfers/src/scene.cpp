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

#include "scene.h"
#include "base/base.h"
#include "base/frame_context.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/swizzle.hpp>
#include <glm/gtx/verbose_operator.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace glm;

GLuint scene::_prg = 0;
GLint scene::_prg_max_sides = -1;
GLint scene::_prg_tb_blocks = -1;
GLint scene::_prg_block_type = -1;
GLint scene::_prg_start_index = -1;
GLint scene::_prg_mvp = -1;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

scene::scene()
	: _tms()
	, _bboxes()
	, _hws()
	, _flags()
	, _blocks()
{
	_tms.reserve(MAX_BLOCK_COUNT);
	_bboxes.reserve(MAX_BLOCK_COUNT);
	_hws.reserve(MAX_BLOCK_COUNT);
	_flags.reserve(MAX_BLOCK_COUNT);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

scene::~scene() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

scene::block* scene::add_block(
	const int type,
	const glm::vec3 &pos,
	const glm::vec3 &size,
	const float heading)
{
	const vec3 half_size = size * 0.5f;

	mat4 tm = rotate(mat4(1), heading, vec3(0,0,1));
	mat4 bbox = tm;

	tm[0].x = half_size.x;
	tm[1].y = half_size.y;
	tm[2].z = half_size.z;
	tm[3] = vec4(pos,1);
	_tms.push_back(tm);

	bbox[3] = vec4(pos + vec3(0,half_size.y,0),1);
	_bboxes.push_back(bbox);

	_hws.push_back(size * 0.5f);
	_flags.push_back(type & TypeMask);

	_blocks.push_back(
		block(
			(_tms.end()-1)._Ptr,
			(_flags.end()-1)._Ptr,
			(_hws.end()-1)._Ptr));
	
	return (_blocks.end()-1)._Ptr;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::load_and_init_shaders(const base::source_location &loc)
{
	assert(_prg == 0);

	_prg=base::create_program(
		base::create_and_compile_shader(
			SRC_LOCATION, "shaders/block_v.glsl", GL_VERTEX_SHADER),
		0,
		base::create_and_compile_shader(
			SRC_LOCATION, "shaders/block_f.glsl", GL_FRAGMENT_SHADER));
	base::link_program(loc, _prg);

	_prg_max_sides = get_uniform_location(loc, _prg, "max_sides");
	_prg_tb_blocks = get_uniform_location(loc, _prg, "tb_blocks");
	_prg_block_type = get_uniform_location(loc, _prg, "block_type");
	_prg_start_index = get_uniform_location(loc, _prg, "start_index");
	_prg_mvp = get_uniform_location(loc, _prg, "mvp");
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

inline glm::vec4 normalize_plane(const glm::vec4 &p) {
	return p*(1.0f/length(p.swizzle(X,Y,Z)));
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::create_frustum_planes(
	vec4 *planes,
	const glm::mat4 &mvp)
{
    // Left plane
    planes[0] = normalize_plane(vec4(
		mvp[0].w+mvp[0].x,
		mvp[1].w+mvp[1].x,
		mvp[2].w+mvp[2].x,
		mvp[3].w+mvp[3].x));
 
    // Right plane
    planes[1]=normalize_plane(vec4(
		mvp[0].w-mvp[0].x,
		mvp[1].w-mvp[1].x,
		mvp[2].w-mvp[2].x,
		mvp[3].w-mvp[3].x));
 
    // Top plane
    planes[2]=normalize_plane(vec4(
		mvp[0].w-mvp[0].y,
		mvp[1].w-mvp[1].y,
		mvp[2].w-mvp[2].y,
		mvp[3].w-mvp[3].y));
 
    // Bottom plane
    planes[3]=normalize_plane(vec4(
		mvp[0].w+mvp[0].y,
		mvp[1].w+mvp[1].y,
		mvp[2].w+mvp[2].y,
		mvp[3].w+mvp[3].y));
 
    // Far plane
    planes[4]=normalize_plane(vec4(
		mvp[0].w-mvp[0].z,
		mvp[1].w-mvp[1].z,
		mvp[2].w-mvp[2].z,
		mvp[3].w-mvp[3].z));

	// Near plane
    planes[5]=normalize_plane(vec4(
		mvp[0].z,mvp[1].z,mvp[2].z,mvp[3].z));
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "base\hptimer.h"
base::hptimer timer;
double elapsed=0;
int iterations = 0;

int scene::frustum_check(base::frame_context *ctx,const bool dont_check)
{
	timer.restart();

	// prepare frustum planes
	const int num_planes=6;
	glm::vec4 planes[num_planes];
	create_frustum_planes(planes,ctx->_mvp);

	glm::vec4 * const planes_end=planes+num_planes;

	const mat4 *bbox = &_bboxes[0];
	const vec3 *hw = &_hws[0];
	unsigned int *flags = &_flags[0];
	const unsigned int *e = &_flags[0] + _flags.size();

	memset(ctx->_num_visible_blocks, 0, sizeof(ctx->_num_visible_blocks));

	for( ; flags != e; ++hw, ++bbox, ++flags) {
		const vec4 *plane = planes;
		if(!dont_check) {
			for(; plane != planes_end; ++plane) {
				// transform plane's normal to bbox space
				const glm::vec3 npv(
					dot((*bbox)[0], *plane),
					dot((*bbox)[1], *plane),
					dot((*bbox)[2], *plane));

				// and do standard p/n vertex aabb check
				const float mp = dot((*bbox)[3], *plane);
				const float np = dot(*hw, abs(npv));

				if(mp + np < 0.0f) break;
			}

			*flags = *flags & (~Visible);
		}

		if(plane == planes_end || dont_check) {
			*flags |= Visible;
			++ctx->_num_visible_blocks[*flags & TypeMask];
		}
	}

	elapsed += timer.elapsed_time();
	iterations++;
	if (iterations % 100 == 0){
		//printf("Culling time : %.3f ms/frame\n", elapsed/double(iterations));
		elapsed = 0.0; iterations = 0;
	}

	return ctx->_num_visible_blocks[0];
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::upload_blocks_to_gpu(
	const base::source_location &loc,
	base::frame_context *ctx)
{
	ctx->_scene_data_ptr_size = 0;
	for(int i = 0; i < NumTypes; ++i)
		ctx->_scene_data_ptr_size += ctx->_num_visible_blocks[i];
	ctx->_scene_data_ptr_size *= sizeof(mat4);

	if(ctx->_scene_data_ptr_size > 0) {
		mat4 *ptr[NumTypes];
	
		ptr[0] = ctx->_scene_data_ptr;

		if(ptr[0] == 0)
			throw base::exception(loc.to_str())
				<< "Cannot map visible blocks VBO to CPU memory!";

		for(int i = 1; i < NumTypes; ++i)
			ptr[i] = ptr[i - 1] + ctx->_num_visible_blocks[i - 1];

		const mat4 *tm = &_tms[0];
		const mat4 *e = &_tms[0] + _tms.size();
		unsigned int *flags = &_flags[0];

		// transform visible blocks to screen and add to VBO 
		for(; tm!=e; ++tm, ++flags)
			if(*flags & Visible)
				*ptr[*flags & TypeMask]++ = /*ctx->_mvp * */(*tm);
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::render_blocks(const base::frame_context *ctx)
{
	glUseProgram(_prg);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	const int num_sides=64;

	// this is only needed on AMD cards due to driver bug wich needs
	// to have attr0 array anabled
	base::set_attr0_vbo_amd_wa();

	const int context_offset = 0;//ctx->_scene_data_ptr - ctx->_scene_data_ptr_base;

	glUniform1f(_prg_max_sides, float(num_sides));
	glUniform1i(_prg_block_type, 0);
	glUniform1i(_prg_start_index, context_offset);
	glUniformMatrix4fv(_prg_mvp, 1, GL_FALSE, glm::value_ptr(ctx->_mvp));

	// bind canvas elements texture buffer
    glUniform1i(_prg_tb_blocks, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, ctx->_scene_tb);

	const int batch_size = 100;

	if(ctx->_num_visible_blocks[0]>0) {
		int pos = context_offset;

		//for(int i = 0; i<ctx->_num_visible_blocks[0] / batch_size; ++i, pos += batch_size) {
			glUniform1i(_prg_start_index, pos);
			glDrawArraysInstanced(
				GL_TRIANGLE_STRIP, 0, (num_sides << 1) + 2, ctx->_num_visible_blocks[0]);
		//}
	}

	if(ctx->_num_visible_blocks[1]) {
		glUniform1i(_prg_block_type, 1);

		int pos = context_offset + ctx->_num_visible_blocks[0];

		//for(int i = 0; i<ctx->_num_visible_blocks[0] / batch_size; ++i, pos += batch_size) {
			glUniform1i(_prg_start_index, pos);
			glDrawArraysInstanced(
				GL_TRIANGLE_STRIP, 0, 8, ctx->_num_visible_blocks[1] << 1);
		//}
	}

	if(ctx->_num_visible_blocks[2]) {
		glUniform1i(_prg_block_type, 2);
		glUniform1i(
			_prg_start_index,
			context_offset 
				+ ctx->_num_visible_blocks[0] 
				+ ctx->_num_visible_blocks[1]);

		glDrawArraysInstanced(
			GL_TRIANGLE_STRIP, 0, 20 * 4, ctx->_num_visible_blocks[2] << 1);
	}

	// AMD workaround
	base::clear_attr0_vbo_amd_wa();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void scene::create_test_scene(scene *s)
{
	const int grid_size = BUILDING_SIDE_SIZE;
	const glm::vec3 pillar_size(0.25f, 2.6f, 0.25f);
	//const glm::vec3 pillar_size(1.5f, 2.6f, 3.5f);
	const glm::vec3 box_size(3.0f, 0.2f, 4.0f);

	// create floor's pillars
	for(int z = 0; z < grid_size; ++z)
		for(int y = 0; y < (grid_size >> 1); ++y)
			for(int x = 0; x < grid_size; ++x) {
				if(z < grid_size-1) 
					s->add_block(
						0, glm::vec3(x * 3, 0.2 + z * 2.8, y * 4), pillar_size, 0);
				s->add_block(
					1, glm::vec3(x * 3, z * 2.8, y * 4), box_size, 0);
			}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
