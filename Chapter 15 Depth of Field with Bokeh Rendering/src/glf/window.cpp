//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/window.hpp>
#include <glf/utils.hpp>
#include <GL/freeglut.h>

//------------------------------------------------------------------------------
// Declarations
//------------------------------------------------------------------------------
bool check();
bool begin();
bool end();
void display();
bool resize(int _w, int _h);


namespace glf
{
	//--------------------------------------------------------------------------
	void init()
	{
		#ifdef WIN32
		glewInit();
		glGetError();
		#endif

		// Retrieve informations about uniform and atomic counter
		GLint value;
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS,&value);					Info("GL_MAX_VERTEX_UNIFORM_BLOCKS                  : %d",value);
		glGetIntegerv(GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS,&value);			Info("GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS            : %d",value);
		glGetIntegerv(GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS,&value);		Info("GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS         : %d",value);
		glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS,&value);				Info("GL_MAX_GEOMETRY_UNIFORM_BLOCKS                : %d",value);
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS,&value);				Info("GL_MAX_FRAGMENT_UNIFORM_BLOCKS                : %d",value);
		glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS,&value);				Info("GL_MAX_COMBINED_UNIFORM_BLOCKS                : %d",value);
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS,&value);				Info("GL_MAX_UNIFORM_BUFFER_BINDINGS                : %d",value);

		glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS,&value);			Info("GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS          : %d",value); 
		glGetIntegerv(GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS,&value);	Info("GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS    : %d",value); 
		glGetIntegerv(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS,&value);Info("GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS : %d",value); 
		glGetIntegerv(GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS,&value);		Info("GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS        : %d",value); 
		glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS,&value);		Info("GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS        : %d",value);
		glGetIntegerv(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS,&value);		Info("GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS         : %d",value);
	}
	//--------------------------------------------------------------------------
	static void keyboard(unsigned char key, int x, int y)
	{
		ctx::ui->Keyboard(key);

		switch(key) 
		{
		case 'u':
			ctx::drawUI = !ctx::drawUI;
			break;
		case 'h':
			ctx::drawHelpers = !ctx::drawHelpers;
			break;
		case 't':
			ctx::drawTimings = !ctx::drawTimings;
			break;
		case 'w':
			ctx::drawWire = !ctx::drawWire;
			break;
		case 27:
			end();
			exit(0);
			break;
		}
	}
	//--------------------------------------------------------------------------
	static void mouse(int Button, int State, int x, int y)
	{
		switch(State)
		{
			case GLUT_DOWN:
			{
				ctx::window.MouseOrigin = ctx::window.MouseCurrent = glm::ivec2(x, y);
				if(!ctx::ui->IsOnFocus())
				{
				switch(Button)
				{
					case GLUT_LEFT_BUTTON:
					{
						ctx::ui->Mouse(x,y,glui::Mouse::LEFT,glui::Mouse::PRESS);
						ctx::window.MouseButtonFlags |= glf::MOUSE_BUTTON_LEFT;
						ctx::window.TranlationOrigin = ctx::window.TranlationCurrent;
						ctx::camera->MouseEvent(x,y, Mouse::LEFT, Mouse::PRESS);
					}
					break;
					case GLUT_MIDDLE_BUTTON:
					{
						ctx::window.MouseButtonFlags |= glf::MOUSE_BUTTON_MIDDLE;
						ctx::camera->MouseEvent(x,y, Mouse::MIDDLE, Mouse::PRESS);
					}
					break;
					case GLUT_RIGHT_BUTTON:
					{
						ctx::window.MouseButtonFlags |= glf::MOUSE_BUTTON_RIGHT;
						ctx::window.RotationOrigin = ctx::window.RotationCurrent;
						ctx::camera->MouseEvent(x,y, Mouse::RIGHT, Mouse::PRESS);
					}
					break;
					// Special case for handling Linux wheel implementation of FreeGLUT
					case 3:
					{
						ctx::camera->MouseEvent(x,y, Mouse::SCROLL_UP, Mouse::PRESS);
					}
					break;
					case 4:
					{
						ctx::camera->MouseEvent(x,y, Mouse::SCROLL_DOWN, Mouse::PRESS);
					}
					break;
				}
				}
			}
			break;
			case GLUT_UP:
			{
				switch(Button)
				{
					case GLUT_LEFT_BUTTON:
					{
						ctx::ui->Mouse(x,y,glui::Mouse::LEFT,glui::Mouse::RELEASE);
						ctx::window.TranlationOrigin += (ctx::window.MouseCurrent - ctx::window.MouseOrigin) / 10.f;
						ctx::window.MouseButtonFlags &= ~glf::MOUSE_BUTTON_LEFT;
						ctx::camera->MouseEvent(x,y, Mouse::LEFT, Mouse::RELEASE);
					}
					break;
					case GLUT_MIDDLE_BUTTON:
					{
						ctx::window.MouseButtonFlags &= ~glf::MOUSE_BUTTON_MIDDLE;
						ctx::camera->MouseEvent(x,y, Mouse::MIDDLE, Mouse::RELEASE);
					}
					break;
					case GLUT_RIGHT_BUTTON:
					{
						ctx::window.RotationOrigin += ctx::window.MouseCurrent - ctx::window.MouseOrigin;
						ctx::window.MouseButtonFlags &= ~glf::MOUSE_BUTTON_RIGHT;
						ctx::camera->MouseEvent(x,y, Mouse::RIGHT, Mouse::RELEASE);
					}
					break;
				}
			}
			break;
		}
	}
	//--------------------------------------------------------------------------
	static void reshape(int w, int h)
	{
		ctx::ui->Reshape(w,h);

		ctx::window.Size = glm::ivec2(w, h);
		glViewport(0,0,w,h);

		resize(w, h);
	}
	//--------------------------------------------------------------------------
	static void idle()
	{
		glutPostRedisplay();
	}
	//--------------------------------------------------------------------------
	static void close()
	{
		end();
	}
	//--------------------------------------------------------------------------
	static void passivemotion(int x, int y)
	{
		ctx::ui->Move(x,y);
		if(!ctx::ui->IsOnFocus())
			ctx::camera->MoveEvent(x,y);
	}
	//--------------------------------------------------------------------------
	static void motion(int x, int y)
	{
		ctx::ui->Move(x,y);

		ctx::window.MouseCurrent = glm::ivec2(x, y);
		ctx::window.TranlationCurrent = ctx::window.MouseButtonFlags & glf::MOUSE_BUTTON_LEFT ? ctx::window.TranlationOrigin + (ctx::window.MouseCurrent - ctx::window.MouseOrigin) / 10.f : ctx::window.TranlationOrigin;
		ctx::window.RotationCurrent = ctx::window.MouseButtonFlags & glf::MOUSE_BUTTON_RIGHT ? ctx::window.RotationOrigin + (ctx::window.MouseCurrent - ctx::window.MouseOrigin) : ctx::window.RotationOrigin;

		if(!ctx::ui->IsOnFocus())
			ctx::camera->MoveEvent(x,y);
	}
	//--------------------------------------------------------------------------
	static void wheel( int b, int dir, int x, int y)
	{
		if (dir > 0)
			ctx::camera->MouseEvent(x,y, Mouse::SCROLL_UP, Mouse::PRESS);
		else
			ctx::camera->MouseEvent(x,y, Mouse::SCROLL_DOWN, Mouse::PRESS);
	}
	//--------------------------------------------------------------------------
	bool Run(	int argc, 
				char* argv[], 
				glm::ivec2 
				const & Size, 
				int Major, 
				int Minor)
	{
		glutInitWindowSize(Size.x, Size.y);
		glutInitWindowPosition(64, 64);
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);// | GLUT_MULTISAMPLE);

		int WindowHandle = glutCreateWindow(argv[0]);
		glewExperimental = GL_TRUE; 
		glewInit();
		glutDestroyWindow(WindowHandle);

		glutInitContextVersion(Major, Minor);
		if(glf::Version(Major, Minor) >= 410)
		{
			glutInitContextProfile(GLUT_CORE_PROFILE);
			//glutInitContextProfile(GLUT_CORE_PROFILE | GLUT_DEBUG);
			//glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE | GLUT_DEBUG);
			//glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
		}

		glutCreateWindow(argv[0]);
		glGetError();
		glf::init();
		ctx::ui = new glui::GlutContext();
		ctx::ui->Initialize(Size.x, Size.y);

		// 
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		if(begin())
		{
			glutDisplayFunc(	display); 
			glutReshapeFunc(	glf::reshape);
			glutMouseFunc(		glf::mouse);
			glutMotionFunc(		glf::motion);
			glutPassiveMotionFunc( glf::passivemotion);
			glutKeyboardFunc(	glf::keyboard);
			glutIdleFunc(		glf::idle);
			glutCloseFunc(		glf::close);
			glutMouseWheelFunc( glf::wheel );
			glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
			glutMainLoop();

			return true;
		}

		delete ctx::ui;
		return false;
	}

}//namespace glf
