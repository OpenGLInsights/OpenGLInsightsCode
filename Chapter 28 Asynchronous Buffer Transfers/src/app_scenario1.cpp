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

#include "app_scenario1.h"

#include "base/base.h"
#include "base/frame_context.h"
#include "base/canvas.h"
#include "scene.h"

#include <glm/gtc/matrix_transform.hpp>

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

app_scenario1::app_scenario1() : app() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

app_scenario1::~app_scenario1() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void app_scenario1::start(const bool)
{
	app::start();

	create_frame_context_pool(false);

	// set projection matrix
	app::load_and_init_shaders(SRC_LOCATION);

	base::canvas::load_and_init_shaders(SRC_LOCATION);
	scene::load_and_init_shaders(SRC_LOCATION);

	_scene.reset(new scene());
	scene::create_test_scene(_scene.get());

	init_screen_capture();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void app_scenario1::draw_frame()
{
	base::frame_context *ctx = pop_frame_context_from_pool();

	static bool first=true;

	if(!ctx->check_fence())
		printf("fence incomplete!\n");

	//printf("scene buffer handle: %.2u, ptr %p\n", ctx->_scene_vbo, ctx->_scene_data_ptr);

	app::begin_frame(ctx);

	app::create_perspective_matrix(ctx);
	app::update_camera(ctx);

	//app::prepare_showtime(ctx);
	//process_captured_screen();

	ctx->map_scene();

	static char* cached_scene_data_ptr;
	static int cached_scene_data_ptr_size;

	//memset(ctx->_scene_data_ptr, 0, ctx->_scene_data_ptr_size);
	_scene->frustum_check(ctx);
	_scene->upload_blocks_to_gpu(SRC_LOCATION, ctx);

	/*static int counter = 100;
	if(counter-- < 0)
		memset(ctx->_scene_data_ptr, 0, ctx->_scene_data_ptr_size);*/

	ctx->flush_scene_data();

	/*_frame_context->map_scene();
	_scene->frustum_check(_frame_context.get());
	_scene->upload_blocks_to_gpu(SRC_LOCATION, _frame_context.get());
	_frame_context->flush_scene_data();*/

	scene::render_blocks(ctx);
	//app::render_showtime(ctx);
	
	ctx->map_canvas();
	app::end_frame(ctx);
	ctx->flush_canvas_data();

	//capture_screen();

	ctx->put_fence();

	push_frame_context_to_pool(ctx);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  