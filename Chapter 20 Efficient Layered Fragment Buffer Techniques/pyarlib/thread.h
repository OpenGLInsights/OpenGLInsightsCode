/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef PYARLIB_THREAD_H
#define PYARLIB_THREAD_H

#ifdef _WIN32
#pragma warning (disable:4996)
#define NOMINMAX
#include <windows.h>
#else
#include <pthread.h>
#endif

class Mutex
{
private:
	//no copying!
	Mutex(const Mutex& other) {}
	Mutex& operator=(const Mutex& other) {return *this;}
#ifdef _WIN32
	HANDLE mutex;
#else
	pthread_mutex_t mutex;
#endif
public:
	Mutex();
	~Mutex();
	void lock();
	void unlock();
};

#ifdef _WIN32
static DWORD WINAPI sillyWindowsThreadStarter(LPVOID lpParam);
#endif

class Thread
{
private:
#ifdef _WIN32
	friend DWORD WINAPI sillyWindowsThreadStarter(LPVOID lpParam);
#endif
	//no copying!
	Thread(const Thread& other) {}
	Thread& operator=(const Thread& other) {return *this;}
#ifdef _WIN32
	HANDLE thread;
#else
	pthread_t thread;
#endif
	bool bWaiting;
	bool bRunning;
	static void* starter(void* instance);
public:
	Thread();
	virtual ~Thread();
	void start();
	virtual void run() =0;
	void wait();
	bool waiting() {return bWaiting;} //returns true if parent called wait()
	bool running() {return bRunning;} //return true if thread is running
	
	//Utility functions. Does not record thread ID and wait() is not available.
	static void create(void (*func)(void));
	static void create(void (*func)(void*), void* args);
};

#endif
