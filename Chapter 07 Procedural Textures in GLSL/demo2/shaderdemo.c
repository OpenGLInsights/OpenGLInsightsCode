/* 
 * Demo for GLSL procedural shaders.
 *
 * Shaders are loaded from several external files, named in
 * the macro definitions VERTSHADER* and FRAGSHADER*.
 * The main program draws a simple scene using the shaders,
 * and reports the performance in frames per second
 * to the console and to the window title each second.
 *
 * This program uses GLFW for convenience, to handle the OS-specific
 * window management stuff. Some Windows-specific stuff for extension
 * loading is still here, but that code is short-circuited on other
 * platforms. This file compiles unedited on Windows, Linux and MacOS X,
 * provided you have the relevant libraries and header files installed
 * and set up your compilation to include the GLFW and OpenGL libraries.
 *
 * Author: Stefan Gustavson (stegu@itn.liu.se) 2004, 2005, 2010, 2011
 * This code is in the public domain.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __WIN32__
#include "GL/glfw.h" // Local libglfw.a as a convenience for Windows users
#include "GL/glext.h" // Windows systems often have an outdated version of this
#else
#include <GL/glfw.h> // GLFW is a cross-platform OpenGL framework
#endif

#ifdef __APPLE__
// MacOS application bundles have the executable inside a directory structure
#define PATH "../../../"
#else
// Windows, Linux and other Unix systems expose executables as naked files
#define PATH ""
#endif

#define VERTSHADER0 "flownoise2.vert"
#define FRAGSHADER0 "flownoise2.frag"
#define VERTSHADER1 "snoise3.vert"
#define FRAGSHADER1 "snoise3.frag"
#define VERTSHADER2 "fbmnoise.vert"
#define FRAGSHADER2 "fbmnoise.frag"
#define VERTSHADER3 "spots.vert"
#define FRAGSHADER3 "spots.frag"
#define VERTSHADER4 "tiles.vert"
#define FRAGSHADER4 "tiles.frag"

#ifdef __WIN32__
/* Global function pointers for everything we need beyond OpenGL 1.1 */
PFNGLCREATEPROGRAMPROC           glCreateProgram      = NULL;
PFNGLDELETEPROGRAMPROC           glDeleteProgram      = NULL;
PFNGLUSEPROGRAMPROC              glUseProgram         = NULL;
PFNGLCREATESHADERPROC            glCreateShader       = NULL;
PFNGLDELETESHADERPROC            glDeleteShader       = NULL;
PFNGLSHADERSOURCEPROC            glShaderSource       = NULL;
PFNGLCOMPILESHADERPROC           glCompileShader      = NULL;
PFNGLGETSHADERIVPROC             glGetShaderiv        = NULL;
PFNGLGETPROGRAMIVPROC            glGetProgramiv       = NULL;
PFNGLATTACHSHADERPROC            glAttachShader       = NULL;
PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog   = NULL;
PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog  = NULL;
PFNGLLINKPROGRAMPROC             glLinkProgram        = NULL;
PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation = NULL;
PFNGLUNIFORM1FVPROC              glUniform1fv         = NULL;
#endif

/*
 * printError() - Signal an error.
 * Simple printf() to console for portability.
 */
void printError(const char *errtype, const char *errmsg) {
  fprintf(stderr, "%s: %s\n", errtype, errmsg);
}


/*
 * Override the Win32 filelength() function with
 * a version that takes a Unix-style file handle as
 * input instead of a file ID number, and which works
 * on platforms other than Windows.
 */
long filelength(FILE *file) {
    long numbytes;
    long savedpos = ftell(file);
    fseek(file, 0, SEEK_END);
    numbytes = ftell(file);
    fseek(file, savedpos, SEEK_SET);
    return numbytes;
}


/*
 * loadExtensions() - Load OpenGL extensions for anything above OpenGL
 * version 1.1. (This is a requirement only on Windows, so on other
 * platforms, this function just checks for the required extensions.)
 */
void loadExtensions() {
    //These extension strings indicate that the OpenGL Shading Language
    // and GLSL shader objects are supported.
    if(!glfwExtensionSupported("GL_ARB_shading_language_100"))
    {
        printError("GL init error", "GL_ARB_shading_language_100 extension was not found");
        return;
    }
    if(!glfwExtensionSupported("GL_ARB_shader_objects"))
    {
        printError("GL init error", "GL_ARB_shader_objects extension was not found");
        return;
    }
    else
    {
#ifdef __WIN32__
        glCreateProgram           = (PFNGLCREATEPROGRAMPROC)glfwGetProcAddress("glCreateProgram");
        glDeleteProgram           = (PFNGLDELETEPROGRAMPROC)glfwGetProcAddress("glDeleteProgram");
        glUseProgram              = (PFNGLUSEPROGRAMPROC)glfwGetProcAddress("glUseProgram");
        glCreateShader            = (PFNGLCREATESHADERPROC)glfwGetProcAddress("glCreateShader");
        glDeleteShader            = (PFNGLDELETESHADERPROC)glfwGetProcAddress("glDeleteShader");
        glShaderSource            = (PFNGLSHADERSOURCEPROC)glfwGetProcAddress("glShaderSource");
        glCompileShader           = (PFNGLCOMPILESHADERPROC)glfwGetProcAddress("glCompileShader");
        glGetShaderiv             = (PFNGLGETSHADERIVPROC)glfwGetProcAddress("glGetShaderiv");
        glGetShaderInfoLog        = (PFNGLGETSHADERINFOLOGPROC)glfwGetProcAddress("glGetShaderInfoLog");
        glAttachShader            = (PFNGLATTACHSHADERPROC)glfwGetProcAddress("glAttachShader");
        glLinkProgram             = (PFNGLLINKPROGRAMPROC)glfwGetProcAddress("glLinkProgram");
        glGetProgramiv            = (PFNGLGETPROGRAMIVPROC)glfwGetProcAddress("glGetProgramiv");
        glGetProgramInfoLog       = (PFNGLGETPROGRAMINFOLOGPROC)glfwGetProcAddress("glGetProgramInfoLog");
        glGetUniformLocation      = (PFNGLGETUNIFORMLOCATIONPROC)glfwGetProcAddress("glGetUniformLocation");
        glUniform1fv              = (PFNGLUNIFORM1FVPROC)glfwGetProcAddress("glUniform1fv");

        if( !glCreateProgram || !glDeleteProgram || !glUseProgram ||
            !glCreateShader || !glDeleteShader || !glShaderSource || !glCompileShader || 
            !glGetShaderiv || !glGetShaderInfoLog || !glAttachShader || !glLinkProgram ||
            !glGetProgramiv || !glGetProgramInfoLog || !glGetUniformLocation ||
            !glUniform1fv )
        {
            printError("GL init error", "One or more required OpenGL functions were not found");
            return;
        }
#endif
    }
}


/*
 * readShaderFile(filename) - read a shader source string from a file
 */
unsigned char* readShaderFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if(file == NULL)
    {
        printError("ERROR", "Cannot open shader file!");
  		  return 0;
    }
    int bytesinfile = filelength(file);
    unsigned char *buffer = (unsigned char*)malloc(bytesinfile+1);
    int bytesread = fread( buffer, 1, bytesinfile, file);
    buffer[bytesread] = 0; // Terminate the string with 0
    fclose(file);
    
    return buffer;
}


/*
 * createShaders() - create, load, compile and link the GLSL shader objects.
 */
void createShader(GLuint *programObject, char *vertexshaderfile, char *fragmentshaderfile) {
     GLuint vertexShader;
     GLuint fragmentShader;

     const char *vertexShaderStrings[1];
     const char *fragmentShaderStrings[1];
     GLint vertexCompiled;
     GLint fragmentCompiled;
     GLint shadersLinked;
     char str[4096]; // For error messages from the GLSL compiler and linker

    // Create the vertex shader.
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    unsigned char *vertexShaderAssembly = readShaderFile( vertexshaderfile );
    vertexShaderStrings[0] = (char*)vertexShaderAssembly;
    glShaderSource( vertexShader, 1, vertexShaderStrings, NULL );
    glCompileShader( vertexShader);
    free((void *)vertexShaderAssembly);

    glGetShaderiv( vertexShader, GL_COMPILE_STATUS,
                               &vertexCompiled );
    if(vertexCompiled  == GL_FALSE)
  	{
        glGetShaderInfoLog(vertexShader, sizeof(str), NULL, str);
        printError("Vertex shader compile error", str);
  	}

  	// Create the fragment shader.
    fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

    unsigned char *fragmentShaderAssembly = readShaderFile( fragmentshaderfile );
    fragmentShaderStrings[0] = (char*)fragmentShaderAssembly;
    glShaderSource( fragmentShader, 1, fragmentShaderStrings, NULL );
    glCompileShader( fragmentShader );
    free((void *)fragmentShaderAssembly);

    glGetProgramiv( fragmentShader, GL_COMPILE_STATUS, 
                               &fragmentCompiled );
    if(fragmentCompiled == GL_FALSE)
   	{
        glGetShaderInfoLog( fragmentShader, sizeof(str), NULL, str );
        printError("Fragment shader compile error", str);
    }

    // Create a program object and attach the two compiled shaders.
    *programObject = glCreateProgram();
    glAttachShader( *programObject, vertexShader );
    glAttachShader( *programObject, fragmentShader );

    // Link the program object and print out the info log.
    glLinkProgram( *programObject );
    glGetProgramiv( *programObject, GL_LINK_STATUS, &shadersLinked );

    if( shadersLinked == GL_FALSE )
	{
		glGetProgramInfoLog( *programObject, sizeof(str), NULL, str );
		printError("Program object linking error", str);
	}
}


/*
 * computeFPS() - Calculate, display and return frame rate statistics.
 * Called every frame, but stats are recomputed only once per second.
 */
double computeFPS() {

    static double t0 = 0.0;
    static int frames = 0;
    static double fps = 0.0;
    static char titlestring[200];

    double t;
    
    // Get current time
    t = glfwGetTime();  // Gets number of seconds since glfwInit()
    // If one second has passed, or if this is the very first frame
    if( (t-t0) > 1.0 || frames == 0 )
    {
        fps = (double)frames / (t-t0);
        sprintf(titlestring, "GLSL procedural shaders (%.1f FPS)", fps);
        glfwSetWindowTitle(titlestring);
	// printf("Speed: %.1f FPS\n", fps);
        t0 = t;
        frames = 0;
    }
    frames ++;
    return fps;
}


/*
 * initQuadList(GLuint *listID, GLdouble scale) - create a display list
 * to render the quad geometry more efficently than by glVertex() calls.
 * (This is currently just as fast as a VBO, and I'm a bit lazy.)
 */
void initQuadList(GLuint *listID)
{
  *listID = glGenLists(1);
  
  glNewList(*listID, GL_COMPILE);
    glColor3f(1.0f, 1.0f, 1.0f); // White base color
    glBegin(GL_TRIANGLE_STRIP);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3f(-1.0f, -1.0f, 0.0f);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3f(1.0f, -1.0f, 0.0f);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3f(-1.0f, 1.0f, 0.0f);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3f(1.0f, 1.0f, 0.0f);
    glEnd();
  glEndList();
}

/*
 * drawTexturedSphere(r, segs) - Draw a sphere centered on the local
 * origin, with radius "r" and approximated by "segs" polygon segments,
 * with 2D texture coordinates in a latitude-longitude mapping.
 */
void drawTexturedSphere(float r, int segs) {
  int i, j;
  float x, y, z, z1, z2, R, R1, R2;

  // Top cap
  glBegin(GL_TRIANGLE_FAN);
  glNormal3f(0,0,1);
  glTexCoord2f(0.5f,1.0f); // This is an ugly (u,v)-mapping singularity
  glVertex3f(0,0,r);
  z = cos(M_PI/segs);
  R = sin(M_PI/segs);
    for(i = 0; i <= 2*segs; i++) {
      x = R*cos(i*2.0*M_PI/(2*segs));
      y = R*sin(i*2.0*M_PI/(2*segs));
      glNormal3f(x, y, z);
      glTexCoord2f((float)i/(2*segs), 1.0f-1.0f/segs);
      glVertex3f(r*x, r*y, r*z);
    }
  glEnd();  

  // Height segments
  for(j = 1; j < segs-1; j++) {
    z1 = cos(j*M_PI/segs);
    R1 = sin(j*M_PI/segs);
    z2 = cos((j+1)*M_PI/segs);
    R2 = sin((j+1)*M_PI/segs);
    glBegin(GL_TRIANGLE_STRIP);
    for(i = 0; i <= 2*segs; i++) {
      x = R1*cos(i*2.0*M_PI/(2*segs));
      y = R1*sin(i*2.0*M_PI/(2*segs));
      glNormal3f(x, y, z1);
      glTexCoord2f((float)i/(2*segs), 1.0f-(float)j/segs);
      glVertex3f(r*x, r*y, r*z1);
      x = R2*cos(i*2.0*M_PI/(2*segs));
      y = R2*sin(i*2.0*M_PI/(2*segs));
      glNormal3f(x, y, z2);
      glTexCoord2f((float)i/(2*segs), 1.0f-(float)(j+1)/segs);
      glVertex3f(r*x, r*y, r*z2);
    }
    glEnd();
  }

  // Bottom cap
  glBegin(GL_TRIANGLE_FAN);
  glNormal3f(0,0,-1);
  glTexCoord2f(0.5f, 1.0f); // This is an ugly (u,v)-mapping singularity
  glVertex3f(0,0,-r);
  z = -cos(M_PI/segs);
  R = sin(M_PI/segs);
    for(i = 2*segs; i >= 0; i--) {
      x = R*cos(i*2.0*M_PI/(2*segs));
      y = R*sin(i*2.0*M_PI/(2*segs));
      glNormal3f(x, y, z);
      glTexCoord2f(1.0f-(float)i/(2*segs), 1.0f/segs);
      glVertex3f(r*x, r*y, r*z);
    }
  glEnd();
}


/*
 * initSphereList(GLuint *listID, GLdouble scale) - create a display list
 * to render the sphere more efficently than calling lots of trigonometric
 * functions for each frame.
 */
void initSphereList(GLuint *listID)
{
  *listID = glGenLists(1);
  
  glNewList(*listID, GL_COMPILE);
  drawTexturedSphere(1.0, 20); // Radius 1.0, 20 segments
  glEndList();
}


/*
 * setupCamera() - set up the OpenGL projection and (model)view matrices
 */
void setupCamera() {

    int width, height;
    
    // Get window size. It may start out different from the requested
    // size, and will change if the user resizes the window.
    glfwGetWindowSize( &width, &height );
    if(height<=0) height=1; // Safeguard against iconified/closed window

    // Set viewport. This is the pixel rectangle we want to draw into.
    glViewport( 0, 0, width, height ); // The entire window

    // Select and setup the projection matrix.
    glMatrixMode(GL_PROJECTION); // "We want to edit the projection matrix"
    // Select and setup the projection matrix.
    glMatrixMode(GL_PROJECTION); // "We want to edit the projection matrix"
    glLoadIdentity(); // Reset the matrix to identity
    // 20 degrees FOV, same aspect ratio as viewport, depth range 1 to 100
    gluPerspective( 15.0f, (GLfloat)width/(GLfloat)height, 1.0f, 100.0f );

    // Select and setup the modelview matrix.
    glMatrixMode( GL_MODELVIEW ); // "We want to edit the modelview matrix"
    glLoadIdentity(); // Reset the matrix to identity
    // Look from 0,-4,0 towards 0,0,0 with Z as "up" in the image
    gluLookAt( 0.0f, -8.0f, 1.0f,  // Eye position
               0.0f, 0.0f, -0.5f,   // View point
               0.0f, 0.0f, 1.0f ); // Up vector
}


/*
 * renderScene() - draw the objects with their corresponding shaders
 */
void renderScene( GLuint quadListID, GLuint sphereListID, GLuint programObject[] )
{
  GLint location_time = -1;
  float time = 0.0f;

  // Render the "ground plane" quad with the first shader
  glUseProgram( programObject[0] );
  // Update the uniform time variable.
  location_time = glGetUniformLocation( programObject[0], "time" );
  // glUniform1f() is bugged in Linux Nvidia driver 260.19.06,
  // so we use glUniform1fv() instead to work around the bug.
  if ( location_time != -1 ) {
    time = (float)glfwGetTime();
    glUniform1fv( location_time, 1, &time );
  }
  glPushMatrix();
    glTranslatef(0.0f, 3.0f, -1.5f);
    glScalef(4.0f, 4.0f, 1.0f);
    glCallList( quadListID );
  glPopMatrix();

  // Render four spheres, each with their own shader
  glUseProgram( programObject[1] );
  location_time = glGetUniformLocation( programObject[1], "time" );
  if ( location_time != -1 ) {
    time = (float)glfwGetTime();
    glUniform1fv( location_time, 1, &time );
  }
  glPushMatrix();
    glTranslatef(-1.8f, -2.0f, 0.0f);
    glScalef(0.5f, 0.5f, 0.5f);
    glCallList( sphereListID );
  glPopMatrix();

  glUseProgram( programObject[2] );
  location_time = glGetUniformLocation( programObject[2], "time" );
  if ( location_time != -1 ) {
    time = (float)glfwGetTime();
    glUniform1fv( location_time, 1, &time );
  }
  glPushMatrix();
    glTranslatef(-0.6f, -2.0f, 0.0f);
    glScalef(0.5f, 0.5f, 0.5f);
    glCallList( sphereListID );
  glPopMatrix();

  glUseProgram( programObject[3] );
  location_time = glGetUniformLocation( programObject[3], "time" );
  if ( location_time != -1 ) {
    time = (float)glfwGetTime();
    glUniform1fv( location_time, 1, &time );
  }
  glPushMatrix();
    glTranslatef(0.6f, -2.0f, 0.0f);
    glScalef(0.5f, 0.5f, 0.5f);
    glCallList( sphereListID );
  glPopMatrix();

  glUseProgram( programObject[4] );
  location_time = glGetUniformLocation( programObject[4], "time" );
  if ( location_time != -1 ) {
    time = (float)glfwGetTime();
    glUniform1fv( location_time, 1, &time );
  }
  glPushMatrix();
    glTranslatef(1.8f, -2.0f, 0.0f);
    glScalef(0.5f, 0.5f, 0.5f);
    glCallList( sphereListID );
  glPopMatrix();
  // Deactivate the shaders.
  glUseProgram(0);
}


/*
 * main(argc, argv) - the standard C entry point for the program
 */
int main(int argc, char *argv[]) {

    GLuint quadList, sphereList;
    GLuint programObject[5]; // One quad and four spheres
    double fps = 0.0;
    GLFWvidmode vidmode;

    GLboolean running = GL_TRUE; // Main loop exits when this is set to GL_FALSE
    
    // Initialise GLFW
    glfwInit();

    // Open a temporary OpenGL window just to determine the desktop size
    if( !glfwOpenWindow(256, 256, 8,8,8,8, 32,0, GLFW_WINDOW) )
    {
        glfwTerminate(); // glfwOpenWindow failed, quit the program.
        return 1;
    }
    glfwGetDesktopMode(&vidmode);
    glfwCloseWindow();

    // Open a window to cover the width of the current desktop
    if( !glfwOpenWindow(vidmode.Width-20, (int)(0.316*(vidmode.Width-20)), 8,8,8,8, 32,0, GLFW_WINDOW) )
    {
        glfwTerminate(); // glfwOpenWindow failed, quit the program.
        return 1;
    }
    
    // Load the extensions for GLSL - note that this has to be done
    // *after* the window has been opened, or we won't have a GL context
    // to query for those extensions and connect to instances of them.
    loadExtensions();
    
    printf("GL vendor:       %s\n", glGetString(GL_VENDOR));
    printf("GL renderer:     %s\n", glGetString(GL_RENDERER));
    printf("GL version:      %s\n", glGetString(GL_VERSION));
    printf("Desktop size:    %d x %d pixels\n", vidmode.Width, vidmode.Height);

    glEnable(GL_DEPTH_TEST); // Use the Z buffer

    glfwSwapInterval(0); // Do not wait for screen refresh between frames

    // Compile a display list for the demo geometry, to render it efficiently
    initQuadList(&quadList);
    initSphereList(&sphereList);
    
	createShader(&programObject[0], PATH VERTSHADER0, PATH FRAGSHADER0);
	createShader(&programObject[1], PATH VERTSHADER1, PATH FRAGSHADER1);
	createShader(&programObject[2], PATH VERTSHADER2, PATH FRAGSHADER2);
	createShader(&programObject[3], PATH VERTSHADER3, PATH FRAGSHADER3);
	createShader(&programObject[4], PATH VERTSHADER4, PATH FRAGSHADER4);
    // Main loop
    while(running)
    {
        // Calculate and update the frames per second (FPS) display
        fps = computeFPS();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.3f, 0.5f, 0.0f);
        // Set up the camera projection.
        setupCamera();
        
        // Draw the scene.
        renderScene(quadList, sphereList, programObject);

        // Swap buffers, i.e. display the image and prepare for next frame.
        glfwSwapBuffers();

        // Exit if the ESC key is pressed or the window is closed.
        if(glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED)) {
          running = GL_FALSE;
        }
    }

    printf("Performance:     %.1f FPS\n", fps);

    // Close the OpenGL window and terminate GLFW.
    glfwTerminate();

    return 0;
}
