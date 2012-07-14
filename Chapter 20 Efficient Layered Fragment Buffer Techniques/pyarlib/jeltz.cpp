/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <assert.h>

#include <GL/glew.h>

#include <GL/gl.h>

#include "jeltz.h"
#include "shader.h"

JeltzPlugin::JeltzPlugin()
{
	enable = true;
	jeltz = NULL;
}
JeltzPlugin::~JeltzPlugin()
{
}
void JeltzPlugin::init()
{
}
void JeltzPlugin::update(float dt)
{
}
void JeltzPlugin::display()
{
}
void JeltzPlugin::cleanup()
{
}

#define CHECKSDLERROR _checkSDLError(__FILE__, __LINE__)
bool _checkSDLError(const char* file, int line)
{
	const char *error = SDL_GetError();
	if (*error != '\0')
	{
		printf("SDL Error: %s(%i): %s\n", file, line, error);
		SDL_ClearError();
		return true;
	}
	return false;
}

int cmpver(SDL_version* ver1, SDL_version* ver2) {
	if (ver1->major == ver2->major && ver1->minor == ver2->minor && ver1->patch == ver2->patch)
		return 0;
	if (ver1->major > ver2->major ||
		(ver1->major == ver2->major && ver1->minor > ver2->minor) ||
		(ver1->major == ver2->major && ver1->minor == ver2->minor && ver1->patch > ver2->patch))
		return 1;
	return -1;
}

std::string strLower(std::string s)
{
	for (std::string::iterator it = s.begin(); it != s.end(); ++it)
		*it = tolower(*it);
	return s;
}

const char* getButtonName(Uint8 button)
{
	switch (button)
	{
	case SDL_BUTTON_LEFT: return "LButton";
	case SDL_BUTTON_MIDDLE: return "MButton";
	case SDL_BUTTON_RIGHT: return "RButton";
	case SDL_BUTTON_X1: return "XButton1";
	case SDL_BUTTON_X2: return "XButton2";
	default: return "Undefined";
	}
}

Jeltz::Jeltz(const char* name)
{
	changeFullscreen = false;
	isFullscreen = false;
	justWarped = false;
	hasFocus = false;
	hasResized = false;
	hasResizedNF = true;
	hasInit = false;
	int len = strlen(name);
	windowTitle = new char[len+1];
	memcpy(windowTitle, name, len+1);
	running = false;
	numPlugins = 0;
	windowSize = vec2i(0);
	windowedSize = windowSize;
	mousePosition = vec2i(0);
	mouseDelta = vec2i(0);
	mouseWheelDelta = vec2i(0);
}
Jeltz::~Jeltz()
{
	delete[] windowTitle;
}
void Jeltz::add(JeltzPlugin* plugin)
{
	if (numPlugins < (int)(sizeof(plugins) / sizeof(JeltzPlugin*)))
	{
		plugin->jeltz = this;
		plugins[numPlugins++] = plugin;
	}
}
void Jeltz::inputDown(const char* name)
{
	keyStates.insert(name);
	keysDown.insert(name);
	//printf("%s\n", name);
}
void Jeltz::inputUp(const char* name)
{
	keyStates.erase(name);
	keysUp.insert(name);
}
void Jeltz::focusChanged(bool inFocus)
{
	if (inFocus)
	{
		hasFocus = true;
		if (hasResizedNF)
			printf("Resize: %i %i\n", windowSize.x, windowSize.y);
	}
	else
	{
		hasFocus = false;
		hasResizedNF = false;
	}
}
#if JELTZ_USE_SDL13
void Jeltz::processEventWindow(SDL_WindowEvent& window)
{
	switch (window.event)
	{
	case SDL_WINDOWEVENT_RESIZED:
		reshape(window.data1, window.data2);
		break;
	case SDL_WINDOWEVENT_FOCUS_GAINED:
		focusChanged(true);
		break;
	case SDL_WINDOWEVENT_FOCUS_LOST:
		focusChanged(false);
		break;
	}
}
void Jeltz::processEventMouseWheel(SDL_MouseWheelEvent& wheel)
{
	mouseWheelDelta.x = wheel.x;
	mouseWheelDelta.y = wheel.y;
}
#else
void Jeltz::processEventActive(SDL_ActiveEvent active)
{
	if (active.state & (SDL_APPINPUTFOCUS | SDL_APPACTIVE))
		focusChanged(active.gain != 0);
}
void Jeltz::processEventResize(SDL_ResizeEvent resize)
{
	if (ignoreNextSDLWindowResizeEvenet)
	{
		ignoreNextSDLWindowResizeEvenet  = false;
		return;
	}
	
#ifndef _WIN32
	//for some reason windows can't resize the surface
	surface = SDL_SetVideoMode(resize.w, resize.h, surfaceBpp, surfaceFlags);
#endif
	reshape(resize.w, resize.h);
}
void Jeltz::processEventExpose(SDL_ExposeEvent expose)
{
}
#endif
void Jeltz::processEventKeyboard(SDL_KeyboardEvent& key)
{
	std::string name = strLower(SDL_GetKeyName(key.keysym.sym));
	bool down = (key.state == SDL_PRESSED);
	bool changed = button(name.c_str()) != down;
	if (changed && down)
		inputDown(name.c_str());
	if (changed && !down)
		inputUp(name.c_str());
}
void Jeltz::processEventMouseMove(SDL_MouseMotionEvent& motion)
{
	//printf("b %i,%i %i,%i\n", motion.x, motion.y, motion.xrel, motion.yrel);
	
	vec2i relativeMotion = vec2i(motion.xrel, motion.yrel);

	bool handledWarp = false;
	if (!justWarped)
	{
		mouseDelta += relativeMotion;
	}
	else
	{
		handledWarp = true;
		justWarped = false;
	}
	//mouseDelta.x = motion.x - mousePosition.x;
	//mouseDelta.y = motion.y - mousePosition.y;
	
	//SDL_GetRelativeMouseState(&mouseDelta.x, &mouseDelta.y);
	
	vec2i predict = mousePosition + relativeMotion;
	
	mousePosition.x = motion.x;
	mousePosition.y = motion.y;
	
	vec2i difference = predict - mousePosition;

	//SDL 1.3 doesn't take into account the mouse position is
	//clamped to the edge of the window, hence the relative movement
	//is an incorrect delta from the window's edge.
	//This fix replaces the incorrect relative motion with its delta.
	#if !defined(_WIN32) && JELTZ_USE_SDL13
	static vec2i lastMotion = vec2i(0);
	if (isMouseDown && (mousePosition.x == 0 ||	mousePosition.x == windowSize.x-1))
		mouseDelta.x -= lastMotion.x;
	if (isMouseDown && (mousePosition.y == 0 ||	mousePosition.y == windowSize.y-1))
		mouseDelta.y -= lastMotion.y;
	lastMotion = relativeMotion;
	#endif
	
	//printf("a %i,%i %i,%i\n", mousePosition.x, mousePosition.y, mouseDelta.x, mouseDelta.y);
	
	//wrap mouse before dragging outside the window - I find this disorientating
	#if 0
	//SDL 1.3 crashes on warp mouse
	#if !JELTZ_USE_SDL13
	if (isMouseDown && (mousePosition.x == 0 || mousePosition.x == windowSize.x-1))
	{
		int m = mousePosition.x > 0 ? 10 : -10;
		SDL_WarpMouse(windowSize.x - mousePosition.x + m , mousePosition.y);
		justWarped = true;
	}
	if (isMouseDown && (mousePosition.y == 0 || mousePosition.y == windowSize.y-1))
	{
		int m = mousePosition.y > 0 ? 10 : -10;
		SDL_WarpMouse(mousePosition.x, windowSize.y - mousePosition.y + m);
		justWarped = true;
	}
	#endif
	#endif
}
void Jeltz::processEventMouseButton(SDL_MouseButtonEvent& btn)
{
	std::string name = strLower(getButtonName(btn.button));
	bool down = (btn.state == SDL_PRESSED);
	//bool changed = button(name.c_str()) != down;
	if (down)
		inputDown(name.c_str());
	if (!down)
		inputUp(name.c_str());
	
	//update isMouseDown (is any button down)
	if (down)
	{
		isMouseDown = true;
		//SDL_WM_GrabInput(SDL_GRAB_ON); //REALLY BUGGY!!!
	}
	else
	{
		if (!button("LButton") &&
			!button("MButton") &&
			!button("RButton") &&
			!button("XButton1") &&
			!button("XButton2"))
		{
			isMouseDown = false;
			//SDL_WM_GrabInput(SDL_GRAB_OFF);
			//SDL_ShowCursor(SDL_ENABLE);
		}
	}
}
void Jeltz::processEvents()
{
	SDL_Event event;
	
	hasResized = false;
	keysDown.clear();
	keysUp.clear();
	mouseDelta = vec2i(0);
	mouseWheelDelta = vec2i(0);

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			quit();
			break;
#if JELTZ_USE_SDL13
		case SDL_WINDOWEVENT:
			processEventWindow(event.window);
			break;
		case SDL_MOUSEWHEEL:
			processEventMouseWheel(event.wheel);
			break;
#else
		case SDL_ACTIVEEVENT:
			processEventActive(event.active);
			break;
		case SDL_VIDEORESIZE:
			processEventResize(event.resize);
			break;
		case SDL_VIDEOEXPOSE:
			processEventExpose(event.expose);
			break;
#endif
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			processEventKeyboard(event.key);
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			processEventMouseButton(event.button);
			break;
		case SDL_MOUSEMOTION:
			processEventMouseMove(event.motion);
			break;
   		default:
   			break;
		}
	}
}
void Jeltz::reshape(int w, int h)
{
	//printf("Reshape %ix%i\n", w, h);

	if (w == windowSize.x && h == windowSize.y)
		return;
	
	windowSize = vec2i(w, h);
	if (!isFullscreen)
		windowedSize = windowSize;
	
	hasResized = true;
	hasResizedNF = true;
	glViewport(0, 0, w, h);
}
bool Jeltz::init()
{
	//check version numbers
	SDL_version versionCompiled;
	SDL_VERSION(&versionCompiled);
	SDL_version versionLinked;
#if JELTZ_USE_SDL13
	SDL_GetVersion(&versionLinked);
#else
	versionLinked = *SDL_Linked_Version();
#endif
	fprintf(stderr, "SDL version %i.%i.%i.\n", versionCompiled.major, versionCompiled.minor, versionCompiled.patch);
	int versionCmp = cmpver(&versionCompiled, &versionLinked);
	if (versionCmp != 0)
	{
		printf("\n\tWarning: linked with a %s version (%i.%i.%i).\n\nPress enter to try anyway.\n",
			versionCmp==1?"lower":"higher",
			versionLinked.major, versionLinked.minor, versionLinked.patch);
		while (getchar() != '\n')
			; //NOP
	}

	int initResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	if (initResult < 0)
	{
		printf("Error: Unable to initialize SDL\n");
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, DEFAULT_DEPTH);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, DEFAULT_STENCIL);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	const SDL_VideoInfo* info = SDL_GetVideoInfo();
	if (!info)
	{
		printf("Error: Unable to query video info\n");
		return false;
	}
	desktopRes.x = info->current_w;
	desktopRes.y = info->current_h;

#if JELTZ_USE_SDL13
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	
	window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		DEFAULT_WIDTH, DEFAULT_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!window)
	{
		printf("Error: Unable to create window\n");
		return false;
	}
	
	CHECKSDLERROR;
	
	glcontext = SDL_GL_CreateContext(window);
#else
	surfaceBpp = info->vfmt->BitsPerPixel;
	surfaceFlags = SDL_GL_DOUBLEBUFFER | SDL_OPENGL | SDL_RESIZABLE;
	surface = SDL_SetVideoMode(DEFAULT_WIDTH, DEFAULT_HEIGHT, surfaceBpp, surfaceFlags);
	if (!surface)
	{
		printf("Error: Unable to create SDL surface\n");
		return false;
	}

	hasFocus = true;
#endif
	
	//no vsync by default
	vsync(false);

	int isDoubleBuffered;
	SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &isDoubleBuffered);
	if (!isDoubleBuffered)
	{
		printf("Error: Unable to create double buffered context\n");
	}
	
	int err;
	if ((err = glewInit()) != GLEW_OK)
	{
		printf("Error: Unable to initialize GLEW: %s\n", glewGetErrorString(err));
		return false;
	}
	
	// for windows consistency
	glEnable(GL_DEPTH_TEST);
	hasInit = true;
	return true;
}
void Jeltz::updateWindow()
{
	if (isFullscreen != changeFullscreen)
	{
		isFullscreen = changeFullscreen;
		#if JELTZ_USE_SDL13
			if (isFullscreen)
			{
				printf("fullscreen %ix%i\n", desktopRes.x, desktopRes.y);
				SDL_SetWindowSize(window, desktopRes.x, desktopRes.y);
			}
			SDL_SetWindowFullscreen(window, isFullscreen?SDL_TRUE:SDL_FALSE);
			if (!isFullscreen)
			{
				printf("windowed %ix%i\n", windowedSize.x, windowedSize.y);
				SDL_SetWindowSize(window, windowedSize.x, windowedSize.y);
				reshape(windowedSize.x, windowedSize.y);
			}
		#else
		#ifdef _WIN32
			printf("Error: Fullscreen SDL 1.2 on windows would destroy the GL context\n");
		#else
			//really?
			if (isFullscreen)
			{
				unsigned int newflags = (surface->flags & ~SDL_RESIZABLE) | SDL_FULLSCREEN;
				printf("fullscreen %ix%i\n", desktopRes.x, desktopRes.y);
				surface = SDL_SetVideoMode(desktopRes.x, desktopRes.y, surface->format->BitsPerPixel, newflags);
				
				//must manually call - silly SDL won't fullscreen with RESIZABLE and won't post events without
				reshape(desktopRes.x, desktopRes.y);
			}
			else
			{
				unsigned int newflags = (surface->flags & (~SDL_FULLSCREEN)) | SDL_RESIZABLE;
				printf("windowed %ix%i\n", windowedSize.x, windowedSize.y);
				surface = SDL_SetVideoMode(windowedSize.x, windowedSize.y, surface->format->BitsPerPixel, newflags);
				
				//SDL bug - resize event returns WRONG values
				ignoreNextSDLWindowResizeEvenet = true;
				reshape(windowedSize.x, windowedSize.y);
			}
		#endif
		#endif
		CHECKSDLERROR;
	}
}
void Jeltz::run()
{
	if (!hasInit)
	{
		printf("Cannot run main loop - have not yet successfully initialized.\n");
		return;
	}
	
	//SDL_ShowWindow(window);
	//SDL_RaiseWindow(window);
	//SDL_WM_GrabInput(SDL_GRAB_ON);
	
	unsigned int lastTime, thisTime, deltaTime;
	
	running = true;
	lastTime = SDL_GetTicks();
	int w, h;
#if JELTZ_USE_SDL13
	SDL_GetWindowSize(window, &w, &h);
#else
	w = surface->w;
	h = surface->h;
#endif
	reshape(w, h);
	
	//init plugins
	for (int i = 0; i < numPlugins; ++i)
		plugins[i]->init();

	//start main loop
	while (running)
	{
		thisTime = SDL_GetTicks();
		deltaTime = thisTime - lastTime;
		lastTime = thisTime;
		float dt = deltaTime * 0.001f;
		
		updateWindow();
	
		for (int i = 0; i < numPlugins; ++i)
			if (plugins[i]->enable)
				plugins[i]->update(dt);
		
		if (callbackUpdate)
			callbackUpdate(dt);

		if (hasFocus)
		{
			if (callbackDisplay)
				callbackDisplay();
		
			for (int i = 0; i < numPlugins; ++i)
				if (plugins[i]->enable)
					plugins[i]->display();
			
#if JELTZ_USE_SDL13
			SDL_GL_SwapWindow(window);
#else
			SDL_GL_SwapBuffers();
#endif
		}
		
		processEvents();
		
		if (button("Escape"))
			break;
	}
	
	for (int i = 0; i < numPlugins; ++i)
		plugins[i]->cleanup();
	
	//many shaders are declared static.
	//this releases them before their destructors are run
	//(which would happen after the context free, causing errors)
	Shader::releaseAll();
	
#if JELTZ_USE_SDL13
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
#else
	SDL_FreeSurface(surface);
#endif
	SDL_Quit();
}
void Jeltz::use(const char* name)
{
	std::string n = strLower(name);
	keysDown.erase(n);
	keyStates.erase(n);
	keysUp.erase(n);
}
bool Jeltz::button(const char* name)
{
	return keyStates.find(strLower(name)) != keyStates.end();
}
bool Jeltz::buttonDown(const char* name)
{
	return keysDown.find(strLower(name)) != keysDown.end();
}
bool Jeltz::buttonUp(const char* name)
{
	return keysUp.find(strLower(name)) != keysUp.end();
}
vec2i Jeltz::mousePos()
{
	return mousePosition;
}
vec2i Jeltz::mouseMove()
{
	return mouseDelta;
}
vec2i Jeltz::mouseWheel()
{
	return mouseWheelDelta;
}
bool Jeltz::resized()
{
	return hasResized;
}
bool Jeltz::focused()
{
	return hasFocus;
}
void Jeltz::quit()
{
	running = false;
}
void Jeltz::vsync(bool enable)
{
#if JELTZ_USE_SDL13
	SDL_GL_SetSwapInterval(enable?1:0);
#else
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, enable?1:0);
#endif
}

vec2i Jeltz::winSize()
{
	return windowSize;
}

void Jeltz::fullScreen(bool enable)
{
	if (changeFullscreen == enable)
		return;
		
	//flag is set so window should change in updateWindow()
	changeFullscreen = enable;
}

