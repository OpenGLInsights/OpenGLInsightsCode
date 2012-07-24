#include "stdafx.h"

#include "commandLine.h"
#include "queue.h"
#include "window.h"
#include "glContext.h"
#include "stage.h"
#include "timer.h"
#include "stageUpload.h"
#include "stageRender.h"
#include "stageReadback.h"
#include "limits.h"

commandLineOptions options;
Stage *stages[limits::MAX_STAGES];
const int numSharedTextures = (limits::MAX_STAGES-1);
queue queues[numSharedTextures*2]; //for each shared texture, we have 2 queues one specified texture consumed and one for texture produced
GLuint srcTex[limits::MAX_STREAMS];
GLuint resultTex[limits::MAX_STREAMS];
glContext::renderingContext *mainRC;

//struct SharedTexture{
//	queue producedQueue; //ids that have been produced into
//	queue consumedQueue; //ids that have been consumed from
//	GLuint texId[[limits::MAX_STREAMS]; //IDs of all textures that are shared
//};
//
//SharedTexture srcTex;
//SharedTexture resultTex;


unsigned int UPLOAD = 0;
unsigned int RENDER = 1;
unsigned int READBACK = 2;

unsigned int START_UPLOAD = 0;
unsigned int END_UPLOAD = 1;
unsigned int START_READBACK = 2;
unsigned int END_READBACK = 3;


bool init(HINSTANCE hInstance) {
	//Create Window
	window::getInstance().setup(hInstance, 0, 0, "OpenGL Copy Engine performance test");
	//Init the OpenGL context class, setup the pixel format
   glContext::getInstance().init(&options);

   //create main context
    mainRC = glContext::getInstance().createRenderingContext();
	//make it current
	glContext::getInstance().makeCurrent(mainRC); 
	{
		// set the main rendering context so that all subsequent context share with that one
		glContext::getInstance().setMainRC(mainRC);

		glewInit();
		//TODO2 check we have all the extensions we need

		// Create the shared textures for upload-render,
		glGenTextures(options._numStreams, srcTex);
		// Create textures for render-readback
		glGenTextures(options._numStreams, resultTex);

		for(unsigned int stream = 0; stream < options._numStreams; stream++)
		{
			//Source texture to be shared between upload-render
			glBindTexture(GL_TEXTURE_2D, srcTex[stream]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, options._inTexWidth,options._inTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


			glBindTexture(GL_TEXTURE_2D, resultTex[stream]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, options._outTexWidth,options._outTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		// initialize timing in case we need to initialize OGL counters etc
		Timer::getInstance().init(&options); 
	}

	glContext::getInstance().makeUnCurrent();

	//Create the different stages in our pipeline
	stages[0] = new StageUpload();
	stages[1] = new StageRender();
	stages[2] = new StageReadback();
	//Initialize them
	for (unsigned int stage=0; stage < options._numStages; stage++) {
		//init will create the ogl context for each of the stages and initialize all required structures..thread has not started yet
		stages[stage]->init(&options);
	}
	//Connect the the output queues of one stage to the input of the other
	//Synchronization for SrcTex that is shared between upload-render
	//======= UPLOAD-RENDER sync for srcTex ==========
	//waits (input) for start_upload queue, which tells upload to start uploading to srcTex. This is set in initialization stage or by render once it has finished using srcTex
	stages[UPLOAD]->attachInput(0, &queues[START_UPLOAD]);
	//signals(output) the end_upload queue, which tells render, Im done uploading use this texture
	stages[UPLOAD]->attachOutput(0, &queues[END_UPLOAD]);
	//waits (input) for end_upload queue, which tells render upload is done to srcTex to render using it
	stages[RENDER]->attachInput(0, &queues[END_UPLOAD]);
	//signals (output) to start_upload queue, render tells upload that it is done using srcTex to start uploading to it
	stages[RENDER]->attachOutput(0, &queues[START_UPLOAD]);

	//========== RENDER-READBACK sync for resultTex  ===========
	//waits (input) for end_readback queue, which tells render that readback is done from resultTex so render than render to it
	stages[RENDER]->attachInput(1, &queues[END_READBACK]);
	//signals (output) to start_readback queue, render tells upload that it is done using srcTex to start uploading to it
	stages[RENDER]->attachOutput(1, &queues[START_READBACK]);
	////waits (input) for start_readback queue, which tells readback to start reading from resultTex. 
	stages[READBACK]->attachInput(0, &queues[START_READBACK]);
	////signals(output) the end_readback queue, which tells render, Im done reading back from this texture, use it now
	stages[READBACK]->attachOutput(0, &queues[END_READBACK]);

	// this is the last stage
	stages[READBACK]->setLastStage();

	return true;

}

bool startExecution() {
	//start the different stages
	for(unsigned int stage = 0; stage < options._numStages; stage++)
	{
		stages[stage]->start();
	}

	//need ogl context because we are gonna create OGL sync objects
	glContext::getInstance().makeCurrent(mainRC);
	// start the first stage
	queue::element *element;

	//for the upload-render part, upload is waiting for the start_upload which will eventually trigger the end_upload -> render
	for(unsigned int stream = 0; stream < options._numStreams; stream++)
	{
		element = new queue::element();
		element->_id = srcTex[stream];
		//TODO
		//srcTex.consumedQueue.enqueue, signifies that the srctex was already consumed 
		queues[START_UPLOAD].enqueue(element);
	}

	//for the render-download part, render will be waiting for the end_readback queue to render to resultTex and then trigger start_readback -> readback
	for(unsigned int stream = 0; stream < options._numStreams; stream++)
	{
	    element = new queue::element();
	    element->_id = resultTex[stream];
	    queues[START_READBACK].enqueue(element);
	}
	glContext::getInstance().makeUnCurrent();
	return true;
}

/// @brief Main function.
///
/// @param hInstance (in) 
/// @param hPrevInstance (in) 
/// @param lpCmdLine (in) 
/// @param nCmdShow (in) 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
	//Parse command line
    if(!parseCommandLine(&lpCmdLine, &options))
    {
       MessageBox(NULL, "Invalid command line option", "Error", MB_OK);
       return FALSE;
    }
	if (options._numStages < 2) {
		printf("Must have at least 2 stages\n");
		return FALSE;
	}

	//Initialize everything including 
	// - window creation and main context
	// - create shared textures
	// - create the different stages in our pipeine and attach their input and output queues
	init(hInstance);
    // Show window.
    window::getInstance().show(nCmdShow);

	//start the different stage threads and also initialize the 1st queue to get the ball rolling
	startExecution();
	window::getInstance().messageLoop();

    // XXX cleanup


	return TRUE;
}
