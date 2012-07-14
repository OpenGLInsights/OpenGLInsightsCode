
/**********************************************************************\
* AUTHOR : HILLAIRE Sébastien
*
* MAIL   : hillaire_sebastien@yahoo.fr
* SITE   : sebastien.hillaire.free.fr
*
*	You are free to totally or partially use this file/code.
* If you do, please credit me in your software or demo and leave this
* note.
*	Share your work and your ideas as much as possible!
\*********************************************************************/

#include "stdlib.h"

#include "GL/glew.h"
#include "GL\glut.h"
#include "glut_callback.h"

#include "stdio.h"

#include "GLUTTimeCounter.h"
#include "GPUProgramManager.h"
#include "OGLShapes.h"
#include "Camera3D.h"
#include "MathLib.h"
#include "FrameBufferObject.h"
#include "LoadTexturesFunctions.h"

#include "TestInstancing.h"


////////////////////////////////////////////////////////////////////////////////
//						Data
////////////////////////////////////////////////////////////////////////////////

//#define GL_TIMER_QUERY_PERFORMANCE_MEASURE					// ENABLE/DISABLE TEST MODE

#ifdef GL_TIMER_QUERY_PERFORMANCE_MEASURE
GLuint query;
GLuint64 elapsed_time=0;
GLuint64 deltaTimeAccu=0;
GLuint64 deltaTimeAccuCount=0;
#endif


GLUTTimeCounter timeCounter;
float lastMX,lastMY;
unsigned int screenWidth;
unsigned int screenHeight;
float screenRatio;
Matrix4 projMat;
Camera3D cam;

static float trianglePosition[TEST_INSTANCING_MAX_INSTANCE*3];

int renderingMethod = 0;

TestInstancing* testInstancing=NULL;



////////////////////////////////////////////////////////////////////////////////
//						GLUT callbacks
////////////////////////////////////////////////////////////////////////////////

void glut_end();

void glut_ReshapeWin(int w,int h)
{
	/*screenWidth = 1;
	screenHeight = 1;*/
	screenWidth = w;
	screenHeight = h;
	screenRatio = (float)screenWidth/(float)screenHeight;
	glViewport(0,0,(GLsizei) screenWidth,(GLsizei) screenHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	projMat.setPerspective(90.0f/screenRatio,screenRatio,0.1f,100.0f);
}

void glut_render(void)
{
	//setup camera
	static Matrix4 viewMat;
	static Matrix4 MVP;
	cam.getViewMatrix(viewMat);
	MVP = projMat * viewMat;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(MVP.toFloatPtr());

	const float screenRatio = (float)screenHeight/(float)screenWidth;

	//begin frame
	glClearColor(0.0,0.0,0.0,0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT);

#ifdef GL_TIMER_QUERY_PERFORMANCE_MEASURE
	glBeginQuery(GL_TIME_ELAPSED,query);
#endif

	//render lines using additive blending and selected rendering path
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);



	///////////// TODO RENDER
	glColor3f(1.0f,1.0f,1.0f);
//	glutWireTeapot(1.0);

	switch(renderingMethod)
	{
	case 0:
		testInstancing->render(TEST_INSTANCING_MAX_INSTANCE,NULL,MVP.toFloatPtr(),viewMat.toFloatPtr(),projMat.toFloatPtr());
		break;
	case 1:
		testInstancing->render(TEST_INSTANCING_MAX_INSTANCE,trianglePosition,MVP.toFloatPtr(),viewMat.toFloatPtr(),projMat.toFloatPtr());
		break;
	case 2:
		testInstancing->renderInstancing(TEST_INSTANCING_MAX_INSTANCE,NULL,MVP.toFloatPtr(),viewMat.toFloatPtr(),projMat.toFloatPtr());
		break;
	case 3:
		testInstancing->renderInstancing(TEST_INSTANCING_MAX_INSTANCE,trianglePosition,MVP.toFloatPtr(),viewMat.toFloatPtr(),projMat.toFloatPtr());
		break;
	default:
		break;
	}


#ifdef GL_TIMER_QUERY_PERFORMANCE_MEASURE
	glEndQuery(GL_TIME_ELAPSED);
	int done=0;
	while (!done)
	{
		glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &done);
	}
	// get the query result
	glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsed_time);
	deltaTimeAccu+=elapsed_time;
	deltaTimeAccuCount++;
#endif


	//end of frame
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();

#ifdef _DEBUG
	GLenum err = glGetError();
	if(err!=GL_NONE)
	{
		//
		printf("OpenGL error!\n");
	}
#endif
}



void glut_program(void)
{
	//update camera
	float elapsedTime = timeCounter.getElapsedTimef();
	timeCounter.reset();
	cam.update(elapsedTime, -(300.0f-lastMY), 400.0f-lastMX);
	lastMX=400.0f;
	lastMY=300.0f;
	glutWarpPointer(400,300);
}



void glut_mouseClic(int boutton,int etat,int x,int y)
{
	lastMX=x;
	lastMY=y;

	switch(boutton)
	{
	case GLUT_LEFT_BUTTON:
		switch(etat)
		{
		case GLUT_DOWN:
			break;
		case GLUT_UP:
			break;
		}
		break;

	case GLUT_MIDDLE_BUTTON:
		switch(etat)
		{
		case GLUT_DOWN:
			break;
		case GLUT_UP:
			break;
		}
		break;

	case GLUT_RIGHT_BUTTON:
		switch(etat)
		{
		case GLUT_DOWN:
			break;
		case GLUT_UP:
			break;
		}
		break;
	}
}

void glut_mouseClicMove(int x,int y)
{
	lastMX=x;
	lastMY=y;
}

void glut_mouseMove(int x,int y)
{
	lastMX=x;
	lastMY=y;
}



void glut_keyboard(unsigned char key,int x,int y)
{
	switch(key)
	{
	case 'q':
	case 'a':
		cam.setLeft(true);
		break;
	case 'd':
		cam.setRight(true);
		break;
	case 'w':
	case 'z':
		cam.setForward(true);
		break;
	case 's':
		cam.setBackward(true);
		break;
	case ' ':
		cam.setUp(true);
		break;
	case 'c':
		cam.setDown(true);
		break;
		


	case 27:
		glut_end();
		break;

	default:
		break;
	}
}

void glut_keyboardUp(unsigned char key,int x,int y)
{
	switch(key)
	{
	case 'q':
	case 'a':
		cam.setLeft(false);
		break;
	case 'd':
		cam.setRight(false);
		break;
	case 'w':
	case 'z':
		cam.setForward(false);
		break;
	case 's':
		cam.setBackward(false);
		break;
	case ' ':
		cam.setUp(false);
		break;
	case 'c':
		cam.setDown(false);
		break;

	default:
		break;
	}
}

#ifdef GL_TIMER_QUERY_PERFORMANCE_MEASURE
void printPerformance()
{
	if(deltaTimeAccuCount>0)
		printf("Method %d performance was %f ms\n",renderingMethod,(float)(((double)deltaTimeAccu/(double)deltaTimeAccuCount)/1000000.0) );
	deltaTimeAccu=0;
	deltaTimeAccuCount=0;

}
#else
inline void printPerformance(){}
#endif

void glut_keyboardSpecial(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_F1:
		printPerformance();
		renderingMethod=0;
		break;
	case GLUT_KEY_F2:
		printPerformance();
		renderingMethod=1;
		break;
	case GLUT_KEY_F3:
		printPerformance();
		renderingMethod=2;
		break;
	case GLUT_KEY_F4:
		printPerformance();
		renderingMethod=3;
		break;
	default:
		break;
	}
}

void glut_keyboardSpecialUp(int key, int x, int y)
{
	switch(key)
	{
	default:
		break;
	}
}

void glut_setup()
{
	//redimensionnement
	glutReshapeFunc(glut_ReshapeWin);
	//affichage
	glutDisplayFunc(glut_render);
	//inactivité graphique
	glutIdleFunc(glut_program);
	//souris
	glutMouseFunc(glut_mouseClic);
	glutMotionFunc(glut_mouseClicMove);
	glutPassiveMotionFunc(glut_mouseMove);
	//clavier
	glutKeyboardFunc(glut_keyboard);
	glutKeyboardUpFunc(glut_keyboardUp);
	glutSpecialFunc(glut_keyboardSpecial);
	glutSpecialUpFunc(glut_keyboardSpecialUp);

	//initialisation de GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  fprintf(stderr, "Error GLEW : %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));



#ifdef GL_TIMER_QUERY_PERFORMANCE_MEASURE
	glGenQueries(1, &query);
#endif

	testInstancing = new TestInstancing();

	const float randMaxF = (float)RAND_MAX;
	for(int i=0;i<TEST_INSTANCING_MAX_INSTANCE;++i)
	{
		trianglePosition[i*3  ] = (float)(rand())/randMaxF;
		trianglePosition[i*3+1] = (float)(rand())/randMaxF;
		trianglePosition[i*3+2] = (float)(rand())/randMaxF;
	}

	glutSetCursor(GLUT_CURSOR_NONE);
	lastMX=400.0f;
	lastMY=300.0f;
	glutWarpPointer(400,300);
}



void glut_end()
{
	delete testInstancing;

	GPUProgramManager::destroy();
	
#ifdef GL_TIMER_QUERY_PERFORMANCE_MEASURE
	glDeleteQueries(1, &query);
#endif

	GPUProgramManager::destroy();
	exit(0);
}



