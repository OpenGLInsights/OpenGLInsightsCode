/*
 * Copyright 1993-2012 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */


/** 
 This sample is designed to test CUDA-OpenGL interoperability performance in a system.
- Requires CUDA version 5.0 or greater.
- It creates a GLUT window and tests mapping/unmapping of a texture of a given size
  using GL/CUDA interoperability on every CUDA device available in the system.
- CUDA is the producer and OpenGL is the consumer in this benchmark. This model is implemented by having
  the interoperability mapping flags set to cudaGraphicsMapFlagsWriteDiscard.
- The application computes "speedup" for each CUDA/GL context configuration possible in a given system.
  The speedup is the frame rate compared to pure workload(no interoperability) framerate 
- The application also auto-adjusts the time it spends in GL work and CUDA kernels
  so that it simulates a specified workload (default is 60 FPS).  This is because
  the number of iterations is sensitive to compiler changes, clocks, GPU type, etc.
 **/


#if defined(_WIN32)
#include <windows.h>
#include <stdint.h>
#define strcasecmp stricmp

#else /* _UNIX */
#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

#endif /* _UNIX */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>


// Important performance knobs to tune

// Texture dimentions set by the user
// - in multi-GPU interop, the whole texture needs to be copied between 
//   the two GPUs on every frame, so this size will modify that amount
//   of data copied
// - each pixel in the texture is 4 bytes (so 32 means 32*32*4 = 4k bytes)
int texWidth = 1920;
int texHeight = 1080;

// The time to spend doing CUDA work set by the user
// - this allows you to simulate different amounts of CUDA work being done
//   per frame (the default is fully-loaded at ~60 fps)
// - the application will spend this many msec running CUDA work
// - this is used to compute knobItersCUDA
float targetMsecCUDA = 17.f;

// The time to spend doing OpenGL work set by the user
// - this allows you to simulate different amounts of OpenGL rendering being done
//   per frame (the default is fully-loaded at ~60 fps)
// - the application will spend this many msec running OpenGL rendering
// - this is used to compute knobItersGL
float targetMsecGL = 17.f;


#define GL_CHECK_ERROR() \
    do { \
        GLenum err; \
        err = glGetError(); \
        if(err != GL_NO_ERROR) \
        { \
            fprintf(stderr, "%s:%d GL error: %s\n", __FILE__, __LINE__, gluErrorString(err)); \
            exit(1); \
        } \
    } while (0)

#define CUDA_CHECK_ERROR() \
    do { \
        cudaError err = cudaGetLastError(); \
        if (cudaSuccess != err) { \
            fprintf(stderr, "Cuda error in file '%s' in line %i : %s.\n", __FILE__, __LINE__, cudaGetErrorString(err) ); \
            exit(1); \
        } \
    } while(0) 

unsigned char *cudaBuffer;
cudaGraphicsResource_t interopTexHandle;
GLuint interopTex;
GLuint vshader;
GLuint fshader;
GLuint program;
GLint programTexLoc;
GLint programItersLoc;
GLint programValueLoc;

GLuint glTimerQuery;

int knobItersCUDA = 64*1024;
int knobItersGL = 64*1024;

cudaStream_t cudaStream = NULL;
struct cudaArray *interopTexArr = NULL;
cudaEvent_t 	cudaTaskStart;
cudaEvent_t 	cudaTaskEnd;

int cudaDevice = 0;

#if defined(_WIN32)

inline bool QueryPerformanceFrequency(int64_t *frequency)
{
	LARGE_INTEGER f;
	bool ret = QueryPerformanceFrequency(&f);
	*frequency = f.QuadPart;
	return ret;
}
inline bool QueryPerformanceCounter(int64_t *performance_count)
{
	LARGE_INTEGER p;
	bool ret = QueryPerformanceCounter(&p);
	*performance_count = p.QuadPart;
	return ret;
}

#else  /* _UNIX */
/* Helpful conversion constants. */
static const unsigned usec_per_sec = 1000000;


/* These functions are written to match the win32
   signatures and behavior as closely as possible.
*/
bool QueryPerformanceFrequency(int64_t *frequency)
{
    /* Sanity check. */
    assert(frequency != NULL);

    /* gettimeofday reports to microsecond accuracy. */
    *frequency = usec_per_sec;

    return true;
}

bool QueryPerformanceCounter(int64_t *performance_count)
{
    struct timeval time;

    /* Sanity check. */
    assert(performance_count != NULL);

    /* Grab the current time. */
    gettimeofday(&time, NULL);
    *performance_count = time.tv_usec + /* Microseconds. */
                         time.tv_sec * usec_per_sec; /* Seconds. */

    return true;
}
#endif /* _UNIX */
//
// Return nanosecond clock value.
//
int64_t GetNanoClock()
{
	int64_t now;
	static int64_t frequency;
	static int gotfrequency = 0;
	int64_t seconds, nsec;

	QueryPerformanceCounter(&now);
	if (gotfrequency == 0) {
		QueryPerformanceFrequency(&frequency);
		gotfrequency = 1;
	}

	seconds = now / frequency;
	nsec = (1000000000 * (now - (seconds * frequency))) / frequency;
    
	return seconds * 1000000000 + nsec;
}


// Simple CUDA kernel designed to waste a whole bunch of cycles also doing nothing
__global__ void K(unsigned char *cudaBuffer, unsigned char value, int iters)
{
    unsigned char *pixel = cudaBuffer + 4*threadIdx.x;
    pixel[0] = value;
    pixel[1] = value;
    pixel[2] = value;
    pixel[3] = value;

    for (int i = 0; i < iters; ++i) {
        pixel[0] = pixel[0] * pixel[1] + i*i;
    }
    pixel[0] &= 0xF0;
    pixel[0] |= value;
}

void buildProgram(void)
{
    const char *vtext = "\
        void main() \n\
        { \n\
            gl_FrontColor = gl_Color; \n\
            gl_Position = ftransform(); \n\
        }";
    const char *ftext = "\
        uniform sampler2D tex; \n\
        uniform int iters; \n\
        uniform float value; \n\
        void main() \n\
        { \n\
            float x = value; \n\
            for (int i = 0; i < iters; ++i) \n\
             { \n\
                x = sqrt(x) + 0.001; \n\
            } \n\
            gl_FragColor = x*gl_Color*texture2D(tex,gl_TexCoord[0].st); \n\
        }";
    vshader = glCreateShader(GL_VERTEX_SHADER);
    fshader = glCreateShader(GL_FRAGMENT_SHADER); 
    glShaderSource(vshader, 1, &vtext, NULL);
    glShaderSource(fshader, 1, &ftext, NULL);
    glCompileShader(vshader);
    glCompileShader(fshader);
    program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);
    programTexLoc   = glGetUniformLocation(program, "tex");
    programItersLoc = glGetUniformLocation(program, "iters");
    programValueLoc = glGetUniformLocation(program, "value");
    GL_CHECK_ERROR();
}

float tuneLinearKnob(int *knob, float (*eval)(void), float target)
{
    int steps = 0;
	float value;
    while (1) {
        value = eval();
        int knobValue = *knob;
        printf("%1.2f with %d iterations...", value, knobValue);

        *knob = (int)( knobValue*(target/value) );
        if (*knob <= 0) {
            *knob = 1;
        }

        steps += 1;
        if (steps == 2) {
            break;
        }
    }
	return value;
}

// Execute a long-running kernel in CUDA, then copy data to the interop cudaArray_t
void doWorkInCUDA(void)
{
    // Run a kernel that wastes a lot of time
    static char value = 0;
    value += 1;
    K<<<1,1,0,cudaStream>>>(cudaBuffer, value, knobItersCUDA);
    CUDA_CHECK_ERROR();

}

// Draw using the interop GL texture
void doWorkInGL(void)
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glViewport(0, 0, 512, 512);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Make a really expensive draw call which uses the interop texture
    // - we make the call expensive by doing lots of iterations in a
    //   loop in the pixel shader
    // - we use a really small viewport so that the number of processors
    //   in the GPU doesn't impact performance (rather, only clocks matter,
    //   and clocks don't change as rapidly across generations)
    {
        glViewport(0, 0, 4, 4);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, interopTex);
        glUseProgram(program);
        glUniform1i(programTexLoc,   0);
        glUniform1i(programItersLoc, knobItersGL);
        glUniform1f(programValueLoc, 1.001);        

        glBegin(GL_QUADS);
        glColor3f(1, 1, 1); 
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.8,  0.8, 0);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.8,  0.8, 0);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.8, -0.8, 0);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.8, -0.8, 0);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }

    // Draw a triangle
    glViewport(0, 0, 512, 512);
    {
        glBegin(GL_TRIANGLES);
        glTexCoord2f(0.0f, 0.0f); glColor3f(0, 0, 1); glVertex3f(-0.6, -0.6, 0);
        glTexCoord2f(0.0f, 1.0f); glColor3f(0, 1, 0); glVertex3f(-0.6,  0.6, 0);
        glTexCoord2f(1.0f, 1.0f); glColor3f(1, 0, 0); glVertex3f( 0.6, -0.6, 0);
        glEnd();
    }
}

int timingIterations = 100;

// Benchmark just the CUDA kernel alone
float benchmarkCUDA(void)
{

    float msecCUDA = 0.f;

    cudaGraphicsMapResources(1, &interopTexHandle, cudaStream);
	CUDA_CHECK_ERROR();	
	cudaGraphicsSubResourceGetMappedArray(&interopTexArr, interopTexHandle, 0, 0);
	CUDA_CHECK_ERROR();	
	//GPU based timing is commented out but can be used for timing as well.
	
	cudaStreamSynchronize(cudaStream);
	int64_t start, end;
	start = GetNanoClock();
	
	//cudaEventRecord(cudaTaskStart,cudaStream);
	//CUDA_CHECK_ERROR();
	for (int i = 0; i < timingIterations; ++i) {

		doWorkInCUDA();
    }
	//cudaEventRecord(cudaTaskEnd,cudaStream);
	//CUDA_CHECK_ERROR();
    //cudaEventSynchronize(cudaTaskEnd);
	//CUDA_CHECK_ERROR(); 
    //cudaEventElapsedTime(&msecCUDA, cudaTaskStart, cudaTaskEnd);
	//CUDA_CHECK_ERROR();
	//msecCUDA /= timingIterations;
	
	cudaStreamSynchronize(cudaStream);
	end = GetNanoClock();
	msecCUDA = end - start;
	msecCUDA /= 1000000.0f;
	msecCUDA /= timingIterations;

	cudaGraphicsUnmapResources(1, &interopTexHandle, cudaStream);	
	CUDA_CHECK_ERROR();  
	

	return msecCUDA;
}

// Benchmark just the GL rendering alone
float benchmarkGL()
{
    float msecGL = 0.f;
	//GPU based timing is commented out but can be used for timing as well.
	 glFinish();   
	int64_t start, end;
	start = GetNanoClock();
	
	//glBeginQuery(GL_TIME_ELAPSED_EXT, glTimerQuery);	
    for (int i = 0; i < timingIterations; ++i) {
        doWorkInGL();
    }
	//glEndQuery(GL_TIME_ELAPSED_EXT);
	//GL_CHECK_ERROR(); 
	//glFinish: this is necessary call to make when timing to make
	//sure that all the preceding OpenGL commands are done before we get the time
    glFinish();    

	//GLuint64EXT timeElapsed = 0;
	//glGetQueryObjectui64vEXT(glTimerQuery, GL_QUERY_RESULT, &timeElapsed);
	//GL_CHECK_ERROR(); 
	//msecGL = timeElapsed*0.000001;	
	//msecGL /= timingIterations;

	end = GetNanoClock();
	msecGL = end - start;
	msecGL /= 1000000.0f;
	msecGL /= timingIterations;
	
    return msecGL;
}

// Benchmark CUDA and GL, with data shared through interop
float benchmarkInterop()
{
	//Must use CPU based timing here since there is a mix of APIs with interdependant calls that
	//are possibly not CPU asynchronous.
    float msecInterop = 0.f;
	//cudaStreamSynchronize: this is necessary call to make when doing CPU based timings to make
	//sure that all the preceding CUDA commands are done before we start timing
	cudaStreamSynchronize(cudaStream);		
	CUDA_CHECK_ERROR();  
	//glFinish: this is necessary call to make when doing CPU based timings to make
	//sure that all the preceding OpenGL commands are done before we start timing
	glFinish();
	
	int64_t start, end;
	//start timing
	start = GetNanoClock();
    for (int i = 0; i < timingIterations; ++i) {
        cudaGraphicsMapResources(1, &interopTexHandle, cudaStream);
		CUDA_CHECK_ERROR();  
        cudaGraphicsSubResourceGetMappedArray(&interopTexArr, interopTexHandle, 0, 0);
		CUDA_CHECK_ERROR();  
        doWorkInCUDA();
        cudaGraphicsUnmapResources(1, &interopTexHandle, cudaStream);
		CUDA_CHECK_ERROR();  
        doWorkInGL();

    }
	//glFinish: this is necessary call to make when doing CPU based timings to make
	//sure that all the preceding OpenGL commands are done before we finish timing
	glFinish();
	//cudaStreamSynchronize: this is necessary call to make when doing CPU based timings to make
	//sure that all the preceding CUDA commands are done before we finish timing
	cudaStreamSynchronize(cudaStream);
	CUDA_CHECK_ERROR();  
	//end timing
	end = GetNanoClock();
	
	msecInterop = end - start;
	msecInterop /= 1000000.0f;
	msecInterop /= timingIterations;
	return msecInterop;
}

void benchmark(int cudaDevice, int glDevices[], int glDevicesCount)
{
    printf("\n");
    printf("CUDA is using device %d\n", cudaDevice);
	bool multiGPU = false;
	for(int i = 0; i < glDevicesCount; i++)
	{
		if(glDevices[i] == cudaDevice)
		{
			multiGPU = false;
			printf("CUDA and OpenGL contexts share the same device\n");
		}
		else
		{
			multiGPU = true;
			printf("CUDA and OpenGL contexts reside on different devices\n");
		}
	}
	printf("  Benchmarking using a %dx%d texture:\n",  texWidth, texHeight);
    printf("  Tuning knobs to make CUDA section take ~%1.2f msec... ", targetMsecCUDA);
    tuneLinearKnob(&knobItersCUDA, benchmarkCUDA, targetMsecCUDA);
    printf("\n");

    printf("  Benchmarking CUDA part of the program\n");
    float msecCUDA = benchmarkCUDA();
    printf("    %f msec/frame\n", msecCUDA);

    printf("  Benchmarking GL part of the program\n");
    float msecGL = benchmarkGL();
    printf("    %f msec/frame\n", msecGL);

    printf("  Benchmarking CUDA and GL combined, using interop\n");
    float msecInterop = benchmarkInterop();
    printf("    %f msec/frame\n", msecInterop);  

	float speedup = (msecCUDA+msecGL)/msecInterop;
	printf("    Speedup(versus pure workload): %.2fx\n", speedup);		
		
	if(multiGPU)
	{
		float maxWorkloadTime = max(msecCUDA,msecGL);
		float overhead = msecInterop - maxWorkloadTime;
		printf("    Overhead: %.2f msec/frame\n",overhead);
		printf("    Overhead: %.2f%%\n", overhead*100.0/msecInterop);
	}
	else
	{
		float overhead = msecInterop - msecCUDA - msecGL;
		printf("    Overhead: %.2f msec/frame\n", overhead);
		printf("    Overhead: %.2f%%\n", overhead*100.0/msecInterop);
	}
}

int ParseCommandLine(int count, char **argv)
{	
	// Parse command line arguments
	for(int i = 0; i < count;)
	{
		const char *szBuffer = argv[i++];
		
		if (!strcasecmp("-help", szBuffer)) {
			printf("\n");
			printf("MultiGPU CUDA/OpenGL Interoperability Benchmark - Help\n\n");			
			printf("Program parameters:\n");
			printf("\t-width #\t\t\t: width of the interop texture. Default 1920.\n");
			printf("\t-height #\t\t\t: height of the interop texture. Default 1080.\n");
			printf("\t-targetMsecCUDA #\t\t: The time to spend doing CUDA work in msec(can be fractions). Default 17msec.\n");
			printf("\t-targetMsecGL #\t\t\t: The time to spend doing OpenGL work in msec(can be fractions). Default 17msec.\n");			
			return 0;
		}
		if (!strcasecmp("-width", szBuffer)) {
			if(i == count)
				return -1;
			szBuffer = argv[i++];		
			texWidth = atoi(szBuffer);
		}else if (!strcasecmp("-height", szBuffer)) {
			if(i == count)
				return -1;
			szBuffer = argv[i++];	
			texHeight = atoi(szBuffer);
		}else if(!strcasecmp("-targetMsecCUDA", szBuffer)) {
			if(i == count)
				return -1;
			szBuffer = argv[i++];	
			targetMsecCUDA = atof(szBuffer);			
		}else if(!strcasecmp("-targetMsecGL", szBuffer)) {
			if(i == count)
				return -1;
			szBuffer = argv[i++];	
			targetMsecGL = atof(szBuffer);			
		}
		else
			return -1;
	}

	return 1;
}



int main(int argc, char *argv[])
{
    int ret = ParseCommandLine(argc-1,&argv[1]);
	if(ret < 0)
	{
		printf("Incorrect parameters\n");
		return -1;
	}
	else if(ret == 0)
	{
		return 0;
	}
	else
    {
        unsigned int glDeviceCount = 16;
        int glDevices[16];
        int window;

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);		
        window = glutCreateWindow("OpenGL Window");
        glutInitWindowSize(512, 512);
        glutInitWindowPosition(0, 0);
		glutHideWindow();
        glewInit();		
        // Create a shader which is super-inefficient, which we'll use to simulate a complex draw callcalls
        buildProgram();

		//glGenQueries(1,&glTimerQuery);

        // Print all devices that are spanned by the current GL context.
        cudaGLGetDevices(&glDeviceCount, glDevices, glDeviceCount, cudaGLDeviceListAll);
        CUDA_CHECK_ERROR();
        printf("OpenGL is using CUDA device(s): ");
        for (unsigned int i = 0; i < glDeviceCount; ++i) {
            printf("%s%d", i == 0 ? "" : ", ", glDevices[i]);
        }
        printf("\n");
		
	    // Allocate the texture we'll use for interop
        glGenTextures(1, &interopTex);
        glBindTexture(GL_TEXTURE_2D, interopTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glBindTexture(GL_TEXTURE_2D, 0);            
		GL_CHECK_ERROR();
	
		printf("  Tuning knobs to make GL section take ~%1.3f msec... ", targetMsecGL);
		tuneLinearKnob(&knobItersGL, benchmarkGL, targetMsecGL);
		printf("\n");					

		// Benchmark with CUDA
        int cudaDeviceCount = 0;
        cudaGetDeviceCount(&cudaDeviceCount);

        CUDA_CHECK_ERROR();
        for (cudaDevice = 0; cudaDevice < cudaDeviceCount; ++cudaDevice) {
      
            cudaSetDevice(cudaDevice);
            CUDA_CHECK_ERROR();
			//cudaEventCreate(&cudaTaskStart);
			//CUDA_CHECK_ERROR();
			//cudaEventCreate(&cudaTaskEnd);
			//CUDA_CHECK_ERROR();
			cudaFree(0);
            CUDA_CHECK_ERROR();
            cudaGraphicsGLRegisterImage(&interopTexHandle, interopTex, GL_TEXTURE_2D, cudaGraphicsRegisterFlagsNone);
            CUDA_CHECK_ERROR();
            cudaGraphicsResourceSetMapFlags(interopTexHandle, cudaGraphicsMapFlagsWriteDiscard);
            CUDA_CHECK_ERROR();
            cudaStreamCreate(&cudaStream);
            CUDA_CHECK_ERROR();
            cudaMalloc(&cudaBuffer, texWidth*texHeight*4);
            CUDA_CHECK_ERROR();

            benchmark(cudaDevice, glDevices, glDeviceCount);
			
			cudaStreamDestroy(cudaStream);
			CUDA_CHECK_ERROR();
			//cudaEventDestroy(cudaTaskStart);  
			//CUDA_CHECK_ERROR();
			//cudaEventDestroy(cudaTaskEnd);  
			//CUDA_CHECK_ERROR();
			cudaGraphicsUnregisterResource(interopTexHandle);
            CUDA_CHECK_ERROR();
            cudaDeviceReset();
            CUDA_CHECK_ERROR();
			
        }
        // Delete the GL texture
        glDeleteTextures(1, &interopTex);
        GL_CHECK_ERROR();

		//glDeleteQueries(1,&glTimerQuery);
        glutDestroyWindow(window);
    }

    return 0;
}

