#include <stdlib.h>
#include <windows.h>
#include <assert.h>
#include <GL/glew.h>
#include <nvTime.h>
#include <cutil.h>
#include "Data.h"
#include "nvThread.h"
#include "framebufferObject.h"
#include "renderbuffer.h"

using namespace nv;

#define GL_GETERROR                                                     \
{                                                                       \
	GLenum  err = glGetError();                                         \
	char    cbuf[64];                                                   \
	if (err != GL_NO_ERROR) {                                           \
	wsprintf((LPTSTR)cbuf, (LPCTSTR)"[%s line %d] GL Error: %s\n",  \
	__FILE__, __LINE__, gluErrorString(err));                     \
	MessageBox(NULL, cbuf, "Error", MB_OK);                         \
	exit(0);                                                        \
	}                                                                   \
}

#define ERR_MSG(STR)                                                    \
	printf("\nERROR - "STR"\n");                                    \
	exit(0); 

GLuint gWinWidth = 512;		// Window Width
GLuint gWinHeight = 512;	// Window Height
GLuint gOffscreenWidth = 4096;	// offscreen render Width - make this really big to show the benefit of copy engines
GLuint gOffscreenHeight = 2048;	// offscreen render Height - make this really big to show the benefit of copy engines
const unsigned int gNumIter = 200; //how many iterations do we want the test to run

bool gUse3d = false; //default is using 2d texture, if 3d is set to true make sure values below for width, height and depth make sense

const char* volumeFilename = "./data/volume-"; //the volumetric time series only used if gUse3d is true
unsigned int gWidth = 4096; //image or volume width
unsigned int gHeight = 2048; //image or volume height
unsigned int gDepth = 1; //image or volume depth, not used for 2d images
unsigned int gNumTimesteps = 5; //no of volume or image timesteps

//By default lets start with gpu async ie using copy engines
TransferMode gDownloadMode = TRANSFER_GPUASYNC;
TransferMode gReadbackMode = TRANSFER_NONE;

Data* theData = 0;
FrameTimer		gFrameTimer;

HWND hWin = NULL;
HDC winDC = NULL;
//context for the main rendering, download and readback
HGLRC renderRC = NULL, downloadRC = NULL, readbackRC = NULL;

static unsigned DownloadFunc(void *param);
static unsigned ReadbackFunc(void *param);

//exit event
nvEvent evExitDownload;
nvEvent evExitReadback;

//OFFSCREEN RENDERING
FramebufferObject *gFBO = NULL; //Instance of fbo class defined in framebufferObject.h
//Readback related events : readback waits on render
const int numReadbackBuffers = 4;
//offscreen renderbuffers where rendering is directed to
GLuint readbackTexID[numReadbackBuffers];

//GENERAL READBACK
//pbo's for async readback from the fbo attachments
#define PING_PONG
#ifdef PING_PONG
GLuint readbackPBO[2];
#else
GLuint readbackPBO[numReadbackBuffers];
#endif
int curPBO = 0; //cur pbo used in the ping pong

//FOR READBACK WITH COPY ENGINES
//signal to readback thread to start waiting on GL fence 
nvEvent startReadbackFenceValid[numReadbackBuffers];
//OGL sync at the point where the readpixels has completed
GLsync startReadbackFence[numReadbackBuffers];
//OGL sync at the point where the readpixels has completed
GLsync endReadbackFence[numReadbackBuffers];
//Event to signal that the readback GL fence was created
nvEvent endReadbackFenceValid[numReadbackBuffers] ; 

unsigned char* dummy; //this is where memcpy puts the readback buffer to

const char* getTransferString(TransferMode mode) {
	switch (mode)
	{
	case TRANSFER_SYNC:
		return "Synchronous";
	case TRANSFER_CPUASYNC:
		return "CPU Asynchronous";
	case TRANSFER_GPUASYNC:
		return "GPU Asynchronous (Copy Engines)";
	default :
		return "None";
	}
	};
void GLInfo() {
	printf("\nOpenGL vendor: %s\n", glGetString(GL_VENDOR));
	printf("OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version: %s\n", glGetString(GL_VERSION));
}

void drawScene() {
	static float angle = 0.0f;
	glViewport(0, 0, (GLsizei)gOffscreenWidth, (GLsizei)gOffscreenHeight);
	glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
	//better to add this as we never know - renderers may messup the projection 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, 1, 100);
	//gluPerspective(30.0, (float) gOffscreenWidth / (float) gOffscreenHeight, 0.01, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0,0.0,-3.0);
//	glRotatef(angle, 0.0,1.0,0.0); //dont rotate as that may affect fps esp for 3d volumes
	theData->render(); 
	angle += 1.0f;
}

void doFrame() {
	static int curRender = 0;
	gFrameTimer.newFrame();

	//PRERENDER: Do whatever needs to be done before render, so either wait for the texture handle if using download ce or step to next texture
	theData->startRender();
	//THE REAL DRAWING
	gFBO->Bind(); //drawing to fbo
	if (gReadbackMode == TRANSFER_GPUASYNC) {//USING COPY ENGINES
		//if readback is still using this renderbuffer for some reason (this can happen if render is too fast and fills up all the renderbuffers)
		nvWaitForEvent(endReadbackFenceValid[curRender]); 
		//	fprintf(stderr,"\nRender : Got permission to render to attachment[%d] \n",curRender);
		//wait for readback to give us permission to render to this renderbuffer
		glWaitSync(endReadbackFence[curRender], 0, GL_TIMEOUT_IGNORED); //arg 2 and 3 must be 0 and GL_TIMEOUT_IGNORED resp
		glDeleteSync(endReadbackFence[curRender]);	endReadbackFence[curRender] = NULL;
	}	
	//attach the right fbo attachment
	gFBO->AttachTexture(GL_TEXTURE_2D, readbackTexID[curRender]);
	gFBO->IsValid();
//	glDrawBuffer(attachment[curRender]); 
	drawScene();	

	//POSTRENDER - what should be done at end of render, if using CE signal we are done with texture, increment the next frame
	theData->endRender();

	//READBACK 
	//after render, signal to readback that we are done with the current renderbuffer
	if (gReadbackMode == TRANSFER_GPUASYNC) {//USING COPY ENGINES
		gFBO->AttachTexture(GL_TEXTURE_2D, 0); //must do so that readback can access this textrue
		FramebufferObject::Disable();
		// Create a fence that readback can wait on before it does a readpixel on this renderbuffer
		startReadbackFence[curRender] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		glFlush(); //make sure GL send this out
		// Tell the readback thread that fence is now valid to use. Otherwise the readback thread may reach to the waitsync before we even created the glfence
		nvSignalEvent(startReadbackFenceValid[curRender]);
		//fprintf(stderr,"Render : Finished rendering created fence [%d]\n",curRender);
	}
	else if (gReadbackMode == TRANSFER_CPUASYNC ) { //no CE just plain readback
		//Texture must already be attached here
		//READBACK-1 - Current frame is readback from fbo ->pbo
		glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, readbackPBO[curPBO]);
		glReadPixels(0, 0, gOffscreenWidth, gOffscreenHeight, GL_RGBA, GL_UNSIGNED_BYTE, 0); //async
		FramebufferObject::Disable();
		//READBACK-2 Previous frame is readback from pbo->app
		glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, readbackPBO[1-curPBO]);
		void* data = glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY);
		assert(data);
		//DO MEMCPY TO YOUR APP BUFFERS
		memcpy(dummy,data,gOffscreenWidth*gOffscreenHeight*4*sizeof(GLubyte));			
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
		glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
		curPBO = 1-curPBO;
	}
	else if (gReadbackMode == TRANSFER_SYNC) {
		//Texture must already be attached here
		glReadPixels(0, 0, gOffscreenWidth, gOffscreenHeight, GL_RGBA, GL_UNSIGNED_BYTE, dummy); //async
		FramebufferObject::Disable();
	}
	else {//no readback at all
		FramebufferObject::Disable();
	}

	//Now Blit to onscreen window
	glDrawBuffer(GL_BACK); {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glViewport(0,0,gWinWidth,gWinHeight);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, 1, 0, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		////Blit from FBO to screen
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, gFBO->GetID());
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
		glBlitFramebuffer(0, 0, gOffscreenWidth, gOffscreenHeight, 0, 0, gWinWidth, gWinHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
	}
	SwapBuffers(winDC);
	curRender = (curRender+1)%numReadbackBuffers;
	GL_GETERROR
}

//Anything that needs a valid OGL context goes in here
void oglInit(void)
{
	glewInit();
	if (!glewIsSupported("GL_VERSION_2_0 GL_VERSION_1_5 GL_EXT_framebuffer_object GL_ARB_multitexture GL_ARB_vertex_buffer_object")) {
		fprintf(stderr, "Required OpenGL extensions missing.");
		exit(-1);
	}
	GLint64 timeoutMaxServer;
	GLint value;

	// Check we can read some constants.
	glGetInteger64v(GL_MAX_SERVER_WAIT_TIMEOUT, &timeoutMaxServer);
	if (glGetError() != GL_NO_ERROR) {
		printf("ERROR: got unexpected error from glGetInteger64v\n");
		return;
	}

	// Check the object state.
	GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	if (!fence) {
		printf("ERROR: glFenceSync failed\n");
		return;
	} else if (!glIsSync(fence)) {
		printf("ERROR: fence object didn't pass glIsSync()\n");
		return;
	}
	glGetSynciv(fence, GL_OBJECT_TYPE, 1, NULL, &value);
	if (value != GL_SYNC_FENCE) {
		printf("ERROR: glGetSynciv(GL_OBJECT_TYPE) did not return GL_SYNC_FENCE\n");
		return;
	}
	glGetSynciv(fence, GL_SYNC_CONDITION, 1, NULL, &value);
	if (value != GL_SYNC_GPU_COMMANDS_COMPLETE) {
		printf("ERROR: glGetSynciv(GL_SYNC_CONDITION) did not return GL_SYNC_GPU_COMMANDS_COMPLETE\n");
		return;
	}
	glGetSynciv(fence, GL_SYNC_FLAGS, 1, NULL, &value);
	if (value != 0) {
		printf("ERROR: glGetSynciv(GL_SYNC_FLAGS) was not 0\n");
		return;
	}
	glFinish();
	glGetSynciv(fence, GL_SYNC_STATUS, 1, NULL, &value);
	if (value != GL_SIGNALED) {
		printf("ERROR: glGetSynciv(GL_SYNC_STATUS) was not GL_SIGNALED\n");
		return;
	}
	glDeleteSync(fence);
	if (glGetError() != GL_NO_ERROR) {
		printf("ERROR: got unexpected error from glGetSynciv() sequence\n");
		return;
	}

	//initialize the data here
	if (gUse3d)
		theData = new Data(volumeFilename, gWidth, gHeight, gDepth, gNumTimesteps, gDownloadMode);
	else
		theData = new Data(NULL,gWidth,gHeight,1, gNumTimesteps, gDownloadMode);

	//CREATE PBO'S FOR READBACK
#ifdef PING_PONG
	glGenBuffersARB(2,readbackPBO);
	for (int i=0;i<2;i++) {
		glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, readbackPBO[i]);
		glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, gOffscreenWidth*gOffscreenHeight*sizeof(GLubyte)*4,NULL, GL_STREAM_READ_ARB);
		GL_GETERROR;
	}
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB,0);
#else
	glGenBuffersARB(numReadbackBuffers,readbackPBO);
	for (int i=0;i<numReadbackBuffers;i++) {
		glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, readbackPBO[i]);
		glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, gOffscreenWidth*gOffscreenHeight*sizeof(GLubyte)*4,NULL, GL_STREAM_READ_ARB);
		GL_GETERROR;
	}
	glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB,0);
#endif

	//CREATE FBOs for offscreen rendering
	gFBO = new FramebufferObject;
//	gFBO->Bind();
//	gFBO->IsValid();
	FramebufferObject::Disable();
	
    glGenTextures(numReadbackBuffers, readbackTexID);
	for (int i=0;i<numReadbackBuffers;i++) {
        glBindTexture(GL_TEXTURE_2D, readbackTexID[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, gOffscreenWidth,gOffscreenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
    glBindTexture(GL_TEXTURE_2D, 0);
	
	GL_GETERROR;
	glEnable(GL_DEPTH_TEST);
}

// setPixelFormat()
int setPixelFormat(HDC hDC)
{
	int pf;
	PIXELFORMATDESCRIPTOR pfd;

	/* fill in the pixel format descriptor */
	pfd.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion     = 1;		    /* version (should be 1) */
	pfd.dwFlags      = PFD_DRAW_TO_WINDOW | /* draw to window (not bitmap) */
		PFD_DOUBLEBUFFER|	
		PFD_SUPPORT_OPENGL ; /* draw using opengl */
	pfd.iPixelType   = PFD_TYPE_RGBA;                /* PFD_TYPE_RGBA or COLORINDEX */
	pfd.cColorBits   = 24;
	pfd.cDepthBits = 32;

	// get the appropriate pixel format 
	pf = ChoosePixelFormat(hDC, &pfd);
	if (pf == 0) {
		printf("ChoosePixelFormat() failed:  Cannot find format specified."); 
		return 0;
	} 

	/* set the pixel format */
	if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
		printf("SetPixelFormat() failed:  Cannot set format specified.");
		return 0;
	} 

	return pf;
}    

void oglResize(GLsizei w, GLsizei h)
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;
	gWinWidth = w;
	gWinHeight = h;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, (float) w / (float) h, 0.01, 10.0);

	glMatrixMode( GL_MODELVIEW );
}

// handle window messages
LONG WINAPI winProc(HWND hWin, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LONG ret = 0;
	switch (uMsg) {
		case WM_CLOSE:
			{
				//	if (!UnregisterClass("OpenGL",GetModuleHandle(NULL)))			// Are We Able To Unregister Class
				//	{
				//		printf("Could Not Unregister Class\n");
				//	}
			}
			PostQuitMessage(0);
			break;
		case WM_SIZE:
			oglResize(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_MOVE:
			break;
		case WM_KEYDOWN:
			switch(wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
			}
		case WM_KEYUP:
			return 0;
		default:
			// pass all unhandled messages to DefWindowProc
			ret = DefWindowProc(hWin, uMsg, wParam, lParam);
			break;
	}
	// return 1 if handled message, 0 if not
	return ret;
}

// create window
//also include char* title
HWND WINAPI createWindow(int posx, int posy, int width, int height)
{

	WNDCLASS    wc;
	HWND        hWin;
	static char szAppClassName[] = "OpenGL";
	static bool classRegistered = 0;

	/* get this modules instance */
	HINSTANCE hInstance = GetModuleHandle(NULL);

	// register the frame class
	if (classRegistered == 0) {
		wc.style         = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
		wc.lpfnWndProc   = (WNDPROC)winProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInstance;
		wc.hIcon         = LoadIcon(NULL,IDI_WINLOGO);
		wc.hCursor       = LoadCursor(NULL,IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = szAppClassName;

		if (!RegisterClass(&wc)) return FALSE;
		classRegistered = 1;
	}

	// Create the frame
	hWin = CreateWindow(szAppClassName,
		"Dummy",
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		posx, posy,
		width,
		height,
		NULL,
		NULL,
		hInstance,
		NULL);

	// make sure window was created
	if (!hWin) 
		return FALSE;
	return hWin;
}

//We do have a window proc but not really using it, just want to benchmark 1000 iterations
int main(int argc, char** argv)
{
	char *filename; 
	int n;
	//check for 3d flag to stream volumes and have default dimensions
	if (cutCheckCmdLineFlag(argc, (const char**) argv, "3d")) {
		gUse3d = true;
		gWidth = gHeight = gDepth = 256;
		gNumTimesteps = 5;
	}

	if (cutGetCmdLineArgumentstr( argc, (const char**) argv, "file", &filename)) {
		volumeFilename = filename;
	}
	if (cutGetCmdLineArgumenti( argc, (const char**) argv, "size", &n)) {
		gWidth = gHeight = gDepth = n;
	}
	if (cutGetCmdLineArgumenti( argc, (const char**) argv, "xsize", &n)) {
		gWidth = n;
	}
	if (cutGetCmdLineArgumenti( argc, (const char**) argv, "ysize", &n)) {
		gHeight = n;
	}
	if (cutGetCmdLineArgumenti( argc, (const char**) argv, "zsize", &n)) {
		gDepth = n;
	}	
	if (cutGetCmdLineArgumenti( argc, (const char**) argv, "tsize", &n)) {
		gNumTimesteps = n;
	}
	if (cutGetCmdLineArgumenti( argc, (const char**) argv, "download", &n)) {
		if ((n>=0) && (n < TRANSFER_NUM_MODES))
			gDownloadMode = (TransferMode)n;
	}
	if (cutGetCmdLineArgumenti( argc, (const char**) argv, "readback", &n)) {
		if ((n>=0) && (n<TRANSFER_NUM_MODES))
			gReadbackMode = (TransferMode)n;
	}

	if (gUse3d) { 
		gOffscreenWidth = 768;
		gOffscreenHeight = 768;
	}
	else {//for 2d let  the readback buffer size = width and height
		gOffscreenWidth = gWidth;
		gOffscreenHeight = gHeight;
	}

	hWin = createWindow(500,10,gWinWidth,gWinHeight);
	if (!hWin)
		exit(-1);
	winDC = GetDC(hWin);
	// Set a pixelformat on the win-DC
	if (setPixelFormat(winDC) == 0)
		return NULL;

	//create main render context
	renderRC = wglCreateContext(winDC);
	//Create another context for download if using CE
	if (gDownloadMode == TRANSFER_GPUASYNC) {
		downloadRC = wglCreateContext(winDC);
		wglShareLists(renderRC, downloadRC);
	}
	//Create another context for readback if using CE
	if (gReadbackMode == TRANSFER_GPUASYNC) {
		readbackRC = wglCreateContext(winDC);
		wglShareLists(renderRC, readbackRC);
	}

	//Attach Window DC to render context
	wglMakeCurrent(winDC, renderRC);
	oglInit();
	ShowWindow(hWin, 1);
	UpdateWindow(hWin);
	SetFocus(hWin);
	GL_GETERROR; //check for errors before we return

	//the thread handles
	nvThread downloadThread = NULL;
	nvThread readbackThread = NULL;

	if (gDownloadMode == TRANSFER_GPUASYNC) {
		evExitDownload = nvCreateEvent(); //for the main thread to signal exit to the download thread
		downloadThread = nvCreateThread(&DownloadFunc, theData);
		if (!downloadThread) {
			printf("ERROR: Unable to create our download thread\n");
			return -1;
		}
	}

	if (gReadbackMode == TRANSFER_GPUASYNC) {
		//create readback events
		for (int i=0;i<numReadbackBuffers;i++) {
			startReadbackFenceValid[i] = nvCreateEvent();
			endReadbackFenceValid[i] = nvCreateEvent();
		}
		//thread stuff
		evExitReadback = nvCreateEvent(); //for the main thread to signal exit to the readback thread
		readbackThread = nvCreateThread(&ReadbackFunc, NULL);
		if (!readbackThread) {
			printf("ERROR: Unable to create our readback thread\n");
			return -1;
		}
	}

	dummy = new unsigned char[gOffscreenWidth*gOffscreenHeight*4]; //where the readback frame is copied to - temp 
	//TODO prime the pump here start the first few frames 

	printf("Download: %s\n",getTransferString(gDownloadMode));
	printf("Readback: %s\n",getTransferString(gReadbackMode));

	if (gUse3d)
		printf("3D Texture dims [%d]x[%d]x[%d] size %0.2fMB\n",gWidth, gHeight, gDepth, gWidth*gHeight*gDepth/(1024.0*1024.0));
	else
		printf("2D Texture dims [%d]x[%d] size %0.2fMB\n",gWidth, gHeight, gWidth*gHeight*4/(1024.0*1024.0));
	printf("Starting download-render-readback for %d iterations...\n",gNumIter);
	gFrameTimer.start();
	for (int numit =0; numit < gNumIter;numit++) {
			doFrame();
			GL_GETERROR;
			//printf("Main Thread : Finished iteration %d\n",numit);
	} //end of for loop
	delete [] dummy;

	gFrameTimer.stop();

	//============= MAIN THREAD IS DONE LETS START CLEANUP =================
	printf("Average FPS %f\n",gFrameTimer.getFramerate());

	if (gDownloadMode == TRANSFER_GPUASYNC) { 
		nvSignalEvent(evExitDownload);	
		unsigned int unused;
		if (!nvWaitForAndDestroyThread(downloadThread, &unused))
			printf("Some error in closing threads\n");
		printf("finished destroying download thread\n");
		nvDestroyEvent(evExitDownload);	
	}

	if (gReadbackMode == TRANSFER_GPUASYNC) {
		// Finished: Tell thread it can quit now.
		nvSignalEvent(evExitReadback);
		unsigned int unused;
		if (!nvWaitForAndDestroyThread(readbackThread, &unused))
			printf("Some error in closing threads\n");
		printf("finished destroying readback thread\n");
		nvDestroyEvent(evExitReadback);
		for (int i=0;i < numReadbackBuffers; i++) {
			nvDestroyEvent(startReadbackFenceValid[i]); startReadbackFence[i] = NULL; //Destroy all the events
			nvDestroyEvent(endReadbackFenceValid[i]); endReadbackFence[i] = NULL;	
		}
	}

	if (theData) delete theData;
	delete gFBO;
	glDeleteTextures(numReadbackBuffers, readbackTexID);
#ifdef PING_PONG
	glDeleteBuffers(2,readbackPBO);
#else
	glDeleteBuffers(numReadbackBuffers,readbackPBO);
#endif

	//Time to destroy all the contexts
	wglMakeCurrent(winDC,NULL);
	if (gDownloadMode == TRANSFER_GPUASYNC) {
		wglDeleteContext(downloadRC); 
	}

	if (gReadbackMode == TRANSFER_GPUASYNC) {
		wglDeleteContext(readbackRC);
	}

	wglDeleteContext(renderRC);

//	return msg.wParam;
	return 0;
}

//Called from DownloadThread
unsigned DownloadFunc(void * param)
{
	wglMakeCurrent(winDC, downloadRC);
	for (int i=0;i<gNumIter;i++) {
		((Data *)param)->step();
		//sleep;
	}
	//hang out here waiting for main thread to signal exit
	nvWaitForEvent(evExitDownload);
	//done, lets exit
	wglMakeCurrent(NULL, NULL);
	return 0;
}


//Ideally readback func should do some cpu processing while waiting for GPU data
//to make the best use of the 
//Need to have a separate fbp with shared render buffer since fbo's cant be shared betwene contexts/threads in ogl 3.0
unsigned ReadbackFunc(void * param)
{
	static int curRead = 0;
	unsigned char* dummy = new unsigned char[gOffscreenWidth*gOffscreenHeight*4];

	wglMakeCurrent(winDC, readbackRC);
	//For readback since fbo's cant be shared
	FramebufferObject * gReadbackFBO = new FramebufferObject;
//	gReadbackFBO->Bind();
//	gReadbackFBO->AttachTexture(GL_TEXTURE_2D, readbackTexID[0]);
//	gReadbackFBO->IsValid();
	GL_GETERROR;
	//signal all the rendering 
	for (int i=0;i<numReadbackBuffers;i++) {
		endReadbackFence[i] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);; //queue a fence to signal when we are done with the readpixels
		glFlush(); //make sure GL send this out
		nvSignalEvent(endReadbackFenceValid[i]); //Tell render main thread to start rendering to their attachments
	}
	for (int i=0;i<gNumIter;i++) {
		nvWaitForEvent(startReadbackFenceValid[curRead]);//wait for fence to be created so that we can wait on it
		//1) Current frame is readback from fbo attachment ->pbo
		//fprintf(stderr,"\nReadbackFunc: Before glWaitSync for renderFence[%d] \n",curRead);
		glWaitSync(startReadbackFence[curRead], 0, GL_TIMEOUT_IGNORED); //arg 2 and 3 must be 0 and GL_TIMEOUT_IGNORED resp
		glDeleteSync(startReadbackFence[curRead]);	startReadbackFence[curRead] = NULL;
		{
			//fprintf(stderr, "\nReadbackFunc: After glWaitSync for renderFence[%d] \n",curRead);
			//At this point, main thread has finished render to attachment[curRead]
#ifdef PING_PONG
			glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, readbackPBO[curPBO]);
#else
			glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, readbackPBO[curRead]);
#endif
			gReadbackFBO->AttachTexture(GL_TEXTURE_2D,readbackTexID[curRead]);
			glReadPixels(0, 0, gOffscreenWidth, gOffscreenHeight, GL_BGRA, GL_UNSIGNED_BYTE, 0);
			gReadbackFBO->AttachTexture(GL_TEXTURE_2D,0);
		}
		endReadbackFence[curRead] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);; //queue a fence to signal when we are done with the readpixels
		glFlush(); //make sure GL send this out
		nvSignalEvent(endReadbackFenceValid[curRead]); //Tell render main thread to start rendering to attachment[curRead] because we are done with it
		//fprintf(stderr,"ReadbackFunc: Finished signal: evStartRender[%d] \n",curRead);
		//====== 2. PBO->APP ======

#ifdef PING_PONG
		glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, readbackPBO[1-curPBO]);
#endif
		void* data = (void*)glMapBufferRange(GL_PIXEL_PACK_BUFFER_ARB, 0, gOffscreenWidth*gOffscreenHeight*4 * sizeof(GLubyte), GL_MAP_READ_BIT);
		{		
			assert(data);
			//DO MEMCPY TO YOUR APP BUFFERS
			memcpy(dummy,data,gOffscreenWidth*gOffscreenHeight*4*sizeof(GLubyte));			
		}
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
		curRead = (curRead+1)%numReadbackBuffers;
#ifdef PING_PONG
		curPBO = 1-curPBO; //ping pong between 0 & 1
#endif
		//fprintf(stderr,"ReadbackFunc: Iteration %d\n",i);
	}
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFinish();
	// Don't quit the thread until the master tells us so we don't do
	// an implicit flush.
	delete [] dummy;
//	FramebufferObject::Disable();
//	delete gReadbackFBO;
	//hang out here waiting for main thread to signal exit
	nvWaitForEvent(evExitReadback);
	wglMakeCurrent(NULL, NULL);
	return 0;
}


	// Process application messages until the application closes
//	MSG  msg;
//	bool done = false;
	//while(!done)									// Loop That Runs While done=FALSE
	//{
	//	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
	//	{
	//		if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
	//		{
	//			done=true;	// If So done=TRUE
	//		}
	//		else									// If Not, Deal With Window Messages
	//		{
	//			TranslateMessage(&msg);				// Translate The Message
	//			DispatchMessage(&msg);				// Dispatch The Message
	//		}
	//	}