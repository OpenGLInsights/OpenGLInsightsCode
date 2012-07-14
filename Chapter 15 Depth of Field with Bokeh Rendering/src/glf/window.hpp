#ifndef GLF_WINDOW_HPP
#define GLF_WINDOW_HPP

//------------------------------------------------------------------------------
// Original version from Christophe Riccio : g-truc.net
// Modified by Charles de Rousiers : derousiers.net
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/utils.hpp>
#include <glf/camera.hpp>
#include <glui/context.hpp>
//------------------------------------------------------------------------------
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/half_float.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/bit.hpp>
//------------------------------------------------------------------------------
// STL
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdarg>

namespace glf
{
	//--------------------------------------------------------------------------
	enum mouse_button
	{
		MOUSE_BUTTON_NONE 	= 0,
		MOUSE_BUTTON_LEFT 	= (1 << 0),
		MOUSE_BUTTON_RIGHT	= (1 << 1),
		MOUSE_BUTTON_MIDDLE	= (1 << 2)
	};
	//--------------------------------------------------------------------------
	struct Window
	{
		Window(glm::ivec2 const & Size) :
			Size(Size),
			MouseOrigin(Size >> 1),
			MouseCurrent(Size >> 1),
			TranlationOrigin(0, 4),
			TranlationCurrent(0, 4),
			RotationOrigin(0), 
			RotationCurrent(0),
			MouseButtonFlags(0)
		{}

		glm::ivec2 Size;
		glm::vec2  MouseOrigin;
		glm::vec2  MouseCurrent;
		glm::vec2  TranlationOrigin;
		glm::vec2  TranlationCurrent;
		glm::vec2  RotationOrigin;
		glm::vec2  RotationCurrent;
		int 	   MouseButtonFlags;
	};
	//--------------------------------------------------------------------------
	bool 		Run(			int argc, 
								char* argv[], 
								glm::ivec2 const & Size, 
								int Major, 
								int Minor);
	inline void	SwapBuffers();
}//namespace glf


// Context
namespace ctx
{
	extern glf::Window 			window;
	extern glf::Camera::Ptr		camera;
	extern glui::GlutContext*	ui;
	extern bool 				drawUI;
	extern bool 				drawTimings;
	extern bool 				drawHelpers;
	extern bool 				drawWire;
}


//-----------------------------------------------------------------------------
// Includes inlines
//-----------------------------------------------------------------------------
#include <glf/window.inl>

#endif
