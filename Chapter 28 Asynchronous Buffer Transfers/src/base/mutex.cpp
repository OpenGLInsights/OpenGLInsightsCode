#include "mutex.h"

#include <Windows.h>

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::mutex::mutex()
	: _cs()
{
	InitializeCriticalSectionAndSpinCount(
		reinterpret_cast<CRITICAL_SECTION*>(&_cs), 4);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

base::mutex::~mutex()
{
	DeleteCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&_cs));
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::mutex::lock()
{
	EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&_cs));
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::mutex::unlock()
{
	LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&_cs));
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bool base::mutex::try_lock()
{
	return 0 != TryEnterCriticalSection(
		reinterpret_cast<CRITICAL_SECTION*>(&_cs));
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
