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

#ifndef __ASYNC_VBO_TRANSFERS_BASE_MUTEX_H__
#define __ASYNC_VBO_TRANSFERS_BASE_MUTEX_H__

namespace base {

class mutex
{
public:
    struct critical_section
    {
        enum { CS_SIZE = 16*4, };
        char   _tmp[CS_SIZE];
    };

	mutex();
	~mutex();

	void lock();
	void unlock();

	bool try_lock();

private:
    critical_section _cs;
};

class mutex_guard
{
protected:

	mutex &_mx;

public:
	explicit mutex_guard(mutex &mx) : _mx(mx) { _mx.lock(); }

	~mutex_guard() { _mx.unlock(); }

private:
	void operator = (const mutex_guard&);
};

} // end of namspace base

#endif // __ASYNC_VBO_TRANSFERS_BASE_MUTEX_H__
