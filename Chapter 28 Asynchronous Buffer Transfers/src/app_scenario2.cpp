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

#include "app_scenario2.h"

#include "base/base.h"
#include "base/frame_context.h"
#include "scene.h"
#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

app_scenario2::app_scenario2() : app() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

app_scenario2::~app_scenario2() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void app_scenario2::start(const bool)
{
	_renderer.reset(new renderer(this));
	_renderer->start(SRC_LOCATION);

	app::start(true);

	_scene.reset(new scene());
	scene::create_test_scene(_scene.get());
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void app_scenario2::renderer_side_start()
{
	app::renderer_side_start();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void app_scenario2::stop()
{
	_renderer->stop(SRC_LOCATION);
	app::stop();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void app_scenario2::draw_frame()
{
	base::frame_context *ctx = 0;
	
	// wait for renderer
	while((ctx = _renderer->pop_frame_context_from_pool()) == 0) base::sleep_ms(0);
	
	assert(ctx != 0);

	app::begin_frame(ctx, true);

	app::create_perspective_matrix(ctx);
	app::update_camera(ctx);

	//memset(ctx->_test_vbo_ptr,0x30,base::frame_context::TEST_VBO_SIZE);

	static int first_time = base::frame_context::POOL_SIZE;
	static char* cached_scene_data_ptr;
	static int cached_scene_data_ptr_size;
	if(first_time) {
		_scene->frustum_check(ctx);
		_scene->upload_blocks_to_gpu(SRC_LOCATION, ctx);
		/*first_time--;
		cached_scene_data_ptr_size = ctx->_scene_data_ptr_size;
		cached_scene_data_ptr = new char[cached_scene_data_ptr_size];
		memcpy(cached_scene_data_ptr, ctx->_scene_data_ptr, cached_scene_data_ptr_size);*/
	}
	else {
		memcpy(ctx->_scene_data_ptr, cached_scene_data_ptr, cached_scene_data_ptr_size);
	}

	app::end_frame(ctx, true);

	_renderer->push_frame_context(ctx);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
