/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


//since quickglut is deprecated, this will be the new FPS display, built on top of quickgui

#ifndef GUI_FPS
#define GUI_FPS

#define GUIFPS_NUM_SAMPLES 20
#define GUIFPS_MIN_TIME 0.6
#define GUIFPS_VERTICAL_BARS 5

#define NINEBOX_BACKGROUND "graph"

#include "quickgui.h"

namespace QG
{

extern NineBoxPool fpsgraphBox;

struct FPSGraph : public Widget
{
	bool gotNewSample; //true if last update() took a tpf sample
	bool print;
	float time;
	float frames;
	float fpsLast;
	float tpfLast;
	float tpfAverage;
	float tpfMedian;
	float tpfMaximum;
	float tpfMinimum;
	float heightScale;
	float times[GUIFPS_NUM_SAMPLES]; //cyclical list
	int currentSample;

	FPSGraph();
	void updateStats();
	void update(float dt); //the interface
	
	virtual void update();
	virtual void drawContent(mat44 mat);
};

}

#endif
