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
#ifndef GLUI_WIDGETS_HPP
#define GLUI_WIDGETS_HPP

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <ctime>
//------------------------------------------------------------------------------
#ifdef WIN32
	#pragma warning( disable : 4201 )
#endif

namespace glui
{
	//-------------------------------------------------------------------------
	// Internal struct for managing widgets
	//-------------------------------------------------------------------------
	struct Point
	{
		Point() : x(0), y(0) {}
		Point(int _ix, int _iy) : x(_ix), y(_iy) {}
		Point(const Point & _p) : x(_p.x), y(_p.y) {}

		const Point& operator= (const Point & _p) { this->x = _p.x; this->y = _p.y; return *this; }

		int x, y;
	};
	//-------------------------------------------------------------------------
    struct Rect
    {
        Rect() : x(0), y(0), w(0), h(0) {}
        Rect(const Point & _p) : x(_p.x), y(_p.y), w(0), h(0) {}

        Rect(int _ix, int _iy, int _iw = 0, int _ih = 0) : x(_ix), y(_iy), w(_iw), h(_ih) {}
        Rect(float _ix, float _iy, float _iw = 0.0, float _ih = 0.0) : xf(_ix), yf(_iy), wf(_iw), hf(_ih) {}

        Rect(const Rect & _r) : x(_r.x), y(_r.y), w(_r.w), h(_r.h) {}

        const Rect& operator= (const Rect & _r) { this->x = _r.x; this->y = _r.y; this->w = _r.w; this->h = _r.h; return *this; }

		union
		{
	        struct{ int   x, y, w, h; };
			struct{ float xf,yf,wf,hf; };
		};

        static const Rect null;
    };
	//-------------------------------------------------------------------------
	struct ButtonFlags
	{
		enum Type {		OFF 		= 0x0,
						ON 			= 0x1,
						BEGIN 		= 0x2,
						END 		= 0x4,
						SHIFT		= 0x8,
						ALT 		= 0x10,
						CTRL 		= 0x20	};
	};
	//-------------------------------------------------------------------------
	struct Mouse
	{
		enum State { 	PRESS 		= 0,
						RELEASE		= 1 };

		enum Button { 	LEFT		= 0,
						RIGHT		= 1,
						MIDDLE		= 2,
						SCROLL_UP	= 3,
						SCROLL_DOWN	= 4 };
	};
	//-------------------------------------------------------------------------
	struct Keyboard
	{
		enum State { 	PRESS 		= 0,
						RELEASE 	= 1 };

		enum Key {		ENTER		= 128,
						SPACE		= 129,
						BACK_SPACE	= 130,
						LEFT		= 131,
						RIGHT		= 132,
						UP			= 133,
						DOWN		= 134,
						PAGE_UP		= 135,
						PAGE_DOWN	= 136,
						HOME		= 137,
						END			= 138,
						INSERT		= 139,
						F1			= 140,
						F2			= 141,	
						F3			= 142,	
						F4			= 143,	
						F5			= 144,	
						F6			= 145,	
						F7			= 146,	
						F8			= 147,	
						F9			= 148,	
						F10			= 149,	
						F11			= 150,	
						F12			= 151 };
	};
	//-------------------------------------------------------------------------
    // The various flags to modify the behavior of the groups   
	//-------------------------------------------------------------------------
    struct Flags
    {
        // Layout behavior flags
		struct Layout
		{
			enum Type {	NONE 		= 0x01,
						VERTICAL 	= 0x02,
						HORIZONTAL 	= 0x04,
						MASK 		= 0x07,
						XMASK 		= 0xffff ^ MASK,

						// Optional flags
						FORCE 		= 0x8000,
						DEFAULT 	= 0x4000,
						NO_MARGIN 	= 0x2000,
						NO_SPACE 	= 0x1000 };
		};

        // Alignment flags for the widgets inserted in the group
		struct Align
		{
			enum Type {	LEFT 		= 0x10,
						RIGHT 		= 0x20,
						TOP 		= 0x40,
						BOTTOM 		= 0x80,
						MASK 		= 0xf0,
						XMASK 		= 0xffff ^ MASK };
		};

        // Start flags defining the starting origin of the group
		struct Start
		{
			enum Type {	LEFT 		= 0x100,
						RIGHT 		= 0x200,
						TOP 		= 0x400,
						BOTTOM 		= 0x800,
						MASK 		= 0xf00,
						XMASK 		= 0xffff ^ MASK	};
		};

        // Predefined configurations
		struct Grow
		{
			enum Type {	RIGHT_FROM_BOTTOM   = Layout::HORIZONTAL	| Start::LEFT   | Align::LEFT   | Start::BOTTOM | Align::BOTTOM,
						RIGHT_FROM_TOP      = Layout::HORIZONTAL    | Start::LEFT   | Align::LEFT   | Start::TOP    | Align::TOP ,
						LEFT_FROM_BOTTOM    = Layout::HORIZONTAL    | Start::RIGHT  | Align::RIGHT  | Start::BOTTOM | Align::BOTTOM,
						LEFT_FROM_TOP       = Layout::HORIZONTAL    | Start::RIGHT  | Align::RIGHT  | Start::TOP    | Align::TOP,
						UP_FROM_LEFT        = Layout::VERTICAL      | Start::BOTTOM | Align::BOTTOM | Start::LEFT   | Align::LEFT ,
						UP_FROM_RIGHT       = Layout::VERTICAL      | Start::BOTTOM | Align::BOTTOM | Start::RIGHT  | Align::RIGHT,
						DOWN_FROM_LEFT      = Layout::VERTICAL      | Start::TOP    | Align::TOP    | Start::LEFT   | Align::LEFT,
						DOWN_FROM_RIGHT     = Layout::VERTICAL      | Start::TOP    | Align::TOP    | Start::RIGHT  | Align::RIGHT,

						DEFAULT_FALLBACK	= Grow::DOWN_FROM_LEFT };
		};
    };
	//-------------------------------------------------------------------------
    struct ButtonState
    {
        int		state;
        time_t 	time;
        Point 	cursor;
    };
	//-------------------------------------------------------------------------
    struct Group
    {
        Rect bounds;   	// Anchor point + width and height of the region
        int flags;    	// Group behavior 
        int margin;   	// Border 
        int space;    	// Interior
		//int components;	// Number of components inside group
		int index;		// Index location in component stack
    };

	//-------------------------------------------------------------------------
    // Class UIComponent
	//-------------------------------------------------------------------------
	struct UIComponent
	{
		virtual ~UIComponent() {}
		virtual Rect CheckRect(const Rect& _rect)=0;
		virtual void Draw(const Rect& _rect)=0;
		virtual bool Mouse(const Rect& _rect, int _x, int _y)=0;
	};

	//---------------------------------------------------------------------
	// Struct to store component before effective drawing
	//---------------------------------------------------------------------
	struct Component
	{
		enum Type {	LABEL			 = 0,
					BUTTON			 = 1,
					RADIO_BUTTON 	 = 2,
					CHECK_BUTTON 	 = 3,
					COMBO_BOX 		 = 4,
					COMBO_BOX_OPTION = 5,
					SLIDER 			 = 6,
					EDIT_LINE		 = 7,
					PANEL_HEADER	 = 8,
					FRAME			 = 9,
					LIST_ITEM		 = 10,
					LIST_BOX		 = 11,
					DISCRET_SLIDER	 = 12,
					GENERIC	 		 = 13 };

		Rect rect;
		//char text[50]; 		// TODO : Change to a variable size ?
		short textIndex;

		union
		{
			float value;	// Value for slider
			int carretPos;
			int selected;
		};

		Rect rt;
		Rect rs;
		Rect bounds;

		Rect rc;
		Rect rd; //ra
		Rect re; //rr

		int margin;
		int nbLines;	// for label, for numOptions
		int hovered;
		bool state;		// State for button (radio, check,
		bool hover;
		bool focus;
		Type type;

		UIComponent* generic;
	};

	//-------------------------------------------------------------------------
    // Class UIPainter
	//-------------------------------------------------------------------------
    class UIPainter
    {
    public:
		//---------------------------------------------------------------------
		// Constructor
		//---------------------------------------------------------------------
        UIPainter() {}

		//---------------------------------------------------------------------
		// Destructor
		//---------------------------------------------------------------------
        virtual ~UIPainter() {}

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        virtual void begin( const Rect & /*window*/ ) { initialize(); }
		virtual void beginDraw() {};
        virtual void end() {}

		//---------------------------------------------------------------------
		//
		// Required
        // These methods should be called between begin/end
		//---------------------------------------------------------------------
        virtual void drawFrame(const Rect & _r, int _margin, int _style) = 0;

		//---------------------------------------------------------------------
		//
		// Required
        // These methods should be called between begin/end
		//---------------------------------------------------------------------
        virtual Rect getLabelRect(const Rect & _r, const char * _text, Rect & _rt, int& _nbLines) const = 0;
        virtual void drawLabel(const Rect & _r, const char * _text, const Rect & _rt, const int& _nbLines, bool _isHover, int _style) = 0;
      
		//---------------------------------------------------------------------
		//
		// Required
        // These methods should be called between begin/end
		//---------------------------------------------------------------------
        virtual Rect getButtonRect(const Rect & _r, const char * _text, Rect & _rt) const = 0;
        virtual void drawButton(const Rect & _r, const char * _text, const Rect & _rt, bool _isDown, bool _isHover, bool _isFocus, int _style) = 0;

		//---------------------------------------------------------------------
		//
		// Required
        // These methods should be called between begin/end
		//---------------------------------------------------------------------    
        virtual Rect getCheckRect(const Rect & _r, const char * _text, Rect & _rt, Rect & _rc) const = 0;
        virtual void drawCheckButton(const Rect & _r, const char * _text, const Rect & _rt, const Rect & _rr, bool isChecked, bool _isHover, bool _isFocus, int _style) = 0;

		//---------------------------------------------------------------------
		//
		// Required
        // These methods should be called between begin/end
		//---------------------------------------------------------------------
        virtual Rect getRadioRect(const Rect & _r, const char * _text, Rect & _rt, Rect & _rr) const = 0;
        virtual void drawRadioButton(const Rect & _r, const char * _text, const Rect & _rt, const Rect & _rr, bool _isOn, bool _isHover, bool _isFocus, int _style) = 0;

		//---------------------------------------------------------------------
		//
		// Required
        // These methods should be called between begin/end
		//---------------------------------------------------------------------
        virtual Rect getHorizontalSliderRect(const Rect & _r, Rect& _rs, float v, Rect& _rc) const = 0;
        virtual void drawHorizontalSlider(const Rect & _r, const Rect& _rs, float v, const Rect& _rc, bool _isHover, int _style) = 0;
        virtual void drawDiscretSlider(const Rect & _r, const Rect& _rs, int _index, int _nIndices, const Rect& _rc, bool _isHover, int _style) = 0;

		//---------------------------------------------------------------------
		//
		// Required
        // These methods should be called between begin/end
		//---------------------------------------------------------------------
        virtual Rect getItemRect(const Rect & _r, const char * _text, Rect & _rt) const = 0;
        virtual void drawListItem(const Rect & _r, const char * _text, const Rect & _rt, bool _isSelected, bool _isHover, int _style) = 0;

		//---------------------------------------------------------------------
		//
		// Required
        // These methods should be called between begin/end
		//--------------------------------------------------------------------- 
        virtual Rect getListRect(const Rect & _r, int _numOptions, const char * _options[], Rect& _ri, Rect & _rt) const = 0;
        virtual void drawListBox(const Rect & _r, int _numOptions, const char ** _options, const Rect& _ri, const Rect & _rt, int _selected, int _hovered, int _style) = 0;
      
		//---------------------------------------------------------------------
		//
		// Required
        // These methods should be called between begin/end
		//---------------------------------------------------------------------
        virtual Rect getComboRect(const Rect & _r, int _numOptions, const char * _options[], int _selected, Rect& _rt, Rect& _ra) const = 0;
        virtual Rect getComboOptionsRect(const Rect & _rCombo, int _numOptions, const char * _options[], Rect& _ri, Rect & _rit) const = 0;

        virtual void drawComboBox(const Rect & _rect, const char * _options, const Rect & _rt, const Rect & _ra, bool _isHover, bool _isFocus, int _style) = 0;
        //virtual void drawComboBox(const Rect & _rect, int _numOptions, const char * _options[], const Rect & _rt, const Rect & _ra, int _selected, bool _isHover, bool _isFocus, int _style) = 0;
        virtual void drawComboOptions(const Rect & _rect, int _numOptions, const char ** _options, const Rect& _ri, const Rect & _rit, int _selected, int _hovered, bool _isHover, bool _isFocus, int _style) = 0;
        //virtual void drawComboOptions(const Rect & _rect, const char * _options, const Rect& _ri, const Rect & _rit, int _hovered, bool _isHover, bool _isFocus, int _style) = 0;


		//---------------------------------------------------------------------
		//
		// Required
        // These methods should be called between begin/end
		//---------------------------------------------------------------------
        virtual Rect getLineEditRect(const Rect & _r, const char * _text, Rect & _rt) const = 0;
        virtual void drawLineEdit(const Rect & _r, const char * _text, const Rect & _rt, int caretPos, bool _isSelected, bool _isHover, int _style) = 0;

		//---------------------------------------------------------------------
		//
		// Required
        // These methods should be called between begin/end
		//---------------------------------------------------------------------
        virtual Rect getPanelRect(const Rect & _r, const char * _text, Rect& _rt, Rect& _ra) const = 0;
        virtual void drawPanel(const Rect & _rect, const char * _text, const Rect & _rt, const Rect & _ra, bool _isUnfold, bool _isHover, bool _isFocus, int _style) = 0;

		//---------------------------------------------------------------------
        // Evalate widget dimensions
		//
		// Required
        // These methods should be called between begin/end
		//---------------------------------------------------------------------
        virtual int getCanvasMargin() const = 0;
        virtual int getCanvasSpace() const = 0;
        virtual int getFontHeight() const = 0;
        virtual int getTextLineWidth(const char * _text) const = 0;
        virtual int getTextSize(const char * _text, int& _nbLines) const = 0;
        virtual int getPickedCharNb(const char * _text, const Point& _at) const = 0;

		//---------------------------------------------------------------------
		//
		// Required
        // These methods should be called between begin/end
		//---------------------------------------------------------------------
        virtual void drawDebugRect(const Rect & _r) = 0;

		//----------------------------------------------------------------------
		// Set reshape window boundaries
		// _w : new window width 
		// _h : new window height
		//----------------------------------------------------------------------
		virtual void reshape(unsigned int _w, unsigned int _h) =0;

		//---------------------------------------------------------------------
		// Initialize painter
		//---------------------------------------------------------------------
        virtual void Initialize()=0;
		
    protected:
		virtual void initialize(){};
    };


    class UIContext
    {
    public:
		//---------------------------------------------------------------------
        // Constructor
		//---------------------------------------------------------------------
        UIContext(UIPainter& painter);

		//---------------------------------------------------------------------
        // Destructor
		//---------------------------------------------------------------------
		virtual ~UIContext();

		//---------------------------------------------------------------------
        // UI method for processing window size events
		//---------------------------------------------------------------------
        void Reshape(int _w, int _h);

		//---------------------------------------------------------------------       
        // Check if the UI is currently on Focus
		//---------------------------------------------------------------------
        bool IsOnFocus() const { return m_uiOnFocus; }

		//---------------------------------------------------------------------
        // UI method for processing mouse events
		//---------------------------------------------------------------------
        void Mouse(Mouse::Button _button, Mouse::State _state, int _modifier, int _x, int _y);

		//---------------------------------------------------------------------
        // UI method for processing mouse events
		//---------------------------------------------------------------------
        void Mouse(Mouse::Button _button, Mouse::State _state, int _x, int _y);
 
		//---------------------------------------------------------------------
        // UI method for processing mouse motion events
		//---------------------------------------------------------------------
        void Move(int _x, int _y);

		//---------------------------------------------------------------------
        // UI method for processing key events
		//---------------------------------------------------------------------
        void Keyboard(unsigned char _k);

		//---------------------------------------------------------------------
        // UI method for entering UI processing mode
        //
        //  This function must be used to begin the UI processing
		//---------------------------------------------------------------------
        void Begin();

		//---------------------------------------------------------------------
        // UI method for leaving UI processing mode
        //
        //  This function must be used to end the UI processing
		//---------------------------------------------------------------------
        void End();

		//---------------------------------------------------------------------//////////
        // UI element processing
        //
        // The following methods provide the interface for rendering and querying
        // UI objects. These methods must be called between begin/end.
		//---------------------------------------------------------------------//////////

		//---------------------------------------------------------------------
        // UI method for drawing a static _text label
        // The label display a non interactive _text.
        // The _text can have multiple lines
        //
        // _rect - optionally provides a location and size for the label
        // _text - Text to display for the label (can have several lines)
		//---------------------------------------------------------------------
        void Label(const Rect & _rect, const char * _text, int _style = 0);

		//---------------------------------------------------------------------
        // UI method for rendering and processing a push _button
        //
        // _rect - optionally provides a location and size for the _button
        // _text - _text to display on the _button
        // _state -  whether the _button is depressed
        //          if _state is NULL, the buttoin behave like a touch _button
        //          else, the _button behave like a toggle _button
        // _style - optional _style flag to modify the look
        //
        // @return  True if the _button'_state changed
		//---------------------------------------------------------------------
        bool Button(const Rect & _rect, const char * _text, bool * _state = NULL, int _style = 0);
        
		//---------------------------------------------------------------------
        // UI method for rendering and processing a check _button
        // Check _button behaves similarly as a toggle _button 
        // used to display and edit a bool property.
        //
        // _rect - optionally provides a location and size for the _button
        // _text - _text to display on the _button
        // _state -  whether the check _button is checked or not
        //          if _state is NULL, the buttoin behave like if a touch _button unchecked
        // _style - optional _style flag to modify the look
        //
        // @return  True if the check _button'_state changed
		//---------------------------------------------------------------------
        bool CheckButton(const Rect & _rect, const char * _text, bool * _state, int _style = 0);

		//---------------------------------------------------------------------
        // UI method for rendering and processing a radio _button
        // Radio buttons are typically used in groups to diplay and edit 
        // the possible _reference values taken by an int _value.
        //
        // One radio _button is representing a possible _reference _value taken by the current _value.
        // It is displaying a boolean _state true if the current _value is equal to the _reference _value.
        //
        // _reference : The _reference int _value represented by this radio _button.
        // _rect : optionally provides a location and size for the _button
        // _text : _text to display on the _button
        // _value :  The _value parameter compared to the _reference _value parameter.
        //          if _value is NULL, the radio _button is off
        // _style : optional _style flag to modify the look
        //
        // @return  True if the radio _button's _value changed
		//---------------------------------------------------------------------
        bool RadioButton(int _reference, const Rect & _r, const char * _text, int * _value, int _style = 0);

		//---------------------------------------------------------------------
        // UI method for rendering and processing a horizontal slider
        // Horizontal slider is used to edit and display a scalar _value in the specified range [_min, _max].
        //
        // _rect - optionally provides a location and size for the widget
        // _min - _min bound of the varying range of the _value
        // _max - _max bound of the varying range of the _value
        // _value -  the _value edited byt the widget
        //          if _value is NULL, the _value is set to _min
        // _style - optional _style flag to modify the look
        //
        // @return  True if the slider's _value changed
		//---------------------------------------------------------------------
        bool HorizontalSlider(const Rect & _rect, float _min, float _max, float * _value, int _style = 0);
        //bool HorizontalSlider(const Rect & _rect, float _min, float _max, float * _value, bool& _isPressed, int _style = 0);


		//---------------------------------------------------------------------
        // UI method for rendering and processing a horizontal discret slider
        // Horizontal slider is used to edit and display a scalar _value at the specified position [_values[0],_values[1],_values[2],...].
        //
        // _rect - optionally provides a location and size for the widget
        // _nValues - number of discret position
        // _values - discret possible positions (could be NULL)
        // _index -  the index of the _value edited by the widget
        // _style - optional _style flag to modify the look
        //
        // @return  True if the slider's _value changed
		//---------------------------------------------------------------------
        bool DiscretSlider(const Rect & _rect, int _nValues, float* _values, int * _index, int _style = 0);

		//---------------------------------------------------------------------
        // UI method for rendering generic component
        //
        // _rect - optionally provides a location and size for the widget
        // _comp - to draw
        // _values - discret possible positions (could be NULL)
        // _index -  the index of the _value edited by the widget
        // _style - optional _style flag to modify the look
        //
        // @return  True if the slider's _value changed
		//---------------------------------------------------------------------
        bool Generic(const Rect & _rect, UIComponent& _comp, int _style = 0);

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        bool ListItem(int _index, const Rect & _rect, const char * _text, int * _selected, int _style = 0);

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        bool ListBox(const Rect & _rect, int _numOptions, const char * _options[], int * _selected, int _style = 0);

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        bool ComboBox(const Rect & _rect, int _numOptions, const char * _options[], int * _selected, int _style = 0);

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        bool LineEdit(const Rect & _rect, char * _text, int maxTextLength, int * nbCharsReturned, int _style = 0);

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        void BeginGroup(int _groupFlags = Flags::Layout::DEFAULT, const Rect& _rect = Rect::null);

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        void EndGroup();

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        void BeginFrame(int _groupFlags = Flags::Layout::DEFAULT, const Rect& _rect = Rect::null, int _style = 0);

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        void EndFrame();

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        bool BeginPanel(Rect & _rect, const char * _text, bool * _isUnfold, int _groupFlags = Flags::Layout::DEFAULT, int _style = 0);

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        void EndPanel();

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        int GetGroupWidth() { return m_groupStack[m_groupIndex].bounds.w; }

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        int GetGroupHeight() { return m_groupStack[m_groupIndex].bounds.h; }

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        int GetCursorX() { return m_currentCursor.x;}

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        int GetCursorY() { return m_currentCursor.y;}

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        const ButtonState& getMouseState(int _button) { return m_mouseButton[_button]; }

		//---------------------------------------------------------------------
        // UI method for drawing a texture view
        // Several parameters control the equation used to display the texel
        //      texel = texture2DFetch(...);
        //      pixel = texelSwizzling( texel * _texelScale + _texelOffset );
        //
        // _rect - provides a location and size for the texture view
        // texID - texture identifier (Graphics API specific)
        // zoomRect - rectangle area of the texture displayed (in normalized coordinate bottom-left:(0.f,0.f) upper-right:(1.f,1.f))
        // mipLevel - mip Level of the texture displayed
        // _texelScale - scale applyed to the texel fetch
        // _texelOffset - offset applyed to the texel after scale
        // texelSwizzling - swizzle applyed to the texel (after scale and offset)
        // _style - optional _style flag to modify the look
		//---------------------------------------------------------------------
        void TextureView(const Rect & _rect, unsigned int _texID,
						 Rect & _zoomRect, int _mipLevel = -1,
	                     float _texelScale = 1.0f, float _texelOffset = 0.0f,
	                     int _red = 0, int _green = 1, int _blue = 2, int _alpha = 3,
	                     int _style = 0);

		
		//---------------------------------------------------------------------
		// Initialize UI context
		// _w : Initial width of UI
		// _h : Initial height of UI
		//---------------------------------------------------------------------		
		virtual void Initialize(int _w, int _h)=0;
		
    protected:
		//---------------------------------------------------------------------
		// Accessor on painter
		//---------------------------------------------------------------------
        UIPainter*  getPainter() { return m_painter; }

		//---------------------------------------------------------------------
		// Accessor on current window
		//---------------------------------------------------------------------
        const Rect& getWindow() const { return m_window; }

    private:
		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        static bool overlap(const Rect & _rect, const Point & p);

		//---------------------------------------------------------------------
		// Set cursor position
		// _x : mouse position on x axis
		// _y : mouse position on y axis
		//---------------------------------------------------------------------
        void setCursor(int _x, int _y);

		//---------------------------------------------------------------------
		// Return true if rectangle has mouse focus, false otherwise
		//---------------------------------------------------------------------
        bool hasFocus(const Rect & _rect);

		//---------------------------------------------------------------------
		// Return true if mouse is over rectangle, false otherwise
		//---------------------------------------------------------------------
        bool isHover(const Rect & _rect);

		//---------------------------------------------------------------------
		//
		//---------------------------------------------------------------------
        Rect placeRect(const Rect & _r);

		//---------------------------------------------------------------------
		// Draw a component
		//---------------------------------------------------------------------
		void draw(const Component& _component);

		//---------------------------------------------------------------------
		// Print component stack
		//---------------------------------------------------------------------
		void printComponentStack() const;

    private:
		//---------------------------------------------------------------------
		// Attributes
		//---------------------------------------------------------------------
        UIPainter * m_painter;

		// Group stack
		const int maxGroup;
        int m_groupIndex;
        Group* m_groupStack;

		// Component stack
		const int maxComponent;
        int m_componentIndex;
        Component* m_componentStack;

		// String stack
		const int maxString;
		const int maxStringSize;
		int m_stringIndex;
		char** m_stringStack;

		// Overdraw stack
		const int maxOverdraw;
        int m_overdrawIndex;
        Component* m_overdrawStack;

        Rect m_window;

        Point m_currentCursor;
        ButtonState m_mouseButton[3];
        unsigned char m_keyBuffer[32];
        int m_nbKeys;

        int m_focusCaretPos;
        Point m_focusPoint;
				public:
        bool m_twoStepFocus;

        bool m_uiOnFocus;
    };

};


#endif
