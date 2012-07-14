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

#include "app_scenario4.h"

#include "base/base.h"
#include "base/frame_context.h"
#include "base/canvas.h"
#include "scene.h"
#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>



//std::auto_ptr<base::frame_context> _frame_context;

copy_thread::copy_thread (app_scenario4 *a):
	_app(a),
	_shutdown(false)
{
}
copy_thread::~copy_thread(){

}
void copy_thread::start(const base::source_location &loc){
	thread::start(loc);
}
void copy_thread::run(){
	while(1){
		//_app->_mutex.lock();
		_app->_scene->frustum_check(_app->_thread_frame_context.get());
		//_app->_mutex.unlock();
		//_app->_event.wait();
		_app->_event.signal();
		_app->_event.wait();
	}

}
void copy_thread::stop(const base::source_location &loc){
	_shutdown = true;
	// todo : add sync here
	thread::stop(loc);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

app_scenario4::app_scenario4() : app() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

app_scenario4::~app_scenario4() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void app_scenario4::start(const bool)
{
	_copy_thread.reset(new copy_thread(this));

	app::start();

	_frame_context.reset(new base::frame_context());
	_frame_context->create_buffers();
	_thread_frame_context.reset(new base::frame_context());
	_thread_frame_context->_scene_tb = _frame_context->_scene_tb;

	// set projection matrix
	app::create_perspective_matrix(_frame_context.get());
	
	base::canvas::load_and_init_shaders(SRC_LOCATION);
	scene::load_and_init_shaders(SRC_LOCATION);

	_scene.reset(new scene());
	scene::create_test_scene(_scene.get());

	glGenBuffers(1, &tmp_buffer);

	_copy_thread->start(SRC_LOCATION);

}
//
////-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
void app_scenario4::renderer_side_start()
{
	app::renderer_side_start();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void app_scenario4::stop()
{
	_copy_thread->stop(SRC_LOCATION);
	app::stop();
}



//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void app_scenario4::draw_frame()
{



	app::begin_frame(_frame_context.get());
	app::update_camera(_frame_context.get());
	_thread_frame_context->_mvp = _frame_context->_mvp;
	//app::update_camera(_thread_frame_context.get());

	//process_captured_screen();

	_frame_context->map_canvas();
	//_frame_context->map_scene();

	//_scene->frustum_check(_frame_context.get());
	

	glBindBuffer(GL_TEXTURE_BUFFER, tmp_buffer);
    glBufferData(GL_TEXTURE_BUFFER, scene::MAX_BLOCK_COUNT * sizeof(glm::mat4) * /*POOL_SIZE*/1, NULL, GL_STREAM_DRAW);
    // this may return a WriteCombined mapping!
	_event.wait();
	_frame_context->_scene_data_ptr = (glm::mat4*)glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
    // fill ptr
	//_mutex.lock();
	for (int i=0; i<4; i++)
		_frame_context->_num_visible_blocks[i] = _thread_frame_context->_num_visible_blocks[i];
	_event.signal();

	_scene->upload_blocks_to_gpu(SRC_LOCATION, _frame_context.get());
    glUnmapBuffer(GL_TEXTURE_BUFFER);
	//_mutex.unlock();_event.signal();

	glBindBuffer(GL_COPY_WRITE_BUFFER, _frame_context->_scene_vbo);
    // this copy ideally requires no CPU work on the data itself.
    glCopyBufferSubData(GL_TEXTURE_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, scene::MAX_BLOCK_COUNT * sizeof(glm::mat4) * /*POOL_SIZE*/1);







	//_frame_context->flush_scene_data();
	_frame_context->flush_canvas_data();

	scene::render_blocks(_frame_context.get());
	
	app::end_frame(_frame_context.get());

	//capture_screen();

}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
