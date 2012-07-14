//
// nvGlutWidgets
//
//  Adaptor classes to integrate the nvWidgets UI library with the GLUT windowing
// toolkit. The adaptors convert native GLUT UI data to native nvWidgets data. All
// adaptor classes are implemented as in-line code in this header. The adaptor
// defaults to using the standard OpenGL painter implementation.
//
// Author: Ignacio Castano, Samuel Gateau, Evan Hart
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <glui/context.hpp>
#include <glui/painter.hpp>
#include <GL/freeglut.h>
#include <cstdlib>
#include <glf/utils.hpp>
namespace glui
{
	//-------------------------------------------------------------------------
	GlutContext::GlutContext() :
	UIContext(*(new GLPainter())),
	m_ownPainter(true),
	m_modifiers(0)
	{

	}
	//-------------------------------------------------------------------------
	GlutContext::~GlutContext()
	{
		if (m_ownPainter) delete getPainter();
	}
	//-------------------------------------------------------------------------
	void GlutContext::Initialize(int _w, int _h)
	{
		getPainter()->Initialize();
		Reshape(_w, _h);
	}
	//-------------------------------------------------------------------------
	void GlutContext::Mouse(int _x, int _y, int _button, int _state)
	{
		glui::Mouse::Button b;
		switch(_button)
		{
			case GLUT_LEFT_BUTTON	: b = glui::Mouse::LEFT; break;
			case GLUT_MIDDLE_BUTTON	: b = glui::Mouse::MIDDLE; break;
			case GLUT_RIGHT_BUTTON	: b = glui::Mouse::RIGHT;	break;
			default 				: return;
		}

		glui::Mouse::State s = _state==GLUT_DOWN?glui::Mouse::PRESS:glui::Mouse::RELEASE;
		UIContext::Mouse(b,s,m_modifiers,_x,_y);
	}
}
