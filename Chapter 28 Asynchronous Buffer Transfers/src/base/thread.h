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

#ifndef __ASYNC_VBO_TRANSFERS_BASE_THREAD_H__
#define __ASYNC_VBO_TRANSFERS_BASE_THREAD_H__

#include "base.h"

namespace base {

class thread
{
protected:

	void* _handle;

public:

	thread();

	virtual ~thread();

	virtual void start(const base::source_location &loc);

	virtual void run() {}

	virtual void stop(const base::source_location &loc);

    static void* thread_fnc(void* data);

	bool wait_for_end(const unsigned int time = -1);

	void terminate();
};

} // end of namspace base

#endif // __ASYNC_VBO_TRANSFERS_BASE_THREAD_H__