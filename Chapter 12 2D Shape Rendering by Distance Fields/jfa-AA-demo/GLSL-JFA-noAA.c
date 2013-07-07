/*
 *======================================================
 * Demo of jump flooding algoritm for EDT using GLSL
 * Author: Stefan Gustavson (stefan.gustavson@gmail.com)
 * 2010-08-24. This code is in the public domain.
 *======================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glfw.h> // GLFW is a cross-platform OpenGL framework
#include "glext.h" // Needed on some systems

/* Global function pointers for extensions we need to load in Windows */
/* Multitexturing */
PFNGLACTIVETEXTUREPROC        glActiveTexture        = NULL;

/* GLSL shaders */
PFNGLCREATEPROGRAMPROC        glCreateProgram        = NULL;
PFNGLDELETEPROGRAMPROC        glDeleteProgram        = NULL;
PFNGLUSEPROGRAMPROC           glUseProgram           = NULL;
PFNGLCREATESHADERPROC         glCreateShader         = NULL;
PFNGLSHADERSOURCEPROC         glShaderSource         = NULL;
PFNGLCOMPILESHADERPROC        glCompileShader        = NULL;
PFNGLGETSHADERIVPROC          glGetShaderiv          = NULL;
PFNGLGETPROGRAMIVPROC         glGetProgramiv         = NULL;
PFNGLATTACHSHADERPROC         glAttachShader         = NULL;
PFNGLGETSHADERINFOLOGPROC     glGetShaderInfoLog     = NULL;
PFNGLGETPROGRAMINFOLOGPROC    glGetProgramInfoLog    = NULL;
PFNGLLINKPROGRAMPROC          glLinkProgram          = NULL;
PFNGLGETUNIFORMLOCATIONPROC   glGetUniformLocation   = NULL;
PFNGLUNIFORM4FPROC            glUniform4f            = NULL;
PFNGLUNIFORM1FPROC            glUniform1f            = NULL;
PFNGLUNIFORM1IPROC            glUniform1i            = NULL;

/* FBO for render-to-texture (no render buffer stuff) */
PFNGLGENFRAMEBUFFERSPROC                     glGenFramebuffers = NULL;
PFNGLDELETEFRAMEBUFFERSPROC                  glDeleteFramebuffers = NULL;
PFNGLBINDFRAMEBUFFERPROC                     glBindFramebuffer = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSPROC              glCheckFramebufferStatus = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv = NULL;
PFNGLGENERATEMIPMAPPROC                      glGenerateMipmap = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC                glFramebufferTexture2D = NULL;


/*
 * printError - Signal an error.
 */
void printError(const char *errtype, const char *errmsg) {
  fprintf(stderr, "%s: %s\n", errtype, errmsg);
}


/*
 * loadExtensions - Load the required OpenGL extensions.
 */
void loadExtensions() {
  // Checking for extensions is prone to breaking, because we are looking for
  // features which are since long in the core profile. Instead, just try to
  // load the stuff we want and check that the function pointers are OK.
  glActiveTexture      = (PFNGLACTIVETEXTUREPROC)glfwGetProcAddress("glActiveTexture");

  glCreateProgram      = (PFNGLCREATEPROGRAMPROC)glfwGetProcAddress("glCreateProgram");
  glDeleteProgram      = (PFNGLDELETEPROGRAMPROC)glfwGetProcAddress("glDeleteProgram");
  glUseProgram         = (PFNGLUSEPROGRAMPROC)glfwGetProcAddress("glUseProgram");
  glCreateShader       = (PFNGLCREATESHADERPROC)glfwGetProcAddress("glCreateShader");
  glShaderSource       = (PFNGLSHADERSOURCEPROC)glfwGetProcAddress("glShaderSource");
  glCompileShader      = (PFNGLCOMPILESHADERPROC)glfwGetProcAddress("glCompileShader");
  glGetShaderiv        = (PFNGLGETSHADERIVPROC)glfwGetProcAddress("glGetShaderiv");
  glGetProgramiv       = (PFNGLGETPROGRAMIVPROC)glfwGetProcAddress("glGetProgramiv");
  glAttachShader       = (PFNGLATTACHSHADERPROC)glfwGetProcAddress("glAttachShader");
  glGetShaderInfoLog   = (PFNGLGETSHADERINFOLOGPROC)glfwGetProcAddress("glGetShaderInfoLog");
  glGetProgramInfoLog  = (PFNGLGETPROGRAMINFOLOGPROC)glfwGetProcAddress("glGetProgramInfoLog");
  glLinkProgram        = (PFNGLLINKPROGRAMPROC)glfwGetProcAddress("glLinkProgram");
  glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glfwGetProcAddress("glGetUniformLocation");
  glUniform4f          = (PFNGLUNIFORM4FPROC)glfwGetProcAddress("glUniform4f");
  glUniform1f          = (PFNGLUNIFORM1FPROC)glfwGetProcAddress("glUniform1f");
  glUniform1i          = (PFNGLUNIFORM1IPROC)glfwGetProcAddress("glUniform1i");

  glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers");
  glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffers");
  glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");
  glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatus");
  glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)glfwGetProcAddress("glGetFramebufferAttachmentParameteriv");
  glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)glfwGetProcAddress("glGenerateMipmap");
  glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glfwGetProcAddress("glFramebufferTexture2D");
  
  if( !glActiveTexture ||
    !glCreateProgram || !glDeleteProgram || !glUseProgram ||
    !glCreateShader || !glCreateShader || !glCompileShader || 
    !glGetShaderiv || !glGetProgramiv || !glAttachShader || !glGetProgramInfoLog || 
    !glLinkProgram || !glGetUniformLocation || !glUniform4f ||
    !glUniform1f || !glUniform1i ||
    !glGenFramebuffers || !glDeleteFramebuffers || !glBindFramebuffer ||
    !glCheckFramebufferStatus || !glGetFramebufferAttachmentParameteriv ||
    !glGenerateMipmap || !glFramebufferTexture2D )
    {
      printError("GL init error", "One or more required functions were not found");
      return;
    }
}


/*
 * filelength - Determine the number of bytes in a file.
 * This is a lazy hack to avoid calling stat(), but it works.
 */
int filelength(const char *filename) {
  FILE *ifp;
  int length = 0;
  
  ifp = fopen(filename, "r");
  fseek(ifp, 0, SEEK_END);
  length = (int)ftell(ifp);
  fclose(ifp);
  return length;
}


/*
 * readShaderFile - read shader source from a file to a string.
 */
unsigned char* readShaderFile(const char *filename) {
  FILE *file = fopen(filename, "r");
  if(file == NULL)
    {
      printError("I/O error", "Cannot open shader file!");
      return NULL;
    }
  int bytesinfile = filelength(filename);
  unsigned char *buffer = (unsigned char*)malloc(bytesinfile+1);
  int bytesread = fread( buffer, 1, bytesinfile, file);
  buffer[bytesread] = 0; // Terminate the string with 0
  fclose(file);
  
  return buffer;
}


/*
 * createShader - create, load, compile and link the shader object.
 */
GLuint createShader(const char *vertfilename, const char *fragfilename) {
  
  GLuint programObj;
  GLuint fragmentShader;
  GLuint vertexShader;
  const char *vertexShaderStrings[1];
  GLint vertexCompiled;
  const char *fragmentShaderStrings[1];
  GLint fragmentCompiled;
  GLint shadersLinked;
  char str[4096]; // For error messages from the GLSL compiler and linker
  
  // Create the vertex and fragment shaders
  vertexShader = glCreateShader( GL_VERTEX_SHADER );
  
  char *vertexShaderAssembly = readShaderFile( vertfilename );
  if(!vertexShaderAssembly) {
    printError("I/O Error", "Vertex shader not found, no shader program created.");
    return 0;
  }
  vertexShaderStrings[0] = vertexShaderAssembly;
  glShaderSource( vertexShader, 1, vertexShaderStrings, NULL );
  glCompileShader( vertexShader );
  free((void *)vertexShaderAssembly);
  
  glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &vertexCompiled );
  if(vertexCompiled == GL_FALSE)
    {
      glGetShaderInfoLog( vertexShader, sizeof(str), NULL, str );
      printError("Vertex shader compile error", str);
    }
  
  fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
  
  char *fragmentShaderAssembly = readShaderFile( fragfilename );
  if(!fragmentShaderAssembly) {
    printError("I/O Error", "Fragment shader not found, no shader program created.");
    return 0;
  }
  fragmentShaderStrings[0] = fragmentShaderAssembly;
  glShaderSource( fragmentShader, 1, fragmentShaderStrings, NULL );
  glCompileShader( fragmentShader );
  free((void *)fragmentShaderAssembly);
  
  glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &fragmentCompiled );
  if(fragmentCompiled == GL_FALSE)
    {
      glGetShaderInfoLog( fragmentShader, sizeof(str), NULL, str );
      printError("Fragment shader compile error", str);
    }
  
  // Create a program object and attach the compiled shaders
  programObj = glCreateProgram();
  glAttachShader( programObj, vertexShader );
  glAttachShader( programObj, fragmentShader );
  
  // Link the program object and print out the info log
  glLinkProgram( programObj );
  glGetProgramiv( programObj, GL_LINK_STATUS, &shadersLinked );
  
  if( shadersLinked == GL_FALSE )
    {
      glGetProgramInfoLog( programObj, sizeof(str), NULL, str );
      printError("Shader program linking error", str);
    }

  return programObj;
}


/*
 * setUniformVariables - set the uniform shader variables we need.
 */
 void setUniformVariables(GLuint programObj,
                          int texture,
                          float texw, float texh,
                          float texlevels,
                          float step) {

  GLint location_texture = -1;
  GLint location_texw = -1;
  GLint location_texh = -1;
  GLint location_texlevels = -1;
  GLint location_step = -1;

  // Activate the shader to set its state
  glUseProgram( programObj );

  // Locate the uniform shader variables by name and set them:
  // one texture and two integers to tell the size of the texture
  location_texture = glGetUniformLocation( programObj, "texture" );
  if(location_texture != -1)
    glUniform1i( location_texture, texture );
  
  location_texw = glGetUniformLocation( programObj, "texw" );
  if(location_texw != -1)
    glUniform1f( location_texw, texw );
  
  location_texh = glGetUniformLocation( programObj, "texh" );
  if(location_texh != -1)
    glUniform1f( location_texh, texh );

  location_texlevels = glGetUniformLocation( programObj, "texlevels" );
  if(location_texlevels != -1)
    glUniform1f( location_texlevels, texlevels );

  location_step = glGetUniformLocation( programObj, "step" );
  if(location_step != -1)
    glUniform1f( location_step, step );

  // Deactivate the shader again
  glUseProgram( 0 );
}


/*
 * loadShapeTexture - load 8-bit shape texture data
 * from a TGA file and set up the corresponding texture object.
 */
void loadShapeTexture(char *filename, GLuint texID, int *texw, int *texh) {
  
  GLFWimage teximg; // Use intermediate GLFWimage to get width and height

  if( !glfwReadImage(filename, &teximg, GLFW_NO_RESCALE_BIT) )
    printError("I/O error", "Failed to load shape texture from TGA file.");
  
  *texw = teximg.Width;
  *texh = teximg.Height;
  
  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, texID );

  glfwLoadTextureImage2D( &teximg, 0 );
  // This is the input image. We want unaltered 1-to-1 pixel values,
  // so specify nearest neighbor sampling to be sure.
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glfwFreeImage( &teximg ); // Clean up the malloc()'ed data pointer
}


/*
 * createBufferTexture - create an 8-bit texture render target
 */
void createBufferTexture(GLuint texID, int texw, int texh) {
  
  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, texID );
  float GLblack[4] = {0.0f, 0.0f, 0.0f, 0.0f};

  // The special shader used to render this texture performs a
  // per-pixel image processing where point sampling is required,
  // so specify nearest neighbor sampling.
  // Also, the flood fill shader handles its own edge clamping, so
  // texture mode GL_REPEAT is inconsequential. "Zero outside" would
  // be useful, but separate edge values are deprecated in OpenGL.
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
  glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, GLblack );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RG16, texw, texh, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
  glBindTexture( GL_TEXTURE_2D, 0 );
}


/*
 * showFPS - Calculate and report texture size and frames per second
 * in the window title bar (updated once per second)
 */
void showFPS(int texw, int texh) {
  
  static double t0 = 0.0;
  static int frames = 0;
  static char titlestr[200];
  double t, fps;
  
  // Get current time
  t = glfwGetTime();  // Number of seconds since glfwInit()
  // If one second has passed, or if this is the very first frame
  if( (t - t0) > 1.0 || frames == 0 )
    {
      fps = (double)frames / (t - t0);
      sprintf(titlestr, "%dx%d texture, %.1f FPS", texw, texh, fps);
      glfwSetWindowTitle(titlestr);
      t0 = t;
      frames = 0;
    }
  frames ++;
}


/*
 * renderScene - the OpenGL commands to render our scene.
 */
void renderScene(GLuint programObj, int width, int height)
{
  // Clear color buffer
  //glClearColor( 0.0f, 0.0f, 0.5f, 0.0f );
  //glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
  //glClear( GL_COLOR_BUFFER_BIT );
  
  // Select and set up the projection matrix
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D( 0, width, 0, height );
  glViewport( 0, 0, width, height );
  
  // Select and set up the modelview matrix
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  
  // Use the shader object for the rendering
  glUseProgram( programObj );
  
  // Draw one texture mapped quad in the (x,y) plane
  glBegin( GL_QUADS );
  glTexCoord2f( 0.0f, 0.0f );
  glVertex2f( 0.0f, 0.0f );
  glTexCoord2f( 1.0f, 0.0f );
  glVertex2f( (GLfloat)width, 0.0f );
  glTexCoord2f( 1.0f, 1.0f );
  glVertex2f( (GLfloat)width, (GLfloat)height );
  glTexCoord2f( 0.0f, 1.0f );
  glVertex2f( 0.0f, (GLfloat)height );
  glEnd();
  
  // Deactivate the shader object.
  glUseProgram(0);
}

/*
 * checkGLError() - check for and report any OpenGL errors
 */
void checkGLError(char *label) {
  GLenum status = glGetError();
  if( status != GL_NO_ERROR )
    printf("GL error at %s: %s\n", label, gluErrorString(status));
}


//========================================================================
// main()
//========================================================================

int main(int argc, char *argv[])
{
  // UI-related variables
  int running = 0;

  // Shader-related variables
  GLuint textureID[3];
  int texw = 0;
  int texh = 0;
  int width, height;
  GLint useShaders = GL_TRUE;
  GLuint programObj[3];
  GLuint fboID;
  int stepsize;
  int lastRendered = 0;
  unsigned short *outdata;
  FILE *outfile;

  // Initialise GLFW
  glfwInit();
  
  // Open OpenGL window
  if( !glfwOpenWindow( 512, 512, 0,0,0,0, 0,0, GLFW_WINDOW ) )
    {
      glfwTerminate();
      return 0;
    }
  
  // Load all extensions required under Windows
  loadExtensions();

  // Load one texture with the original image
  // and create two textures of the same size for the iterative rendering
  checkGLError("After opening window");
  glEnable(GL_TEXTURE_2D);
  glActiveTexture( GL_TEXTURE0 );
  glGenTextures( 3, textureID );
  checkGLError("After generating texture IDs");
  loadShapeTexture("shape1.tga", textureID[0], &texw, &texh);
  createBufferTexture(textureID[1], texw, texh);
  createBufferTexture(textureID[2], texw, texh);
  checkGLError("After creating textures");

  // Create the FBO to be used for rendering to textures 
  glGenFramebuffers(1, &fboID);
  checkGLError("After generating FBO");
  
  // Create, load and compile the shader programs
  programObj[0] = createShader("vertex.glsl", "fragment_seed.glsl");
  programObj[1] = createShader("vertex.glsl", "fragment_flood.glsl");
  programObj[2] = createShader("vertex.glsl", "fragment_display.glsl");
  checkGLError("After creating shaders");
  
  // Disable vertical sync (on cards that support
  // it, and if current driver settings so permit)
  glfwSwapInterval( 0 );
  
  // Main loop
  running = GL_TRUE;
  while( running )
    {
      showFPS(texw, texh);
      
      if(!useShaders) {
           glBindTexture( GL_TEXTURE_2D, textureID[0] ); // Pass-through
      }
      else {
           // Set the uniform shader variables for seed initialisation
           setUniformVariables( programObj[0], 0, (float)texw, (float)texh,
                                (float)65536, (float)0 );

           // Init seeds: bind input image and render to texture 1
           glBindTexture( GL_TEXTURE_2D, textureID[0] );
	   checkGLError("After binding input texture");
           // Bind the FBO and attach texture 1 to its color attachment point
           glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fboID );
	   checkGLError("After binding FBO");
           lastRendered = 1;
           glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, textureID[lastRendered], 0 );
	   checkGLError("After attaching texture to FBO");
           // Render the scene to texture 1 with the seed shader active
           renderScene( programObj[0], texw, texh );
	   checkGLError("After rendering seed pass");

           // Jump flooding, halving step sizes from texsize/2 down to 1
           stepsize = (texw > texh ? texw/2: texh/2);
           while (stepsize > 0) {
               setUniformVariables( programObj[1], 0, (float)texw, (float)texh,
                                    (float)65536, (float)stepsize );
               glBindTexture( GL_TEXTURE_2D, textureID[lastRendered] );
	       checkGLError("After binding texture most recently rendered to");
               lastRendered = (lastRendered == 1 ? 2 : 1); // Swap 1 <-> 2
               // Swap which texture is attached to the FBO
               glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                       GL_TEXTURE_2D, textureID[lastRendered], 0 );
	       checkGLError("After binding new texture to FBO");
               renderScene(programObj[1], texw, texh);
	       checkGLError("After rendering flood pass");
               stepsize = stepsize / 2;
           }

           // Result display: deactivate the FBO, bind the last texture rendered to
           glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
	   checkGLError("After binding window to FBO");
           glBindTexture( GL_TEXTURE_2D, textureID[lastRendered] );
	   checkGLError("After binding final texture rendered to");
      }

      // Get window size (may be resized at any time)
      glfwGetWindowSize( &width, &height );
      if( height<1 ) height = 1;
      if( width<1 ) width = 1;
      // Render the scene
      setUniformVariables( programObj[2], 0, (float)texw, (float)texh,
                           (float)65536, (float)0 );
      renderScene(programObj[2], width, height);
      checkGLError("After rendering display pass");
      
      if(glfwGetKey('W')) {
	checkGLError("When receiving W keystroke event");
        glBindTexture( GL_TEXTURE_2D, textureID[lastRendered] );
	checkGLError("After binding texture to write to file");
	printf("Writing texture %d to file 'output.raw'.\n", lastRendered);
	outdata = (unsigned short*)malloc(texw*texh*4*sizeof(unsigned short));
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_SHORT, (void *)outdata);
	checkGLError("After reading texture using glGetTexImage");
	outfile = fopen("outdata.raw", "w");
	fwrite(outdata, sizeof(unsigned short), 4*texw*texh, outfile);
	fclose(outfile);
	free(outdata);
      }

      glfwSwapBuffers();
      
      if(glfwGetKey('1')) {
        loadShapeTexture("shape1.tga", textureID[0], &texw, &texh);
        createBufferTexture(textureID[1], texw, texh);
        createBufferTexture(textureID[2], texw, texh);
      }
      if(glfwGetKey('2')) {
        loadShapeTexture("shape2.tga", textureID[0], &texw, &texh);
        createBufferTexture(textureID[1], texw, texh);
        createBufferTexture(textureID[2], texw, texh);
      }
      if(glfwGetKey('3')) {
        loadShapeTexture("shape3.tga", textureID[0], &texw, &texh);
        createBufferTexture(textureID[1], texw, texh);
        createBufferTexture(textureID[2], texw, texh);
      }
      if(glfwGetKey('4')) {
        loadShapeTexture("shape4.tga", textureID[0], &texw, &texh);
        createBufferTexture(textureID[1], texw, texh);
        createBufferTexture(textureID[2], texw, texh);
      }
      if(glfwGetKey(GLFW_KEY_F1)) {
        useShaders = GL_TRUE;
      }
      if(glfwGetKey(GLFW_KEY_F2)) {
        useShaders = GL_FALSE;
      }

      // Check if the ESC key is pressed or the window has been closed
      running = !glfwGetKey( GLFW_KEY_ESC ) &&
	glfwGetWindowParam( GLFW_OPENED );

    }
  
  // Close the window (if still open) and terminate GLFW
  glfwTerminate();
  
  return 0;
}
