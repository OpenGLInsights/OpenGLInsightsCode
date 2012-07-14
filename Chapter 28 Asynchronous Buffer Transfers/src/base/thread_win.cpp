#include "thread.h"

#include <iostream>

#include <Windows.h>

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::thread::thread()
	: _handle(0)
{}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::thread::~thread()
{}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::thread::start(const base::source_location &loc)
{
    _handle = CreateThread(
		0,
		0,
		(LPTHREAD_START_ROUTINE)thread::thread_fnc,
		this,
		0,
		0);

	if(_handle==0) {
		throw base::exception(loc.to_str()) 
			<< "thread::start failed! (err: "
			<< GetLastError()
			<< ")\n";
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::thread::stop(const base::source_location &loc)
{	
	loc;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void* base::thread::thread_fnc(void* data)
{
	try {
		reinterpret_cast<thread*>(data)->run();
	}
	catch(const base::exception &e) {
		std::cout << e.text();
		return (void*)-1;
	}
	PostQuitMessage(-1);

	return 0;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bool base::thread::wait_for_end(const unsigned int time /* = -1 */)
{
	return WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)_handle, time);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::thread::terminate()
{
	TerminateThread((HANDLE)_handle, DWORD(-1));
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
