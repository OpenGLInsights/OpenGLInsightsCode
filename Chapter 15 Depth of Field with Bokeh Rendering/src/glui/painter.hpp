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
#ifndef GLUI_PAINTER_HPP
#define GLUI_PAINTER_HPP

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glui/widgets.hpp>

#include <glf/font.hpp>
#include <glf/buffer.hpp>
#include <glf/wrapper.hpp>

using namespace glf;

namespace glui
{

	//-------------------------------------------------------------------------
	// Store a shape for drawing
	//-------------------------------------------------------------------------
	struct Shape
	{
		VertexBuffer4F					Vertices;
		VertexBuffer3F					TexCoords;
		IndexBuffer						Indices;

		void Initialize(unsigned int _n)
		{
			Vertices.Allocate(_n, GL_STREAM_DRAW);
			TexCoords.Allocate(_n, GL_STREAM_DRAW);
			Indices.Allocate(_n, GL_STATIC_DRAW);

			unsigned int* ind = Indices.Lock();
			for(unsigned int i=0;i<_n;++i)
				ind[i] = i;
			Indices.Unlock();

			//GL_TRIANGLES_STRIP
		}
	};
	//-------------------------------------------------------------------------
	struct Quad
	{
		VertexBuffer4F					Vertices;
		VertexBuffer3F					TexCoords;
		IndexBuffer						Indices;

		void Initialize()
		{
			Vertices.Allocate(4);
			TexCoords.Allocate(4);
			Indices.Allocate(6);

			unsigned int* indices = Indices.Lock();
			indices[0] = 0;
			indices[1] = 1;
			indices[2] = 2;
			indices[3] = 0;
			indices[4] = 2;
			indices[5] = 3;
			Indices.Unlock();

			//GL_TRIANGLES
		}
	};


	//-------------------------------------------------------------------------
    // GL Painter
	//-------------------------------------------------------------------------
    class GLPainter : public UIPainter
    {
    public:

		//---------------------------------------------------------------------
		// Type of layer
		//---------------------------------------------------------------------
//		struct Layer
//		{
//			enum Type { BACKGROUND = 0,
//						MAIN	   = 1,
//						OVERLAY	   = 2 };
//		};

		//----------------------------------------------------------------------
		// Constructor
		//----------------------------------------------------------------------
        //GLPainter(const std::string _font, unsigned int _size);
		GLPainter();

		//----------------------------------------------------------------------
		// Initialize painter
		//----------------------------------------------------------------------
		virtual void Initialize();
		
		//----------------------------------------------------------------------
		// Destructor
		//----------------------------------------------------------------------
        virtual ~GLPainter();

		//----------------------------------------------------------------------
		// 
		//----------------------------------------------------------------------
        virtual void begin( const Rect& _window );
		virtual void beginDraw();
        virtual void end();

		////////////////////////////////////////////////////////////////////////
        // These methods should be called between begin/end
		////////////////////////////////////////////////////////////////////////

		//----------------------------------------------------------------------
		//
		//----------------------------------------------------------------------
        virtual void drawFrame(const Rect & _r, int _margin, int _style);

		//----------------------------------------------------------------------
		//
		//----------------------------------------------------------------------
        virtual Rect getLabelRect(const Rect & _r, const char * _text, Rect & _rt, int& _nbLines) const;
        virtual void drawLabel(const Rect & _r, const char * _text, const Rect & _rt, const int& _nbLines, bool _isHover, int _style);

		//----------------------------------------------------------------------
		//
		//----------------------------------------------------------------------     
        virtual Rect getButtonRect(const Rect & _r, const char * _text, Rect & _rt) const;
        virtual void drawButton(const Rect & _r, const char * _text, const Rect & _rt, bool _isDown, bool _isHover, bool _isFocus, int _style);

		//----------------------------------------------------------------------
		//
		//----------------------------------------------------------------------    
        virtual Rect getCheckRect(const Rect & __r, const char * _text, Rect & _rt, Rect & _rc) const;
        virtual void drawCheckButton(const Rect & _r, const char * _text, const Rect& _rt, const Rect& _rr, bool _isChecked, bool _isHover, bool _isFocus, int _style);

		//----------------------------------------------------------------------
		//
		//----------------------------------------------------------------------        
		virtual Rect getRadioRect(const Rect & _r, const char * _text, Rect & _rt, Rect & _rr) const;
        virtual void drawRadioButton(const Rect & _r, const char * _text, const Rect & _rt, const Rect & _rr, bool _isOn, bool _isHover, bool _isFocus, int _style);

		//----------------------------------------------------------------------
		//
		//----------------------------------------------------------------------
        virtual Rect getHorizontalSliderRect(const Rect & _r, Rect& _rs, float _v, Rect& _rc) const;
        virtual void drawHorizontalSlider(const Rect & _r, const Rect& _rs, float _v, const Rect& _rc, bool _isHover, int _style);
        virtual void drawDiscretSlider(const Rect & _r, const Rect& _rs, int _index, int _nIndices, const Rect& _rc, bool _isHover, int _style);

		//----------------------------------------------------------------------
		//
		//----------------------------------------------------------------------
        virtual Rect getItemRect(const Rect & _r, const char * _text, Rect & _rt) const;
        virtual Rect getListRect(const Rect & _r, int _numOptions, const char * _options[], Rect& _ri, Rect & _rt) const;
        virtual void drawListItem(const Rect & _r, const char * _text, const Rect & _rt, bool _isSelected, bool _isHover, int _style);
        virtual void drawListBox(const Rect & _r, int _numOptions, const char ** _options, const Rect& _ri, const Rect & _rt, int _selected, int _hovered, int _style);


		//----------------------------------------------------------------------
		//
		//----------------------------------------------------------------------
        virtual Rect getComboRect(const Rect & _r, int _numOptions, const char * _options[], int _selected, Rect& _rt, Rect& _ra) const;
        virtual Rect getComboOptionsRect(const Rect & _rCombo, int _numOptions, const char * _options[], Rect& _ri, Rect & _rit) const;
//        virtual void drawComboBox(const Rect & _rect, int _numOptions, const char * _options[], const Rect & _rt, const Rect& _rd, int _selected, bool _isHover, bool _isFocus, int _style);
        virtual void drawComboBox(const Rect & _rect, const char * _options, const Rect & _rt, const Rect & _ra, bool _isHover, bool _isFocus, int _style);
//        virtual void drawComboOptions(const Rect & _rect, const char * _options, const Rect& _ri, const Rect & _rit, int _hovered, bool _isHover, bool _isFocus, int _style);
        virtual void drawComboOptions(const Rect & _rect, int _numOptions, const char** _options, const Rect& _ri, const Rect & _rit, int _selected, int _hovered, bool _isHover, bool _isFocus, int _style);

		//----------------------------------------------------------------------
		//
		//----------------------------------------------------------------------
        virtual Rect getLineEditRect(const Rect & _r, const char * _text, Rect & _rt) const;
        virtual void drawLineEdit(const Rect & _r, const char * _text, const Rect & _rt, int _caretPos, bool _isSelected, bool _isHover, int _style);

		//----------------------------------------------------------------------
		//
		//----------------------------------------------------------------------
        virtual Rect getPanelRect(const Rect & _r, const char * _text, Rect& _rt, Rect& _ra) const;
        virtual void drawPanel(const Rect & _rect, const char * _text, const Rect & _rt, const Rect & _ra, bool _isUnfold, bool _isHover, bool _isFocus, int _style);

		//----------------------------------------------------------------------
		// Eval widget dimensions
		//----------------------------------------------------------------------        
        virtual int getCanvasMargin() const;
        virtual int getCanvasSpace() const;
        virtual int getFontHeight() const;
        virtual int getTextLineWidth(const char * _text) const;
        virtual int getTextSize(const char * _text, int& _nbLines) const;
        virtual int getTextLineWidthAt(const char * _text, int _charNb) const;
        virtual int getPickedCharNb(const char * _text, const Point& _at) const;

		//----------------------------------------------------------------------
		//
		//----------------------------------------------------------------------
        virtual void drawDebugRect(const Rect & _r);
		virtual void reshape(unsigned int _w, unsigned int _h);


    protected:

		//----------------------------------------------------------------------
        // Draw primitive shapes
		//----------------------------------------------------------------------
        void drawText( const Rect& _r , const char * _text, int _nbLines = 1, int _caretPos = -1, bool _isHover = false, bool _isOn = false, bool _isFocus = false );
        void drawFrame( const Rect& _rect, const Point& _corner, bool _isHover = false, bool _isOn = false, bool _isFocus = false ) const;
        void drawBoolFrame( const Rect& _rect, const Point& _corner, bool _isHover = false, bool _isOn = false, bool _isFocus = false ) const;

		//----------------------------------------------------------------------
		//
		//----------------------------------------------------------------------
        //void drawString( int _x, int _y, const char * _text, int _nbLines );
        void drawRect( const Rect& _rect, int _fillColorId, int _borderColorId ) const;
        void drawRoundedRect( const Rect& _rect, const Point& _corner, int _fillColorId, int _borderColorId ) const;
        void drawRoundedRectOutline( const Rect& _rect, const Point& _corner, int _borderColorId ) const;
        void drawCircle( const Rect& _rect, int _fillColorId, int _borderColorId ) const;
        void drawMinus( const Rect& _rect, int _width, int _fillColorId, int _borderColorId ) const;
        void drawPlus( const Rect& _rect, int _width, int _fillColorId, int _borderColorId ) const;
        void drawDownArrow( const Rect& _rect, int _width, int _fillColorId, int _borderColorId ) const;
        void drawUpArrow( const Rect& _rect, int _width, int _fillColorId, int _borderColorId ) const;

        void init();

    private:
		//---------------------------------------------------------------------
		// Type
		//---------------------------------------------------------------------
		#define MAX_OVERDRAW_OPTIONS				32
		#define MAX_OVERDRAW_OPTION_SIZE			128
		static const unsigned int MaxOptions;		// Max number of options
		static const unsigned int MaxOptionSize;	// Max size of an option with its null terminal character '\0'
		//const int MaxOverdraw;					// Max number of combobox
		//---------------------------------------------------------------------
		struct Overdraw
		{
			Rect r;
			int  numOptions;
			char* options[MAX_OVERDRAW_OPTIONS];
			Rect ri;
			Rect rt;
			int  selected;
			int  hovered;
			bool isHover;
			bool isFocus;
			int  style;

			//-----------------------------------------------------------------
			// Constructor
			//-----------------------------------------------------------------
			Overdraw()  
			{ 
				for(unsigned int i=0;i<MaxOptions;++i)
					options[i] = new char[MaxOptionSize];
			}

			//-----------------------------------------------------------------
			// Destructor
			//-----------------------------------------------------------------
			~Overdraw() 
			{
				for(unsigned int i=0;i<MaxOptions;++i)
					delete[] options[i];
			}
		};		
		//---------------------------------------------------------------------
		struct CommonWidget
		{
			Program		program;
			GLint		viewVar;
			GLint		projectionVar;
			GLint 		fillColorVar;
			GLint 		borderColorVar;
			GLint		zonesVar;

			VertexArray quadVar;
			VertexArray shape4Var;
			VertexArray shape7Var;
			VertexArray shape8Var;

			//-----------------------------------------------------------------
			// 			
			//-----------------------------------------------------------------
			CommonWidget():
			program("WidgetRenderer")
			{

			}
			//-----------------------------------------------------------------
			void Build( const char* _vs, const char* _fs)
			{
				program.Compile(_vs,_fs);
				viewVar 		= program["View"].location;
				projectionVar 	= program["Projection"].location;
				fillColorVar 	= program["FillColor"].location;
				borderColorVar 	= program["BorderColor"].location;
				zonesVar 		= program["Zones"].location;
			}
		};

		//---------------------------------------------------------------------
		// Attributes
		//---------------------------------------------------------------------
		glf::Font								font;
		glf::FontRenderer						fontRenderer;
		mutable Quad							quad;			// Quad
		mutable Shape							shape4;			// Shape with 4 vertices
		mutable Shape							shape7;			// Shape with 7 vertices
		mutable Shape							shape8;			// Shape with 8 vertices

		glm::mat4 								view, projection;
		mutable CommonWidget					commonWidget;

		Overdraw								overdraw;		// Store all overdraw
		unsigned int							nOverdraws;		// Store all
	};
}


#endif
