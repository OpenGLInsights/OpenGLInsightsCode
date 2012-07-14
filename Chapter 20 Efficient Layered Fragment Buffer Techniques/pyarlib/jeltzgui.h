/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef JELTZ_GUI
#define JELTZ_GUI

#include "quickgui.h"

#include "jeltz.h"
#include "guifps.h"

class JeltzGUI : public JeltzPlugin
{
private:
	virtual void init();
	virtual void update(float dt);
	virtual void display();
	virtual void cleanup();
	bool ignoring;
	mat44 modelview;
public:
	void ignoreNextTime(); //use after a load() call to stop spikes in framerate
	bool visible;
	QG::Widget body; //horizontal packing for columns
	QG::Widget controls; //the public main widget class. usage eg: jeltzGui.controls.add(new QG::Label(...));
	QG::FPSGraph fps;
	QG::Label fpsNum;
	JeltzGUI();
	virtual ~JeltzGUI();
};

#endif
