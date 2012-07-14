//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/utils.hpp>
#include <glf/debug.hpp>
#include <glm/gtx/transform.hpp>
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
#define VERBOSE_COMPILATION 1
#define NOMINMAX
#define ENABLE_MSVC_CONSOLE 1

//------------------------------------------------------------------------------
// Helpers function for WIN32
//------------------------------------------------------------------------------
#ifdef WIN32
#pragma warning( disable : 4996 )
#endif


namespace glf
{
	//-------------------------------------------------------------------------
	namespace directory
	{
		std::string ShaderDirectory	 = "../resources/shaders/";
		std::string TextureDirectory = "../resources/textures/";
		std::string SceneDirectory	 = "../resources/scenes/";
		std::string ModelDirectory	 = "../resources/models/";
		std::string ConfigDirectory	 = "../resources/configs/";
	}
	//-------------------------------------------------------------------------
	glm::mat4	ScreenQuadTransform()
	{
		glm::mat4 proj = glm::ortho(-0.5f,0.5f,-0.5f,0.5f,0.1f,100.f);
		glm::mat4 view = glm::lookAt(	glm::vec3(0.5f,0.5f,5.0f),
										glm::vec3(0.5f,0.5f,-1.0f),
										glm::vec3(0.0f,1.0f,0.0f));
		return proj * view;
	}
	//-------------------------------------------------------------------------
	bool IsPower2( int x )
	{
		// http://bytes.com/topic/c/answers/213306-test-whether-number-power-2-a
		return x && !( (x-1) & x );

		// http://www.gamedev.net/topic/406313-c-fast-power-of-2-test/
		//return ( (x > 0) && ((x & (x - 1)) == 0) );
	}
	//-------------------------------------------------------------------------
	int	MipmapLevels( int _n )
	{	
		//return int(ceil( log( float(_n) ) / log( 2.f ) )) + 1;
		return int(1.0 + floor(log2(_n)));
	}
	//-------------------------------------------------------------------------
	int	NearestSuperiorPowerOf2( int _n )
	{	
		return int(pow( 2, ceil( log( float(_n) ) / log( 2.f ) ) ));
	}
	//-------------------------------------------------------------------------
	int NextMipmapDimension(int _dim, int _level)
	{
		return int(std::max(1.f, float(floor( float(_dim / (1<<_level)) )) ));
	}
	//-------------------------------------------------------------------------
	std::string ToString(const glm::mat4& _mat)
	{
		std::stringstream out;
		out << _mat[0].x << " " << _mat[1].x << " " << _mat[2].x << " " << _mat[3].x << std::endl; 
		out << _mat[0].y << " " << _mat[1].y << " " << _mat[2].y << " " << _mat[3].y << std::endl; 
		out << _mat[0].z << " " << _mat[1].z << " " << _mat[2].z << " " << _mat[3].z << std::endl; 
		out << _mat[0].w << " " << _mat[1].w << " " << _mat[2].w << " " << _mat[3].w << std::endl; 

		return out.str();
	}
	//-------------------------------------------------------------------------
	std::string ToString(const glm::vec2& _vec)
	{
		std::stringstream out;
		out << _vec.x << " " << _vec.y << std::endl; 
		return out.str();
	}
	//-------------------------------------------------------------------------
	std::string ToString(const glm::vec3& _vec)
	{
		std::stringstream out;
		out << _vec.x << " " << _vec.y << " " << _vec.z << std::endl; 
		return out.str();
	}
	//-------------------------------------------------------------------------
	std::string ToString(const glm::vec4& _vec)
	{
		std::stringstream out;
		out << _vec.x << " " << _vec.y << " " << _vec.z << " " << _vec.w << std::endl; 
		return out.str();
	}
	//-------------------------------------------------------------------------
	void Info(const std::string& _m) 
	{
		Info("%s",_m.c_str());
	}
	//-------------------------------------------------------------------------
	void Warning(const std::string& _m) 
	{
		Warning("%s",_m.c_str());
	}
	//-------------------------------------------------------------------------
	void Error(const std::string& _m) 
	{
		Error("%s",_m.c_str());
	}
	//-------------------------------------------------------------------------
	void Info(const char* _format, ...) 
	{
		static char sBuffer[BUFFER_LOG_SIZE];
		va_list Params;
		va_start(Params, _format);
		vsprintf(sBuffer, _format, Params);
		va_end(Params);
		
		#if (defined WIN32) && ENABLE_MSVC_CONSOLE
		char ssBuffer[BUFFER_LOG_SIZE];
		sprintf(ssBuffer,"[Info] %s\n",sBuffer);
		OutputDebugString(s2ws(std::string(ssBuffer)).c_str());
		#else
		std::cout << "[Info] " << sBuffer << std::endl << std::flush;
		#endif
	}
	//-------------------------------------------------------------------------
	void Warning(const char* _format, ...) 
	{
		static char sBuffer[BUFFER_LOG_SIZE];
		va_list Params;
		va_start(Params, _format);
		vsprintf(sBuffer, _format, Params);
		va_end(Params);
		
		#if (defined WIN32) && ENABLE_MSVC_CONSOLE
		char ssBuffer[BUFFER_LOG_SIZE];
		sprintf(ssBuffer,"[Warning] %s\n",sBuffer);
		OutputDebugString(s2ws(std::string(ssBuffer)).c_str());
		#else
		std::cout << "[Warning] " << sBuffer << std::endl << std::flush;
		#endif
	}
	//-------------------------------------------------------------------------
	void Error(const char* _format, ...) 
	{
		static char sBuffer[BUFFER_LOG_SIZE];
		va_list Params;
		va_start(Params, _format);
		vsprintf(sBuffer, _format, Params);
		va_end(Params);
		
		#if (defined WIN32) && ENABLE_MSVC_CONSOLE
		char ssBuffer[BUFFER_LOG_SIZE];
		sprintf(ssBuffer,"[Error] %s\n",sBuffer);
		OutputDebugString(s2ws(std::string(ssBuffer)).c_str());
		#else
		std::cout << "[Error] " << sBuffer << std::endl << std::flush;
		#endif

		#if ENABLE_ASSERT_ON_ERROR
		assert(false);
		#endif

		#if ENABLE_EXIT_ON_ERROR
		exit(-1);
		#endif
	}
	//--------------------------------------------------------------------------
	int Version(int Major, int Minor)
	{
		return Major * 100 + Minor * 10;
	}
	//--------------------------------------------------------------------------
	bool CheckGLVersion(GLint MajorVersionRequire, GLint MinorVersionRequire)
	{
		GLint MajorVersionContext = 0;
		GLint MinorVersionContext = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &MajorVersionContext);
		glGetIntegerv(GL_MINOR_VERSION, &MinorVersionContext);
		return glf::Version(MajorVersionContext, MinorVersionContext) 
			>= glf::Version(MajorVersionRequire, MinorVersionRequire);
	}
	//--------------------------------------------------------------------------
	bool CheckExtension(char const * String)
	{
		GLint ExtensionCount = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &ExtensionCount);
		for(GLint i = 0; i < ExtensionCount; ++i)
			if(std::string((char const*)glGetStringi(GL_EXTENSIONS, i)) == std::string(String))
				return true;
		return false;
	}
	//--------------------------------------------------------------------------
	std::string LoadFile(std::string const & Filename)
	{
		std::ifstream stream(Filename.c_str(), std::ios::in);

		if(!stream.is_open())
		{
			glf::Error("Cannot open : %s",Filename.c_str());
			assert(false);
			return "";
		}

		std::string Line = "";
		std::string Text = "";

		while(getline(stream, Line))
			Text += "\n" + Line;

		stream.close();

		return Text;
	}
	//--------------------------------------------------------------------------
	bool CheckError(const char* Title)
	{
		#if ENABLE_CHECK_ERROR
		int Error;
		if((Error = glGetError()) != GL_NO_ERROR)
		{
			std::string ErrorString;
			switch(Error)
			{
			case GL_INVALID_ENUM:
				ErrorString = "GL_INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				ErrorString = "GL_INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				ErrorString = "GL_INVALID_OPERATION";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				ErrorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
				break;
			case GL_OUT_OF_MEMORY:
				ErrorString = "GL_OUT_OF_MEMORY";
				break;
			default:
				ErrorString = "UNKNOWN";
				break;
			}
			::glf::Error("OpenGL Error(%s): %s\n", ErrorString.c_str(), Title);
		}
		assert(Error == GL_NO_ERROR);
		return Error == GL_NO_ERROR;
		#else
		return true;
		#endif
	}
	//--------------------------------------------------------------------------
	bool CheckFramebuffer(GLuint FramebufferName)
	{
		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		switch(Status)
		{
		case GL_FRAMEBUFFER_UNDEFINED:
			glf::Error("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_UNDEFINED");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			glf::Error("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			glf::Error("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			glf::Error("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			glf::Error("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			glf::Error("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_UNSUPPORTED");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			glf::Error("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			glf::Error("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
			break;
		case GL_FRAMEBUFFER_COMPLETE:
			//glf::Info("OpenGL OK(%s)\n", "GL_FRAMEBUFFER_COMPLETE");
			break;
		default:
			glf::Error("OpenGL Error(%d !=GL_FRAMEBUFFER_COMPLETE(%d))\n", Status,GL_FRAMEBUFFER_COMPLETE);
		}
		return Status == GL_FRAMEBUFFER_COMPLETE;
	}
	//--------------------------------------------------------------------------
	bool ValidateProgram(GLuint ProgramName)
	{
		if(!ProgramName)
			return false;

		glValidateProgram(ProgramName);
		GLint Result = GL_FALSE;
		glGetProgramiv(ProgramName, GL_VALIDATE_STATUS, &Result);

		if(Result == GL_FALSE)
		{
			int InfoLogLength;
			glGetProgramiv(ProgramName, GL_INFO_LOG_LENGTH, &InfoLogLength);
			std::vector<char> Buffer(InfoLogLength);
			glGetProgramInfoLog(ProgramName, InfoLogLength, NULL, &Buffer[0]);
			Error("Program validation failed : \n\n%s\n",&Buffer[0]);
		}

		return Result == GL_TRUE;
	}
	//--------------------------------------------------------------------------
	bool CheckProgram(GLuint ProgramName)
	{
		if(!ProgramName)
			return false;

		GLint Result = GL_FALSE;
		glGetProgramiv(ProgramName, GL_LINK_STATUS, &Result);

		int InfoLogLength;
		glGetProgramiv(ProgramName, GL_INFO_LOG_LENGTH, &InfoLogLength);
		std::vector<char> Buffer(std::max(InfoLogLength, int(1)));
		glGetProgramInfoLog(ProgramName, InfoLogLength, NULL, &Buffer[0]);

		if(Result != GL_TRUE)
			Error("Program linking failed : \n\n%s\n",&Buffer[0]);

		return Result == GL_TRUE;
	}
	//--------------------------------------------------------------------------
	bool CheckShader(GLuint ShaderName, const char* Source)
	{
		if(!ShaderName)
			return false;

		GLint Result = GL_FALSE;
		glGetShaderiv(ShaderName, GL_COMPILE_STATUS, &Result);

		int InfoLogLength;
		glGetShaderiv(ShaderName, GL_INFO_LOG_LENGTH, &InfoLogLength);
		std::vector<char> Buffer(InfoLogLength);
		glGetShaderInfoLog(ShaderName, InfoLogLength, NULL, &Buffer[0]);

		if(Result != GL_TRUE)
		{
			// Append line number in front of sources
			std::stringstream sourceWithLineNumbers;
			std::vector<std::string> lines;
			Split(Source,'\n',lines);
			for(unsigned int i=0;i<lines.size();++i)
				sourceWithLineNumbers << i+1 << " : " << lines[i] << std::endl;
			Error("Shader compilation failded\n\nSource : \n%s\n\nError : \n%s\n", sourceWithLineNumbers.str().c_str(), &Buffer[0]);	
		}

		return Result == GL_TRUE;
	}
	//--------------------------------------------------------------------------
	GLuint CreateShader(	GLenum Type,
							std::string const & Source)
	{
		bool validated = true;
		GLuint Name = 0;

		if(!Source.empty())
		{
			char const * SourcePointer = Source.c_str();
			Name = glCreateShader(Type);
			glShaderSource(Name, 1, &SourcePointer, NULL);
			glCompileShader(Name);
			validated = glf::CheckShader(Name, SourcePointer);
			assert(validated);
		}

		return Name;
	}
	//--------------------------------------------------------------------------
	GLuint CreateProgram(	std::string const & _name,
							GLenum 				_type,
							std::string const & _source,
							bool 				_separable)
	{
		Info("Compiling : %s",_name.c_str());

		GLuint programName;
		programName = glCreateProgram();
		glProgramParameteri(programName, GL_PROGRAM_SEPARABLE, (_separable?GL_TRUE:GL_FALSE));
		GLuint shaderName = glf::CreateShader(_type,_source);
		glAttachShader(programName, shaderName);
		glDeleteShader(shaderName);

		glLinkProgram(programName);
		assert(glf::CheckProgram(programName));
		assert(glf::ValidateProgram(programName));

		return programName;
	}
	//--------------------------------------------------------------------------
	GLuint CreateProgram(	std::string const & _name,
							std::string const & _vSource,
							std::string const & _cSource,
							std::string const & _eSource,
							std::string const & _gSource,
							std::string const & _fSource,
							bool 				_separable)
	{
		Info("Compiling : %s",_name.c_str());

		const std::string stages[5] = {		"::Vertex", 
											"::Control", 
											"::Evaluation", 
											"::Geometry", 
											"::Fragment"};

		const std::string* sources[5] = {	&_vSource, 
											&_cSource,
											&_eSource,
											&_gSource,
											&_fSource};
		GLenum types[5] = { 				GL_VERTEX_SHADER,
											GL_TESS_CONTROL_SHADER,
											GL_TESS_EVALUATION_SHADER,
											GL_GEOMETRY_SHADER,
											GL_FRAGMENT_SHADER};

		GLuint programName;
		programName = glCreateProgram();
		glProgramParameteri(programName, GL_PROGRAM_SEPARABLE, (_separable?GL_TRUE:GL_FALSE));
		for(int i=0;i<5;++i)
		{
			if(sources[i]->compare("")!=0)
			{
				Info(" - SubCompiling : %s",(_name + stages[i]).c_str());
				GLuint shaderName = glf::CreateShader(types[i],*(sources[i]));
				glAttachShader(programName, shaderName);
				glDeleteShader(shaderName);
			}
		}
		glLinkProgram(programName);
		assert(glf::CheckProgram(programName));
		assert(glf::ValidateProgram(programName));

		return programName;
	}
	//--------------------------------------------------------------------------
	void Split(	const std::string & _in, 
				char delim, 
				std::vector<std::string>& _out) 
	{
		std::stringstream ss(_in);
		std::string item;
		while(std::getline(ss, item, delim)) 
			_out.push_back(item);
	}
	//--------------------------------------------------------------------------
	std::size_t	GetFileSize(std::ifstream& _file)
	{
		long pos = _file.tellg();
		_file.seekg(0, std::ios_base::end);
		std::size_t dataSize = _file.tellg();
		_file.seekg(pos, std::ios_base::beg);
		return dataSize;
	}
	//--------------------------------------------------------------------------
	bool GetExtension(const std::string& _filename, std::string& _extension)
	{
		// Extract "." position
		int position = _filename.find_last_of(".", -1);
		if(position == -1) return false;
		_extension = _filename.substr(position,  _filename.size() - ++position);
		return true;
	}
	//--------------------------------------------------------------------------
}
