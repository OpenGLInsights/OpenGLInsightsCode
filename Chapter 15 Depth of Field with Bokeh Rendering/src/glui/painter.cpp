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

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glui/painter.hpp>
#include <glui/arial12.hpp>
#include <glm/glm.hpp>
//#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
//------------------------------------------------------------------------------
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>

#ifdef WIN32
	#pragma warning( disable : 4244 )
#endif

namespace glui
{
	const unsigned int GLPainter::MaxOptions    = MAX_OVERDRAW_OPTIONS;
	const unsigned int GLPainter::MaxOptionSize = MAX_OVERDRAW_OPTION_SIZE;
	//--------------------------------------------------------------------------
	#define norm255( i ) ( (float) ( i ) / 255.0f )
	//--------------------------------------------------------------------------
	enum Color
	{
		cBase 		 = 0,
		cBool 		 = 4,
		cOutline 	 = 8,
		cFont 		 = 12,
		cFontBack 	 = 16,
		cTranslucent = 20,
		cNbColors 	 = 24,
	};
	//--------------------------------------------------------------------------
	static glm::vec4 s_colors[cNbColors] = 
	{
		// cBase
		glm::vec4(norm255(89),  norm255(89),  norm255(89),  0.7f),
		glm::vec4(norm255(166), norm255(166), norm255(166), 0.8f),
		glm::vec4(norm255(212), norm255(228), norm255(60),  0.5f),
		glm::vec4(norm255(227), norm255(237), norm255(127), 0.5f),

		// cBool
		glm::vec4(norm255(99),  norm255(37),  norm255(35),  1.0f),
		glm::vec4(norm255(149), norm255(55),  norm255(53),  1.0f),
		glm::vec4(norm255(212), norm255(228), norm255(60),  1.0f),
		glm::vec4(norm255(227), norm255(237), norm255(127), 1.0f),

		// cOutline
		glm::vec4(norm255(255), norm255(255), norm255(255), 1.0f),
		glm::vec4(norm255(255), norm255(255), norm255(255), 1.0f),
		glm::vec4(norm255(255), norm255(255), norm255(255), 1.0f),
		glm::vec4(norm255(255), norm255(255), norm255(255), 1.0f),

		// cFont
		glm::vec4(norm255(255), norm255(255), norm255(255), 1.0f),
		glm::vec4(norm255(255), norm255(255), norm255(255), 1.0f),
		glm::vec4(norm255(255), norm255(255), norm255(255), 1.0f),
		glm::vec4(norm255(255), norm255(255), norm255(255), 1.0f),

		// cFontBack
		glm::vec4(norm255(79),  norm255(129), norm255(189), 1.0),
		glm::vec4(norm255(79),  norm255(129), norm255(189), 1.0),
		glm::vec4(norm255(128), norm255(100), norm255(162), 1.0),
		glm::vec4(norm255(128), norm255(100), norm255(162), 1.0),

		// cTranslucent
		glm::vec4(norm255(0), 	norm255(0),   norm255(0), 	0.0),
		glm::vec4(norm255(0), 	norm255(0),   norm255(0), 	0.0),
		glm::vec4(norm255(0), 	norm255(0),   norm255(0), 	0.0),
		glm::vec4(norm255(0),	norm255(0),   norm255(0), 	0.0)
	};
	//--------------------------------------------------------------------------
	const char* cWidgetVSSource = {
		"#version 330\n\
		in vec4 Position;\n\
		in vec3 TexCoord;\n\
		\n\
		uniform mat4 Projection;\n\
		uniform mat4 View;\n\
		\n\
		out vec3 LocalTexCoord;\n\
		void main()\n\
		{\n\
		    gl_Position   = Projection * View * Position;\n\
		    LocalTexCoord = TexCoord;\n\
		}\n\
		"};

	// @@ IC: Use standard GLSL. Do not initialize uniforms.
	//--------------------------------------------------------------------------
	const char* cWidgetFSSource = {
		"#version 330\n\
		uniform vec4 FillColor;\n\
		uniform vec4 BorderColor;\n\
		uniform vec2 Zones;\n\
		\n\
		in vec3 LocalTexCoord;\n\
		\n\
		out vec4 FragColor;\n\
		\n\
		void main()\n\
		{\n\
		    float doTurn 	   = float(LocalTexCoord.y > 0);\n\
		    float radiusOffset = doTurn * abs(LocalTexCoord.z);\n\
		    float turnDir 	   = sign(LocalTexCoord.z );\n\
		    vec2 uv 		   = vec2(LocalTexCoord.x + turnDir*radiusOffset, LocalTexCoord.y);\n\
			\n\
		    float l = abs( length(uv) - radiusOffset );\n\
		    float a = clamp( l - Zones.x, 0.0, 2.0);\n\
		    float b = clamp( l - Zones.y, 0.0, 2.0);\n\
		    b = exp2(-2.0*b*b);\n\
			\n\
		    FragColor 	 = (FillColor * b + (1.0-b)*BorderColor);\n\
		    FragColor.a *= exp2(-2.0*a*a);\n\
		}\n\
		"};

	//*************************************************************************
	//* GLPainter
	//*************************************************************************
	GLPainter::GLPainter():
	UIPainter(),
	font(),
	fontRenderer(512,512),
	shape4(),
	shape7(),
	shape8(),
	nOverdraws(0)
	{

	}
	//--------------------------------------------------------------------------
	GLPainter::~GLPainter()
	{
	
	}
	//--------------------------------------------------------------------------
	void GLPainter::Initialize()
	{
		shape4.Initialize(4);
		shape7.Initialize(7);
		shape8.Initialize(8);
		quad.Initialize();
		font.Load<Arial12>();
		
		commonWidget.Build(cWidgetVSSource,cWidgetFSSource);

		commonWidget.quadVar.Add(quad.Vertices, commonWidget.program["Position"].location,4,GL_FLOAT);
		commonWidget.quadVar.Add(quad.TexCoords,commonWidget.program["TexCoord"].location,3,GL_FLOAT);

		commonWidget.shape4Var.Add(shape4.Vertices, commonWidget.program["Position"].location,4,GL_FLOAT);
		commonWidget.shape4Var.Add(shape4.TexCoords,commonWidget.program["TexCoord"].location,3,GL_FLOAT);

		commonWidget.shape7Var.Add(shape7.Vertices, commonWidget.program["Position"].location,4,GL_FLOAT);
		commonWidget.shape7Var.Add(shape7.TexCoords,commonWidget.program["TexCoord"].location,3,GL_FLOAT);

		commonWidget.shape8Var.Add(shape8.Vertices, commonWidget.program["Position"].location,4,GL_FLOAT);
		commonWidget.shape8Var.Add(shape8.TexCoords,commonWidget.program["TexCoord"].location,3,GL_FLOAT);
	}
	//--------------------------------------------------------------------------
	int getWidgetMargin() 
	{
		return 3 ; //2  ;
	}
	//--------------------------------------------------------------------------
	int getWidgetSpace() 
	{
		return 2 ;
	}
	//--------------------------------------------------------------------------
	int getAutoWidth() 
	{
		return 100 ;
	}
	//--------------------------------------------------------------------------
	int getAutoHeight() 
	{
		return 12+4;
	}
	//--------------------------------------------------------------------------
	int GLPainter::getCanvasMargin() const
	{
	//    return 5;
		return 5;
	}
	//--------------------------------------------------------------------------
	int GLPainter::getCanvasSpace() const
	{
		return 5;
	}
	//--------------------------------------------------------------------------
	int GLPainter::getFontHeight() const
	{
		return 12+4 ;
	}
	//--------------------------------------------------------------------------
	int GLPainter::getTextLineWidth(const char * _text) const
	{
		int w = 0;
		while(*_text != '\0' && *_text != '\n')
		{
		    w += font.CharWidth(*_text);
		    _text++;
		}
		w += 2;
		return w;
	}
	//--------------------------------------------------------------------------
	int GLPainter::getTextSize(const char * _text, int& _nbLines) const
	{
		int w = 0;
		int wLine = 0;
		_nbLines = 1;
		while(*_text != '\0')
		{
		    if (*_text != '\n')
		    {
		        wLine += font.CharWidth(*_text);;
		    }
		    else
		    {
		        _nbLines++;
				w = std::max<int>(w, wLine);
		        wLine = 0;
		    }

		    _text++;
		}
		w = std::max<int>(w, wLine) + 2;

		return w;
	}
	//--------------------------------------------------------------------------
	int GLPainter::getTextLineWidthAt(const char * _text, int _charNb) const
	{
		int w = 1;
		for (int i = 0; i < _charNb; i++)
		{
		    if (*_text != '\0' && *_text != '\n')
		    {
		        w += font.CharWidth(*_text);
		    }
		    else
		    {
		        return w+1;
		    }
		    _text++;
		}

		return w;
	 }
	//--------------------------------------------------------------------------
	int GLPainter::getPickedCharNb(const char * _text, const Point& _at) const
	{

		const char * textstart = _text;
		
		int w = 1;
		if ( _at.x < w ) return 0;

		while(*_text != '\0' && *_text != '\n')
		{
		    w += font.CharWidth(*_text);
		    
		    if ( _at.x < w ) return (_text - textstart); 

		    _text++;
		}

		return (_text - textstart);
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawFrame(const Rect & _r, int _margin, int /*_style*/)
	{

	  	//drawRoundedRect( _rect, _corner , lColorNb, cOutline );
		int lColorNb = cBase;// + (_isFocus << 2);
		drawRoundedRect(Rect(_r.x - _margin, _r.y - _margin, _r.w + 2*_margin, _r.h + 2*_margin), Point(_margin, _margin), lColorNb, cOutline);

		drawRoundedRectOutline(Rect(_r.x - _margin, _r.y - _margin, _r.w + 2*_margin, _r.h + 2*_margin), Point(_margin, _margin), cOutline);
	}
	//--------------------------------------------------------------------------
	Rect GLPainter::getLabelRect(const Rect & _r, const char * _text, Rect & _rt, int& _nbLines ) const
	{
		Rect rect = _r;
		_rt.x = getWidgetMargin();
		_rt.y = getWidgetMargin();

		// Eval Nblines and max line _width anyway.
		_rt.w = getTextSize(_text, _nbLines);

		if (rect.w == 0)
		{
		    rect.w = _rt.w + 2*_rt.x;
		}
		else
		{
		    _rt.w = rect.w - 2*_rt.x;
		}
		if (rect.h == 0)
		{
		    _rt.h = getFontHeight()*_nbLines;
		    rect.h = _rt.h + 2*_rt.y;
		}
		else
		{
		    _rt.h = rect.h - 2*_rt.y;
		}
		return rect;
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawLabel(const Rect & _r, const char * _text, const Rect & _rt, const int& _nbLines, bool /*_isHover*/, int _style)
	{
		if (_style > 0 )
		    drawFrame( _r, Point( _rt.x, _rt.y ), false, false, false );
		drawText( Rect(_r.x+_rt.x, _r.y+_rt.y, _rt.w, _rt.h), _text, _nbLines);
	}
	//--------------------------------------------------------------------------
	Rect GLPainter::getLineEditRect(const Rect & _r, const char * _text, Rect & _rt) const
	{
		Rect rect = _r;
		_rt.x = getWidgetMargin();
		_rt.y = getWidgetMargin();


		if (rect.w == 0)
		{
		    _rt.w = std::max<int>(getTextLineWidth(_text), 100);
		    rect.w = _rt.w + 2*_rt.x;
		}
		else
		{
		    _rt.w = rect.w - 2*_rt.x;
		}
		if (rect.h == 0)
		{
		    _rt.h = getFontHeight();
		    rect.h = _rt.h + 2*_rt.y;
		}
		else
		{
		    _rt.h = rect.h - 2*_rt.y;
		}
		return rect;
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawLineEdit(const Rect & _r, const char * _text, const Rect & _rt, int _caretPos, bool _isSelected, bool /*_isHover*/, int /*_style*/)
	{
		drawFrame( _r, Point( _rt.x, _rt.y ), true, _isSelected, false );
		drawText( Rect(_r.x+_rt.x, _r.y+_rt.y, _rt.w, _rt.h), _text, 1, _caretPos);
	}
	//--------------------------------------------------------------------------
	Rect GLPainter::getButtonRect(const Rect & _r, const char * _text, Rect & _rt) const
	{
		Rect rect = _r;
		_rt.x = /*4* */getWidgetMargin();
		_rt.y = /*4* */getWidgetMargin();
		
		if (rect.w == 0)
		{
		    _rt.w = getTextLineWidth(_text);
		    rect.w = _rt.w + 2*_rt.x;
		}
		else
		{
		    _rt.w = rect.w - 2*_rt.x;
		}
		if (rect.h == 0)
		{
		    _rt.h = getFontHeight();
		    rect.h = _rt.h + 2*_rt.y;
		}
		else
		{
		    _rt.h = rect.h - 2*_rt.y;
		}
		return rect;
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawButton(const Rect & _r, const char * _text, const Rect & _rt, bool _isDown, bool _isHover, bool _isFocus, int /*_style*/)
	{
		drawFrame( _r, Point( _rt.x, _rt.y ), _isHover, _isDown, _isFocus);
		drawText( Rect(_r.x+_rt.x, _r.y+_rt.y, _rt.w, _rt.h) , _text);
	}
	//--------------------------------------------------------------------------
	Rect GLPainter::getCheckRect(const Rect & _r, const char * _text, Rect & _rt, Rect& _rc) const
	{
		Rect rect = _r;

		int rcOffset = (int) (0.125*getAutoHeight());
		_rc.h = getAutoHeight() - 2*rcOffset;
		_rc.w = _rc.h;
		
		_rc.x = getWidgetMargin() + rcOffset;
		_rc.y = getWidgetMargin() + rcOffset;
		    
		_rt.x = getAutoHeight() + 2*getWidgetMargin();
		_rt.y = getWidgetMargin();


		if (rect.w == 0)
		{
		    _rt.w = getTextLineWidth(_text);
		    rect.w = _rt.x + _rt.w + getWidgetMargin();
		}

		if (rect.h == 0)
		{
		    _rt.h = getFontHeight();
		    rect.h = _rt.h + 2*_rt.y;
		}

		return rect;
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawCheckButton(const Rect & _r, const char * _text, const Rect & _rt, const Rect& rc, bool isChecked, bool _isHover, bool _isFocus, int _style)
	{
		if (_style) drawFrame( _r, Point( _rt.y, _rt.y ), _isHover, false, _isFocus );
		drawBoolFrame( Rect(_r.x+rc.x, _r.y+rc.y, rc.w, rc.h), Point( rc.w/6, rc.h/6 ), _isHover, isChecked, false );
		drawText( Rect(_r.x+_rt.x, _r.y+_rt.y, _rt.w, _rt.h) , _text);
	}
	//--------------------------------------------------------------------------
	Rect GLPainter::getRadioRect(const Rect & _r, const char * _text, Rect & _rt, Rect& _rr) const
	{
		Rect rect = _r;
		    
		int rrOffset = (int) (0.125*getAutoHeight());
		_rr.h = getAutoHeight() - 2*rrOffset;
		_rr.w = _rr.h;
		
		_rr.x = getWidgetMargin() + rrOffset;
		_rr.y = getWidgetMargin() + rrOffset;

		_rt.x = getAutoHeight() + 2*getWidgetMargin();
		_rt.y = getWidgetMargin();

		if (rect.w == 0)
		{
		    _rt.w   = getTextLineWidth(_text);
		    rect.w = _rt.w + _rt.x + getWidgetMargin();
		}

		if (rect.h == 0)
		{
		    _rt.h   = getFontHeight();
		    rect.h = _rt.h + 2*_rt.y;
		}

		return rect;
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawRadioButton(const Rect & _r, const char * _text, const Rect & _rt, const Rect & _rr, bool _isOn, bool _isHover, bool _isFocus, int _style)
	{
		if (_style) drawFrame( _r, Point( _rt.y, _rt.y ), _isHover, false, _isFocus );
		drawBoolFrame( Rect(_r.x+_rr.x, _r.y+_rr.y, _rr.w, _rr.h), Point( _rr.w/2, _rr.h/2 ), _isHover, _isOn, false );
		drawText( Rect(_r.x+_rt.x, _r.y+_rt.y, _rt.w, _rt.h) , _text);
	}
	//--------------------------------------------------------------------------
	Rect GLPainter::getHorizontalSliderRect(const Rect & _r, Rect& rs, float v, Rect& rc) const
	{
		Rect rect = _r;

		if (rect.w == 0)
		    rect.w = getAutoWidth() + 2 * getWidgetMargin();

		if (rect.h == 0)
		    rect.h = getAutoHeight() + 2 * getWidgetMargin();
		
		// Eval the sliding & cursor rect
		rs.y = getWidgetMargin();
		rs.h = rect.h - 2 * rs.y;
		
		rc.y = rs.y;
		rc.h = rs.h;
		
		rs.x = 0;//getWidgetMargin(); 
		rc.w = rc.h;
		rs.w = rect.w - 2 * rs.x - rc.w;
		rc.x = int(v * rs.w);

		return rect;
	}
	//-------------------------------------------------------------------------
	void GLPainter::drawHorizontalSlider(const Rect & _r, const Rect& rs, float /*v*/, const Rect& rc, bool _isHover, int /*_style*/)
	{
		int sliderHeight = rs.h/3;
		drawFrame( Rect(_r.x + rs.x, _r.y + rs.y + sliderHeight, _r.w - 2*rs.x, sliderHeight), Point(sliderHeight/2, sliderHeight/2), _isHover, false, false );
		drawFrame( Rect(_r.x + rs.x + rc.x, _r.y + rc.y, rc.w, rc.h), Point(rc.w/2, rc.h/2), _isHover, true, false );
	}
	//-------------------------------------------------------------------------
    void GLPainter::drawDiscretSlider(const Rect & _r, const Rect& _rs, int _index, int _nIndices, const Rect& _rc, bool _isHover, int /*_style*/)
	{
		int sliderHeight = _rs.h/3;
		drawFrame( Rect(_r.x + _rs.x, _r.y + _rs.y + sliderHeight, _r.w - 2*_rs.x, sliderHeight), Point(sliderHeight/2, sliderHeight/2), _isHover, false, false );

		int maxValue = _nIndices==1?1:_nIndices-1;
		Rect localRC = _rc;		
		localRC.x 	 = 0;
		for(int i=0; i<_nIndices; ++i)
		{				
			localRC.x = floor(i/(float)(maxValue) * (_r.w - _rc.w));

			drawFrame( Rect(_r.x + _rs.x + localRC.x,
							_r.y + localRC.y,
							localRC.w,
							localRC.h),
					   Point(_rc.w/2, _rc.h/2), 
					   false/*_isHover*/,
					   (_index == i),
					   false );
			//drawFrame( Rect(_r.x + rs.x + rc.x, _r.y + rc.y, rc.w, rc.h), Point(rc.w/2, rc.h/2), _isHover, true, false);
		}

		//drawFrame( const Rect& _rect, const Point& _corner, bool _isHover, bool _isOn, bool /*_isFocus*/ );
	}
	//-------------------------------------------------------------------------
	Rect GLPainter::getItemRect(const Rect & _r, const char * _text, Rect & _rt) const
	{
		Rect rect = _r;
		_rt.x = 0;
		_rt.y = 0;


		if (rect.w == 0)
		{
		    _rt.w = getTextLineWidth(_text);
		    rect.w = _rt.w + 2*_rt.x;
		}
		else
		{
		    _rt.w = rect.w - 2*_rt.x;
		}
		if (rect.h == 0)
		{
		    _rt.h = getFontHeight();
		    rect.h = _rt.h + 2*_rt.y;
		}
		else
		{
		    _rt.h = rect.h - 2*_rt.y;
		}
		return rect;
	}
	//--------------------------------------------------------------------------
	Rect GLPainter::getListRect(const Rect & _r, int _numOptions, const char * __options[], Rect& _ri, Rect & _rt) const
	{
		Rect rect = _r;
		_ri.x = getWidgetMargin();
		_ri.y = getWidgetMargin();
		_rt.x = getWidgetMargin();
		_rt.y = getWidgetMargin();

		if (rect.w == 0)
		{
		    _rt.w = 0;
		    for (int i = 0; i < _numOptions; i++)
		    {
		        int l = getTextLineWidth(__options[i]);
		        _rt.w = ( l > _rt.w ? l : _rt.w ); 
		    }
		    _ri.w  = _rt.w + 2*_rt.x;
		    rect.w = _ri.w + 2*_ri.x;
		}
		else
		{
		    _ri.w = rect.w - 2*_ri.x;
		    _rt.w = _ri.w - 2*_rt.x;
		}
		if (rect.h == 0)
		{
		    _rt.h = getFontHeight();
		    _ri.h = _rt.h + _rt.y;
		    rect.h = _numOptions * _ri.h + 2*_ri.y;
		}
		else
		{
		    _ri.h = (rect.h - 2*_ri.y) / _numOptions;
		    _rt.h = _ri.h - _rt.y;
		}
		return rect;
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawListItem(const Rect & _r, const char * _text, const Rect & _rt, bool _isSelected, bool _isHover, int /*_style*/)
	{
	//    drawFrame( _r, Point(0, 0), _isHover, _isSelected, false );    
		drawText( Rect(_r.x+_rt.x, _r.y+_rt.y, _rt.w, _rt.h), _text, _isHover, _isSelected);
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawListBox(const Rect & _r, int _numOptions, const char ** _options, const Rect& _ri, const Rect & _rt, int _selected, int hovered, int /*_style*/)
	{
		drawFrame( _r, Point(_ri.x, _ri.y) );

		Rect ir( _r.x + _ri.x, _r.y + _r.h - _ri.y - _ri.h, _ri.w, _ri.h );  
		for ( int i = 0; i < _numOptions; i++ )
		{    
		    if ( (i == hovered) || (i == _selected))
		    {
		        drawFrame( ir, Point(_ri.x, _ri.y), false, (i == _selected));
		    }

		    drawText( Rect(ir.x + _rt.x , ir.y + _rt.y, _rt.w, _rt.h), _options[i]);    

		    ir.y -= ir.h;
		}
	}
	//--------------------------------------------------------------------------
	Rect GLPainter::getComboRect(const Rect & _r, int _numOptions, const char * _options[], int /*_selected*/, Rect& _rt, Rect& _rd) const
	{
		Rect rect = _r;
		_rt.x = getWidgetMargin();
		_rt.y = getWidgetMargin();
		
		if (rect.h == 0)
		{
		    _rt.h = getFontHeight();
		    rect.h = _rt.h + 2*_rt.y;
		}
		else
		{
		    _rt.h = rect.h - 2*_rt.y;
		}

		_rd.h = _rt.h;
		_rd.w = _rd.h;     
		_rd.y = _rt.y;

		if (rect.w == 0)
		{
		    _rt.w = 0;
		    for (int i = 0; i < _numOptions; i++)
		    {
		        int l = getTextLineWidth(_options[i]);
		        _rt.w = ( l > _rt.w ? l : _rt.w ); 
		    }
		    rect.w = _rt.w + 2*_rt.x;

		    //Add room for drop down button
		    rect.w += _rt.h + _rt.x;
		}
		else
		{
		    //Add room for drop down button
		    _rt.w = rect.w - 3*_rt.x - _rt.h;
		}
		_rd.x = 2*_rt.x + _rt.w;

		return rect;
	}
	//--------------------------------------------------------------------------
	Rect GLPainter::getComboOptionsRect(const Rect & _rCombo, int _numOptions, const char * _options[], Rect& _ri, Rect & _rit) const
	{
		// the _options frame is like a list box
		Rect rect = getListRect( Rect(), _numOptions, _options, _ri, _rit);
		
		// offset by the Combo box pos itself
		rect.x = _rCombo.x;
		rect.y = _rCombo.y - rect.h;

		return rect;
	}
	//--------------------------------------------------------------------------
    void GLPainter::drawComboBox(const Rect & _r, const char * _option, const Rect & _rt, const Rect & _rd, bool _isHover, bool _isFocus, int /*_style*/)
	{
		drawFrame( _r, Point(_rt.x, _rt.y), _isHover, false, _isFocus );
		drawText( Rect(_r.x+_rt.x, _r.y+_rt.y, _rt.w, _rt.h), _option );

		drawDownArrow( Rect(_r.x+_rd.x, _r.y+_rd.y, _rd.w, _rd.h), int(_rd.h * 0.15f), cBase + (!_isHover) + (_isFocus << 2), cOutline);
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawComboOptions(const Rect & _r, int _numOptions, const char** _options, const Rect& _ri, const Rect & _rt, int _selected, int _hovered, bool _isHover, bool /*_isFocus*/, int _style)
	{
		drawListBox(_r, _numOptions, _options, _ri, _rt, _selected, _hovered, _style);

		/*
		unsigned int nOptions = _numOptions;
		//Assert(nOverdraws==MaxOverdraw);
		Assert(nOptions<=MaxOptions);

		//Overdraw& over 	= overdrawQueue[nOverdraws++];
		overdraw.r 			= _r;
		overdraw.numOptions = _numOptions;
		overdraw.ri 		= _ri;
		overdraw.rt 		= _rt;
		overdraw.selected 	= _selected;
		overdraw.hovered	= _hovered;
		overdraw.isHover 	= _isHover;
		overdraw.isFocus 	= _isFocus;
		overdraw.style 		= _style;


		for(unsigned int i=0; i<nOptions; ++i)
		{
			Assert(strlen(_options[i])<MaxOptionSize-1);
			strcpy(overdraw.options[i],_options[i]);
		}
		
		++nOverdraws;*/
	}
	//--------------------------------------------------------------------------
	Rect GLPainter::getPanelRect(const Rect & _r, const char * _text, Rect& _rt, Rect& _ra) const
	{
		Rect rect = _r;
		_rt.x = getWidgetMargin();
		_rt.y = getWidgetMargin();
		
		if (rect.h == 0)
		{
		    _rt.h = getFontHeight();
		    rect.h = _rt.h + 2 * _rt.y;
		}
		else
		{
		    _rt.h = rect.h - 2 * _rt.y;
		}   

		_ra.h = _rt.h;
		_ra.w = _ra.h;     
		_ra.y = _rt.y;

		if (rect.w == 0)
		{
		    _rt.w = getTextLineWidth(_text);
		    rect.w = _rt.w + 2 * _rt.x;
		    
		    // Add room for drop down button
		    rect.w += _ra.h + _rt.x;
		}
		else
		{
		    // Add room for drop down button
		    _rt.w = rect.w - 3 * _rt.x - _ra.h;
		}
		_ra.x = 2 * _rt.x + _rt.w;

		return rect;

	}
	//--------------------------------------------------------------------------
	void GLPainter::drawPanel(const Rect & _r, const char * _text, const Rect & _rt, const Rect & _ra, bool _isUnfold, bool _isHover, bool _isFocus, int /*_style*/)
	{
		drawFrame( _r, Point(_rt.x, _rt.y), _isHover, false, _isFocus );
		drawText( Rect(_r.x+_rt.x, _r.y+_rt.y, _rt.w, _rt.h), _text );
		if (_isUnfold)
		    drawMinus( Rect(_r.x+_ra.x, _r.y+_ra.y, _ra.w, _ra.h), int(_ra.h * 0.15f), cBase + (!_isHover) + (_isFocus << 2), cOutline);
		else
		    drawPlus( Rect(_r.x+_ra.x, _r.y+_ra.y, _ra.w, _ra.h), int(_ra.h * 0.15f), cBase + (!_isHover) + (_isFocus << 2), cOutline);
	}
	//--------------------------------------------------------------------------
	void GLPainter::init()
	{

	}
	//--------------------------------------------------------------------------
	void GLPainter::begin(const Rect& _window)
	{
		//Tools::Logger::Info("draw");
		view 		= glm::mat4(1.f);
		projection 	= glm::ortho(float(_window.x), float(_window.w), float(_window.y), float(_window.h));

		glProgramUniformMatrix4fv(commonWidget.program.id, commonWidget.projectionVar, 	1, GL_FALSE, &projection[0][0]);
		glProgramUniformMatrix4fv(commonWidget.program.id, commonWidget.viewVar, 		1, GL_FALSE, &view[0][0]);
	}
	//--------------------------------------------------------------------------
	void GLPainter::beginDraw()
	{
		//Tools::Logger::Info("beginDraw");
	}
	//--------------------------------------------------------------------------
	void GLPainter::end()
	{
		// Draw over draw (Combo box options)
		//if(nOverdraws>0)
		//{
		//    drawListBox(overdraw.r, overdraw.numOptions, const_cast<const char**>(overdraw.options), overdraw.ri, overdraw.rt, overdraw.selected, overdraw.hovered, overdraw.style);
		//	--nOverdraws;
		//}

		//Tools::Logger::Info("end");
	}
	//--------------------------------------------------------------------------
	// Draw Primitive shapes
	//void GLPainter::drawString( int _x, int _y, const char * _text, int _nbLines )
	//{
	//	Tools::Logger::Info("drawString");
	//	//glm::vec4 color(1.f,1.f,1.f,1.f);
	//	//fontRender.DrawString(x,y,font,_text,color);
	//}
	//--------------------------------------------------------------------------
	void GLPainter::drawRect( const Rect & _rect, int _fillColorId, int _borderColorId ) const
	{
		//Tools::Logger::Info("drawRect");

		glm::vec2 zones(0.f,0.f);

		float x0 = _rect.x;
		float x1 = _rect.x + _rect.w;
		float y0 = _rect.y;
		float y1 = _rect.y + _rect.h;

		glm::vec4* v = shape4.Vertices.Lock();
		v[0] = glm::vec4(x0, y0, 0.f, 1.f);
		v[1] = glm::vec4(x1, y0, 0.f, 1.f);
		v[2] = glm::vec4(x0, y1, 0.f, 1.f);
		v[3] = glm::vec4(x1, y1, 0.f, 1.f);
		shape4.Vertices.Unlock();

		glm::vec3* t = shape4.TexCoords.Lock();
		t[0] = glm::vec3(0,0,0);
		t[1] = glm::vec3(0,0,0);
		t[2] = glm::vec3(0,0,0);
		t[3] = glm::vec3(0,0,0);
		shape4.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.fillColorVar, 	1, &s_colors[_fillColorId][0]);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.borderColorVar, 	1, &s_colors[_borderColorId][0]);
		glProgramUniform2fv(commonWidget.program.id, commonWidget.zonesVar, 		1, &zones[0]);
		commonWidget.shape4Var.Draw(GL_TRIANGLE_STRIP,shape4.Indices);
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawRoundedRect( const Rect& _rect, const Point& _corner, int _fillColorId, int _borderColorId ) const
	{
		//Tools::Logger::Info("drawRoundedRect");
		//Tools::Logger::Info("Rect  x:%f y:%f w:%f h:%f",_rect.x,_rect.y,_rect.w,_rect.h);
		//Tools::Logger::Info("Point x:%f y:%f",_corner.x,_corner.y);

		glm::vec2 zones(_corner.x - 1, _corner.x - 2);

		float xb = _corner.x;
		float yb = _corner.y;

		float x0 = _rect.x;
		float x1 = _rect.x + _corner.x;
		float x2 = _rect.x + _rect.w - _corner.x;
		float x3 = _rect.x + _rect.w;
		
		float y0 = _rect.y;
		float y1 = _rect.y + _corner.y;
		float y2 = _rect.y + _rect.h - _corner.y;
		float y3 = _rect.y + _rect.h;

		glm::vec4* v;
		glm::vec3* t;

		v 	 = shape8.Vertices.Lock();
		v[0] = glm::vec4(x0, y0, 0.f, 1.f);
		v[1] = glm::vec4(x1, y0, 0.f, 1.f);
		v[2] = glm::vec4(x0, y1, 0.f, 1.f);
		v[3] = glm::vec4(x1, y1, 0.f, 1.f);
		v[4] = glm::vec4(x0, y2, 0.f, 1.f);
		v[5] = glm::vec4(x1, y2, 0.f, 1.f);
		v[6] = glm::vec4(x0, y3, 0.f, 1.f);
		v[7] = glm::vec4(x1, y3, 0.f, 1.f);
		shape8.Vertices.Unlock();

		t	 = shape8.TexCoords.Lock();
		t[0] = glm::vec3(xb, yb,  0);
		t[1] = glm::vec3( 0, yb, 0);
		t[2] = glm::vec3(xb,  0, 0);
		t[3] = glm::vec3( 0,  0, 0);
		t[4] = glm::vec3(xb,  0, 0);
		t[5] = glm::vec3( 0,  0, 0);
		t[6] = glm::vec3(xb, yb, 0);
		t[7] = glm::vec3( 0, yb, 0);
		shape8.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.fillColorVar, 	1, &s_colors[_fillColorId][0]);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.borderColorVar, 	1, &s_colors[_borderColorId][0]);
		glProgramUniform2fv(commonWidget.program.id, commonWidget.zonesVar, 		1, &zones[0]);
		commonWidget.shape8Var.Draw(GL_TRIANGLE_STRIP,shape8.Indices);

		v 	 = shape8.Vertices.Lock();
		v[0] = glm::vec4(x2, y0, 0.f, 1.f);
		v[1] = glm::vec4(x3, y0, 0.f, 1.f);
		v[2] = glm::vec4(x2, y1, 0.f, 1.f);
		v[3] = glm::vec4(x3, y1, 0.f, 1.f);
		v[4] = glm::vec4(x2, y2, 0.f, 1.f);
		v[5] = glm::vec4(x3, y2, 0.f, 1.f);
		v[6] = glm::vec4(x2, y3, 0.f, 1.f);
		v[7] = glm::vec4(x3, y3, 0.f, 1.f);
		shape8.Vertices.Unlock();

		t	 = shape8.TexCoords.Lock();
		t[0] = glm::vec3( 0, yb, 0);
		t[1] = glm::vec3(xb, yb, 0);
		t[2] = glm::vec3( 0,  0, 0);
		t[3] = glm::vec3(xb,  0, 0);
		t[4] = glm::vec3( 0,  0, 0);
		t[5] = glm::vec3(xb,  0, 0);
		t[6] = glm::vec3( 0, yb, 0);
		t[7] = glm::vec3(xb, yb, 0);
		shape8.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		commonWidget.shape8Var.Draw(GL_TRIANGLE_STRIP,shape8.Indices);

		v 	 = shape8.Vertices.Lock();
		v[0] = glm::vec4(x1, y0, 0.f, 1.f);
		v[1] = glm::vec4(x2, y0, 0.f, 1.f);
		v[2] = glm::vec4(x1, y1, 0.f, 1.f);
		v[3] = glm::vec4(x2, y1, 0.f, 1.f);
		v[4] = glm::vec4(x1, y2, 0.f, 1.f);
		v[5] = glm::vec4(x2, y2, 0.f, 1.f);
		v[6] = glm::vec4(x1, y3, 0.f, 1.f);
		v[7] = glm::vec4(x2, y3, 0.f, 1.f);
		shape8.Vertices.Unlock();

		t	 = shape8.TexCoords.Lock();
		t[0] = glm::vec3( 0, yb, 0);
		t[1] = glm::vec3( 0, yb, 0);
		t[2] = glm::vec3( 0,  0, 0);
		t[3] = glm::vec3( 0,  0, 0);
		t[4] = glm::vec3( 0,  0, 0);
		t[5] = glm::vec3( 0,  0, 0);
		t[6] = glm::vec3( 0, yb, 0);
		t[7] = glm::vec3( 0, yb, 0);
		shape8.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		commonWidget.shape8Var.Draw(GL_TRIANGLE_STRIP,shape8.Indices);
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawRoundedRectOutline( const Rect& _rect, const Point& _corner, int _borderColorId ) const
	{
		//Tools::Logger::Info("RoundedRectOutline");

		glm::vec2 zones(_corner.x - 1, _corner.x - 2);

		float xb = _corner.x;
		float yb = _corner.y;

		float x0 = _rect.x;
		float x1 = _rect.x + _corner.x;
		float x2 = _rect.x + _rect.w - _corner.x;
		float x3 = _rect.x + _rect.w;
		
		float y0 = _rect.y;
		float y1 = _rect.y + _corner.y;
		float y2 = _rect.y + _rect.h - _corner.y;
		float y3 = _rect.y + _rect.h;

		glm::vec4* v;
		glm::vec3* t;

		v 	 = shape8.Vertices.Lock();
		v[0] = glm::vec4(x0, y0, 0.f, 1.f);
		v[1] = glm::vec4(x1, y0, 0.f, 1.f);
		v[2] = glm::vec4(x0, y1, 0.f, 1.f);
		v[3] = glm::vec4(x1, y1, 0.f, 1.f);
		v[4] = glm::vec4(x0, y2, 0.f, 1.f);
		v[5] = glm::vec4(x1, y2, 0.f, 1.f);
		v[6] = glm::vec4(x0, y3, 0.f, 1.f);
		v[7] = glm::vec4(x1, y3, 0.f, 1.f);
		shape8.Vertices.Unlock();

		t	 = shape8.TexCoords.Lock();
		t[0] = glm::vec3(xb, yb,  0);
		t[1] = glm::vec3( 0, yb, 0);
		t[2] = glm::vec3(xb,  0, 0);
		t[3] = glm::vec3( 0,  0, 0);
		t[4] = glm::vec3(xb,  0, 0);
		t[5] = glm::vec3( 0,  0, 0);
		t[6] = glm::vec3(xb, yb, 0);
		t[7] = glm::vec3( 0, yb, 0);
		shape8.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.fillColorVar, 	1, &s_colors[cTranslucent][0]);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.borderColorVar, 	1, &s_colors[_borderColorId][0]);
		glProgramUniform2fv(commonWidget.program.id, commonWidget.zonesVar, 		1, &zones[0]);
		commonWidget.shape8Var.Draw(GL_TRIANGLE_STRIP,shape8.Indices);

		v 	 = shape8.Vertices.Lock();
		v[0] = glm::vec4(x2, y0, 0.f, 1.f);
		v[1] = glm::vec4(x3, y0, 0.f, 1.f);
		v[2] = glm::vec4(x2, y1, 0.f, 1.f);
		v[3] = glm::vec4(x3, y1, 0.f, 1.f);
		v[4] = glm::vec4(x2, y2, 0.f, 1.f);
		v[5] = glm::vec4(x3, y2, 0.f, 1.f);
		v[6] = glm::vec4(x2, y3, 0.f, 1.f);
		v[7] = glm::vec4(x3, y3, 0.f, 1.f);
		shape8.Vertices.Unlock();

		t	 = shape8.TexCoords.Lock();
		t[0] = glm::vec3( 0, yb, 0);
		t[1] = glm::vec3(xb, yb, 0);
		t[2] = glm::vec3( 0,  0, 0);
		t[3] = glm::vec3(xb,  0, 0);
		t[4] = glm::vec3( 0,  0, 0);
		t[5] = glm::vec3(xb,  0, 0);
		t[6] = glm::vec3( 0, yb, 0);
		t[7] = glm::vec3(xb, yb, 0);
		shape8.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		commonWidget.shape8Var.Draw(GL_TRIANGLE_STRIP,shape8.Indices);

		v = shape4.Vertices.Lock();
		v[0] = glm::vec4(x1, y0, 0.f, 1.f);
		v[1] = glm::vec4(x2, y0, 0.f, 1.f);
		v[2] = glm::vec4(x1, y1, 0.f, 1.f);
		v[3] = glm::vec4(x2, y1, 0.f, 1.f);
		shape4.Vertices.Unlock();

		t = shape4.TexCoords.Lock();
		t[0] = glm::vec3( 0, yb, 0);
		t[1] = glm::vec3( 0, yb, 0);
		t[2] = glm::vec3( 0,  0, 0);
		t[3] = glm::vec3( 0,  0, 0);
		shape4.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		commonWidget.shape4Var.Draw(GL_TRIANGLE_STRIP,shape4.Indices);

		v 	 = shape4.Vertices.Lock();
		v[0] = glm::vec4(x1, y2, 0.f, 1.f);
		v[1] = glm::vec4(x2, y2, 0.f, 1.f);
		v[2] = glm::vec4(x1, y3, 0.f, 1.f);
		v[3] = glm::vec4(x2, y3, 0.f, 1.f);
		shape4.Vertices.Unlock();

		t 	 = shape4.TexCoords.Lock();
		t[0] = glm::vec3( 0,  0, 0);
		t[1] = glm::vec3( 0,  0, 0);
		t[2] = glm::vec3( 0, yb, 0);
		t[3] = glm::vec3( 0, yb, 0);
		shape4.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		commonWidget.shape4Var.Draw(GL_TRIANGLE_STRIP,shape4.Indices);
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawCircle( const Rect& _rect, int _fillColorId, int _borderColorId ) const
	{
		//Tools::Logger::Info("Circle");

		glm::vec2 zones((_rect.w / 2) - 1, (_rect.w / 2) - 2);

		float xb = _rect.w / 2;
		float yb = _rect.w / 2;
		float x0 = _rect.x;
		float x1 = _rect.x + _rect.w;
		float y0 = _rect.y;
		float y1 = _rect.y + _rect.h;

		glm::vec4* v = shape4.Vertices.Lock();
		v[0] = glm::vec4(x0, y0, 0.f, 1.f);
		v[1] = glm::vec4(x1, y0, 0.f, 1.f);
		v[2] = glm::vec4(x0, y1, 0.f, 1.f);
		v[3] = glm::vec4(x1, y1, 0.f, 1.f);
		shape4.Vertices.Unlock();

		glm::vec3* t = shape4.TexCoords.Lock();
		t[0] = glm::vec3(-xb,-yb,0);
		t[1] = glm::vec3( xb,-yb,0);
		t[2] = glm::vec3(-xb, yb,0);
		t[3] = glm::vec3( xb, yb,0);
		shape4.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.fillColorVar, 	1, &s_colors[_fillColorId][0]);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.borderColorVar, 	1, &s_colors[_borderColorId][0]);
		glProgramUniform2fv(commonWidget.program.id, commonWidget.zonesVar, 		1, &zones[0]);
		commonWidget.shape4Var.Draw(GL_TRIANGLE_STRIP,shape4.Indices);
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawMinus( const Rect& _rect, int _width, int _fillColorId, int _borderColorId ) const
	{
		//Tools::Logger::Info("Minus");

		float xb = _width;
		float yb = _width;
		float xoff = xb ;
		float yoff = yb ;
		float x0 = _rect.x + _rect.w * 0.1 ;
		float x1 = _rect.x + _rect.w * 0.9;
		float y1 = _rect.y + _rect.h * 0.5;

		glm::vec2 zones((xb) - 1, (xb) - 2);

		glm::vec4* v;
		glm::vec3* t;

		v 	 = shape8.Vertices.Lock();
		v[0] = glm::vec4(x0, 		y1+yoff, 0.f, 1.f);
		v[1] = glm::vec4(x0, 		y1-yoff, 0.f, 1.f);
		v[2] = glm::vec4(x0+xoff,  y1+yoff, 0.f, 1.f);
		v[3] = glm::vec4(x0+xoff,  y1-yoff, 0.f, 1.f);
		v[4] = glm::vec4(x1-xoff,  y1+yoff, 0.f, 1.f);
		v[5] = glm::vec4(x1-xoff,  y1-yoff, 0.f, 1.f);
		v[6] = glm::vec4(x1, 		y1+yoff, 0.f, 1.f);
		v[7] = glm::vec4(x1, 		y1-yoff, 0.f, 1.f);
		shape8.Vertices.Unlock();

		t	 = shape8.TexCoords.Lock();
		t[0] = glm::vec3(-xb,-yb, 0);
		t[1] = glm::vec3( xb,-yb, 0);
		t[2] = glm::vec3(-xb,  0, 0);
		t[3] = glm::vec3( xb,  0, 0);
		t[4] = glm::vec3(-xb,  0, 0);
		t[5] = glm::vec3( xb,  0, 0);
		t[6] = glm::vec3(-xb,-yb, 0);
		t[7] = glm::vec3( xb,-yb, 0);
		shape8.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.fillColorVar, 	1, &s_colors[_fillColorId][0]);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.borderColorVar, 	1, &s_colors[_borderColorId][0]);
		glProgramUniform2fv(commonWidget.program.id, commonWidget.zonesVar, 		1, &zones[0]);
		commonWidget.shape8Var.Draw(GL_TRIANGLE_STRIP,shape8.Indices);
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawPlus( const Rect& _rect, int _width, int _fillColorId, int _borderColorId ) const
	{
		//Tools::Logger::Info("Plus");

		float xb = _width;
		float yb = _width;
		float xoff = xb ;
		float yoff = yb ;
		float x0 = _rect.x + _rect.w * 0.1 ;
		float x1 = _rect.x + _rect.w * 0.5;
		float x2 = _rect.x + _rect.w * 0.9;
		float y0 = _rect.y + _rect.h * 0.1;
		float y1 = _rect.y + _rect.h * 0.5;
		float y2 = _rect.y + _rect.h * 0.9;

		glm::vec2 zones((xb) - 1, (xb) - 2);

		glm::vec4* v;
		glm::vec3* t;

/*		v 	 = shape7.Vertices.Lock();
		v[0] = glm::vec4(x0, 		y1+yoff, 0.f, 1.f);
		v[1] = glm::vec4(x0, 		y1-yoff, 0.f, 1.f);
		v[2] = glm::vec4(x0+xoff,  y1+yoff, 0.f, 1.f);
		v[3] = glm::vec4(x0+xoff,  y1-yoff, 0.f, 1.f);
		v[4] = glm::vec4(x1-xoff,  y1+yoff, 0.f, 1.f);
		v[5] = glm::vec4(x1-xoff,  y1-yoff, 0.f, 1.f);
		v[6] = glm::vec4(x1, 		y1, 0.f, 1.f);
		shape7.Vertices.Unlock();

		t	 = shape7.TexCoords.Lock();
		t[0] = glm::vec3(-xb,-yb,  0);
		t[1] = glm::vec3( xb,-yb, 0);
		t[2] = glm::vec3(-xb,  0, 0);
		t[3] = glm::vec3( xb,  0, 0);
		t[4] = glm::vec3(-xb,  0, 0);
		t[5] = glm::vec3( xb,  0, 0);
		t[6] = glm::vec3(  0, yb, 0);
		shape7.TexCoords.Unlock();

		Render::Input::SetEffect(commonWidget.program);
		Render::Input::SetVariable(commonWidget.fillColorVar);
		Render::Input::SetVariable(commonWidget.borderColorVar);
		Render::Input::SetVariable(commonWidget.zonesVar);
		Render::Input::DrawPrimitive(commonWidget.shape7Var);


		v 	 = shape7.Vertices.Lock();
		v[0] = glm::vec4(x1, 		y1, 	 0.f, 1.f);
		v[1] = glm::vec4(x1+xoff, 	y1+yoff, 0.f, 1.f);
		v[2] = glm::vec4(x1+xoff,  y1-yoff, 0.f, 1.f);
		v[3] = glm::vec4(x2-xoff,  y1+yoff, 0.f, 1.f);
		v[4] = glm::vec4(x2-xoff,  y1-yoff, 0.f, 1.f);
		v[5] = glm::vec4(x2, 		y1+yoff, 0.f, 1.f);
		v[6] = glm::vec4(x2, 		y1-yoff, 0.f, 1.f);
		shape7.Vertices.Unlock();

		t	 = shape7.TexCoords.Lock();
		t[0] = glm::vec3(  0, yb, 0);
		t[1] = glm::vec3(-xb,  0, 0);
		t[2] = glm::vec3( xb,  0, 0);
		t[3] = glm::vec3(-xb,  0, 0);
		t[4] = glm::vec3( xb,  0, 0);
		t[5] = glm::vec3(-xb,-yb, 0);
		t[6] = glm::vec3( xb,-yb, 0);
		shape7.TexCoords.Unlock();

		Render::Input::SetEffect(commonWidget.program);
		Render::Input::SetVariable(commonWidget.fillColorVar);
		Render::Input::SetVariable(commonWidget.borderColorVar);
		Render::Input::SetVariable(commonWidget.zonesVar);
		Render::Input::DrawPrimitive(commonWidget.shape7Var);

*/

		v 	 = shape8.Vertices.Lock();
		v[0] = glm::vec4(x0, 		y1+yoff, 0.f, 1.f);
		v[1] = glm::vec4(x0, 		y1-yoff, 0.f, 1.f);
		v[2] = glm::vec4(x0+xoff,  y1+yoff, 0.f, 1.f);
		v[3] = glm::vec4(x0+xoff,  y1-yoff, 0.f, 1.f);
		v[4] = glm::vec4(x2-xoff,  y1+yoff, 0.f, 1.f);
		v[5] = glm::vec4(x2-xoff, 	y1-yoff, 0.f, 1.f);
		v[6] = glm::vec4(x2, 		y1+yoff, 0.f, 1.f);
		v[7] = glm::vec4(x2, 		y1-yoff, 0.f, 1.f);
		shape8.Vertices.Unlock();

		t	 = shape8.TexCoords.Lock();
		t[0] = glm::vec3(-xb,-yb, 0);
		t[1] = glm::vec3( xb,-yb, 0);
		t[2] = glm::vec3(-xb,  0, 0);
		t[3] = glm::vec3( xb,  0, 0);
		t[4] = glm::vec3(-xb,  0, 0);
		t[5] = glm::vec3( xb,  0, 0);
		t[6] = glm::vec3(-xb,-yb, 0);
		t[7] = glm::vec3( xb,-yb, 0);
		shape8.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.fillColorVar, 	1, &s_colors[_fillColorId][0]);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.borderColorVar, 	1, &s_colors[_borderColorId][0]);
		glProgramUniform2fv(commonWidget.program.id, commonWidget.zonesVar, 		1, &zones[0]);
		commonWidget.shape8Var.Draw(GL_TRIANGLE_STRIP,shape8.Indices);

		v 	 = shape8.Vertices.Lock();
		v[0] = glm::vec4(x1+xoff, y0, 0.f, 1.f);
		v[1] = glm::vec4(x1-xoff, y0, 0.f, 1.f);
		v[2] = glm::vec4(x1+xoff, y0+yoff, 0.f, 1.f);
		v[3] = glm::vec4(x1-xoff, y0+yoff, 0.f, 1.f);
		v[4] = glm::vec4(x1+xoff, y2-yoff, 0.f, 1.f);
		v[5] = glm::vec4(x1-xoff, y2-yoff, 0.f, 1.f);
		v[6] = glm::vec4(x1+xoff, y2, 0.f, 1.f);
		v[7] = glm::vec4(x1-xoff, y2, 0.f, 1.f);
		shape8.Vertices.Unlock();

		t	 = shape8.TexCoords.Lock();
		t[0] = glm::vec3(-xb,-yb, 0);
		t[1] = glm::vec3( xb,-yb, 0);
		t[2] = glm::vec3(-xb,  0, 0);
		t[3] = glm::vec3( xb,  0, 0);
		t[4] = glm::vec3(-xb,  0, 0);
		t[5] = glm::vec3( xb,  0, 0);
		t[6] = glm::vec3(-xb,-yb, 0);
		t[7] = glm::vec3( xb,-yb, 0);
		shape8.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		commonWidget.shape8Var.Draw(GL_TRIANGLE_STRIP,shape8.Indices);
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawDownArrow( const Rect& _rect, int _width, int _fillColorId, int _borderColorId ) const
	{
		//Tools::Logger::Info("DowmArrow");

		float offset = sqrt(2.0)/2.0 ;	   
		float xb = _width;
		float yb = _width;		
		float xoff = offset * xb ;
		float yoff = offset * yb ;
		float xoff2 = offset * xb *2.0 ;
		float yoff2 = offset * yb *2.0;
		float x0 = _rect.x + xoff2;
		float x1 = _rect.x + _rect.w * 0.5;
		float x2 = _rect.x + _rect.w - xoff2;
		float y0 = _rect.y + _rect.h * 0.1 + yoff2;
		float y1 = _rect.y + _rect.h * 0.6;

		glm::vec2 zones((xb) - 1, (xb) - 2);

		glm::vec4* v;
		glm::vec3* t;

		v 	 = shape7.Vertices.Lock();
		v[0] = glm::vec4(x0, 		y1+yoff2, 	0.f, 1.f);
		v[1] = glm::vec4(x0-xoff2,	y1, 		0.f, 1.f);
		v[2] = glm::vec4(x0+xoff,  y1+yoff, 	0.f, 1.f);
		v[3] = glm::vec4(x0-xoff,  y1-yoff, 	0.f, 1.f);
		v[4] = glm::vec4(x1,  		y0+yoff2, 	0.f, 1.f);
		v[5] = glm::vec4(x1-xoff2,	y0, 		0.f, 1.f);
		v[6] = glm::vec4(x1, 		y0-yoff2, 	0.f, 1.f);
		shape7.Vertices.Unlock();

		t	 = shape7.TexCoords.Lock();
		t[0] = glm::vec3(-xb, -yb,  0);
		t[1] = glm::vec3( xb, -yb,  0);
		t[2] = glm::vec3(-xb,   0,  0);
		t[3] = glm::vec3( xb,   0,  0);
		t[4] = glm::vec3(-xb,   0, xb);
		t[5] = glm::vec3( xb,   0, xb);
		t[6] = glm::vec3( xb,2*yb, xb);
		shape7.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.fillColorVar, 	1, &s_colors[_fillColorId][0]);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.borderColorVar, 	1, &s_colors[_borderColorId][0]);
		glProgramUniform2fv(commonWidget.program.id, commonWidget.zonesVar, 		1, &zones[0]);
		commonWidget.shape7Var.Draw(GL_TRIANGLE_STRIP,shape7.Indices);


		v 	 = shape7.Vertices.Lock();
		v[0] = glm::vec4(x2+xoff2, y1, 		0.f, 1.f);
		v[1] = glm::vec4(x2,		y1+yoff2, 	0.f, 1.f);
		v[2] = glm::vec4(x2+xoff,  y1-yoff, 	0.f, 1.f);
		v[3] = glm::vec4(x2-xoff,  y1+yoff, 	0.f, 1.f);
		v[4] = glm::vec4(x1+xoff2, y0, 		0.f, 1.f);
		v[5] = glm::vec4(x1,		y0+yoff2, 	0.f, 1.f);
		v[6] = glm::vec4(x1, 		y0-yoff2, 	0.f, 1.f);
		shape7.Vertices.Unlock();

		t	 = shape7.TexCoords.Lock();
		t[0] = glm::vec3( xb, -yb,  0);
		t[1] = glm::vec3(-xb, -yb,  0);
		t[2] = glm::vec3( xb,   0, xb);
		t[3] = glm::vec3(-xb,   0, xb);
		t[4] = glm::vec3( xb,   0, xb);
		t[5] = glm::vec3(-xb,   0, xb);
		t[6] = glm::vec3( xb,2*yb, xb);
		shape7.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		commonWidget.shape7Var.Draw(GL_TRIANGLE_STRIP,shape7.Indices);
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawUpArrow( const Rect& _rect, int _width, int _fillColorId, int _borderColorId ) const
	{
		//Tools::Logger::Info("UpdArrow");

		float offset = sqrt(2.0)/2.0 ;   
		float xb = _width;
		float yb = _width;		
		float xoff = offset * xb ;
		float yoff = - offset * yb ;
		float xoff2 = offset * xb *2.0 ;
		float yoff2 = - offset * yb *2.0;
		float x0 = _rect.x + xoff2;
		float x1 = _rect.x + _rect.w * 0.5;
		float x2 = _rect.x + _rect.w - xoff2;
		float y0 = _rect.y + _rect.h * 0.9 + yoff2;
		float y1 = _rect.y + _rect.h * 0.4;

		glm::vec2 zones((xb) - 1, (xb) - 2);

		glm::vec4* v;
		glm::vec3* t;

		v 	 = shape7.Vertices.Lock();
		v[0] = glm::vec4(x0, 		y1+yoff2, 	0.f, 1.f);
		v[1] = glm::vec4(x0-xoff2,	y1,  		0.f, 1.f);
		v[2] = glm::vec4(x0+xoff,  y1+yoff,  	0.f, 1.f);
		v[3] = glm::vec4(x0-xoff,  y1-yoff,  	0.f, 1.f);
		v[4] = glm::vec4(x1,  		y0+yoff2,  	0.f, 1.f);
		v[5] = glm::vec4(x1-xoff2, y0,  		0.f, 1.f);
		v[6] = glm::vec4(x1, 		y0-yoff2,  	0.f, 1.f);
		shape7.Vertices.Unlock();

		t	 = shape7.TexCoords.Lock();
		t[0] = glm::vec3(-xb, -yb, 0);
		t[1] = glm::vec3( xb, -yb, 0);
		t[2] = glm::vec3(-xb,   0, 0);
		t[3] = glm::vec3( xb,   0, 0);
		t[4] = glm::vec3(-xb,   0, xb);
		t[5] = glm::vec3( xb,   0, xb);
		t[6] = glm::vec3( xb,2*yb, xb);
		shape7.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.fillColorVar, 	1, &s_colors[_fillColorId][0]);
		glProgramUniform4fv(commonWidget.program.id, commonWidget.borderColorVar, 	1, &s_colors[_borderColorId][0]);
		glProgramUniform2fv(commonWidget.program.id, commonWidget.zonesVar, 		1, &zones[0]);
		commonWidget.shape7Var.Draw(GL_TRIANGLE_STRIP,shape7.Indices);


		v 	 = shape7.Vertices.Lock();
		v[0] = glm::vec4(x2+xoff2, y1, 		0.f, 1.f);
		v[1] = glm::vec4(x2,		y1+yoff2, 	0.f, 1.f);
		v[2] = glm::vec4(x2+xoff,  y1-yoff,  	0.f, 1.f);
		v[3] = glm::vec4(x2-xoff,  y1+yoff,  	0.f, 1.f);
		v[4] = glm::vec4(x1+xoff2, y0,  		0.f, 1.f);
		v[5] = glm::vec4(x1, 		y0+yoff2,  	0.f, 1.f);
		v[6] = glm::vec4(x1, 		y0-yoff2,  	0.f, 1.f);
		shape7.Vertices.Unlock();

		t	 = shape7.TexCoords.Lock();
		t[0] = glm::vec3( xb, -yb, 0);
		t[1] = glm::vec3(-xb, -yb, 0);
		t[2] = glm::vec3( xb,   0, xb);
		t[3] = glm::vec3(-xb,   0, xb);
		t[4] = glm::vec3( xb,   0, xb);
		t[5] = glm::vec3(-xb,   0, xb);
		t[6] = glm::vec3( xb,2*yb, xb);
		shape7.TexCoords.Unlock();

		glUseProgram(commonWidget.program.id);
		commonWidget.shape7Var.Draw(GL_TRIANGLE_STRIP,shape7.Indices);
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawText( const Rect& _r, const char * _text, int /*_nbLines*/, int _caretPos, bool _isHover, bool _isOn, bool /*isFocus*/ )
	{
		if (_isHover || _isOn /* || _isFocus*/)
		{
		    drawRect(_r, cFontBack + (_isHover) + (_isOn << 1), cOutline);    
		}

		fontRenderer.Draw(_r.x, _r.y,font,_text,s_colors[cFont]);
		//drawString(_r.x, _r.y, _text, _nbLines, s_colors[cFont]);
		
		if (_caretPos != -1)
		{
		    int w = getTextLineWidthAt( _text, _caretPos);

		    drawRect(Rect( _r.x + w, _r.y, 2, _r.h), cOutline, cOutline);
		}
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawFrame( const Rect& _rect, const Point& _corner, bool _isHover, bool _isOn, bool /*_isFocus*/ ) const
	{
		int lColorNb = cBase + (_isHover) + (_isOn << 1);// + (_isFocus << 2);

		if (_corner.x + _corner.y == 0)
		    drawRect( _rect , lColorNb, cOutline);
		else
		    drawRoundedRect( _rect, _corner , lColorNb, cOutline );
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawBoolFrame( const Rect& _rect, const Point& _corner, bool _isHover, bool _isOn, bool /*_isFocus*/ ) const
	{
		int lColorNb = cBool + (_isHover) + (_isOn << 1);// + (_isFocus << 2);
		    
		drawRoundedRect( _rect, _corner , lColorNb, cOutline );
	}
	//--------------------------------------------------------------------------
	void GLPainter::drawDebugRect(const Rect & /*_rect*/)
	{
		//Tools::Logger::Info("DebugRect");
/*
		glBegin(GL_LINE_STRIP);
		    glVertex2i( _rect.x + 1, _rect.y + 1);
		    glVertex2i( _rect.x + _rect.w, _rect.y + 1);
		    glVertex2i( _rect.x + _rect.w, _rect.y + _rect.h);
		    glVertex2i( _rect.x + 1, _rect.y + _rect.h);
		    glVertex2i( _rect.x + 1, _rect.y);
		glEnd();
*/
	}
	//--------------------------------------------------------------------------
	void GLPainter::reshape(unsigned int _w, unsigned int _h)
	{
		fontRenderer.Reshape(_w,_h);
	}
}
