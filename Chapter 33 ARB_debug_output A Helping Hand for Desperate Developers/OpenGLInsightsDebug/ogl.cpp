/*
This demo was built for the book OpenGL Insights

It works with VSDebugLib, an interface library for ARB_debug_output extension

The source code is provided as is, with no warranties, with the only purpose
of showing the library in action.

This demo requires freeGLUT and GLEW

*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "vsDebugLib.h"




#define M_PI       3.14159265358979323846f

// Data for drawing Axis
float verticesAxis[] = {	-20.0f, 0.0f, 0.0f, 1.0f,
							 20.0f, 0.0f, 0.0f, 1.0f,

							 0.0f, -20.0f, 0.0f, 1.0f,
							 0.0f,  20.0f, 0.0f, 1.0f,

							 0.0f, 0.0f, -20.0f, 1.0f,
							 0.0f, 0.0f,  20.0f, 1.0f};


float colorAxis[] = {	0.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 0.0f};

// Data for triangle 1
float vertices1[] = {	-3.0f, 0.0f, -5.0f, 1.0f,  
						-1.0f, 0.0f, -5.0f, 1.0f,   
						-2.0f, 2.0f, -5.0f, 1.0f};


float colors1[] = {		0.0f, 0.0f, 1.0f, 1.0f,  
						0.0f, 1.0f, 0.0f, 1.0f,   
						1.0f, 0.0f, 0.0f, 1.0f};

// Data for triangle 2
float vertices2[] = {	1.0f, 0.0f, -5.0f, 1.0f,  
						3.0f, 0.0f, -5.0f, 1.0f,   
						2.0f, 2.0f, -5.0f, 1.0f};


float colors2[] = {		1.0f, 0.0f, 0.0f, 1.0f,  
						1.0f, 0.0f, 0.0f, 1.0f,   
						1.0f, 0.0f, 0.0f, 1.0f};

// Shader Names
char *vertexFileName = "colorUniformBlock.vert";
char *fragmentFileName = "color.frag";

// Program and Shader Identifiers
GLuint p,v,f;

// Vertex Attribute Locations
GLuint vertexLoc, colorLoc;


// Uniform buffer
GLuint uniforms;
GLuint uniformsLoc;
// Uniform variable Locations
GLuint projMatrixLoc, viewMatrixLoc;

// Vertex Array Objects Identifiers
GLuint vao[3];

// storage for Matrices
float projMatrix[16];
float viewMatrix[16];


char *textFileRead(char *fn) {

	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) {

		fp = fopen(fn,"rt");
		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

// ----------------------------------------------------
// VECTOR STUFF
//


// res = a cross b;
void crossProduct( float *a, float *b, float *res) {

	res[0] = a[1] * b[2]  -  b[1] * a[2];
	res[1] = a[2] * b[0]  -  b[2] * a[0];
	res[2] = a[0] * b[1]  -  b[0] * a[1];
}


// Normalize a vec3
void normalize(float *a) {

	float mag = sqrt(a[0] * a[0]  +  a[1] * a[1]  +  a[2] * a[2]);

	if (mag) {
		a[0] /= mag;
		a[1] /= mag;
		a[2] /= mag;
	}
}


// ----------------------------------------------------
// MATRIX STUFF
//

// sets the square matrix mat to the identity matrix,
// size refers to the number of rows (or columns)
void setIdentityMatrix( float *mat, int size) {

	// fill matrix with 0s
	for (int i = 0; i < size * size; ++i)
			mat[i] = 0.0f;

	// fill diagonal with 1s
	for (int i = 0; i < size; ++i)
		mat[i + i * size] = 1.0f;
}


//
// a = a * b;
//
void multMatrix(float *a, float *b) {

	float res[16];

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			res[j*4 + i] = 0.0f;
			for (int k = 0; k < 4; ++k) {
				res[j*4 + i] += a[k*4 + i] * b[j*4 + k]; 
			}
		}
	}
	memcpy(a, res, 16 * sizeof(float));

}


// Defines a transformation matrix mat with a translation
void setTranslationMatrix(float *mat, float x, float y, float z) {

	setIdentityMatrix(mat,4);
	mat[12] = x;
	mat[13] = y;
	mat[14] = z;
}


// ----------------------------------------------------
// Projection Matrix 
//

void buildProjectionMatrix(float fov, float ratio, float nearp, float farp) {

	float f = 1.0f / tanf (fov * (M_PI / 360.0f));

	setIdentityMatrix(projMatrix,4);

	projMatrix[0] = f / ratio;
	projMatrix[1 * 4 + 1] = f;
	projMatrix[2 * 4 + 2] = (farp + nearp) / (nearp - farp);
	projMatrix[3 * 4 + 2] = (2.0f * farp * nearp) / (nearp - farp);
	projMatrix[2 * 4 + 3] = -1.0f;
	projMatrix[3 * 4 + 3] = 0.0f;

	glBindBuffer(GL_UNIFORM_BUFFER,uniforms);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(projMatrix), projMatrix);
	glBindBuffer(GL_UNIFORM_BUFFER,0);

}


// ----------------------------------------------------
// View Matrix
//
// note: it assumes the camera is not tilted, 
// i.e. a vertical up vector (remmeber gluLookAt?)
//

void setCamera(float posX, float posY, float posZ, 
			   float lookAtX, float lookAtY, float lookAtZ) {

	float dir[3], right[3], up[3];

	up[0] = 0.0f;	up[1] = 1.0f;	up[2] = 0.0f;

	dir[0] =  (lookAtX - posX);
	dir[1] =  (lookAtY - posY);
	dir[2] =  (lookAtZ - posZ);
	normalize(dir);

	crossProduct(dir,up,right);
	normalize(right);

	crossProduct(right,dir,up);
	normalize(up);

	float aux[16];

	viewMatrix[0]  = right[0];
	viewMatrix[4]  = right[1];
	viewMatrix[8]  = right[2];
	viewMatrix[12] = 0.0f;

	viewMatrix[1]  = up[0];
	viewMatrix[5]  = up[1];
	viewMatrix[9]  = up[2];
	viewMatrix[13] = 0.0f;

	viewMatrix[2]  = -dir[0];
	viewMatrix[6]  = -dir[1];
	viewMatrix[10] = -dir[2];
	viewMatrix[14] =  0.0f;

	viewMatrix[3]  = 0.0f;
	viewMatrix[7]  = 0.0f;
	viewMatrix[11] = 0.0f;
	viewMatrix[15] = 1.0f;

	setTranslationMatrix(aux, -posX, -posY, -posZ);

	multMatrix(viewMatrix, aux);
	
	glBindBuffer(GL_UNIFORM_BUFFER, uniforms);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(projMatrix), sizeof(viewMatrix), viewMatrix);
	glBindBuffer(GL_UNIFORM_BUFFER,0);
}


// ----------------------------------------------------

void changeSize(int w, int h) {

	float ratio;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	ratio = (1.0f * w) / h;
	buildProjectionMatrix(53.13f, ratio, 1.0f, 100.0f);
}


void setupBuffers() {

	GLuint buffers[2];

	glGenVertexArrays(3, vao);
	//
	// VAO for first triangle
	//
	glBindVertexArray(vao[0]);
	// Generate two slots for the vertex and normal buffers
	glGenBuffers(2, buffers);
	// bind buffer for vertices and copy data into buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertexLoc);
	glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);

	// bind buffer for normals and copy data into buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors1), colors1, GL_STATIC_DRAW);
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, 0, 0, 0);

	//
	// VAO for second triangle
	//
	glBindVertexArray(vao[1]);
	// Generate two slots for the vertex and normal buffers
	glGenBuffers(2, buffers);

	// bind buffer for vertices and copy data into buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertexLoc);
	glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);

	// bind buffer for normals and copy data into buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors2), colors2, GL_STATIC_DRAW);
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, 0, 0, 0);

	//
	// This VAO is for the Axis
	//
	glBindVertexArray(vao[2]);
	// Generate two slots for the vertex and normal buffers
	glGenBuffers(2, buffers);
	// bind buffer for vertices and copy data into buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAxis), verticesAxis, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertexLoc);
	glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);

	// bind buffer for normals and copy data into buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorAxis), colorAxis, GL_STATIC_DRAW);
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, 0, 0, 0);

	glBindVertexArray(0);

	//
	// Uniform Block
	//
	glGenBuffers(1,&uniforms);
	glBindBuffer(GL_UNIFORM_BUFFER, uniforms);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(projMatrix) + sizeof(viewMatrix),NULL,GL_DYNAMIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER,0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 2, uniforms, 0, sizeof(projMatrix) *2);	//setUniforms();
}


void setUniforms() {

	// must be called after glUseProgram
	glUniformMatrix4fv(projMatrixLoc,  1, false, projMatrix);
	glUniformMatrix4fv(viewMatrixLoc,  1, false, viewMatrix);
}


void renderScene(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setCamera(10,2,10,0,2,-5);

	glUseProgram(p);

	glBindVertexArray(vao[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(vao[2]);
	glDrawArrays(GL_LINES, 0, 6);

	glBindVertexArray(0);

	glUseProgram(0);

	glutSwapBuffers();
}


void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27) {
		glDeleteVertexArrays(3,vao);
		glDeleteBuffers(1, &uniforms);
		glDeleteProgram(p);
		glDeleteShader(v);
		glDeleteShader(f);
		exit(0);
	}
}


void printShaderInfoLog(GLuint obj) {

    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}


void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}


GLuint setupShaders() {

	GLuint p;

	char *vs = NULL,*fs = NULL,*fs2 = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead(vertexFileName);
	fs = textFileRead(fragmentFileName);

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v, 1, &vv,NULL);
	glShaderSource(f, 1, &ff,NULL);

	free(vs);free(fs);

	glCompileShader(v);
	glCompileShader(f);

	printShaderInfoLog(v);
	printShaderInfoLog(f);

	p = glCreateProgram();
	glAttachShader(p,v);
	glAttachShader(p,f);

	glBindFragDataLocation(p, 0, "outputColor");

	glLinkProgram(p);
	printProgramInfoLog(p);

	vertexLoc = glGetAttribLocation(p,"position");
	colorLoc = glGetAttribLocation(p, "color"); 

	uniformsLoc = glGetUniformBlockIndex(p,"Matrices");
	glUniformBlockBinding(p, uniformsLoc, 2);

	return(p);
}




int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	glutInitContextVersion (4, 1);
	glutInitContextProfile (GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_DEBUG );

	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("Lighthouse 3D");

	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(processNormalKeys);

	glewExperimental = true;
	glewInit();
	if (glewIsSupported("GL_VERSION_3_3"))
		printf("Ready for OpenGL 3.3\n");
	else {
		printf("OpenGL 3.3 not supported\n");
		exit(1);
	}

	VSDebugLib::init();

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0,1.0,1.0,1.0);

	// some errors to show the debug lib working
	glPolygonMode(GL_FRONT, GL_LINE);
	glBindBuffer(GL_ARRAY_BUFFER,27);

	VSDebugLib::enableUserMessages(true);
	VSDebugLib::addApplicationMessage(12345, 
					GL_DEBUG_TYPE_OTHER_ARB,
					GL_DEBUG_SEVERITY_LOW_ARB,
					"This is just a test");


	
	p = setupShaders();

	setupBuffers();

	glutMainLoop();

	return 0;
}

