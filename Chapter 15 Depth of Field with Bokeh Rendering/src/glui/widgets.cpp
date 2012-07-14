////////////////////////////////////////////////////////////////////////////////
// Based on :
//
// Author: Ignacio Castano, Samuel Gateau, Evan Hart
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
//
// Modified by : Charles de Rousiers charles.derousiers@gmail.com
//
////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#pragma warning( disable : 4996 )
#endif

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glui/widgets.hpp>
#include <ctime>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <sstream>

#define PSEUDO_RETAINED_MODE 1

#if PSEUDO_RETAINED_MODE
	#define _style
#endif

namespace glui
{
	const Rect Rect::null;

	//--------------------------------------------------------------------------
	UIContext::UIContext(UIPainter& _painter) :
	m_painter(&_painter),
	maxGroup(64),
	maxComponent(128),
	maxString(256),
	maxStringSize(100),
	maxOverdraw(8),
	m_focusCaretPos(-1),
	m_twoStepFocus(false)
	{
		m_groupIndex		= 0;
		m_componentIndex	= 0;
		m_stringIndex		= 0;
		m_overdrawIndex		= 0;

		m_window.x			= 0;
		m_window.y			= 0;
		m_window.w			= 512;
		m_window.h			= 512;
		m_currentCursor.x	= 0;
		m_currentCursor.y	= 0;
		m_nbKeys			= 0;
		m_focusCaretPos		= 0;
		m_focusPoint		= m_currentCursor;
		m_twoStepFocus		= false;
		m_uiOnFocus			= false;

		m_mouseButton[0].state 	= Mouse::RELEASE;
		m_mouseButton[0].cursor	= m_currentCursor;
		m_mouseButton[1].state 	= Mouse::RELEASE;
		m_mouseButton[1].cursor	= m_currentCursor;
		m_mouseButton[2].state 	= Mouse::RELEASE;
		m_mouseButton[2].cursor	= m_currentCursor;

		// Create group stack
		m_groupStack = new Group[maxGroup];

		// Create component stack
		m_componentStack = new Component[maxComponent];

		// Create string stack
		m_stringStack = new char*[maxString];
		for(int i=0;i<maxString;++i)
			m_stringStack[i] = new char[maxStringSize];

		// Create overdraw stack
		m_overdrawStack = new Component[maxOverdraw];
	}
	//--------------------------------------------------------------------------
	UIContext::~UIContext()
	{
		delete[] m_groupStack;
		delete[] m_componentStack;
		for(int i=0;i<maxString;++i)
			delete[] m_stringStack[i];
		delete[] m_stringStack;
		delete[] m_overdrawStack;
	}
	//--------------------------------------------------------------------------
	void UIContext::Reshape(int _w, int _h)
	{
		m_window.x = 0;
		m_window.y = 0;
		m_window.w = _w;
		m_window.h = _h;

		m_painter->reshape(_w,_h);
	}
	//--------------------------------------------------------------------------
	void UIContext::Mouse(Mouse::Button _button, Mouse::State _state, int _modifier, int _x, int _y)
	{
		setCursor(_x, _y);

		int idx = -1;
		switch(_button)
		{
			case glui::Mouse::LEFT   : idx = 0; break;
			case glui::Mouse::MIDDLE : idx = 1; break;
			case glui::Mouse::RIGHT  : idx = 2; break;
			default : break;
		}

		_modifier &= ButtonFlags::ALT|ButtonFlags::SHIFT|ButtonFlags::CTRL;

		if (idx >= 0)
		{
		    if (_state == Mouse::PRESS) 
			{
		        m_mouseButton[idx].state 	= ButtonFlags::ON|ButtonFlags::BEGIN|_modifier;
		        m_mouseButton[idx].time 	= clock();
		        m_mouseButton[idx].cursor.x = _x;
		        m_mouseButton[idx].cursor.y = m_window.h - _y;
		    }

		    if (_state == Mouse::RELEASE)
			{
		        m_mouseButton[idx].state = ButtonFlags::ON|ButtonFlags::END|_modifier;
		    }
		}
	}
	//--------------------------------------------------------------------------
    void UIContext::Mouse(Mouse::Button _button, Mouse::State _state, int _x, int _y)
	{ 
		Mouse( _button, _state, 0, _x, _y); 
	}
	//--------------------------------------------------------------------------
	void UIContext::Move(int _x, int _y)
	{
		setCursor(_x, _y);
	}
	//--------------------------------------------------------------------------
	void UIContext::Keyboard(unsigned char _k)
	{
		//setCursor(_x, _y);
		m_keyBuffer[m_nbKeys] = _k;
		m_nbKeys++;
	}
	//--------------------------------------------------------------------------
	void UIContext::Begin()
	{
		m_painter->begin(m_window);

		m_groupIndex = 0;
		m_groupStack[m_groupIndex].flags  = Flags::Layout::NONE;
		m_groupStack[m_groupIndex].margin = m_painter->getCanvasMargin();
		m_groupStack[m_groupIndex].space  = m_painter->getCanvasSpace();
		m_groupStack[m_groupIndex].bounds = m_window;

		m_componentIndex = 0;
		m_overdrawIndex	 = 0;
		m_stringIndex	 = 0;
	}
	//--------------------------------------------------------------------------
	void UIContext::End()
	{
		//printComponentStack();
		m_painter->beginDraw();

		#if PSEUDO_RETAINED_MODE
		// Draw retained components
		for(int i=0;i<m_componentIndex;i++)
		{
			draw(m_componentStack[i]);
		}
		#endif

		// Draw overdraw
		for(int i=0;i<m_overdrawIndex;i++)
		{
			draw(m_overdrawStack[i]);
		}

		m_painter->end();

		// Release focus.
		if (m_mouseButton[0].state & ButtonFlags::END) m_uiOnFocus = false;

		// Update _state for next frame.
		for (int i = 0; i < 3; i++)
		{
			if (m_mouseButton[i].state & ButtonFlags::BEGIN) m_mouseButton[i].state ^= ButtonFlags::BEGIN;
			/*else*/ 
			if (m_mouseButton[i].state & ButtonFlags::END) m_mouseButton[i].state = ButtonFlags::OFF;
		}

		// Flush key buffer
		m_nbKeys = 0;
	}
	//--------------------------------------------------------------------------
	void UIContext::draw(const Component& _component)
	{
		int style = 0;
		switch(_component.type)
		{
			case Component::LABEL :
				m_painter->drawLabel(_component.rect, 
									 m_stringStack[_component.textIndex],
									 _component.rt, 
									 _component.nbLines, 
									 _component.hover, 
									 style);
				break;
			case Component::BUTTON :
				m_painter->drawButton(_component.rect, m_stringStack[_component.textIndex], _component.rt, _component.state, _component.hover, _component.focus, style);
				break;
			case Component::RADIO_BUTTON :
				m_painter->drawRadioButton(_component.rect, m_stringStack[_component.textIndex], _component.rt, _component.re, _component.state, _component.hover, _component.focus, style);
				break;
			case Component::CHECK_BUTTON :
				m_painter->drawCheckButton(_component.rect, m_stringStack[_component.textIndex], _component.rt, _component.rc, _component.state, _component.hover, _component.focus, style);
				break;
			case Component::LIST_ITEM :
				m_painter->drawListItem(_component.rect, m_stringStack[_component.textIndex], _component.rt, _component.state, _component.hover, style);
				break;
			case Component::LIST_BOX :
				m_painter->drawListBox(_component.rect, _component.nbLines, const_cast<const char**>(&(m_stringStack[_component.textIndex])), _component.rt, _component.rc, _component.selected, _component.hovered, style);
				break;
			case Component::COMBO_BOX :
			    m_painter->drawComboBox(_component.rect, m_stringStack[_component.textIndex], _component.rt, _component.rd, _component.hover, _component.focus, style);
				break;
			case Component::COMBO_BOX_OPTION :
				m_painter->drawComboOptions( _component.rect, _component.nbLines, const_cast<const char**>(&(m_stringStack[_component.textIndex])), _component.rt, _component.rd, _component.selected, _component.hovered, _component.hover, _component.focus, style);  // TODO : Change painter
				break;
			case Component::SLIDER :
				m_painter->drawHorizontalSlider(_component.rect, _component.rs, _component.value, _component.rc, _component.hover, style);
				break;
			case Component::DISCRET_SLIDER :
				m_painter->drawDiscretSlider(_component.rect, _component.rs, _component.selected, _component.nbLines, _component.rc, _component.hover, style);
				break;
			case Component::EDIT_LINE :
				m_painter->drawLineEdit(_component.rect, m_stringStack[_component.textIndex], _component.rt, _component.carretPos, _component.focus, _component.hover, style);
				break;
			case Component::PANEL_HEADER :
			    m_painter->drawPanel(_component.rect, m_stringStack[_component.textIndex], _component.rt, _component.rd, _component.state, _component.hover, _component.focus, style);
				break;
			case Component::FRAME :
				m_painter->drawFrame( _component.bounds, _component.margin, 0);
				break;
			case Component::GENERIC :
				_component.generic->Draw(_component.rect);
				break;
			default : 
				assert(false);
		}
	}
	//--------------------------------------------------------------------------
	void UIContext::printComponentStack() const
	{
		std::stringstream out;
		out << "--- Component stack dump ---\n";
		for(int i=m_componentIndex-1;i>=0;--i)
		{
			switch(m_componentStack[i].type)
			{
				case Component::LABEL :
					out << "Label\n";
					break;
				case Component::BUTTON :
					out << "Button\n";
					break;
				case Component::RADIO_BUTTON :
					out << "RadioButton\n";
					break;
				case Component::CHECK_BUTTON :
					out << "CheckButton\n";
					break;
				case Component::LIST_ITEM :
					out << "ListItem\n";
					break;
				case Component::LIST_BOX :
					out << "ListBox\n";
					break;
				case Component::COMBO_BOX :
					out << "ComboBox\n";
					break;
				case Component::COMBO_BOX_OPTION :
					out << "ComboBoxOption\n";
					break;
				case Component::SLIDER :
					out << "Slider\n";
					break;
				case Component::EDIT_LINE :
					out << "EditLine\n";
					break;
				case Component::PANEL_HEADER :
					out << "PanelHeader\n";
					break;
				case Component::FRAME :
					out << "Frame (x:"<< m_componentStack[i].bounds.x <<",y:"<< m_componentStack[i].bounds.y <<",w:"<< m_componentStack[i].bounds.w <<",h:"<< m_componentStack[i].bounds.h <<") \n";
					break;
				default : assert(false);
			}
		}
		printf("[Info] %s\n",out.str().c_str());
		fflush(stdout);
	}
	//--------------------------------------------------------------------------
	Rect UIContext::placeRect(const Rect & _r)
	{
		Group & group = m_groupStack[m_groupIndex];

		Rect rect = _r;

		int layout = group.flags & Flags::Layout::MASK;
		int alignment = group.flags & Flags::Align::MASK;


		if (layout == Flags::Layout::NONE)
		{
		    // Translate rect to absolute coordinates.
		    rect.x += group.bounds.x;
		    rect.y += group.bounds.y;
		}
		else if (layout == Flags::Layout::VERTICAL)
		{
		    // Vertical behavior
		    if (alignment & Flags::Align::TOP)
		    {
		        // Move down bounds.y with the space for the new rect
		        group.bounds.y -= ((group.bounds.h > 0) * group.space + rect.h);

		        // Widget's y is the group bounds.y
		        rect.y = group.bounds.y;
		    }
		    else
		    {
		        rect.y = group.bounds.y + group.bounds.h + (group.bounds.h > 0) * group.space;
		    }
		    // Add space after first object inserted in the group
		    group.bounds.h += (group.bounds.h > 0) * group.space + rect.h;

		    // Horizontal behavior
		    if (alignment & Flags::Align::RIGHT)
		    {
		        rect.x += group.bounds.x + group.bounds.w - rect.w;
		        
		        int minBoundX = std::min(group.bounds.x, rect.x);
		        group.bounds.w = group.bounds.x + group.bounds.w - minBoundX;
		        group.bounds.x = minBoundX;
		    }
		    else
		    {
		        group.bounds.w = std::max(group.bounds.w, rect.x + rect.w);
		        rect.x += group.bounds.x;
		   }
		}
		else if (layout == Flags::Layout::HORIZONTAL)
		{
		    // Horizontal behavior
		    if (alignment & Flags::Align::RIGHT)
		    {
		        // Move left bounds.x with the space for the new rect
		        group.bounds.x -= ((group.bounds.w > 0) * group.space + rect.w);

		        // Widget's x is the group bounds.x
		        rect.x = group.bounds.x;
		    }
		    else
		    {
		        rect.x = group.bounds.x + group.bounds.w + (group.bounds.w > 0) * group.space;
		    }
		    // Add space after first object inserted in the group
		    group.bounds.w += (group.bounds.w > 0) * group.space + rect.w;

		    // Vertical behavior
		    if (alignment & Flags::Align::TOP)
		    {
		        rect.y += group.bounds.y + group.bounds.h - rect.h;
		        
		        int minBoundY  = std::min(group.bounds.y, rect.y);
		        group.bounds.h = group.bounds.y + group.bounds.h - minBoundY;
		        group.bounds.y = minBoundY;
		    }
		    else
		    {
		        group.bounds.h = std::max(group.bounds.h, rect.y + rect.h);
		        rect.y += group.bounds.y;
		    }
		}
		return rect;
	}
	//--------------------------------------------------------------------------
	void UIContext::BeginGroup(int _flags, const Rect& _r)
	{
		// Push one more group.
		Group & parentGroup = m_groupStack[m_groupIndex];
		m_groupIndex++;
		Group & newGroup = m_groupStack[m_groupIndex];

		#if PSEUDO_RETAINED_MODE
		newGroup.index = m_componentIndex-1;
		#endif

		// Assign layout behavior
		int parentLayout = parentGroup.flags & Flags::Layout::MASK;
		int parentAlign = parentGroup.flags & Flags::Align::MASK;

		// If the _flags ask to force the layout then keep the newcanvas layout as is
		// otherwise, adapt it to the parent's behavior
		if ( ! (_flags & Flags::Layout::FORCE) || ! (_flags & Flags::Layout::NONE) )
		{
		    // If default then use parent _style except if none layout => default fallback
		    if ( _flags & Flags::Layout::DEFAULT )
		    {
		        if ( parentLayout &  Flags::Layout::NONE )
		            _flags = Flags::Grow::DEFAULT_FALLBACK; 
		        else
		            _flags = parentGroup.flags;
		    }
		    else if (   parentLayout & ( Flags::Layout::VERTICAL | Flags::Layout::HORIZONTAL) 
		             && _flags & ( Flags::Layout::VERTICAL | Flags::Layout::HORIZONTAL) )
		    {
		        _flags = (_flags & Flags::Align::XMASK) | parentAlign;
		    }
		} 

		newGroup.margin = ((_flags & Flags::Layout::NO_MARGIN) == 0) * m_painter->getCanvasMargin();
		newGroup.space  = ((_flags & Flags::Layout::NO_SPACE) == 0) * m_painter->getCanvasSpace();
		newGroup.flags  = _flags;
		//int newLayout = _flags & Flags::Layout::Mask;
		int newAlign 	= _flags & Flags::Align::MASK;
		int newStart 	= _flags & Flags::Start::MASK;

		

		// Place _a regular _rect in current group, this will be the new group _rect start pos
		Rect _rect = _r;

		// Don't modify parent group bounds yet, done in endGroup()
		// Right now place the new group _rect
		if ( parentLayout == Flags::Layout::NONE)
		{
		    // Horizontal behavior.
		    _rect.x +=   parentGroup.bounds.x + newGroup.margin
		              + ((newStart & Flags::Start::RIGHT)>0)*parentGroup.bounds.w
		              - ((newAlign & Flags::Align::RIGHT)>0)*(2*newGroup.margin + _rect.w);

		    // Vertical behavior.
		    _rect.y +=   parentGroup.bounds.y + newGroup.margin
		              + ((newStart & Flags::Start::TOP)>0)*parentGroup.bounds.h
		              - ((newAlign & Flags::Align::TOP)>0)*(2*newGroup.margin + _rect.h);
		}
		else if ( parentLayout == Flags::Layout::VERTICAL)
		{
		    // Horizontal behavior.
		    _rect.x +=   parentGroup.bounds.x + newGroup.margin
		              + ((parentAlign & Flags::Align::RIGHT)>0)*(parentGroup.bounds.w - 2*newGroup.margin - _rect.w);

		    // Vertical behavior.
		    if (parentAlign & Flags::Align::TOP)
		    {
		        _rect.y += parentGroup.bounds.y - ((parentGroup.bounds.h > 0) * parentGroup.space) - newGroup.margin - _rect.h;
		    }
		    else
		    {
		        _rect.y += parentGroup.bounds.y + parentGroup.bounds.h + (parentGroup.bounds.h > 0) * parentGroup.space + newGroup.margin;
		    }
		}
		else if ( parentLayout == Flags::Layout::HORIZONTAL)
		{
		    // Horizontal behavior.
		    if (parentAlign & Flags::Align::RIGHT)
		    {
		        _rect.x += parentGroup.bounds.x - ((parentGroup.bounds.w > 0) * parentGroup.space) - newGroup.margin - _rect.w;
		    }
		    else
		    {
		        _rect.x += parentGroup.bounds.x + parentGroup.bounds.w + (parentGroup.bounds.w > 0) * parentGroup.space + newGroup.margin;
		    }

		    // Vertical behavior.
		    _rect.y +=   parentGroup.bounds.y + newGroup.margin
		              + ((parentAlign & Flags::Align::TOP)>0)*(parentGroup.bounds.h - 2*newGroup.margin - _rect.h);
		}

		newGroup.bounds = _rect;
	}
	//--------------------------------------------------------------------------
	void UIContext::EndGroup()
	{
		// Pop the new group.
		Group & newGroup = m_groupStack[m_groupIndex];
		m_groupIndex--;
		Group & parentGroup = m_groupStack[m_groupIndex];

		// add any increment from the embedded group
		if (parentGroup.flags & ( Flags::Layout::VERTICAL | Flags::Layout::HORIZONTAL ) )
		{    
		    int maxBoundX = std::max(parentGroup.bounds.x + parentGroup.bounds.w, newGroup.bounds.x + newGroup.bounds.w + newGroup.margin);
		    int minBoundX = std::min(parentGroup.bounds.x, newGroup.bounds.x - newGroup.margin);
		    parentGroup.bounds.x = minBoundX;
		    parentGroup.bounds.w = maxBoundX - minBoundX;
		    
		    int maxBoundY = std::max(parentGroup.bounds.y + parentGroup.bounds.h, newGroup.bounds.y + newGroup.bounds.h + newGroup.margin);
		    int minBoundY = std::min(parentGroup.bounds.y, newGroup.bounds.y - newGroup.margin);
		    parentGroup.bounds.y = minBoundY;
		    parentGroup.bounds.h = maxBoundY - minBoundY;
		}
	}
	//--------------------------------------------------------------------------
	void UIContext::BeginFrame(int _flags, const Rect& _rect, int /*_style*/)
	{
		#if PSEUDO_RETAINED_MODE
		Component& component = m_componentStack[m_componentIndex++];
		component.type		 = Component::FRAME;
		component.bounds	 = _rect;
		component.margin	 = 0;
		#endif

		BeginGroup(_flags, _rect);
	}
	//--------------------------------------------------------------------------
	void UIContext::EndFrame()
	{
		EndGroup();

		#if PSEUDO_RETAINED_MODE
		Component& component = m_componentStack[m_groupStack[m_groupIndex + 1].index];
		component.bounds	 = m_groupStack[m_groupIndex + 1].bounds;
		component.margin	 = m_groupStack[m_groupIndex + 1].margin;
		#else
		m_painter->drawFrame( m_groupStack[m_groupIndex + 1].bounds, m_groupStack[m_groupIndex + 1].margin, 0);
		#endif
	}
	//--------------------------------------------------------------------------
	bool UIContext::BeginPanel(Rect & _r, const char * _text, bool * _isUnfold, int _flags, int _style)
	{
		Rect rt, ra;
		Rect rpanel = m_painter->getPanelRect(Rect(_r.x, _r.y), _text, rt, ra);

		if (_flags & Flags::Layout::DEFAULT)
		    _flags = Flags::Grow::DEFAULT_FALLBACK;
		BeginGroup( ( _flags | Flags::Layout::NO_MARGIN |Flags::Layout::NO_SPACE ) & Flags::Start::XMASK , rpanel );
		    
	    Rect rect = m_groupStack[m_groupIndex].bounds;

	    bool focus = hasFocus(rect);
	    bool hover = isHover(rect);
	    
	    if (focus)
	    {
	        m_uiOnFocus = true;
	       
	        _r.x += m_currentCursor.x - m_mouseButton[0].cursor.x;
	        _r.y += m_currentCursor.y - m_mouseButton[0].cursor.y;
	        rect.x += m_currentCursor.x - m_mouseButton[0].cursor.x;
	        rect.y += m_currentCursor.y - m_mouseButton[0].cursor.y;

	        m_mouseButton[0].cursor = m_currentCursor;
	    }

	    if (m_mouseButton[0].state & ButtonFlags::END && focus && overlap( Rect(rect.x + ra.x, rect.y + ra.y, ra.w, ra.h) , m_currentCursor))
	    {
	        if ( _isUnfold )
	            *_isUnfold = !*_isUnfold; 
	    }

		#if PSEUDO_RETAINED_MODE
		Component& component = m_componentStack[m_componentIndex++];
		component.type		 = Component::PANEL_HEADER;
		component.rect 		 = rect;
		component.rt 		 = rt;
		component.rd 		 = ra;
		component.state		 = *_isUnfold;
		component.hover 	 = hover;
		component.focus 	 = focus;
		component.textIndex	 = m_stringIndex;
		strcpy(m_stringStack[m_stringIndex++], _text);
		#else
	    m_painter->drawPanel(rect, _text, rt, ra, *_isUnfold, hover, focus, _style);
		#endif
//TODO place after BeginFrame ?

	    if (_isUnfold && *_isUnfold)
	    {
	        BeginFrame( _flags, Rect(0, 0, _r.w, _r.h) );
	        return true;
	    }
	    else
	    {
	        EndGroup();
	        return false;
	    }
	}
	//--------------------------------------------------------------------------
	void UIContext::EndPanel()
	{
		EndFrame();
		EndGroup();
	}
	//--------------------------------------------------------------------------
	void UIContext::Label(const Rect & _r, const char * _text, int _style)
	{
		Rect rt;
		int nbLines;
		Rect rect = placeRect(m_painter->getLabelRect(_r, _text, rt, nbLines));

		#if PSEUDO_RETAINED_MODE
		Component& component = m_componentStack[m_componentIndex++];
		component.type		 = Component::LABEL;
		component.rect 		 = rect;
		component.rt 		 = rt;
		component.nbLines	 = nbLines;
		component.hover 	 = isHover(rect);
		component.textIndex	 = m_stringIndex;
		strcpy(m_stringStack[m_stringIndex++], _text);
		#else
		m_painter->drawLabel(rect, _text, rt, nbLines, isHover(rect), _style);
		#endif
	}
	//--------------------------------------------------------------------------
	bool UIContext::Button(const Rect & _r, const char * _text, bool * _state, int _style)
	{
		Rect rt;
		Rect rect = placeRect(m_painter->getButtonRect(_r, _text, rt));

		bool focus = hasFocus(rect);
		bool hover = isHover(rect);
		bool isDown = false;
		
		if ( _state )
		{
		    isDown = *_state;
		}
		else
		{
		    isDown = (m_mouseButton[0].state & ButtonFlags::ON) && hover && focus;
		}

		#if PSEUDO_RETAINED_MODE
		Component& component = m_componentStack[m_componentIndex++];
		component.type		 = Component::BUTTON;
		component.rect 		 = rect;
		component.rt 		 = rt;
		component.state	 	 = isDown;
		component.hover 	 = hover;
		component.focus 	 = focus;
		component.textIndex	 = m_stringIndex;
		strcpy(m_stringStack[m_stringIndex++], _text);
		#else
		m_painter->drawButton(rect, _text, rt, isDown, hover, focus, _style);
		#endif

		if (focus)
		{
		    m_uiOnFocus = true;
		}

		if (m_mouseButton[0].state & ButtonFlags::END && focus && overlap(rect, m_currentCursor))
		{
		    if ( _state )
		        *_state = !*_state; 
		    return true;
		}

		return false;
	}
	//--------------------------------------------------------------------------
	bool UIContext::CheckButton(const Rect & _r, const char * _text, bool * _state, int _style)
	{
		Rect rt, rc;
		Rect rect = placeRect(m_painter->getCheckRect(_r, _text, rt, rc));

		//Tools::Logger::Info("Button::rect  (x:%d y:%d) (w:%d h:%d)",rect.x,rect.y,rect.w,rect.h);
		bool focus = hasFocus(rect);
		bool hover = isHover(rect);

		#if PSEUDO_RETAINED_MODE
		Component& component = m_componentStack[m_componentIndex++];
		component.type		 = Component::CHECK_BUTTON;
		component.rect 		 = rect;
		component.rt 		 = rt;
		component.rc 		 = rc;
		component.state	 	 = ( _state ) && (*_state);
		component.hover 	 = hover;
		component.focus 	 = focus;
		component.textIndex	 = m_stringIndex;
		strcpy(m_stringStack[m_stringIndex++], _text);
		#else
		m_painter->drawCheckButton(rect, _text, rt, rc, ( _state ) && (*_state), hover, focus, _style);
		#endif

		if (hasFocus(rect))
		{
		    m_uiOnFocus = true;
		}

		if (m_mouseButton[0].state & ButtonFlags::END && focus && overlap(rect, m_currentCursor))
		{
		    if ( _state )
		        *_state = !*_state;
		    return true;
		}

		return false;
	}
	//--------------------------------------------------------------------------
	bool UIContext::RadioButton(int _reference, const Rect & _r, const char * _text, int * _selected, int _style)
	{
		Rect rr, rt;
		Rect rect = placeRect(m_painter->getRadioRect(_r, _text, rt, rr));
		
		bool focus = hasFocus(rect);
		bool hover = isHover(rect);
		
		#if PSEUDO_RETAINED_MODE
		Component& component = m_componentStack[m_componentIndex++];
		component.type		 = Component::RADIO_BUTTON;
		component.rect 		 = rect;
		component.rt 		 = rt;
		component.re 		 = rr;
		component.state	 	 = (_selected) && (_reference == *_selected);
		component.hover 	 = hover;
		component.focus 	 = focus;
		component.textIndex	 = m_stringIndex;
		strcpy(m_stringStack[m_stringIndex++], _text);
		#else
		m_painter->drawRadioButton(rect, _text, rt, rr, (_selected) && (_reference == *_selected), hover, focus, _style);
		#endif

		if (focus)
		{
		    m_uiOnFocus = true;
		}

		if (m_mouseButton[0].state & ButtonFlags::END && focus && overlap(rect, m_currentCursor))
		{
		    if (_selected)
		        *_selected = _reference;
		    return true;
		}

		return false;
	}
	//--------------------------------------------------------------------------
	bool UIContext::Generic(const Rect & _rect, UIComponent& _comp, int _style)
	{
		Rect rect = placeRect(_comp.CheckRect(_rect));

		//Tools::Logger::Info("Button::rect  (x:%d y:%d) (w:%d h:%d)",rect.x,rect.y,rect.w,rect.h);
		bool focus = hasFocus(rect);
		bool hover = isHover(rect);

		#if PSEUDO_RETAINED_MODE
		Component& component = m_componentStack[m_componentIndex++];
		component.type		 = Component::GENERIC;
		component.rect 		 = rect;
		component.hover 	 = hover;
		component.focus 	 = focus;
		component.generic	 = &_comp;
		#else
		_comp.Draw(rect);
		#endif

		if (hasFocus(rect))
		{

		    m_uiOnFocus = true;
		}

		if(m_mouseButton[0].state & ButtonFlags::END && focus && overlap(rect, m_currentCursor))
		{
			return _comp.Mouse(rect, m_currentCursor.x, m_currentCursor.y);
		}

		return false;
	}
	//--------------------------------------------------------------------------
	bool UIContext::ListItem(int _index, const Rect & _r, const char * _text, int * _selected, int _style)
	{
		Rect rt;
		Rect rect = placeRect(m_painter->getItemRect(_r, _text, rt));


		#if PSEUDO_RETAINED_MODE
		Component& component = m_componentStack[m_componentIndex++];
		component.type		 = Component::LIST_ITEM;
		component.rect 		 = rect;
		component.rt 		 = rt;
		component.state	 	 = (_selected) && (_index == *_selected);
		component.hover 	 = isHover(rect);
		component.textIndex	 = m_stringIndex;
		strcpy(m_stringStack[m_stringIndex++], _text);
		#else
		m_painter->drawListItem(rect, _text, rt, (_selected) && (_index == *_selected), isHover(rect), _style);
		#endif

		return isHover(rect);
	}
	//--------------------------------------------------------------------------
	bool UIContext::ListBox(const Rect & _r, int _numOptions, const char * _options[], int * _selected, int _style)
	{
		Rect ri;
		Rect rt;
		Rect rect = placeRect(m_painter->getListRect(_r, _numOptions, _options, ri, rt));

		bool focus = hasFocus(rect);
		bool hover = isHover(rect);
		int hovered = -1;
		
		if ( hover )
		{
		    hovered = _numOptions - 1 - (m_currentCursor.y - (rect.y+ri.y)) / (ri.h);
		}
		
		int lSelected = -1;
		if (_selected)
		    lSelected = *_selected;

		#if PSEUDO_RETAINED_MODE
		Component& component = m_componentStack[m_componentIndex++];
		component.type		 = Component::LIST_BOX;
		component.rect 		 = rect;
		component.rt 		 = ri;
		component.rc 		 = rt;
		component.selected 	 = lSelected;
		component.hovered 	 = hovered;
		component.nbLines	 = _numOptions;
		component.textIndex	 = m_stringIndex;
		for(int i=0;i<_numOptions;++i)
			strcpy(m_stringStack[m_stringIndex++], _options[i]);
		#else
		m_painter->drawListBox(rect, _numOptions, _options, ri, rt, lSelected, hovered, _style);
		#endif

		if (focus)
		{
		    m_uiOnFocus = true;
		}

		if (m_mouseButton[0].state & ButtonFlags::END && focus && overlap(rect, m_currentCursor) && (lSelected != hovered) )
		{
		    if (_selected)
		        *_selected = hovered;
		    return true;
		}

		return false;
	}
	//--------------------------------------------------------------------------
	bool UIContext::ComboBox(const Rect & _r, int _numOptions, const char * _options[], int * _selected, int _style)
	{
		// First get the rect of the COmbo box itself and do some test with it
		Rect rt, ra;
		Rect rect = placeRect(m_painter->getComboRect(_r, _numOptions, _options, *_selected, rt, ra));

		bool focus = hasFocus(rect);
		bool hover = isHover(rect);

		if (focus)
		{
		    m_uiOnFocus = true;

		    // then if the combo box has focus, we can look for the geometry of the optios frame
		    Rect ro, ri, rit;
		    ro = m_painter->getComboOptionsRect(rect, _numOptions, _options, ri, rit);

		    int hovered = -1;
		    bool hoverOptions = overlap(ro, m_currentCursor);
		    if ( hoverOptions )
		    {
		        hovered = _numOptions - 1 - (m_currentCursor.y - (ro.y+ri.y)) / (ri.h);
		    }

		    // draw combo anyway
		    #if PSEUDO_RETAINED_MODE
			Component& component = m_componentStack[m_componentIndex++];
			component.type		 = Component::COMBO_BOX;
			component.rect 		 = rect;
			component.rt 		 = rt;
			component.rd 		 = ra;
			component.hover 	 = hover;
			component.focus 	 = focus;
			component.textIndex	 = m_stringIndex;
			strcpy(m_stringStack[m_stringIndex++], _options[*_selected]);
			#else
		    m_painter->drawComboBox(rect, _options[*_selected], rt, ra, hover, focus, _style);
			#endif
		   
		    // draw _options
		    #if PSEUDO_RETAINED_MODE
			Component& overComp  = m_overdrawStack[m_overdrawIndex++];
			overComp.type		 = Component::COMBO_BOX_OPTION;
			overComp.rect 		 = ro;
			overComp.rt 		 = ri;
			overComp.rd 		 = rit;
			overComp.selected	 = *_selected;
			overComp.hovered 	 = hovered;
			overComp.hover	 	 = hover;
			overComp.focus 		 = focus;
			overComp.nbLines	 = _numOptions;
			overComp.textIndex	 = m_stringIndex;
			for(int i=0;i<_numOptions;++i)
				strcpy(m_stringStack[m_stringIndex++], _options[i]);
			#else
			assert(false); // Not managed
		    m_painter->drawComboOptions(ro, _numOptions, _options, ri, rit, *_selected, hovered, hover, focus, _style);
			#endif
 
		    // When the widget get the focus, cache the focus point
		    if (!m_twoStepFocus)
		    {
		        if (hover && m_mouseButton[0].state & ButtonFlags::END)
		        {
		            m_focusPoint = m_mouseButton[0].cursor;
		            m_twoStepFocus = true;                 
		        }
		    }
		    else
		    {
		        // Else Release the 2level focus when left mouse down out or up anyway
		        // replace the stored left mouse down pos with the focus point to keep focus
		        // on this widget for the next widget rendered in the frame
		        if (!(hoverOptions || hover)
		            &&   (    m_mouseButton[0].state & ButtonFlags::BEGIN
		                ||    m_mouseButton[0].state & ButtonFlags::END))
		        {
		            m_twoStepFocus = false;
		        }
		        else if ( (hoverOptions || hover) && m_mouseButton[0].state & ButtonFlags::END)
		        {
		            m_mouseButton[0].cursor = m_focusPoint;
		            m_twoStepFocus = false;
		        }

		        if (hoverOptions && m_mouseButton[0].state & ButtonFlags::BEGIN)
		        {
		            m_mouseButton[0].cursor = m_focusPoint;
		        }
		    }

		    // On mouse left bouton up, then select it
		    if ( (hovered > -1) && (hovered < _numOptions) && (m_mouseButton[0].state & ButtonFlags::END ) )
		    {
		        *_selected = hovered;
		        return true;
		    }
		}
		else
		{

		    // draw combo anyway
		    #if PSEUDO_RETAINED_MODE
			Component& component = m_componentStack[m_componentIndex++];
			component.type		 = Component::COMBO_BOX;
			component.rect 		 = rect;
			component.rt 		 = rt;
			component.rd 		 = ra;
			component.hover 	 = hover;
			component.focus 	 = focus;
			component.textIndex	 = m_stringIndex;
			strcpy(m_stringStack[m_stringIndex++], _options[*_selected]);
			#else
		    m_painter->drawComboBox(rect, _options[*_selected], rt, ra, hover, focus, _style); // TODO : Change painter
			#endif
		}

		return false;
	}
	//--------------------------------------------------------------------------
	bool UIContext::HorizontalSlider(const Rect & _r, float _min, float _max, float * value, /*bool& _isPressed, */int _style)
	{
		// Map current value to 0-1.
		float f = (*value - _min) / (_max - _min);
		if (f < 0.0f) f = 0.0f;
		else if (f > 1.0f) f = 1.0f;

		Rect rs;
		Rect rc;
		Rect rect = placeRect(m_painter->getHorizontalSliderRect( _r, rs, f, rc ));
	  
		bool changed = false;
		if (hasFocus(rect))
		{
		    m_uiOnFocus = true;

		    int xs = rect.x + rs.x + rc.w / 2;
		    int x = m_currentCursor.x - xs;

		    if (x < 0) x = 0;
		    else if (x > rs.w) x = rs.w;

		    rc.x = x;

		    float f = (float(x)) / ((float) rs.w);
		    f = f * (_max - _min) + _min;

		    if (fabs(f - *value) > (_max - _min) * 0.01f) 
		    {
				//_isPressed 	= m_mouseButton[0].state & ButtonFlags::BEGIN;
		        changed 	= true;
		        *value  	= f;
		    }
		}

		bool hover = isHover(rect);
		#if PSEUDO_RETAINED_MODE
		Component& component = m_componentStack[m_componentIndex++];
		component.type		 = Component::SLIDER;
		component.rect 		 = rect;
		component.rs 		 = rs;
		component.rc 		 = rc;
		component.hover 	 = hover;
		component.value 	 = f;
		component.textIndex	 = m_stringIndex;
		sprintf(m_stringStack[m_stringIndex++],"%f",f);
		#else
		m_painter->drawHorizontalSlider(rect, rs, f, rc, hover, _style);
		#endif

		return changed;
	}
	//--------------------------------------------------------------------------
	bool UIContext::DiscretSlider(const Rect & _r, int _nValues, float* _values, int* _index, int _style)
	{
		// Map current value to 0-1.
		int maxValue = _nValues==1?1:_nValues-1;
		float f = *_index / (float)(maxValue);
		if (f < 0.0f) f = 0.0f;
		else if (f > 1.0f) f = 1.0f;

		Rect rs;	// Slider
		Rect rc;	// Cursor
		Rect rect = placeRect(m_painter->getHorizontalSliderRect( _r, rs, f, rc ));
	  
		bool changed = false;
		if (hasFocus(rect))
		{
		    m_uiOnFocus = true;

		    int xs = rect.x + rs.x + rc.w / 2;
		    int x = m_currentCursor.x - xs;

		    if (x < 0) x = 0;
		    else if (x > rs.w) x = rs.w;

		    rc.x = x;

		    float f = (float(x)) / ((float) rs.w);
		    int newIndex = int(floor(f * _nValues));
			newIndex = newIndex>_nValues-1?_nValues-1:newIndex;

		    if(newIndex != *_index) 
		    {
		        changed = true;
		        *_index = newIndex;
		    }
		}

		bool hover = isHover(rect);
		#if PSEUDO_RETAINED_MODE
		Component& component = m_componentStack[m_componentIndex++];
		component.type		 = Component::DISCRET_SLIDER;
		component.rect 		 = rect;
		component.rs 		 = rs;
		component.rc 		 = rc;
		component.selected	 = *_index;
		component.nbLines 	 = _nValues;
		component.hover 	 = hover;
		component.textIndex	 = m_stringIndex;
		if(_values != NULL)
			sprintf(m_stringStack[m_stringIndex++],"%f",_values[*_index]);
		else
			sprintf(m_stringStack[m_stringIndex++],"NULL (No value)");
		#else
		m_painter->drawDiscretSlider(rect, rs, index, nIndices, rc, hover, _style);
		#endif

		return changed;
	}
	//--------------------------------------------------------------------------
	void UIContext::setCursor(int _x, int _y)
	{
		m_currentCursor.x = _x;
		m_currentCursor.y = m_window.h - _y;
	}
	//--------------------------------------------------------------------------
	bool UIContext::overlap(const Rect & _rect, const Point & _p)
	{
		return _p.x >= _rect.x && _p.x < _rect.x + _rect.w && 
		       _p.y >= _rect.y && _p.y < _rect.y + _rect.h;
	}
	//--------------------------------------------------------------------------
	// Left mouse _button down, and down cursor inside _rect.
	bool UIContext::hasFocus(const Rect & _rect)
	{
		if (m_twoStepFocus)
		{
		    return overlap(_rect, m_focusPoint);
		}
		else
		{
		    return (m_mouseButton[0].state & ButtonFlags::ON) && overlap(_rect, m_mouseButton[0].cursor);
		}
	}
	//--------------------------------------------------------------------------
	bool UIContext::isHover(const Rect & _rect)
	{
		if (m_uiOnFocus && !hasFocus(_rect)) return false;
		return overlap(_rect, m_currentCursor);
	}
}


