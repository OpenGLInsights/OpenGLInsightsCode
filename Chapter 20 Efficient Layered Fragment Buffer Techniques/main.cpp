/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */



//opengl32.lib glew32.lib freetype248MT.lib libpng.lib zlib.lib openctm.lib SDL.lib glu32.lib

#include "pyarlib/prec.h"
#include "pyarlib/pyarlib.h"

#include <assert.h>

#include "extraglenums.h"

#include "lfbRagged.h"
#include "lfbBasic.h"
#include "lfbLinked.h"
#include "lfbPages.h"

Jeltz jeltz("LFB");
JeltzFly fly;
JeltzGUI gui;

int maxFragments = 64;
bool lfbRaggedBlendingToggle = true;
int currentModel = 0;
bool rotateCamera = false;

const char* modelNames[] = {"dragon.ctm", "sponza/sponza.3ds", "chinesedragon.ctm", "happybuddha.ctm", "bunny.ctm", "armadillo.ctm", "lucy.ctm"};

Profile profiler;
Camera& camera = fly.camera;
VBOMesh model;
VBOMesh nextModel;

LFB* lfb = NULL;
int currentMethod = -1;

//for screenshots and rendering to really large targets
#if 0
#define RENDERSCALE 1
FrameBuffer rtt; 
#endif

bool wireframe = false;
int numFrags = 0;

volatile bool modelLoading = false; //don't start multiple loading threads :)
volatile bool modelReady = false; //essentially a semaphore for the loading thread to notifiy the main loop

//this timer waits a little after resizing before drawing using the LFB
//a resize soon after is common and annoying if the program freezes a little immediately after each adjustment
float resizeTimer = 0.0f;

Shader phong("phong");
Shader renderProgram("lfbRender");
Shader transparencyProgram("lfbDisplay");

QG::Widget methodGroup;
QG::Widget infoGroup;
QG::Label modelName("");
QG::Slider alpha("Alpha", 0, 1, true);
QG::RadioButton methodNone("0. Off");
QG::RadioButton methodBasic("1. Basic");
QG::RadioButton methodLinked("2. Linked Lists");
QG::RadioButton methodPages("3. Linked Pages");
QG::RadioButton methodLinearized("4. Linearized");
QG::CheckBox useBlending("Blending");
QG::CheckBox useGlobalSort("Global Sort");

QG::Label profileInfo("<Times>");
QG::Label memoryInfo("<Memory>");

void setMethod(int index)
{
	if (currentMethod == index)
		return; //already using this method

	currentMethod = index;
	
	//delete the current LFB
	delete lfb;
	
	//clear #defines in shaders (the next LFB may not define some values the previous LFB did)
	renderProgram.undefAll();
	transparencyProgram.undefAll();

	//only show blending option for lfbRagged
	useBlending.hide(index != 4);
	
	//create the new LFB
	switch (index)
	{
	case 1: lfb = new BasicLFB(); break;
	case 2: lfb = new LinkedLFB(); break;
	case 3: lfb = new PagesLFB(); break;
	case 4:
		lfb = new RaggedLFB();
		static_cast<RaggedLFB*>(lfb)->useBlending(lfbRaggedBlendingToggle);
		static_cast<RaggedLFB*>(lfb)->useGlobalSort(useGlobalSort.b);
		break;
	default:
		memoryInfo.textf("<Memory>");
		lfb = NULL;
	}
	
	//separate console text when changing LFBs
	printf("\n");
	
	//print the name of the new LFB
	if (lfb)
		printf("%s\n", lfb->getName().c_str());
	else
		printf("No LFB\n");
	
	//attach the profiler to the LFB for step time info
	if (lfb)
		lfb->profile = &profiler;
	
	//update the GUI
	methodBasic.set(index);
	profileInfo.width = 10;
	profileInfo.height = 10;
	memoryInfo.width = 10;
	memoryInfo.height = 10;
	
	//allocation of the LFB will cause a frame-time spike. ignore the next sample
	gui.ignoreNextTime();
}

void clickMethod()
{
	setMethod(methodNone.current);
}

void drawScene()
{
	GLint program = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	
	GLuint modelviewLoc = -1;
	GLuint projectionLoc = -1;
	GLuint normalMatLoc = -1;
	GLuint lightLoc = -1;
	if (program != 0)
	{
		modelviewLoc = glGetUniformLocation(program, "modelviewMat");
		projectionLoc = glGetUniformLocation(program, "projectionMat");
		normalMatLoc = glGetUniformLocation(program, "normalMat");
		lightLoc = glGetUniformLocation(program, "lightPos");
		assert(modelviewLoc != (GLuint)-1);
		assert(projectionLoc != (GLuint)-1);
		//assert(normalMatLoc != (GLuint)-1);
		glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, camera.getInverse().m);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, camera.getProjection().m);
		glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, mat33(camera.getTransform().transpose()).m);
		vec4f light = camera.getInverse() * vec4f(10, 10, 10, 1);
		glUniform4f(lightLoc, light.x, light.y, light.z, light.w);
	}
	
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//similarly to the above uniform locations, .draw() gets sets attributes for the currently bound shader
	model.draw();
			
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void stencilRecord()
{
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
}

void stencilMask()
{
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void stencilOff()
{
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDisable(GL_STENCIL_TEST);
}

void display()
{
	CHECKERROR;
	#ifdef RENDERSCALE
	rtt.bind();
	#endif

	if (resizeTimer > 0.0 || !lfb)
	{
		//standard rendering without the LFB
		
		profiler.begin();
		
		if (alpha.f < 1)
			glDisable(GL_DEPTH_TEST);
		else
			glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		phong.use();
		phong.set("alpha", alpha.f);

		if (phong.error())
		{
			//opengl fixed pipeline fallback
			fly.uploadCamera();
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glEnable(GL_NORMALIZE);
		}

		drawScene();
		phong.unuse();
		profiler.time("Render");
	}
	else
	{
		//we're rendering all layers. no depth testing
		glDisable(GL_DEPTH_TEST);
	
		//update static defines. shaders compile on the first .use() call and will only recompile as #defines change
		lfb->setMaxFrags(maxFragments);
		lfb->setDefines(renderProgram);
		lfb->setDefines(transparencyProgram);
		
		//init lfb, zero buffers etc
		lfb->begin(); //this assumes the LFB resolution is the same as GL_VIEWPORT
	
		//use our render shader, external to the LFB implementation
		renderProgram.use();
		if (renderProgram.error())
		{
			resizeTimer = 1.0;
			return;
		}
	
		//fill an empty pixel mask for the resolve pass.
		//if RaggedLFB blending is used, the blending FBO may be bound. fill the stencil buffer in pass 2
		if (!dynamic_cast<RaggedLFB*>(lfb))
			stencilRecord();
	
		//set the uniforms for our shader and draw the scene
		if (lfb->setUniforms(renderProgram))
			drawScene(); //FIRST RENDERING PASS
		else
		{
			printf("A problem has occurred with the LFB. Most likely out of memory.\n");
			resizeTimer = 1.0;
		}
		
		stencilOff();
		
		//the first pass is done
		renderProgram.unuse();
	
		//count() will compute prefix sums for the ragged method and check for lists out of memory
		if (lfb->count())
		{
			//for the ragged LFB we create the stencil mask during the second render
			if (dynamic_cast<RaggedLFB*>(lfb))
				stencilRecord();
			
			//some LFB methods require an additional pass (always for ragged, if out of memory for linked lists)
			renderProgram.use();
			if (lfb->setUniforms(renderProgram))
				drawScene(); //SECOND RENDERING PASS
			else
			{
				printf("A problem has occurred with the LFB. Most likely out of memory.\n");
				resizeTimer = 1.0;
			}
			renderProgram.unuse();
			
			stencilOff();
		}
	
		//render to LFB done. get the total number of rendered fragments
		numFrags = lfb->end();
		
		//on to drawing transparency. clear first
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		profiler.time("Clear");
	
		//our post process transparency resolver, again external to the LFB
		transparencyProgram.use();
		if (transparencyProgram.error())
		{
			resizeTimer = 1.0;
			return;
		}
		
		//this matrix allows wolrd space fragment positions to be computed from depth
		//transparencyProgram.set("clipToWorld", (camera.getTransform() * camera.getProjection().inverse()));
		
		transparencyProgram.set("alpha", alpha.f);
		
		//empty pixels take a long time to render. we reduce that problem using the stencil buffer
		stencilMask();

		//render a post process pass
		if (lfb->setUniforms(transparencyProgram))
		{
			drawSSQuad();
		}
		else
		{
			printf("A problem has occurred with the LFB. Most likely out of memory.\n");
			resizeTimer = 1.0;
		}
		transparencyProgram.unuse();
		
		//done rendering transparency
		stencilOff();
		
		glMemoryBarrierEXT(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT);
		profiler.time("Resolve");
	}
	
	#ifdef RENDERSCALE
	rtt.unbind();
	rtt.blit(0, false, jeltz.winSize());
	#endif
	
	CHECKERROR;
}

void loadModel()
{
	assert(modelLoading);

	//load the next model
	printf("Loading %s\n", modelNames[currentModel]);
	if (!nextModel.load(modelNames[currentModel]))
	{
		nextModel.release();
		nextModel = VBOMesh::grid(vec2i(16, 8), VBOMesh::paramSphere);
		nextModel.transform(mat44::scale(vec3f(0.05f)));
	}
	
	printf("%i Polygons\n", nextModel.numPolygons);
	printf("Centering...\n");
	if (!nextModel.computeInfo())
		return;
	mat44 center = mat44::translate(-nextModel.center);
	vec3f size = nextModel.boundsMax - nextModel.boundsMin;
	if (size.size() < 0.01 || size.size() > 1.0)
	{
		//model is tiny or huge. scale to better range
		center = mat44::scale(vec3f(0.2f/size.y)) * center;
		printf("\t(and scaling)\n");
	}
	nextModel.transform(center);
	nextModel.computeInfo();
	
	currentModel = (currentModel + 1) % (sizeof(modelNames) / sizeof(char*));
	modelReady = true;
	modelLoading = false;
}

void loadNextModel()
{
	if (!modelLoading)
	{
		modelName.hide(false);
		modelName.width = 10;
		modelName.textf("Loading %s", modelNames[currentModel]);
		modelLoading = true;
		Thread::create(loadModel);
	}
	else
		printf("Error: Next model already loading.\n");
}

void toggleBlending()
{
	//toggle the use of blending for the linearized LFB counting pass
	RaggedLFB* rlfb = dynamic_cast<RaggedLFB*>(lfb);
	if (rlfb)
	{
		lfbRaggedBlendingToggle = !lfbRaggedBlendingToggle;
		printf("Blending: %i\n", lfbRaggedBlendingToggle);
		renderProgram.define("COUNT_USING_BLENDING", intToString((int)lfbRaggedBlendingToggle));
		rlfb->useBlending(lfbRaggedBlendingToggle);
	}
	else
		printf("Error: Blending only applied to the Linearized LFB\n");
	useBlending.b = lfbRaggedBlendingToggle;
	useBlending.setDirty();
}

void toggleGlobalSort()
{
	RaggedLFB* rlfb = dynamic_cast<RaggedLFB*>(lfb);
	if (rlfb)
	{
		printf("Global Sort: %i\n", useGlobalSort.b);
		rlfb->useGlobalSort(useGlobalSort.b);
	}
	else
	{
		printf("Error: Global Sort only applied to the Linearized LFB\n");
		useGlobalSort.b = false;
		useGlobalSort.setDirty();
	}
}

void update(float dt)
{
	if (rotateCamera)
	{
		fly.camera.rotate(vec2f(0.0f, dt));
		fly.camera.regenCamera();
	}

	static float printFragsTimer = 0.0f;
	printFragsTimer -= dt;
	if (printFragsTimer < 0.0)
	{
		printFragsTimer += 3.0;
		printf("Total Fragments: %i\n", numFrags);
	}

	//this timer waits a little after resizing before drawing using the LFB
	//a resize soon after is common and annoying if the program freezes a little immediately after each adjustment
	if (resizeTimer > 0.0)
		resizeTimer -= dt;

	if (modelReady)
	{
		modelName.hide();
		
		modelReady = false;
		model = nextModel;
		nextModel = VBOMesh();
		printf("Next Model Ready. Buffering...\n");
		model.upload();
		printf("Done\n");
		gui.ignoreNextTime(); //.update() causes a spike
	}
	
	//resize event
	if (jeltz.resized())
	{
		resizeTimer = 0.3f;

		#ifdef RENDERSCALE
		CHECKERROR;
		if (rtt.resize(jeltz.winSize()*RENDERSCALE))
			rtt.attach();
		CHECKERROR;
		//printf("Res: %ix%i\n", rtt.size.x, rtt.size.y);
		#else
		//printf("Res: %ix%i\n", x, y);
		#endif
		
		gui.ignoreNextTime(); //the lfb will also resize this frame, causing new allocation and a spike
	}
	
	//update time and memory info
	if (gui.fps.gotNewSample) //don't update every frame - .textf() is expensive
	{
		profileInfo.textf("%s", profiler.toString().c_str());
		if (lfb)
		{
			int total, available, used, evicted;
			CHECKERROR;
			glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total);
			glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &available);
			glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &evicted);
			if (CHECKERROR_SILENT)
			{
				//probably an ATI card
				memoryInfo.textf("%sFragments: %.2fM", lfb->getMemoryInfo().c_str(), numFrags/(1000000.0f));
			}
			else
			{
				used = total - available;
				memoryInfo.textf("%sFragments: %.2fM\nGPU-M Usage: %.0fMB\nGPU-M Evicted: %.0fMB", lfb->getMemoryInfo().c_str(), numFrags/(1000000.0f), used/1000.0f, evicted/1000.0f);
			}
		}
	}
	
	//key events (ugly polling...)
	if (jeltz.buttonDown("`"))
	{
		gui.visible = !gui.visible;
		gui.fps.print = !gui.visible;
	}
	if (jeltz.buttonDown("m"))
	{
		loadNextModel();
	}
	if (jeltz.buttonDown("r"))
	{
		rotateCamera = !rotateCamera;
	}
	
	//method selection
	if (jeltz.buttonDown("0")) setMethod(0);
	if (jeltz.buttonDown("1")) setMethod(1);
	if (jeltz.buttonDown("2")) setMethod(2);
	if (jeltz.buttonDown("3")) setMethod(3);
	if (jeltz.buttonDown("4")) setMethod(4);
	
	if (jeltz.buttonDown("f"))
		wireframe = !wireframe;
	
	if (jeltz.buttonDown("F4"))
	{
		static bool fullscreen = false;
		fullscreen = !fullscreen;
		jeltz.fullScreen(fullscreen);
	}
	
	if (jeltz.buttonDown("c"))
	{
		vec3f size = model.boundsMax - model.boundsMin;
		float ygrad = sin(0.5f*70.0f*(pi/180.0f));
		float xgrad = ygrad * camera.getAspectRatio();
		float ydist = (size.y / 2.0f) / ygrad;
		float xdist = (size.x / 2.0f) / xgrad;
		camera.zoomAt(vec3f(0, 0, size.z * 0.5f + max(xdist, ydist)), vec3f(0));
		camera.regen();
	}
	if (jeltz.buttonDown("p"))
	{
		#ifdef RENDERSCALE
		QI::ImagePNG screenie;
		screenie.readTexture(*rtt.colour[0]);
		screenie.saveImage("screenie.png");
		#else
		printf("Error: please recompile with RENDERSCALE defined for screenshots.\n");
		#endif
	}
	if (jeltz.buttonDown("l"))
	{
		ofstream ofile("profile.txt");
		ofile << profiler.toString() << endl;
		ofile.close();
	}
	if (jeltz.buttonDown("b"))
	{
		toggleBlending();
	}
}

#define PRINTGLINFO(s) _printGLInfo(s, #s)
void _printGLInfo(GLenum e, const char* name)
{
	GLint i[32];
	const GLubyte* s;
	CHECKERROR;
	glGetIntegerv(e, i);
	if (glGetError() != GL_INVALID_ENUM)
		{printf("%s %i\n", name, i[0]); return;}
	s = glGetString(e);
	if (glGetError() != GL_INVALID_ENUM)
		{printf("%s %s\n", name, s); return;}
	CHECKERROR;
}

void printInfo()
{	
	PRINTGLINFO(GL_VENDOR);
	PRINTGLINFO(GL_RENDERER);
	PRINTGLINFO(GL_VERSION);
	
	PRINTGLINFO(GL_MAX_IMAGE_UNITS);
	PRINTGLINFO(GL_MAX_FRAGMENT_ATOMIC_COUNTERS);
	PRINTGLINFO(GL_MAX_TEXTURE_BUFFER_SIZE);
}

void printControls()
{
	printf("Controls:\n\tEsc     Exit\n\tSpace:  Toggle orbit\n" \
		"\tC:      Center view\n" \
		"\tR:      Rotating camera\n" \
		"\tWASD:   Free fly\n\t`:      Toggle GUI rendering\n\tM:      Cycle model\n" \
		"\tB:      Toggle counting with blending\n" \
		"\tP:      Screenshot -> \"screenie.png\"\n");
}

int main(int argc, char** argv)
{
	printf("Copyright 2011 Pyarelal Knowles\n");

	jeltz.setUpdate(update);
	jeltz.setDisplay(display);
	jeltz.add(&gui);
	jeltz.add(&fly);
	jeltz.init();
	
	CHECKERROR;
	loadGlewExtensions();
	CHECKERROR;

	if (!phong.reload())
	{
		printf("Warning: could not load default phong shader.\n" \
			"\tIf you're using vs2008, make sure to set the correct working directory.\n" \
			"\tPress enter to continue anyway\n");
		while (getchar() != '\n')
			; //NOP
	}
	
	printInfo();
	printControls();
	
	//register the ctm model loader
	VBOMeshCTM::smoothGeneratedNormals = true;
	VBOMeshCTM::registerLoader();
	VBOMesh3DS::registerLoader();
	VBOMeshOBJ::registerLoader();
	
	//the backup model
	model = VBOMesh::grid(vec2i(16, 8), VBOMesh::paramSphere);
	model.transform(mat44::scale(vec3f(0.05f)));
	model.upload();
	
	//begin loading the first model (starts a thread)
	loadNextModel();
	
	#ifdef RENDERSCALE
	rtt.colour[0] = new Texture2D(vec2i(0), GL_RGBA);
	rtt.depth = new RenderBuffer(vec2i(0), GL_DEPTH24_STENCIL8);
	rtt.stencil = rtt.depth;
	#endif
	
	//default gui values
	alpha.f = 0.8;
	useBlending.b = lfbRaggedBlendingToggle;
	
	//gui heirarchy and geometry
	gui.body.add(infoGroup);
	gui.controls.add(modelName);
	gui.controls.add(methodGroup);
	infoGroup.add(profileInfo);
	infoGroup.add(memoryInfo);
	methodGroup.add(alpha);
	methodGroup.add(methodNone);
	methodGroup.add(methodBasic);
	methodGroup.add(methodLinked);
	methodGroup.add(methodPages);
	methodGroup.add(methodLinearized);
	methodGroup.add(useBlending);
	methodGroup.add(useGlobalSort);
	methodNone.capture(QG::CLICK, clickMethod);
	methodBasic.capture(QG::CLICK, clickMethod);
	methodLinked.capture(QG::CLICK, clickMethod);
	methodPages.capture(QG::CLICK, clickMethod);
	methodLinearized.capture(QG::CLICK, clickMethod);
	useBlending.capture(QG::CLICK, toggleBlending);
	useGlobalSort.capture(QG::CLICK, toggleGlobalSort);

	useGlobalSort.hide(); //experimental. useless
	
	profileInfo.border = 2;
	memoryInfo.border = 2;
	
	modelName.hide();
	modelName.fill = QG::NONE;
	modelName.anchor = QG::CENTER;
	
	infoGroup.expand = QG::BOTH;
	infoGroup.fill = QG::BOTH;
	profileInfo.anchor = QG::TOP_RIGHT;
	memoryInfo.anchor = QG::TOP_RIGHT;
	profileInfo.fill = QG::NONE;
	memoryInfo.fill = QG::NONE;
	
	methodGroup.margin = 0;
	methodGroup.padding = 0;
	methodGroup.border = 0;
	methodGroup.fill = QG::NONE;
	
	if (argc > 1)
	{
		int m = stringToInt(argv[1]);
		setMethod(m);
	}
	else
		setMethod(0);
	
	//main loop
	jeltz.run();
	return 0;
}
