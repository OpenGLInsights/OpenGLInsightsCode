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

#include "base.h"
#include "app.h"

#include "gl/glew.h"
#include "gl/wglew.h"

#include <Windows.h>
#include <Windowsx.h>
#include <stdio.h>

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

LRESULT WINAPI MsgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	static base::app *myapp=0;

	switch(msg) {
	case WM_CREATE:
		myapp=reinterpret_cast<base::app*>(
			reinterpret_cast<const CREATESTRUCT*>(lParam)->lpCreateParams);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		ValidateRect(hwnd,0);
		break;

	case WM_MOUSEMOVE:
		myapp->mouse_move(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case WM_KEYDOWN:
		if(wParam == VK_ESCAPE) {
			PostQuitMessage(0);
		}
		else {
			if((lParam & (1<<30)) == 0)
				myapp->key(wParam,true);
		}
		break;

	case WM_KEYUP:
		myapp->key(wParam,false);
		break;

	case WM_ACTIVATEAPP:
		myapp->activated(wParam == TRUE);
		break;
	}

	return DefWindowProc(hwnd,msg,wParam,lParam);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void create_window_win(
	base::app *a,
	HWND *hwnd,
	const char *window_name,
	const char *class_name,
	int width,
	int height)
{
	WNDCLASSEX wc={ 
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		MsgProc,
		0L,
		0L,
		GetModuleHandle(0),
		0,0,0,0,
		class_name,
		0 
	};

	if(RegisterClassEx(&wc)==0)
		throw base::exception(SRC_LOCATION_STR)
			<< "RegisterClassEx failed! (err:" << GetLastError() << ")";

	RECT wr={0,0,width,height};
	DWORD exstyle=WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU;

	if(!AdjustWindowRectEx(&wr,exstyle,FALSE,0))
		throw base::exception(SRC_LOCATION_STR)
			<< "Cannot adjust window rect (err:" << GetLastError() << ")";

    width=abs(wr.left)+wr.right;
    height=abs(wr.top)+wr.bottom;

	*hwnd = CreateWindow(
		class_name,
		window_name,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		0,0,
		width,height,
		GetDesktopWindow(),
		0,
		wc.hInstance,
		a);
	if(*hwnd == 0)
		throw base::exception(SRC_LOCATION_STR)
		<< "CreateWindow failed! (err:" << GetLastError() << ")";

	ShowWindow(*hwnd,SW_SHOWDEFAULT);
	UpdateWindow(*hwnd);

	ShowCursor(0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

HWND __hwnd = 0;
HDC __hdc = 0;
HGLRC __shared_ctx = 0;
/*bool __use_dbg_context = false;
bool __use_dbg_sync = false;
bool __use_pinned_memory = false;
bool __nvidia_fast_download = false;*/

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void* base::get_window_handle() { return __hwnd; }

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::swap_buffers() { SwapBuffers(__hdc); }

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::init_opengl_win(const bool create_shared_ctx)
{
	HDC hdc = GetWindowDC(__hwnd);
	if(hdc == 0)
		throw base::exception(SRC_LOCATION_STR)
			<< "GetWindowDC failed! (err:" << GetLastError() << ")";

	int pf;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
		1,								// version number
		PFD_DRAW_TO_WINDOW |			// support window
		PFD_SUPPORT_OPENGL |			// support OpenGL
		PFD_DOUBLEBUFFER |				// double buffered
		PFD_GENERIC_ACCELERATED |
		PFD_SWAP_COPY,					// dont copy just exchange
		PFD_TYPE_RGBA,					// RGBA type
		32,								// 24-bit color depth
		0, 0, 0, 0, 0, 0,				// color bits ignored
		0,								// no alpha buffer
		0,								// shift bit ignored
		0,								// no accumulation buffer
		0, 0, 0, 0,						// accum bits ignored
		24,								// 32-bit z-buffer
		8,								// 8-bit stencil buffer
		0,								// no auxiliary buffer
		PFD_MAIN_PLANE,					// main layer
		0,								// reserved
		0, 0, 0							// layer masks ignored
	};

	if((pf = ChoosePixelFormat(hdc, &pfd)) == 0) 
		throw base::exception(SRC_LOCATION_STR) << "ChoosePixelFormat failed!";

	if(SetPixelFormat(hdc, pf, &pfd) == 0) 
		throw base::exception(SRC_LOCATION_STR) << "SetPixelFormat failed!";

	HGLRC hrc;

	if((hrc=wglCreateContext(hdc)) == 0) 
		throw base::exception(SRC_LOCATION_STR) << "wglCreateContext failed!";

	if(wglMakeCurrent(hdc,hrc) == 0)
		throw base::exception(SRC_LOCATION_STR) << "wglMakeCurrent failed!";

    glewExperimental=GL_TRUE;

	const GLenum err=glewInit();
	if(GLEW_OK != err)
		throw base::exception(SRC_LOCATION_STR) << "glewInit failed!";

    if(wglewIsSupported("WGL_ARB_create_context") != 1)
		throw base::exception(SRC_LOCATION_STR)
			<< "WGL_ARB_create_context is not supported! (is it OpenGL 3.2 capable card?)";

    wglMakeCurrent(0,0);
	wglDeleteContext(hrc);

	int attribs[]={
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, cfg().use_debug_context ? WGL_CONTEXT_DEBUG_BIT_ARB : 0,
		0
	};

	if(0 == (hrc = wglCreateContextAttribsARB(hdc, 0, attribs)))
        throw base::exception(SRC_LOCATION_STR) << "wglCreateContext failed!";

	if(create_shared_ctx)
		__shared_ctx = wglCreateContextAttribsARB(hdc, hrc, 0);

    if(!wglMakeCurrent(hdc, hrc))
		throw base::exception(SRC_LOCATION_STR) << "wglMakeCurrent failed!";
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void WINAPI gl_debug_msg_proc_arb(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	GLvoid *userParam)
{
	userParam;
	//message;
	length;
	severity;
	id;
	type;
	source;
	
	printf("%s\n",message);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void APIENTRY gl_debug_msg_proc_amd(
	GLuint id,
	GLenum category,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	GLvoid* userParam)
{
	userParam;
	//message;
	length;
	severity;
	category;
	id;

	printf("%s\n",message);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::init_opengl_dbg_win()
{
	if(cfg().use_debug_context) {
		glDebugMessageControlARB=
			(PFNGLDEBUGMESSAGECONTROLARBPROC)wglGetProcAddress("glDebugMessageControlARB");
		glDebugMessageCallbackARB=
			(PFNGLDEBUGMESSAGECALLBACKARBPROC)wglGetProcAddress("glDebugMessageCallbackARB");
		glDebugMessageInsertARB=
			(PFNGLDEBUGMESSAGEINSERTARBPROC)wglGetProcAddress("glDebugMessageInsertARB");

		if(GLEW_AMD_debug_output) {
			glDebugMessageCallbackAMD(gl_debug_msg_proc_amd,0);
			glDebugMessageEnableAMD(0,0,0,0,GL_TRUE);
		}
		else if(glDebugMessageControlARB 
			&& glDebugMessageCallbackARB 
			&& glDebugMessageInsertARB) {
			glDebugMessageCallbackARB(&gl_debug_msg_proc_arb,0);
			glDebugMessageControlARB(GL_DONT_CARE,GL_DONT_CARE,GL_DONT_CARE,0,0,GL_TRUE);

			if(cfg().use_debug_context && cfg().use_debug_sync)
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		}
		else {
			std::cout<<"No OpenGL debug extension..."<<std::endl;
		}
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::run_app_win(base::app *a, const bool initgl)
{
	create_window_win(
		a,
		&__hwnd,
		a->get_wnd_name(),
		a->get_wnd_cls(),
		a->get_wnd_width(),
		a->get_wnd_height());

	__hdc = GetWindowDC(__hwnd);
	if(!__hdc)
		throw base::exception(SRC_LOCATION_STR)
			<< "Cannot get device context!";

	if(initgl) {
		init_opengl_win();
		init_opengl_dbg_win();
	}

	MSG msg;

	a->start();

	hptimer timer;
	timer.start();

	double start_time = 0.0;
	int nb_frames = 0;

	for( ;; ) {
		bool exit = false;
		while(PeekMessage(&msg, 0, 0U, 0U, PM_REMOVE) != 0) {
			if(msg.message==WM_QUIT)
				exit = true;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		if(exit) break;

		a->draw_frame();
		a->inc_frame_number();

		if(initgl)
			swap_buffers();

		nb_frames++;

		double current_time = timer.elapsed_time();
		if (current_time - start_time > 1000.0){
			printf(
				"%.3f ms/frame, %.1f FPS\n",
				(current_time-start_time) / double(nb_frames),
				double(nb_frames) * (current_time-start_time) * 0.001);
			nb_frames = 0;
			start_time = current_time;
		}
	}

	a->stop();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

glm::ivec2 base::get_mouse_pos()
{
	POINT pos;
	GetCursorPos(&pos);
	return glm::ivec2(pos.x,pos.y);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::set_mouse_pos(const glm::ivec2 &pos)
{
	SetCursorPos(int(pos.x),int(pos.y));
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::sleep_ms(const int time)
{
	Sleep(time);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void* base::get_shared_context() 
{
	return __shared_ctx;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void* base::get_window_hdc()
{
	return __hdc;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::make_current_shared_context()
{
	wglMakeCurrent(
		(HDC)base::get_window_hdc(),
		(HGLRC)base::get_shared_context());
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
