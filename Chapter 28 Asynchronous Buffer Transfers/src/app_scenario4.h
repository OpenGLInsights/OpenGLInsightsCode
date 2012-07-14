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

#ifndef __ASYNC_VBO_TRANSFERS_app_scenario4_H__
#define __ASYNC_VBO_TRANSFERS_app_scenario4_H__

#include "base/app.h"
#include "base/thread.h"
#include "base/event.h"
#include "base/mutex.h"

#include <memory>
#include <assert.h>

#include <glm/glm.hpp>

class app_scenario4;
	//class renderer;
class copy_thread : public base::thread{

	app_scenario4 *_app;
public:
	//std::auto_ptr<base::frame_context> _frame_context;
	bool _shutdown ;

	copy_thread (app_scenario4 *a);
	virtual ~copy_thread();

	virtual void start(const base::source_location &loc);

	virtual void run();

	virtual void stop(const base::source_location &loc);

	void frustum_cull(base::frame_context *ctx);

};

class app_scenario4
	: public base::app
{
public://todo
	std::auto_ptr<scene> _scene;
	std::auto_ptr<base::frame_context> _frame_context;
	std::auto_ptr<base::frame_context> _thread_frame_context;

	std::auto_ptr<copy_thread> _copy_thread;

	GLuint tmp_buffer;
	base::thread_event _event;
	base::mutex _mutex;

public:
	app_scenario4();
	virtual ~app_scenario4();

	virtual const char* get_app_name() const { return "scenario 4"; }

	virtual void start(const bool);
	virtual void renderer_side_start();

	virtual void stop();

	virtual void draw_frame();

private:
	app_scenario4(const app_scenario4&);
	void operator =(const app_scenario4&);
};

#endif // __ASYNC_VBO_TRANSFERS_APP_SCENARIO4_H__
