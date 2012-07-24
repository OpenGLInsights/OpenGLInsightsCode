#include "stdafx.h"

#include "commandLine.h"
#include "queue.h"
#include "window.h"
#include "glContext.h"
#include "stage.h"
#include "timer.h"
#include "stageProducer.h"
#include "stageConsumer.h"
#include "limits.h"

commandLineOptions options;
stageProducer* producer = NULL;
stageConsumer* consumer = NULL;
queue queues[2]; //for the shared texture, we have 2 queues one specified texture consumed and one for texture produced
glContext::renderingContext *mainRC;
HDC srcDC =NULL, destDC=NULL;


//struct SharedTexture{
//	queue producedQueue; //ids that have been produced into
//	queue consumedQueue; //ids that have been consumed from
//	GLuint texId[[limits::MAX_STREAMS]; //IDs of all textures that are shared
//};
//
//SharedTexture producedTex;

//Index for teh queues
unsigned int START_PRODUCER = 0;
unsigned int END_PRODUCER = 1;


bool init(HINSTANCE hInstance) {
	//Create Window
	window::getInstance().setup(hInstance, 0, 0, "OpenGL MultiGPU Affinity CopyImage");
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
		//TODO check we have all the extensions we need
		//GL_ARB_SYNC,     GL_NV_copy_image
		//GL_ARB_vertex_buffer_object 
		//GL_EXT_framebuffer_object 
		//GL_ARB_shader_objects)
		if (!options._singleGPU) {
        UINT GPUIdx;
        UINT displayDeviceIdx;
        GPU_DEVICE gpuDevice;
        HGPUNV hGPU;
        HGPUNV gpuMask[2];
        bool bDisplay, bPrimary;

        gpuDevice.cb = sizeof(gpuDevice);
        GPUIdx = 0;

        while(wglEnumGpusNV(GPUIdx, &hGPU)) // First call this function to get a handle to the gpu
        {
            if(options._verbose)
                printf("Device# %d:\n", GPUIdx);

            bDisplay = false;
            bPrimary = false;

            // Now get the detailed information about this device:
            // how many displays it's attached to
            displayDeviceIdx = 0;
            while(wglEnumGpuDevicesNV(hGPU, displayDeviceIdx, &gpuDevice))
            {			
                bDisplay = true;
                bPrimary |= (gpuDevice.Flags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0;

                if(options._verbose) {
                    printf(" Display# %d:\n", displayDeviceIdx);
                    printf("  Name: %s\n", gpuDevice.DeviceName);
                    printf("  String: %s\n", gpuDevice.DeviceString);
                
                    if(gpuDevice.Flags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
                    {
                        printf("  Attached to the desktop: LEFT=%d, RIGHT=%d, TOP=%d, BOTTOM=%d\n",
                            gpuDevice.rcVirtualScreen.left, gpuDevice.rcVirtualScreen.right, gpuDevice.rcVirtualScreen.top, gpuDevice.rcVirtualScreen.bottom);
                    }
                    else
                    {
                        printf("  Not attached to the desktop\n");
                    }
                  
                    // See if it's the primary GPU
                    if(gpuDevice.Flags & DISPLAY_DEVICE_PRIMARY_DEVICE)
                    {
                        printf("  This is the PRIMARY Display Device\n");
                    }
                }
                displayDeviceIdx++;
            }

            if(bPrimary) { //primary GPU create the destination here, for Win7 default dc is bound to primary , so not really needed
#if 0
                // Create the destination on the primary
                gpuMask[0] = hGPU;
                gpuMask[1] = NULL;

                destDC = wglCreateAffinityDCNV(gpuMask);
                if(!destDC)
                {
                    MessageBox(state->_main._hWnd, "wglCreateAffinityDCNV failed", "Error", MB_OK); 
                    return false;
                }
#endif
                if(options._verbose)
                    printf(" This is the destination GPU\n");
            }
            else  { //this is not the primary GPU so make it the source GPU
                // Create the source on any other
				gpuMask[0] = hGPU;
                gpuMask[1] = NULL;

                srcDC = wglCreateAffinityDCNV(gpuMask);
                if(!srcDC)
                {
					window::getInstance().messageBoxError("wglCreateAffinityDCNV failed");
                    return false;
                }
                if(options._verbose)
                    printf(" This is the source GPU\n");
            }
        
            GPUIdx++;
        }
		}//end of if!singleGPU
		// initialize timing in case we need to initialize OGL counters etc
		Timer::getInstance().init(&options); 
	}

	glContext::getInstance().makeUnCurrent();

	//Create the producer and consumer stage in our pipeline
	producer = new stageProducer();
	consumer = new stageConsumer();
	//set the appropriate GPU or device context
	producer->setAffinityDC(srcDC);
	consumer->setAffinityDC(destDC);
	//init will create the ogl context for each of the stages and initialize all required structures..thread has not started yet
	producer->init(&options);
	consumer->init(&options);
	//Connect the the output queues of one stage to the input of the other
	//Synchronization for producedTex that is shared between producer-consumer
	//waits (input) for start_producer queue, which tells producer to start rendering and copying over to producedTex. 
	//This is set in initialization stage or by consumer once it has finished using producedTex
	producer->attachInput(0, &queues[START_PRODUCER]);
	//signals(output) the end_producer queue, which tells consumer, Im done rendering&copying over, so use this texture
	producer->attachOutput(0, &queues[END_PRODUCER]);
	//waits (input) for end_producer queue, which tells consumer render+copy is done to producedTex so that consumer can start using it
	consumer->attachInput(0, &queues[END_PRODUCER]);
	//signals (output) to start_producer queue, consumer tells producer that it is done using producedTex so that producer can start 
	consumer->attachOutput(0, &queues[START_PRODUCER]);

	// this is the last stage
	consumer->setLastStage();
	//get the consumer OGL context and pass it to producer which will need it for the nvvopyimage
	producer->setDestRC(consumer->getRC());

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

	//Initialize everything including 
	// - window creation and main context
	// - create shared textures
	// - create the different stages in our pipeine and attach their input and output queues
	init(hInstance);
	// Show window.
	window::getInstance().show(nCmdShow);

	//start the different stage threads 
	producer->start();
	consumer->start(); //This also initialize the 1st queue to get the ball rolling
	window::getInstance().messageLoop();

	// XXX cleanup


	return TRUE;
}
