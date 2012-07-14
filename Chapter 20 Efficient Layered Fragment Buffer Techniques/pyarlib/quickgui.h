/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef QUICK_WIDGET_H
#define QUICK_WIDGET_H

#define MAX_TEXT_LENGTH (1024*1024)
#define MAX_TEXT_LINES 64

#define NINEBOX_LABEL "label"
#define NINEBOX_BUTTON "buttons"
#define NINEBOX_SLIDER_SLIDE "sliderrail"
#define NINEBOX_SLIDER_SLIDER "sliderbar"
#define NINEBOX_CHECKBOX_CHECK "checkbox"
#define NINEBOX_RADIO_CHECK "radiocheck"

#include "includegl.h"

#include <vector>
#include <map>
#include <string>
#include <stdarg.h>

#include "vec.h"
#include "text.h"
#include "immediate.h"
#include "ninebox.h"
#include "matstack.h"

using namespace std;

namespace QG
{

enum WidgetEvent
{
	CLICK,
	SCROLL
};

enum Direction
{
	NONE = 0x00,
	HORIZONTAL = 0x01,
	VERTICAL = 0x10,
	BOTH = 0x11
};

enum Anchor
{
	CENTER,
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT
};

/*
struct Size
{
	int top;
	int right;
	int bottom;
	int left;
	Size() {top=right=bottom=left=0;}
	Size(int i) {top=right=bottom=left=i;}
	void operator=(int i) {top=right=bottom=left=i;}
};*/
typedef Dimensions Size;

typedef void (*WidgetCallback)();
typedef void (*WidgetCallbackArg)(void*);

struct WidgetCallbackData
{
	void* arg;
	union {
		WidgetCallback ptrv;
		WidgetCallbackArg ptra;
	};
	void operator()();
};

class Widget;

typedef vector<Widget*> WidgetList;
typedef set<Widget*> WidgetSet;

class Widget
{
public:
	//change these sizes at will :) (remember to call setDirty afterwards!)
	Size margin;
	Size border;
	Size padding;
	
	vec2i size;
	int& width; //== size.x
	int& height; //== size.y
	
	//helper geometry functions
	vec2i getStart(); //returns offset from the top left bounds to the content area
	vec2i getBounds(); //returns the bounding box size of the widget
	
	//packing parameters
	//widget dimension in stack direction only expanded as needed.
	//widget dimension not in stack direction always shrink to best fit.
	Direction stack; //probably shouldn't be NONE or BOTH :P
	Direction fill;
	Direction expand;
	Anchor anchor;
	
	//IMPORTANT:
	//allow this widget to be deleted in the parent's .rem() method. off by default
	bool autoDelete;
	
protected:

	int subImage; //indexes sub images (eg button up/down)
	int nineboxImage; //unique
	static NineBoxPool nineboxPool; //shared
	
	//this widget requires repacking. a full repack is awlways done.
	//incrementally repacking the tree is way too complicated and in most cases isn't worth it
	bool dirty;
	
	//widget bi-directional tree structure
	Widget* parent; //set in add()
	Widget* root; //set in pack()
	WidgetList children;
	static WidgetSet* instances;
	static WidgetSet* roots;
	static Immediate mygl; //fallback "immediate" mode rendering. USE SPARINGLY!!!
	
	//interaction/event variables
	bool hidden; //set via hide()
	bool mouseOver;
	static Widget* focus; //NULL if no focus. eg, used to get current button held down even while the mouse is not over it
	map<WidgetEvent, WidgetCallbackData> callbacks;
	
	//be aware these are only valid if !dirty
	vec2i offset; //offset of inner, content box relative to parent
	vec2i position; //global position of content box
	
public:
	Widget();
	virtual ~Widget();
	
	static void loadImages(std::string mapfile); //reads a file containing a mapping of names to ninebox textures/attributes
	static void releaseAll();
	
	//just because of the stupid protected member I can't access parent->children from RadioButton.
	const WidgetList& getChildren() {return children;}
	
	//build the widget tree
	void add(Widget& w); //standard method to add widgets
	void add(Widget* w); //WARNING: widget w is deleted with parent in destructor
	
	//rem() removes the widget from the tree
	void rem(Widget& w);
	void rem(Widget* w);
	
	void setDirty(); //call this when ANY geometric changes are made to the widget
	void hide(bool hidden = true); //use to toggle widgets on/off quickly - rem() is slow
	
	//callbacks for the GUI user
	void capture(WidgetEvent e, WidgetCallback func);
	void capture(WidgetEvent e, WidgetCallbackArg func, void* arg);
	
	//interface. call these yourself in the GUI manager
	static void draw(mat44 modelview); //resolves dirty and draws ALL widgets!!!
	bool mouseDown(vec2i pos); 
	void mouseUp(vec2i pos);
	bool mouseMove(vec2i pos);

	void pack(); //packs all child widgets recursively
	void packPos(); //recursively updates global Widget::positions
protected:

	//helper function
	vec2i placeWidthAnchor(vec2i bounds, vec2i size, Anchor a);
	
	//Widget generates these interface events for the subclasses
	//call setDirty() from these if changes are made to the widget
	virtual void drag(vec2i pos); //mouse down or movement. argument is relative position
	virtual void mouseDown(); //simply notify of mouse down ON the widget
	virtual void mouseUp(); //mouse up after widget focus. Guaranteed after a mouseDown
	virtual void mouseEnter();
	virtual void mouseLeave();
	virtual void keyDown(unsigned char c);
	virtual void keyUp(unsigned char c);
	virtual void click(); //mouse up after widget focus ONLY if still over widget
	
	virtual void onAdd(); //called when widget is added
	
	//the most important function to override
	//any changes to the widget appearance must be done here
	//this is called during/just before pack() to resolve dirty widgets
	virtual void update();
	
	//post-pack operation. used to update nine-box graphics
	//this is called during packPos()
	virtual void packed();
	
	//recursive. only override if custom drawing is required
	virtual void drawContent(mat44 mat);

private:
	Widget(const Widget& other) : width(size.x), height(size.y) {}  //no copying
	void operator=(const Widget& other) {} //no copying
};

class Label : public Widget
{
public:
	vec2i textOffset;
	Text text;
	bool center;
	Label(const char* t = "");
	virtual ~Label();
	void operator=(const char* str);
	void textf(const char* fmt, ...);
	virtual void update();
	virtual void drawContent(mat44 mat);
};

class Slider : public Label
{
public:
	bool castInt;
	bool showNumber;
	float f;
	int i;
	
	int sliderRail; //icon along which the bar slides
	int sliderBar; //the icon indicating the slider value
	
	int maxTextWidth;
	int barWidth;
	
	float upper;
	float lower;
	
	int* ptri;
	float* ptrf;
	
	string baseText;
	Slider(const char* t, float a, float b, bool decimal = false);
	virtual ~Slider();
	template<class T> Slider(const char* t, float a, float b, bool decimal, T* p);
	void defaults();
	virtual void packed();
	virtual void update();
	virtual void drag(vec2i pos);
};

class Button : public Label
{
public:
	vec2i textClickOffset;
	static const int imageMain = 3;
	static const int imageFocus = 2;
	static const int imageHover = 2;
	static const int imageClick = 1;
	Button(const char* t);
	virtual void update();
	virtual void mouseDown();
	virtual void mouseUp();
	virtual void mouseEnter();
	virtual void mouseLeave();
	virtual void drawContent(mat44 mat);
};

class CheckBox : public Button
{
public:
	vec2i iconSize;
	vec2i iconPos;
	bool b;
	bool* ptrb;
	CheckBox(const char* t);
	CheckBox(const char* t, bool* p);
	virtual ~CheckBox();
	int iconImage;
	void updateIcon();
	virtual void click();
	virtual void packed();
	virtual void update();
};

class RadioButton : public CheckBox
{
public:
	int index;
	int current;
	RadioButton(const char* t);
	virtual void onAdd();
	virtual void click();
	virtual void update();
	void set(int i = -1);
};

//not ported from old gui code yet
/*
class Image : public Widget
{
public:
	unsigned int type;
	unsigned int img;
	unsigned int* imgPtr;
	int face;
	Image();
	Image(unsigned int& i, unsigned int t = GL_TEXTURE_2D);
	void set(unsigned int i);
	void setType(unsigned int t);
	void setFace(int i);
	virtual void drawContent(mat44 mat);
};
*/

}

#endif
