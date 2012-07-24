#include "stdafx.h"

#include "timer.h"
#include "window.h"

/// Constructor
Timer::Timer()
{
}

/// Destructor
Timer::~Timer()
{
}

/// Init Timer
///
/// @param options [in] command line options
///
/// @return false if failed
bool
Timer::init(const commandLineOptions *options)
{
	_options = *options;
	QueryPerformanceFrequency(&_frequency);

	return true;
}

/// Start the Timer, called from the last stage
void
Timer::start()
{
	_frames = 0;
	QueryPerformanceCounter(&_startCPUTime);
}

/// Update Timer, called from the last stage
///
/// @return if true continue the Timer loop
bool
Timer::update()
{
	LARGE_INTEGER currentCPUTime;
	_frames++;

	QueryPerformanceCounter(&currentCPUTime);

	if((double)(currentCPUTime.QuadPart - _startCPUTime.QuadPart) / (double)_frequency.QuadPart > _options._runTime)
	{
		glFinish();
		QueryPerformanceCounter(&currentCPUTime);

		const float frameTimeCPU = (float)(currentCPUTime.QuadPart - _startCPUTime.QuadPart) / _frames;
		const float buffersPerSecondCPU = ((float)_frequency.QuadPart / (float)frameTimeCPU) * _options._numStreams;
		const float dataSize = _options._texWidth * _options._texHeight * _options._numStreams*4.f;
		const float rate = buffersPerSecondCPU * dataSize;

		char string[256];
		if(rate > 1024.f * 1024.f)
			sprintf_s(string, sizeof(string), "%4.2f buffers/s %4.2f MBytes/s", buffersPerSecondCPU, rate / (1024.f * 1024.f));
		else if(rate > 1024.f)
			sprintf_s(string, sizeof(string), "%4.2f buffers/s %4.2f kBytes/s", buffersPerSecondCPU, rate / 1024.f);
		else
			sprintf_s(string, sizeof(string), "%4.2f buffers/s %4.2f Bytes/s", buffersPerSecondCPU, rate);
		window::getInstance().messageBoxError(string);
		PostMessage(window::getInstance().hWnd(), WM_CLOSE, 0, 0);
		return false;
	}


	return true;
}

/// Get the information string
///
/// @param string (in/out) the string to write to
/// @param stringlen [in] maximum string length
void
Timer::information(char *string, size_t stringlen)
{
	const float buffersPerSecondCPU = ((float)_frequency.QuadPart / _frameTimeCPU) * _options._numStreams;
	const float dataSize = (_options._texWidth * _options._texHeight * 4.f)/(1024.f * 1024.f);

	sprintf_s(string, stringlen, "CPU %4.2f buffers/s, %8.2f Mbyte/s",
		buffersPerSecondCPU, (buffersPerSecondCPU * dataSize));
}
