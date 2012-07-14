#ifndef GLF_UTILS_HPP
#define GLF_UTILS_HPP

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
// OpenGL
#ifdef WIN32
#	include <windows.h>
#	include <GL/glew.h>
#	include <GL/wglew.h>
//#	include <GL/glext.h>
//#	define glfGetProcAddress wglGetProcAddress
//#	define GLEW_EXT_direct_state_access_memory 0
#elif defined(linux) || defined(__linux)
#	include <GL/glew.h>
#	define GL_GLEXT_PROTOTYPES 1
#	include <GL/gl.h>
#	include <GL/glext.h>
#else
#	error "Unsupported platform"
#endif
//------------------------------------------------------------------------------
#include <glm/glm.hpp>
#include <string>
#include <vector>

#ifdef WIN32
//------------------------------------------------------------------------------
// Helpers function for WIN32
//------------------------------------------------------------------------------
inline double log2( double n )  
{  
	// log(n)/log(2) is log2.  
	return log( n ) / log( 2.0 );  
}
//------------------------------------------------------------------------------
inline bool isnan(float x) 
{
    // when x == NAN, then x == x is false and x != x is always true
    return x != x;
}
//------------------------------------------------------------------------------
inline bool isinf(float x)
{
    // x == INF if x == +infini || x == -infini, in fact if |x| == +infini (HUGE_VAL)
    return std::fabs(x) == HUGE_VAL;
}
//------------------------------------------------------------------------------
inline std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}
//------------------------------------------------------------------------------
inline std::wstring c2ws(char*& s)
{
    int len;
    int slength = (int)strlen(s) + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s, slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s, slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}
//------------------------------------------------------------------------------
#define M_PI 3.141592653589793238462643f
//------------------------------------------------------------------------------
#endif

//------------------------------------------------------------------------------
// Constants & Macro
//------------------------------------------------------------------------------
#define GLF_BUFFER_OFFSET(i) 	((char *)NULL + (i))
#define BUFFER_LOG_SIZE 		 20000 
#define INVALID_ID				-1

namespace glf
{
	//-------------------------------------------------------------------------
	namespace directory
	{
		extern std::string ShaderDirectory;
		extern std::string TextureDirectory;
		extern std::string SceneDirectory;
		extern std::string ModelDirectory;
		extern std::string ConfigDirectory;
	}
	//-------------------------------------------------------------------------
	std::string ToString(					const glm::mat4& _mat);
	std::string ToString(					const glm::vec2& _vec);
	std::string ToString(					const glm::vec3& _vec);
	std::string ToString(					const glm::vec4& _vec);
	//-------------------------------------------------------------------------
	//glm::mat4	ScreenQuadTransform(		);
	bool		IsPower2( 					int _n );
	int			MipmapLevels( 				int _n );
	int			NearestSuperiorPowerOf2( 	int _n );
	int 		NextMipmapDimension(		int _dim, int _level);
	//-------------------------------------------------------------------------
	void 		Info(						const char* _format, ...);
	void 		Warning(					const char* _format, ...);
	void 		Error(						const char* _format, ...);
	//-------------------------------------------------------------------------
	void 		Info(						const std::string& _m);
	void 		Warning(					const std::string& _m);
	void 		Error(						const std::string& _m);
	//--------------------------------------------------------------------------
	int 		Version(					int Major, 
											int Minor);
	std::string LoadFile(					std::string const & Filename);
	bool 		ValidateProgram(			GLuint ProgramName);
	bool 		CheckGLVersion(				GLint MajorVersionRequire, 
											GLint MinorVersionRequire);
	bool 		CheckExtension(				char const * String);
	bool 		CheckError(					const char* Title);
	bool 		CheckFramebuffer(			GLuint FramebufferName);
	bool 		CheckProgram(				GLuint ProgramName);
	bool 		CheckShader(				GLuint ShaderName, 
											const char* 		_source);
	GLuint 		CreateShader(				GLenum 				_type,
											std::string const & _source);
	GLuint 		CreateProgram(				std::string const & _name,
											GLenum 				_type,
											std::string const & _source,
											bool 				_separable=false);
	GLuint 		CreateProgram(				std::string const & _name,
											std::string const & _vs,
											std::string const & _cs,
											std::string const & _es,
											std::string const & _gs,
											std::string const & _fs,
											bool 				_separable=false);
	//--------------------------------------------------------------------------
	void 		Split(						const std::string & _in, 
											char delim, 
											std::vector<std::string>& _out);
	std::size_t	GetFileSize(				std::ifstream& _file);
	bool		GetExtension(				const std::string& _filename, 
											std::string& _extension);
}

#endif
