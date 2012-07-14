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

#ifndef __ASYNC_VBO_TRANSFERS_APP_SCENARIO1_H__
#define __ASYNC_VBO_TRANSFERS_APP_SCENARIO1_H__

#include "base/app.h"

#include <memory>
#include <assert.h>

#include <glm/glm.hpp>

class app_scenario1
	: public base::app
{
protected:
	//std::auto_ptr<base::frame_context> _frame_context;
	std::auto_ptr<scene> _scene;

public:
	app_scenario1();
	virtual ~app_scenario1();

	virtual int get_wnd_width() const { return 1280; }
	virtual int get_wnd_height() const { return 720; }

	virtual const char* get_app_name() const { return "scenario 1"; }

	virtual void start(const bool);
	virtual void draw_frame();

private:
	app_scenario1(const app_scenario1&);
	void operator =(const app_scenario1&);
};

#endif // __ASYNC_VBO_TRANSFERS_APP_SCENARIO1_H__
