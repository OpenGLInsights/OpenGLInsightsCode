/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include "thread.h"

Mutex::Mutex()
{
#ifdef _WIN32
	mutex = CreateMutex(NULL, FALSE, NULL);
#else
	pthread_mutex_init(&mutex, NULL);
#endif
}
Mutex::~Mutex()
{
#ifdef _WIN32
	CloseHandle(mutex);
#else
	pthread_mutex_destroy(&mutex);
#endif
}
void Mutex::lock()
{
#ifdef _WIN32
	WaitForSingleObject(mutex, INFINITE);
#else
	pthread_mutex_lock(&mutex);
#endif
}
void Mutex::unlock()
{
#ifdef _WIN32
	ReleaseMutex(mutex);
#else
	pthread_mutex_unlock(&mutex);
#endif
}

#ifdef _WIN32
struct SillyWindowsThreadStarterData
{
	void* (*function1)(void);
	void* (*function2)(void*);
	void* actualData;
	HANDLE thread;
};
static DWORD WINAPI sillyWindowsThreadStarter(LPVOID lpParam)
{
	SillyWindowsThreadStarterData* arg = (SillyWindowsThreadStarterData*)lpParam;
	if (arg->function1)
		arg->function1();
	else if (arg->function2)
		arg->function2(arg->actualData); //actualData is arbitrary function argument
	else
		Thread::starter(arg->actualData); //actualData is a Thread instance
	delete arg;
	return 0;
}
#endif

Thread::Thread()
{
	bWaiting = false;
	bRunning = false;
#ifdef _WIN32
	thread = 0;
#endif
}
Thread::~Thread()
{
#ifdef _WIN32
	if (thread > 0)
		CloseHandle(thread); //wait() was never called since creating the thread
	thread = 0;
#endif
}
void* Thread::starter(void* instance)
{
	Thread* thread = (Thread*)instance;
	thread->run();
	thread->bRunning = false;
	return NULL;
}
void Thread::start()
{
	if (running()) return; //can't start multiple times!
	bRunning = true;
	
#ifdef _WIN32
	if (thread > 0)
		CloseHandle(thread); //wait() was never called since creating the last thread
	thread = 0;

	SillyWindowsThreadStarterData* d = new SillyWindowsThreadStarterData;
	d->function1 = NULL;
	d->function2 = NULL;
	d->actualData = this;
	thread = CreateThread(NULL, 0, sillyWindowsThreadStarter, d, CREATE_SUSPENDED, NULL);
	d->thread = thread;
	ResumeThread(thread); //just in case it tries to close d->thread before it is initialized
#else
	pthread_create(&thread, NULL, (void* (*)(void*))starter, (void*)this);
#endif
}
void Thread::wait()
{
	if (!running()) return; //haven't start()ed thread yet
	
	bWaiting = true;
#ifdef _WIN32
	DWORD result = WaitForSingleObject(thread, INFINITE);
	if (result == WAIT_FAILED)
		printf("OMGWTF CRITICAL ERROR!!! WaitForSingleObject(thread) failed!\n");
	else if (result != WAIT_OBJECT_0)
	{
		printf("OMGWTF CRITICAL ERROR!!! WaitForSingleObject(thread) failed, somehow without returning WAIT_FAILED!\n");
	}
	CloseHandle(thread);
	thread = 0;
#else
	pthread_join(thread, NULL);
#endif
	bWaiting = false;
}
void Thread::create(void (*func)(void))
{
#ifdef _WIN32
	SillyWindowsThreadStarterData* d = new SillyWindowsThreadStarterData;
	d->function1 = (void* (*)(void))func;
	d->function2 = NULL;
	d->actualData = NULL;
	d->thread = CreateThread(NULL, 0, sillyWindowsThreadStarter, d, CREATE_SUSPENDED, NULL);
	ResumeThread(d->thread); //just in case it tries to close d->thread before it is initialized
	CloseHandle(d->thread); //don't need the reference any more
	d->thread = 0;
#else
	pthread_t thread;
	pthread_create(&thread, NULL, (void* (*)(void*))func, NULL);
#endif
}
void Thread::create(void (*func)(void*), void* args)
{
#ifdef _WIN32
	SillyWindowsThreadStarterData* d = new SillyWindowsThreadStarterData;
	d->function1 = NULL;
	d->function2 = (void* (*)(void*))func;
	d->actualData = args;
	d->thread = CreateThread(NULL, 0, sillyWindowsThreadStarter, d, CREATE_SUSPENDED, NULL);
	ResumeThread(d->thread); //just in case it tries to close d->thread before it is initialized
	CloseHandle(d->thread); //don't need the reference any more
	d->thread = 0;
#else
	pthread_t thread;
	pthread_create(&thread, NULL, (void* (*)(void*))func, args);
#endif
}
