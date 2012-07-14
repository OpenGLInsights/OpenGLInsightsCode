

/*

This code is provided as a companion to the text in the chapter
"GPU Tessellation: We still have a LOD of terrain to cover"
from the book "OpenGL Insights"

The code comes with no warranties, it is provided only as a demo

*/

// Have Fun :-)

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>

// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

#include <math.h>
#include <fstream>
#include <map>
#include <string>
#include <vector>

// VS Libs
#include "vslibs.h"

// -------------------------
// ****  CONFIGURATION  ****

#define DEBUG TRUE

#define PATCHSIZE 64

// set to 256 or 65536 depending in whether the image is 8 or 16 bit respectively.
#define MAX_VALUE_PER_PIXEL 65536

std::string terrainName = "heightMaps/pugget_sound/ps_height_1k.png";
std::string terrainTexture = "heightMaps/pugget_sound/ps_texture_2k.png";
float heightStep = 0.1f;
int scaleFactor = 2;
float gridSpacing = 160.0f;


int pixelsPerTriangle = 1;
// 0 - simple LOD; 1 - roughness LOD; 2 - full tessellation
int roughnessMode = 1;
char *rougnessModeS[] = {"simple LOD", "roughness LOD", "full tessellation"};
// flag to enable/disable tessellation shader based view frustum culling
int culling = 1;



// ****  END CONFIGURATION  ****
// -----------------------------



unsigned int primitiveCounter = 0;

// Test Data

float angle = 0.0f;
float testTime = 0.0f;
float testPrimitiveCount = 0.0f;

VSMathLib *vsml;

VSShaderLib shaderTerrainTess, shaderDefault;



VSTerrainLib *terrainModel;
VSTerrainLODSingleScaledLib terrainTess;

// Query to track the number of primitives
// issued by the tesselation shaders
GLuint counterQ;

// POLYGON MODE
GLuint polygonMode = GL_FILL;


// CAMERA
#define SURFACE 0
#define EXPLORER 1
#define TESTCAM 2
int camMode = EXPLORER;

// Explorer Camera Position
float camX, camY, camZ;
// Camera Spherical Coordinates
float alpha = 0.0f, beta = -0.5f;
float r = 1.0f;

// Surface Camera Settings
float posX, posY, posZ;
float alpha2 = 25.0f, beta2 = 0.0f;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Frame counting and FPS computation
long myTime, timebase = 0, frame = 0;
//char s[64];

// window size ratio
float ratio;




// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	ratio = (1.0f * w) / h;
	vsml->loadIdentity(VSMathLib::PROJECTION);
	vsml->perspective(53.13f, ratio, 10.0f, 400000.0f);

	int vpAux[2];
	vpAux[0] = w;
	vpAux[1] = h;
	shaderTerrainTess.setUniform("viewportDim", vpAux);
}


// ------------------------------------------------------------
//
// Render stuff
//

void renderScene(void) {


	static char s[128];
	float aux;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	vsml->loadIdentity(VSMathLib::MODEL);
	vsml->loadIdentity(VSMathLib::VIEW);
	// set camera

	float  gridSpacing = terrainModel->getGridSpacing();
	int gridSize = terrainModel->getGridSize();
	aux =  terrainModel->getHeight(posX, posZ) + 150.0f;

	vsml ->lookAt(posX, posY, posZ,  posX+camX, posY+camY, posZ+camZ,  0.0f, 1.0f, 0.0f);
	shaderTerrainTess.setUniform("pvm", vsml->get(VSMathLib::PROJ_VIEW_MODEL));

	// use our shader
	VSShaderLib *shader;
	shader = &shaderTerrainTess;
	glUseProgram(shader->getProgramIndex());


//			shader->prepareSamplerUniforms();		
#if DEBUG == TRUE
		glBeginQuery(GL_PRIMITIVES_GENERATED, counterQ);
		glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
#endif
		terrainModel->render();			
#if DEBUG == TRUE
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEndQuery(GL_PRIMITIVES_GENERATED);
#endif


	// FPS computation and display
	frame++;
	myTime=glutGet(GLUT_ELAPSED_TIME);
	if (myTime - timebase > 1000) {
		sprintf(s,"FPS:%4.2f  Counter: %d PPT: %d Mode: %s",
			frame*1000.0/(myTime-timebase) , primitiveCounter, pixelsPerTriangle, rougnessModeS[roughnessMode]);
		timebase = myTime;
		frame = 0;
		glutSetWindowTitle(s);
	}
	
	glutSwapBuffers();

#if DEBUG == TRUE	
	glGetQueryObjectuiv(counterQ, GL_QUERY_RESULT, &primitiveCounter);
#endif
}


// ------------------------------------------------------------
//
// Events from the Keyboard
//

void processKeys(unsigned char key, int xx, int yy) 
{
	std::string s;

	switch(key) {

		case 27: 

			glutLeaveMainLoop();
			break;

		case 'u':
			culling = 1;
			shaderTerrainTess.setUniform("culling",&culling);
			break;
		case 'i':
			culling = 0;
			shaderTerrainTess.setUniform("culling", &culling);
			break;

		case 'l' :
				polygonMode = GL_LINE;
				break;
		case 'f' :
				polygonMode = GL_FILL;
				break;
		case 'r':
				roughnessMode = 1;
				shaderTerrainTess.setUniform("useRoughness", &roughnessMode);
				break;
		case 't':
				roughnessMode = 0;
				shaderTerrainTess.setUniform("useRoughness", &roughnessMode);
				break;
		case 'y': 
				roughnessMode = 2;
				shaderTerrainTess.setUniform("useRoughness", &roughnessMode);
				break;
		case '+':
				if (pixelsPerTriangle < 64)
					pixelsPerTriangle++;
				shaderTerrainTess.setUniform("pixelsPerEdge", &pixelsPerTriangle);
				break;
		case '-':
				if (pixelsPerTriangle > 1)
					pixelsPerTriangle--;
				shaderTerrainTess.setUniform("pixelsPerEdge", &pixelsPerTriangle);
				break;


	}

//  uncomment this if not using an idle func
//	glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy) 
{
	switch(key) {
	
		case GLUT_KEY_UP: 
					posX += camX * terrainModel->getGridSpacing() * 2.5f;
					posY += camY * terrainModel->getGridSpacing() * 2.5f;
					posZ += camZ  *terrainModel->getGridSpacing() * 2.5f;
					break;

		case GLUT_KEY_DOWN:
					posX -= camX * terrainModel->getGridSpacing() * 2.5f;
					posY -= camY * terrainModel->getGridSpacing() * 2.5f;
					posZ -= camZ * terrainModel->getGridSpacing() * 2.5f;
					break;

	}
}


// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy) 
{
	// start tracking the mouse
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		if (tracking == 1) {
				alpha -= (xx - startX);
				beta -= (yy - startY);
		}
		tracking = 0;
	}
}


// Track mouse motion while buttons are pressed
void processMouseMotion(int xx, int yy)
{

	int deltaX, deltaY;
	float alphaAux, betaAux;

	deltaX =  - xx + startX;
	deltaY =  -  yy + startY;

	// left mouse button: move camera
	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0f)
			betaAux = 85.0f;
		else if (betaAux < -85.0f)
			betaAux = -85.0f;


		camX = r * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
		camZ = r * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
		camY = r *   						        sin(betaAux * 3.14f / 180.0f);
	}
}




// --------------------------------------------------------
//
// Shader Stuff
//


GLuint setupShaders() {

	shaderDefault.init();
	shaderDefault.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/dirlightdiffambpix.vert");
	shaderDefault.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");

	shaderDefault.setProgramOutput(0,"outputF");
	shaderDefault.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "position");
	shaderDefault.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	shaderDefault.setVertexAttribName(VSShaderLib::NORMAL_ATTRIB, "normal");

	shaderDefault.prepareProgram();


	shaderTerrainTess.init();

	shaderTerrainTess.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/terrainTessDemoSingleScaled.vert");
	shaderTerrainTess.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/terrainTessDemoSingleScaled.frag");
	shaderTerrainTess.loadShader(VSShaderLib::TESS_CONTROL_SHADER, "shaders/terrainTessDemoSingleScaled.tesc");
	shaderTerrainTess.loadShader(VSShaderLib::TESS_EVAL_SHADER, "shaders/terrainTessDemoSingleScaled.tese");
	
	shaderTerrainTess.setProgramOutput(0,"outputF");
	shaderTerrainTess.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "pos");

	shaderTerrainTess.prepareProgram();
	printf("Shader Terrain Tess InfoLog\n%s\n\n", shaderTerrainTess.getAllInfoLogs().c_str());

	int t = 0;
	shaderDefault.setUniform("texUnit", &t);

	t = 2;
	shaderTerrainTess.setUniform("texUnit", &t);

	shaderTerrainTess.setUniform("roughFactor", &roughnessMode);

	t = 0;
	shaderTerrainTess.setUniform("heightMap", &t);

	shaderTerrainTess.setUniform("patchSize", PATCHSIZE*1.0f);

	return(1);
}

// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//

int initGL()					 
{

	// init VSL
	vsml = VSMathLib::getInstance();
	vsml->setUniformBlockName("Matrices");
	vsml->setUniformName(VSMathLib::PROJECTION, "projMatrix");
	vsml->setUniformName(VSMathLib::VIEW_MODEL, "modelviewMatrix");
	vsml->setUniformName(VSMathLib::PROJ_VIEW_MODEL, "projModelViewMatrix");
	vsml->setUniformName(VSMathLib::NORMAL, "normalMatrix");


	// init terrain
	terrainTess.setGridSpacing(gridSpacing);
	terrainTess.setScaleFactor(scaleFactor);
	shaderTerrainTess.setUniform("scaleFactor", &scaleFactor);

	printf("Loading terrain for tesselation\n");
	float f;
	int i = 8;
	shaderTerrainTess.setUniform("pixelsPerEdge",&pixelsPerTriangle);
	
	f = terrainTess.getGridSpacing();
	shaderTerrainTess.setUniform("gridSpacing", &f);
	f = 0.1f * scaleFactor * MAX_VALUE_PER_PIXEL;
	shaderTerrainTess.setUniform("heightStep", &f);
	terrainTess.setHeightStep(f);
	shaderTerrainTess.setUniform("patchSize", PATCHSIZE*1.0f);

	terrainTess.load(terrainName);
	terrainTess.addTexture(2,terrainTexture);
	terrainModel = &terrainTess;
	printf("Done Loading\n");

	terrainModel->setMaterialBlockName("Material");

	glGenQueries(1,&counterQ);

	// Camera settings are based on spherical coordinates (alpha, beta, r);
	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

	posX = 0.0f;
	posY = 1000.0f;
	posZ = 0.0f;

	// Some GL render settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_MULTISAMPLE);

	glClampColorARB(GL_CLAMP_READ_COLOR_ARB, GL_FALSE);
	glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE);
	glClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);

	printf("Done Init\n");

	return true;					
}




// ------------------------------------------------------------
//
// Main function


int main(int argc, char **argv) {


		
//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_MULTISAMPLE);

	glutInitContextVersion (4, 1);
	glutInitContextProfile (GLUT_CORE_PROFILE );
	glutInitContextFlags(GLUT_DEBUG);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(1024,1024);
	glutCreateWindow("Lighthouse3D - Tesselation Demo");

//  Callback Registration
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutSpecialFunc(processSpecialKeys);
	
//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

//	Init GLEW
	// Load ALL extensions
	glewExperimental = GL_TRUE;
	glewInit();
	if (glewIsSupported("GL_VERSION_4_1"))
		printf("Ready for OpenGL 4.1\n");
	else {
		printf("OpenGL 4.1 not supported\n");
		exit(1);
	}

    VSGLInfoLib::getGeneralInfo();

	GLint maxTess;
	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTess);
	printf("Maximum Tessellation level : %d\n", maxTess);

	setupShaders();

	if (!initGL())
		printf("Could not Load the Model\n");


	printf("Entering main loop\n");
	//  GLUT main loop
	glutMainLoop();
	
	return 1;

}

