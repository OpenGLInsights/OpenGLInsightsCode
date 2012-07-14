
/*
Copyright (c) 2011/2012, Movania Muhammad Mobeen
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list
of conditions and the following disclaimer in the documentation and/or other
materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.

///////////////////////////////////////////////////////////////////////////////////////

Demo code accompanying the Chapter: "Realtime Physically-based Deformation Using Transform Feedback" by
Muhammad Mobeen Movania and Lin Feng. This demo implements Verlet integration using three modes 
as discussed in the chapter. The first mode is the GPU mode using Transform Feedback.
The second mode is the unoptimized CPU mode while the third mode is optimized CPU mode.
This mode uses all of the CPU cores (if the system has them) by using the OpenMP parallel 
threads. The three modes can be toggled using the spacebar key.

Dependencies: You will need glm, glew and freeglut to compile and run this code.

This code is part of the OpenCloth project. For details visit: http://code.google.com/p/opencloth

Author: Muhammad Mobeen Movania
Last Modified: 9 September 2011.
*/

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/freeglut.h>

#define _USE_MATH_DEFINES
#include <cmath>

#include "GLSLShader.h"
#include <vector>

#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cassert>
#include <omp.h>

using namespace std;  

#pragma comment(lib, "glew32.lib")

#define CHECK_GL_ERRORS assert(glGetError()==GL_NO_ERROR);

const int width = 1024, height = 1024;
const int numX = 63, numY=63;
const int total_points = (numX+1)*(numY+1);
const int sizeX = 4, sizeY = 4;
const float hsize = sizeX/2.0f;
const int NUM_ITER = 1;
int selected_index = -1;
bool bDisplayMasses=true;
enum Mode {CPU, CPU_OPT, GPU_TF, TOTAL_MODES};

struct Spring {
	int p1, p2;
	float rest_length;
	float Ks, Kd; 
};

Mode current_mode = GPU_TF;
string mode = "GPU TF";

vector<GLushort> indices;
vector<Spring> springs;

vector<glm::vec4> X;		//current positions
vector<glm::vec4> X_last;	//previous positions
vector<glm::vec3> F;

int oldX=0, oldY=0;
float rX=18, rY=-40;
int state =1 ;
float dist=-11;
const int GRID_SIZE=10;

 
int spring_count=0;

char info[MAX_PATH]={0};

const float DEFAULT_DAMPING =  -0.0125f;
float	KsStruct = 50.75f,KdStruct = -0.25f; 
float	KsShear = 50.75f,KdShear = -0.25f;
float	KsBend = 50.95f,KdBend = -0.25f;
glm::vec3 gravity=glm::vec3(0.0f,-0.00981f,0.0f);  
float mass = 1.0f;

float timeStep =  1.0f/60.0f;
float currentTime = 0;
double accumulator = timeStep;

GLint viewport[4];
GLdouble MV[16];
GLdouble P[16];

glm::mat4 mMVP;
glm::mat4 mMV;
glm::mat4 mP;
glm::vec3 Up=glm::vec3(0,1,0), Right, viewDir;

LARGE_INTEGER frequency;        // ticks per second
LARGE_INTEGER t1, t2;           // ticks
double frameTimeQP=0;
float frameTime =0 ;
int texture_size_x=0;
int texture_size_y=0;

float startTime =0, fps=0 ;
int totalFrames=0;
GLuint primitives_written=0;
GLfloat pointSize = 30;
GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat vBeige[] = { 1.0f, 0.8f, 0.7f, 1.0f };
GLfloat vWhite[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat vGray[] = { .25f, .25f, .25f, 1.0f };

float delta_time=0;

int readID=0, writeID = 1;
  
GLuint	vboID_Pos[2],
		vboID_PrePos[2];

GLuint vaoUpdateID[2], vaoRenderID[2], vboIndices;
GLuint texPosID[2];
GLuint texPrePosID[2];
 
size_t i=0;
   
GLSLShader	massSpringShader, 
			particleShader,
			renderShader;

GLuint t_query, query; 
GLuint64 elapsed_time;  

GLuint gridVAOID, gridVBOVerticesID, gridVBOIndicesID;
vector<glm::vec3> grid_vertices;
vector<GLushort> grid_indices;

GLuint clothVAOID, clothVBOVerticesID, clothVBOIndicesID;

GLuint tfID;


glm::vec3 vec3(glm::vec4 v) {
	return glm::vec3(v.x, v.y, v.z);
}
 
void StepPhysics(float dt );
void AddSpring(int a, int b, float ks, float kd ) {
	Spring spring;
	spring.p1=a;
	spring.p2=b;
	spring.Ks=ks;
	spring.Kd=kd; 
	glm::vec3 deltaP = vec3(X[a]-X[b]);
	spring.rest_length = sqrt(glm::dot(deltaP, deltaP));
	springs.push_back(spring);
}
void createVBO()
{
	//fill the vertices
	int count = 0;
		 
    // create buffer object
	glGenVertexArrays(2, vaoUpdateID);
	glGenVertexArrays(2, vaoRenderID);
	
	glGenBuffers( 2, vboID_Pos);
	glGenBuffers( 2, vboID_PrePos);
	glGenBuffers(1, &vboIndices);
	glGenTextures(2, texPosID);
	glGenTextures(2, texPrePosID);
  	
	//set update vao
	for(int i=0;i<2;i++) {
		glBindVertexArray(vaoUpdateID[i]);
		glBindBuffer( GL_ARRAY_BUFFER, vboID_Pos[i]);
		glBufferData( GL_ARRAY_BUFFER, X.size()* sizeof(glm::vec4), &(X[0].x), GL_DYNAMIC_COPY);		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,  4, GL_FLOAT, GL_FALSE, 0, 0);
	
		CHECK_GL_ERRORS
	
		glBindBuffer( GL_ARRAY_BUFFER, vboID_PrePos[i]);
		glBufferData( GL_ARRAY_BUFFER, X_last.size()*sizeof(glm::vec4), &(X_last[0].x), GL_DYNAMIC_COPY);	
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,  4, GL_FLOAT, GL_FALSE, 0,0);		

		CHECK_GL_ERRORS;		
	}

	CHECK_GL_ERRORS;

	//set render vao
	for(int i=0;i<2;i++) {
		glBindVertexArray(vaoRenderID[i]);
		glBindBuffer( GL_ARRAY_BUFFER, vboID_Pos[i]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,  4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
		if(i==0)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
	}
	
	glBindVertexArray(0);
	
	for(int i=0;i<2;i++) {
		glBindTexture( GL_TEXTURE_BUFFER, texPosID[i]);
		glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA32F, vboID_Pos[i]);

		glBindTexture( GL_TEXTURE_BUFFER, texPrePosID[i]);
		glTexBuffer( GL_TEXTURE_BUFFER, GL_RGBA32F, vboID_PrePos[i]);
	}
			 
	//glColor3f(0.5f, 0.5f, 0.5f);
	
	for(int i=-GRID_SIZE;i<=GRID_SIZE;i++)
	{
		grid_vertices.push_back(glm::vec3((float)i,0,(float)-GRID_SIZE));
		grid_vertices.push_back(glm::vec3((float)i,0,(float)GRID_SIZE));

		grid_vertices.push_back(glm::vec3((float)-GRID_SIZE,0,(float)i));
		grid_vertices.push_back(glm::vec3((float)GRID_SIZE,0,(float)i));
	}
	
	for(int i=0;i<GRID_SIZE*GRID_SIZE;i+=4) {
		grid_indices.push_back(i);
		grid_indices.push_back(i+1);
		grid_indices.push_back(i+2);
		grid_indices.push_back(i+3);
	}
	//Create grid VAO/VBO
	glGenVertexArrays(1, &gridVAOID);
	glGenBuffers (1, &gridVBOVerticesID);
	glGenBuffers (1, &gridVBOIndicesID);
	glBindVertexArray(gridVAOID);
		glBindBuffer (GL_ARRAY_BUFFER, gridVBOVerticesID);
		glBufferData (GL_ARRAY_BUFFER, sizeof(float)*3*grid_vertices.size(), &grid_vertices[0].x, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE,0,0);
		
		CHECK_GL_ERRORS
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridVBOIndicesID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*grid_indices.size(), &grid_indices[0], GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &clothVAOID);
	glGenBuffers (1, &clothVBOVerticesID);
	glGenBuffers (1, &clothVBOIndicesID);
	glBindVertexArray(clothVAOID);
		glBindBuffer (GL_ARRAY_BUFFER, clothVBOVerticesID);
		glBufferData (GL_ARRAY_BUFFER, sizeof(float)*4*X.size(), &X[0].x, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE,0,0);
		
		CHECK_GL_ERRORS
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, clothVBOIndicesID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*indices.size(), &indices[0], GL_STATIC_DRAW);
		

	 

	glBindVertexArray(0);

    CHECK_GL_ERRORS;
}

void OnMouseDown(int button, int s, int x, int y)
{
	if (s == GLUT_DOWN) 
	{
		oldX = x; 
		oldY = y; 
		int window_y = (height - y);
		float norm_y = float(window_y)/float(height/2.0);
		int window_x = x ;
		float norm_x = float(window_x)/float(width/2.0);
		
		float winZ=0;
		glReadPixels( x, height-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
		if(winZ==1)
			winZ=0; 
		double objX=0, objY=0, objZ=0;
		gluUnProject(window_x,window_y, winZ,  MV,  P, viewport, &objX, &objY, &objZ);
		glm::vec3 pt(objX,objY, objZ); 
		int i=0;
		if(current_mode == CPU || current_mode == CPU_OPT) {
			for(i=0;i<total_points;i++) {			 
				if( glm::distance(vec3(X[i]),pt)<0.1) {
					selected_index = i;
					printf("Intersected at %d\n",i);
					break;
				}
			}
		} else {
			glBindVertexArray(vaoRenderID[readID]);
			glBindBuffer(GL_ARRAY_BUFFER, vboID_Pos[writeID]);
			GLfloat* pData = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		
			for(i=0;i<total_points*4;i+=4) {			 
				if( abs(pData[i]-pt.x)<0.1 && 
					abs(pData[i+1]-pt.y)<0.1  &&
					abs(pData[i+2]-pt.z)<0.1 ) {
					selected_index = i/4;
					printf("Intersected at %d\n",i);
					break;
				}
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glBindVertexArray(0);	
		}
	}	

	if(button == GLUT_MIDDLE_BUTTON)
		state = 0;
	else
		state = 1;

	if(s==GLUT_UP) {
		selected_index= -1;
		glutSetCursor(GLUT_CURSOR_INHERIT);
	}
}

void OnMouseMove(int x, int y)
{
	if(selected_index == -1) {
		if (state == 0)
			dist *= (1 + (y - oldY)/60.0f); 
		else
		{
			rY += (x - oldX)/5.0f; 
			rX += (y - oldY)/5.0f; 
		} 
	} else {
		float delta = 1500/abs(dist);
		float valX = (x - oldX)/delta; 
		float valY = (oldY - y)/delta; 
		if(abs(valX)>abs(valY))
			glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
		else 
			glutSetCursor(GLUT_CURSOR_UP_DOWN);

		if(current_mode==CPU || current_mode == CPU_OPT) {
			X[selected_index].x += Right[0]*valX ;
			float newValue = X[selected_index].y+Up[1]*valY;
			if(newValue>0)
				X[selected_index].y = newValue;
			X[selected_index].z += Right[2]*valX + Up[2]*valY;		
			X_last[selected_index] = X[selected_index];
		} else {
			
			glBindVertexArray(vaoRenderID[readID]);
			glBindBuffer(GL_ARRAY_BUFFER, vboID_Pos[writeID]);
				GLfloat* pData = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
					pData[selected_index*4]	  += Right[0]*valX ;
					float newValue = pData[selected_index*4+1]+Up[1]*valY;
					if(newValue>0)
						pData[selected_index*4+1] = newValue;
					pData[selected_index*4+2] += Right[2]*valX + Up[2]*valY;
				glUnmapBuffer(GL_ARRAY_BUFFER);

			glBindBuffer(GL_ARRAY_BUFFER, vboID_PrePos[writeID]);
				pData = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
					pData[selected_index*4]	  += Right[0]*valX ;
					newValue = pData[selected_index*4+1]+Up[1]*valY;
					if(newValue>0)
						pData[selected_index*4+1] = newValue;
					pData[selected_index*4+2] += Right[2]*valX + Up[2]*valY;
				glUnmapBuffer(GL_ARRAY_BUFFER);
			glBindVertexArray(0);	
		  
		}
	}
	oldX = x; 
	oldY = y; 

	glutPostRedisplay(); 
}


void DrawGrid()
{ 
	renderShader.Use();
		glBindVertexArray(gridVAOID); 
		glUniformMatrix4fv(renderShader("MVP"), 1, GL_FALSE, glm::value_ptr(mMVP));						
			glDrawElements(GL_LINES, grid_indices.size(),GL_UNSIGNED_SHORT,0);
		glBindVertexArray(0);
	renderShader.UnUse();
}

void DrawCloth()
{ 
	renderShader.Use();
		glBindVertexArray(clothVAOID); 
		glUniformMatrix4fv(renderShader("MVP"), 1, GL_FALSE, glm::value_ptr(mMVP));						
			glDrawElements(GL_TRIANGLES, indices.size(),GL_UNSIGNED_SHORT,0);
		//glBindVertexArray(0);
	renderShader.UnUse();
}

void DrawClothPoints()
{ 
	particleShader.Use();
		//glBindVertexArray(clothVAOID); 
		glUniform1i(particleShader("selected_index"), selected_index);
		glUniformMatrix4fv(particleShader("MV"), 1, GL_FALSE, glm::value_ptr(mMV));	
		glUniformMatrix4fv(particleShader("MVP"), 1, GL_FALSE, glm::value_ptr(mMVP));										
			//draw the masses last						
			glDrawArrays(GL_POINTS, 0, total_points);					
		glBindVertexArray(0);
	particleShader.UnUse();	
}

void InitGL() { 
	glClearColor(1,1,1,1);
	glGenQueries(1, &query); 
	glGenQueries(1, &t_query); 

	texture_size_x =  numX+1;
	texture_size_y =  numY+1;

	CHECK_GL_ERRORS
	startTime = (float)glutGet(GLUT_ELAPSED_TIME);
	// get ticks per second
    QueryPerformanceFrequency(&frequency);

    // start timer
    QueryPerformanceCounter(&t1);

	 
	size_t i=0, j=0, count=0;
	int l1=0, l2=0; 
	int v = numY+1;
	int u = numX+1;

	printf("Total triangles: %3d\n",numX*numY*2);
	indices.resize( numX*numY*2*3);
	 
	
	X.resize(total_points);
	X_last.resize(total_points); 
	F.resize(total_points);
  
	//fill in positions
	for(int j=0;j<=numY;j++) {		 
		for(int i=0;i<=numX;i++) {	 
			X[count] = glm::vec4( ((float(i)/(u-1)) *2-1)* hsize, sizeX+1, ((float(j)/(v-1) )* sizeY),1);
			X_last[count] = X[count];
			count++;
		}
	} 

	//fill in indices
	GLushort* id=&indices[0];
	for (int i = 0; i < numY; i++) {        
		for (int j = 0; j < numX; j++) {            
			int i0 = i * (numX+1) + j;            
			int i1 = i0 + 1;            
			int i2 = i0 + (numX+1);            
			int i3 = i2 + 1;            
			if ((j+i)%2) {                
				*id++ = i0; *id++ = i2; *id++ = i1;                
				*id++ = i1; *id++ = i2; *id++ = i3;            
			} else {                
				*id++ = i0; *id++ = i2; *id++ = i3;                
				*id++ = i0; *id++ = i3; *id++ = i1;            
			}        
		}    
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glEnable(GL_CULL_FACE);
	
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glCullFace(GL_BACK); 
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	//Disable vsync
	wglSwapIntervalEXT(0);

	// Setup springs
	// Horizontal
	for (l1 = 0; l1 < v; l1++)	// v
		for (l2 = 0; l2 < (u - 1); l2++) {
			AddSpring((l1 * u) + l2,(l1 * u) + l2 + 1,KsStruct,KdStruct);
		}

	// Vertical
	for (l1 = 0; l1 < (u); l1++)	
		for (l2 = 0; l2 < (v - 1); l2++) {
			AddSpring((l2 * u) + l1,((l2 + 1) * u) + l1,KsStruct,KdStruct);
		}

	// Shearing Springs
	for (l1 = 0; l1 < (v - 1); l1++)	
		for (l2 = 0; l2 < (u - 1); l2++) {
			AddSpring((l1 * u) + l2,((l1 + 1) * u) + l2 + 1,KsShear,KdShear);
			AddSpring(((l1 + 1) * u) + l2,(l1 * u) + l2 + 1,KsShear,KdShear);
		}

	// Bend Springs
	for (l1 = 0; l1 < (v); l1++) {
		for (l2 = 0; l2 < (u - 2); l2++) {
			AddSpring((l1 * u) + l2,(l1 * u) + l2 + 2,KsBend,KdBend);
		}
		AddSpring((l1 * u) + (u - 3),(l1 * u) + (u - 1),KsBend,KdBend);
	}
	for (l1 = 0; l1 < (u); l1++) {
		for (l2 = 0; l2 < (v - 2); l2++) {
			AddSpring((l2 * u) + l1,((l2 + 2) * u) + l1,KsBend,KdBend);
		}
		AddSpring(((v - 3) * u) + l1,((v - 1) * u) + l1,KsBend,KdBend);
	}


	massSpringShader.LoadFromFile(GL_VERTEX_SHADER, "shaders/Spring.vp");
	particleShader.LoadFromFile(GL_VERTEX_SHADER,"shaders/Basic.vp");
	particleShader.LoadFromFile(GL_FRAGMENT_SHADER,"shaders/Basic.fp");
	renderShader.LoadFromFile(GL_VERTEX_SHADER,"shaders/Passthrough.vp");
	renderShader.LoadFromFile(GL_FRAGMENT_SHADER,"shaders/Passthrough.fp");
	massSpringShader.CreateAndLinkProgram();
	massSpringShader.Use();
		massSpringShader.AddAttribute("position_mass");
		massSpringShader.AddAttribute("prev_position");		 
		massSpringShader.AddUniform("tex_position_mass");
		massSpringShader.AddUniform("tex_pre_position_mass");
		massSpringShader.AddUniform("MVP");		
		massSpringShader.AddUniform("dt");
		massSpringShader.AddUniform("gravity");
		massSpringShader.AddUniform("ksStr");
		massSpringShader.AddUniform("ksShr");
		massSpringShader.AddUniform("ksBnd");
		massSpringShader.AddUniform("kdStr");
		massSpringShader.AddUniform("kdShr");
		massSpringShader.AddUniform("kdBnd");
		massSpringShader.AddUniform("DEFAULT_DAMPING"); 
		massSpringShader.AddUniform("texsize_x"); 
		massSpringShader.AddUniform("texsize_y"); 
		massSpringShader.AddUniform("step"); 
		massSpringShader.AddUniform("inv_cloth_size");
		massSpringShader.AddUniform("ellipsoid");

		glUniform1i(massSpringShader("tex_position_mass"), 0); 
		glUniform1i(massSpringShader("tex_pre_position_mass"), 1); 
		
	massSpringShader.UnUse();

	CHECK_GL_ERRORS

	
	particleShader.CreateAndLinkProgram();
	particleShader.Use();
		particleShader.AddAttribute("position_mass");
		particleShader.AddUniform("pointSize");
		particleShader.AddUniform("MV");
		particleShader.AddUniform("MVP");
		particleShader.AddUniform("vColor");
		particleShader.AddUniform("selected_index");
		glUniform1f(particleShader("pointSize"), pointSize);
		glUniform4fv(particleShader("vColor"),1, vRed);
	particleShader.UnUse();
 
	renderShader.CreateAndLinkProgram();
	renderShader.Use();
		renderShader.AddAttribute("position_mass");
		renderShader.AddUniform("MVP");
		renderShader.AddUniform("vColor");
		glUniform4fv(renderShader("vColor"),1, vGray);
	renderShader.UnUse();

	CHECK_GL_ERRORS

	//create vbo 
	createVBO(); 
 
	//setup transform feedback attributes	    	 
	glGenTransformFeedbacks(1, &tfID);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfID); 
	const char* varying_names[]={"out_position_mass", "out_prev_position"};	
	glTransformFeedbackVaryings(massSpringShader.GetProgram(), 2, varying_names, GL_SEPARATE_ATTRIBS);		
	glLinkProgram(massSpringShader.GetProgram());

  
}

void OnReshape(int nw, int nh) {
	glViewport(0,0,nw, nh);
	
	mP = glm::perspective(60.0f, (GLfloat)nw/nh, 1.0f, 100.f);	
	for(int j=0;j<4;j++) 
		for(int i=0;i<4;i++) 
			P[i+j*4] = mP[j][i] ;
	
	glGetIntegerv(GL_VIEWPORT, viewport); 
}

 
void RenderGPU_TF() { 
	massSpringShader.Use();
		glUniformMatrix4fv(massSpringShader("MVP"), 1, GL_FALSE, glm::value_ptr(mMVP));					
		glUniform1f(massSpringShader("dt"),  timeStep);
		glUniform3fv(massSpringShader("gravity"),1,&gravity.x);
	 	glUniform1f(massSpringShader("ksStr"),  KsStruct); 
		glUniform1f(massSpringShader("ksShr"),  KsShear); 
		glUniform1f(massSpringShader("ksBnd"),  KsBend); 
		glUniform1f(massSpringShader("kdStr"),  KdStruct/1000.0f); 
		glUniform1f(massSpringShader("kdShr"),  KdShear/1000.0f); 
		glUniform1f(massSpringShader("kdBnd"),  KdBend/1000.0f); 
		glUniform1f(massSpringShader("DEFAULT_DAMPING"),  DEFAULT_DAMPING);  
		glUniform1i(massSpringShader("texsize_x"),texture_size_x);
		glUniform1i(massSpringShader("texsize_y"),texture_size_y);
		
		glUniform2f(massSpringShader("inv_cloth_size"),float(sizeX)/numX,float(sizeY)/numY);		
		glUniform2f(massSpringShader("step"),1.0f/(texture_size_x-1.0f),1.0f/(texture_size_y-1.0f));
	
		for(int i=0;i<NUM_ITER;i++) {
			glActiveTexture( GL_TEXTURE0);					
			glBindTexture( GL_TEXTURE_BUFFER, texPosID[writeID]);

			glActiveTexture( GL_TEXTURE1);					
			glBindTexture( GL_TEXTURE_BUFFER, texPrePosID[writeID]);
						
			glBindVertexArray( vaoUpdateID[writeID]);
				glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vboID_Pos[readID]);
				glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, vboID_PrePos[readID]);
				glEnable(GL_RASTERIZER_DISCARD);    // disable rasterization
					glBeginQuery(GL_TIME_ELAPSED,t_query); 
						glBeginTransformFeedback(GL_POINTS); 							
							glDrawArrays(GL_POINTS, 0, total_points);							
						glEndTransformFeedback();
					glEndQuery(GL_TIME_ELAPSED); 
					glFlush();
				glDisable(GL_RASTERIZER_DISCARD);	

			int tmp = readID;
			readID=writeID;
			writeID = tmp;
		}
		// get the query result 
		glGetQueryObjectui64v(t_query, GL_QUERY_RESULT, &elapsed_time); 		
		delta_time = elapsed_time / 1000000.0f;	
	massSpringShader.UnUse();
				 
	CHECK_GL_ERRORS;

	glBindVertexArray(vaoRenderID[writeID]);
		glDisable(GL_DEPTH_TEST);
			renderShader.Use();
				glUniformMatrix4fv(renderShader("MVP"), 1, GL_FALSE, glm::value_ptr(mMVP));						
				glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT,0);								 
			renderShader.UnUse();
		glEnable(GL_DEPTH_TEST);
		 
		if(bDisplayMasses) {
			particleShader.Use();
				glUniform1i(particleShader("selected_index"), selected_index);
				glUniformMatrix4fv(particleShader("MV"), 1, GL_FALSE, glm::value_ptr(mMV));	
				glUniformMatrix4fv(particleShader("MVP"), 1, GL_FALSE, glm::value_ptr(mMVP));										
					//draw the masses last						
			  		glDrawArrays(GL_POINTS, 0, total_points);		
					//glDrawTransformFeedbackStream(GL_POINTS, tfID, 0); 
			particleShader.UnUse();		
		}
				
		glBindVertexArray( 0);	 
	
	CHECK_GL_ERRORS

	
}
 
void RenderCPU() {
	 
	/*	
	//Semi-fixed time stepping
	if ( frameTime > 0.0 )
    {
        const float deltaTime = min( frameTime, timeStep );
        StepPhysics(deltaTime );
        frameTime -= deltaTime;    		
    }
	*/


	//Fixed time stepping + rendering at different fps	
	if ( accumulator >= timeStep ) {	 
        StepPhysics(timeStep );		
        accumulator -= timeStep;
    }

 	//draw polygons 

	//Update positions
	glBindVertexArray(clothVAOID);
	glBindBuffer(GL_ARRAY_BUFFER,clothVBOVerticesID);
	GLfloat* pData = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	memcpy(pData, &X[0].x, X.size()*sizeof(glm::vec4));
	glUnmapBuffer(GL_ARRAY_BUFFER);
	 
	glDisable(GL_DEPTH_TEST);
	DrawCloth(); 
	glEnable(GL_DEPTH_TEST);

	//draw points	
	if(bDisplayMasses) { 
		DrawClothPoints();
	} 
} 
 

void OnRender() {		
	 
	float newTime = (float) glutGet(GLUT_ELAPSED_TIME);
	frameTime = newTime-currentTime;
	currentTime = newTime;
	//accumulator += frameTime;

	//Using high res. counter
    QueryPerformanceCounter(&t2);
	 // compute and print the elapsed time in millisec
    frameTimeQP = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	t1=t2;
	accumulator += frameTimeQP;

	++totalFrames;
	if((newTime-startTime)>1000)
	{		
		float elapsedTime = (newTime-startTime);
		fps = (totalFrames/ elapsedTime)*1000 ;
		startTime = newTime;
		totalFrames=0;
		sprintf_s(info, "FPS: %3.2f, Mode: %s, Frame time (GLUT): %3.4f msecs, Frame time (QP): %3.3f, TF Time: %3.3f", fps, mode.c_str(), frameTime, frameTimeQP, delta_time);	
	}
		
	glutSetWindowTitle(info);

	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	glm::mat4 T		= glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 0.0f, dist));
	glm::mat4 Rx	= glm::rotate(T,  rX, glm::vec3(1.0f, 0.0f, 0.0f));
	mMV	= glm::rotate(Rx, rY, glm::vec3(0.0f, 1.0f, 0.0f)); 
	mMVP = mP*mMV;

	for(int j=0;j<4;j++) 
		for(int i=0;i<4;i++) 
			MV[i+j*4] = mMV[j][i] ;

	viewDir.x = (float)-MV[2];
	viewDir.y = (float)-MV[6];
	viewDir.z = (float)-MV[10];
	Right = glm::cross(viewDir, Up);

	//draw grid
 	DrawGrid();

	  
	switch(current_mode) {
		case CPU:
		case CPU_OPT:
			RenderCPU();
		break;

		case GPU_TF:
			RenderGPU_TF();
		break; 
	} 	
	glutSwapBuffers();
}

void OnShutdown() {
	X.clear();
	X_last.clear();
	F.clear();
	indices.clear();
	springs.clear();

	glDeleteQueries(1, &query); 
	glDeleteQueries(1, &t_query); 

	glDeleteTextures( 2, texPosID);    
	glDeleteTextures( 2, texPrePosID); 

	glDeleteVertexArrays(2, vaoUpdateID);
	glDeleteVertexArrays(2, vaoRenderID);
	glDeleteVertexArrays(1, &clothVAOID);
	glDeleteVertexArrays(1, &gridVAOID);
	 
	glDeleteBuffers( 1, &gridVBOVerticesID);
	glDeleteBuffers( 1, &gridVBOIndicesID);
	glDeleteBuffers( 1, &clothVBOVerticesID);
	glDeleteBuffers( 1, &clothVBOIndicesID);
	 
    glDeleteBuffers( 2, vboID_Pos);    
	glDeleteBuffers( 2, vboID_PrePos); 
	glDeleteBuffers( 1, &vboIndices);

	glDeleteTransformFeedbacks(1, &tfID);
	renderShader.DeleteProgram();
	massSpringShader.DeleteProgram();
	particleShader.DeleteProgram();	
}



inline glm::vec3 GetVerletVelocity(glm::vec3 x_i, glm::vec3 xi_last, float dt ) {
	return  (x_i - xi_last) / dt;
}
void ComputeForcesOptimized(float dt) {
	int i=0;
	 
	#pragma omp parallel for  
	for(i=0;i<total_points;i++) {
		F[i] = glm::vec3(0);
		glm::vec3 V = GetVerletVelocity(vec3(X[i]), vec3(X_last[i]), dt);
		//add gravity force
		if(i!=0 && i!=( numX)	)		 
			F[i] += gravity*mass;
		//add force due to damping of velocity
		F[i] += DEFAULT_DAMPING*V;
	}	 

	#pragma omp parallel for  
	for(i=0;i<(int)springs.size();i++) {
		glm::vec3 p1 = vec3(X[springs[i].p1]);
		glm::vec3 p1Last = vec3(X_last[springs[i].p1]);
		glm::vec3 p2 = vec3(X[springs[i].p2]);
		glm::vec3 p2Last = vec3(X_last[springs[i].p2]);

		glm::vec3 v1 = GetVerletVelocity(p1, p1Last, dt);
		glm::vec3 v2 = GetVerletVelocity(p2, p2Last, dt);
		
		glm::vec3 deltaP = p1-p2;
		glm::vec3 deltaV = v1-v2;
		float dist = glm::length(deltaP);
		
		float leftTerm = -springs[i].Ks * (dist-springs[i].rest_length);
		float rightTerm = springs[i].Kd * (glm::dot(deltaV, deltaP)/dist);		
		glm::vec3 springForce = (leftTerm + rightTerm)*glm::normalize(deltaP);
			 
		if(springs[i].p1 != 0 && springs[i].p1 != numX)
			F[springs[i].p1] += springForce; 
		if(springs[i].p2 != 0 && springs[i].p2 != numX )
			F[springs[i].p2] -= springForce;
	}
}

void IntegrateVerletOptimized(float deltaTime) {
	float deltaTime2Mass = (deltaTime*deltaTime)/mass;
	 	
	#pragma omp parallel for  
	for(int i=0;i<total_points;i++) {		
		glm::vec4 buffer = X[i];
		 
		X[i] = X[i] + (X[i] - X_last[i]) + deltaTime2Mass*glm::vec4(F[i],0);
		  
		X_last[i] = buffer;

		if(X[i].y <0) {
			X[i].y = 0; 
		}
	}
}

void ComputeForces(float dt) {
	int i=0;
	
	for(i=0;i<total_points;i++) {
		F[i] = glm::vec3(0);
		glm::vec3 V = GetVerletVelocity(vec3(X[i]), vec3(X_last[i]), dt);
		//add gravity force
		if(i!=0 && i!=( numX)	)		 
			F[i] += gravity*mass;
		//add force due to damping of velocity
		F[i] += DEFAULT_DAMPING*V;
	}	 
	
	for(i=0;i<(int)springs.size();i++) {
		glm::vec3 p1 = vec3(X[springs[i].p1]);
		glm::vec3 p1Last = vec3(X_last[springs[i].p1]);
		glm::vec3 p2 = vec3(X[springs[i].p2]);
		glm::vec3 p2Last = vec3(X_last[springs[i].p2]);

		glm::vec3 v1 = GetVerletVelocity(p1, p1Last, dt);
		glm::vec3 v2 = GetVerletVelocity(p2, p2Last, dt);
		
		glm::vec3 deltaP = p1-p2;
		glm::vec3 deltaV = v1-v2;
		float dist = glm::length(deltaP);
		
		float leftTerm = -springs[i].Ks * (dist-springs[i].rest_length);
		float rightTerm = springs[i].Kd * (glm::dot(deltaV, deltaP)/dist);		
		glm::vec3 springForce = (leftTerm + rightTerm)*glm::normalize(deltaP);
			 
		if(springs[i].p1 != 0 && springs[i].p1 != numX)
			F[springs[i].p1] += springForce; 
		if(springs[i].p2 != 0 && springs[i].p2 != numX )
			F[springs[i].p2] -= springForce;
	}
}

void IntegrateVerlet(float deltaTime) {
	float deltaTime2Mass = (deltaTime*deltaTime)/mass;
	 	  
	for(int i=0;i<total_points;i++) {		
		glm::vec4 buffer = X[i];
		 
		X[i] = X[i] + (X[i] - X_last[i]) + deltaTime2Mass*glm::vec4(F[i],0);
		  
		X_last[i] = buffer;
	}
}

void GroundPlaneCollision() {
	for(int i=0;i<total_points;i++) {
		if(X[i].y <0) {
			X[i].y = 0; 
		}
	}
}
void OnIdle() {		
	glutPostRedisplay();	
}

void StepPhysics(float dt ) {
	if(current_mode == CPU_OPT) {
		ComputeForcesOptimized(dt);		
		IntegrateVerletOptimized(dt);  
	} else {
		ComputeForces(dt);		
		IntegrateVerlet(dt);  
	}
	GroundPlaneCollision(); 
}

void OnKey(unsigned char key, int , int) {
	switch(key) {
		case ' ':	current_mode = (Mode)((current_mode+1)% TOTAL_MODES);	break;
		case 'm':	bDisplayMasses=!bDisplayMasses;							break;  
	}
	
	switch(current_mode) {
		case CPU:		mode="CPU";						break;
		case CPU_OPT:	mode="CPU Optimzed";			break;
		case GPU_TF:	mode="GPU Transform Feedback";	break;
	}

	glutPostRedisplay();
}

void main(int argc, char** argv) {
	
	glutInit(&argc, argv);
	glutInitContextVersion(3,3); 
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE); 

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("GLUT Cloth Demo using Transform Feedback");
	
	glutDisplayFunc(OnRender);
	glutReshapeFunc(OnReshape);
	glutIdleFunc(OnIdle);
	
	glutMouseFunc(OnMouseDown);
	glutMotionFunc(OnMouseMove); 
	glutKeyboardFunc(OnKey);
	glutCloseFunc(OnShutdown);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));		 
	}

	GLuint error = glGetError();	
	
	// Only continue, if OpenGL 3.3 is supported.
	if (!glewIsSupported("GL_VERSION_3_3"))
	{
  		puts("OpenGL 3.3 not supported.");
		exit(EXIT_FAILURE);
	} else {
		puts("OpenGL 3.3 supported.");
	}
	if(!glewIsExtensionSupported("GL_ARB_transform_feedback2"))
	{
		puts("Your hardware does not support a required extension [GL_ARB_transform_feedback2].");
		exit(EXIT_FAILURE);
	} else {
		puts("GL_ARB_transform_feedback2 supported.");
	}
	printf("Using GLEW ver.: %s\n",glewGetString(GLEW_VERSION));
	printf("Vendor: %s\n",glGetString (GL_VENDOR));
	printf("Renderer: %s\n",glGetString (GL_RENDERER));
	printf("Version: %s\n",glGetString (GL_VERSION));
	printf("GLSL: %s\n",glGetString (GL_SHADING_LANGUAGE_VERSION));
		
	InitGL();
	
	glutMainLoop();		
}


