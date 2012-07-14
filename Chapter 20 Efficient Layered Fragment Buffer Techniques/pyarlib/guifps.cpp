/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <set>

#include <memory.h>
#include <stdio.h>
#include <algorithm>

#include "guifps.h"

#include <GL/gl.h>

namespace QG
{

static float sortedTimes[GUIFPS_NUM_SAMPLES];

FPSGraph::FPSGraph()
{
	for (currentSample = 0; currentSample < GUIFPS_NUM_SAMPLES; ++currentSample)
	{
		times[currentSample] = 0.0f;
	}
	time = 0.0f;
	frames = 0;
	width = 100;
	height = 100;
	padding = 0;
	border = 8;
	margin = 0;
	
	fill = QG::NONE;
	
	heightScale = 1.0f;
	
	tpfLast = 0.0f;
	fpsLast = 0.0f;
	tpfAverage = 0.0f;
	tpfMedian = 0.0f;
	tpfMaximum = 0.0f;
	tpfMinimum = 0.0f;
	
	print = false;
	gotNewSample = false;
}

void FPSGraph::updateStats()
{
	memcpy(sortedTimes, times, GUIFPS_NUM_SAMPLES * sizeof(float));
	std::sort(sortedTimes, sortedTimes + GUIFPS_NUM_SAMPLES);
	
	tpfAverage = 0.0f;
	tpfMaximum = sortedTimes[0];
	tpfMinimum = sortedTimes[0];
	for (int i = 0; i < GUIFPS_NUM_SAMPLES; ++i)
	{
		tpfAverage += sortedTimes[i];
		if (tpfMaximum < sortedTimes[i]) tpfMaximum = sortedTimes[i];
		if (tpfMinimum > sortedTimes[i]) tpfMinimum = sortedTimes[i];
	}
	tpfAverage /= GUIFPS_NUM_SAMPLES;
	tpfMedian = sortedTimes[GUIFPS_NUM_SAMPLES/2];
}

void FPSGraph::update(float dt)
{
	time += dt;
	++frames;
	if (time > GUIFPS_MIN_TIME)
	{
		tpfLast = time / frames;
		fpsLast = frames / time;
		time = 0.0f;
		frames = 0;
		
		currentSample = (currentSample + 1) % GUIFPS_NUM_SAMPLES;
		times[currentSample] = tpfLast;
		
		updateStats();
		
		if (print)
			printf("%.2ffps %.2fms\n", 1.0/tpfLast, 1000.0*tpfLast);
		
		gotNewSample = true;
	}
	else
		gotNewSample = false;
	
	if (tpfMaximum > 0)
	{
		float step = dt * 10000.0;
		float targetHeight = 0.6666 / tpfMaximum;
		float to = targetHeight - heightScale;
		if (fabs(to) > step)
			heightScale += step * to / fabs(to);
		else
			heightScale = targetHeight;
	}
}

void FPSGraph::update()
{
	if (nineboxImage < 0)
	{
		nineboxImage = nineboxPool.create(NINEBOX_BACKGROUND);
	}
	Widget::update();
}

void FPSGraph::drawContent(mat44 mat)
{
	mygl.projection = mat;
	
	const float fps30 = 1.0/30.0;
	const float fps60 = 1.0/60.0;
	glEnable(GL_BLEND);
	
	float scalew = width / (float)(GUIFPS_NUM_SAMPLES-1);
	float offsetx = - time / GUIFPS_MIN_TIME;
	
	//graph grid
	mygl.colour(0, 1, 0, 0.3);
	mygl.begin(Immediate::LINES);
	const int vbarFreq = GUIFPS_NUM_SAMPLES / GUIFPS_VERTICAL_BARS;
	for (int i = vbarFreq - currentSample % vbarFreq; i < GUIFPS_NUM_SAMPLES; i += vbarFreq)
	{
		float x = i + offsetx;
		if (x < 0.0f) x += 1.0f;
		mygl.vertex(x*scalew, 0);
		mygl.vertex(x*scalew, height);
	}
	mygl.end(false);
	 
	//horizontal bars at 30 and 60 fps
	mygl.colour(0, 1, 0, 0.5);
	mygl.begin(Immediate::LINES);
	if (fps30 * heightScale <= 1.0)
	{
		mygl.vertex(0, height - fps30 * heightScale * height);
		mygl.vertex(width, height - fps30 * heightScale * height);
	}
	if (fps60 * heightScale <= 1.0)
	{
		mygl.vertex(0, height - fps60 * heightScale * height);
		mygl.vertex(width, height - fps60 * heightScale * height);
	}
	mygl.end(false);
		
	//graph line
	mygl.colour(0, 1, 0, 0.5);
	mygl.begin(Immediate::LINE_STRIP);
	for (int i = 0; i < GUIFPS_NUM_SAMPLES; ++i)
	{
		float t = times[(currentSample + 1 + i) % GUIFPS_NUM_SAMPLES];
		float x = i + offsetx;
		if (x < 0.0f) x = 0.0f;
		mygl.vertex(x*scalew, height - t * heightScale * height);
	}
	mygl.end(false);
	
	//plot median and average
	mygl.begin(Immediate::LINES);
	if (tpfAverage * heightScale <= 1.0)
	{
		mygl.colour(0, 0.5, 1, 0.5);
		mygl.vertex(0, height - tpfAverage * heightScale * height);
		mygl.vertex(width, height - tpfAverage * heightScale * height);
	}
	if (tpfMedian * heightScale <= 1.0)
	{
		mygl.colour(1, 0.5, 0.5, 0.5);
		mygl.vertex(0, height - tpfMedian * heightScale * height);
		mygl.vertex(width, height - tpfMedian * heightScale * height);
	}
	mygl.end(false);
	
	//plot points
	mygl.colour(0, 1, 0, 1);
	mygl.begin(Immediate::POINTS);
	for (int i = 0; i < GUIFPS_NUM_SAMPLES; ++i)
	{
		float t = times[(currentSample + 1 + i) % GUIFPS_NUM_SAMPLES];
		if (t > fps30)
			mygl.colour(1, 0, 0, 1);
		else
			mygl.colour(0, 1, 0, 1);
		float x = i + offsetx;
		if (x < 0.0f) x = 0.0f;
		mygl.vertex(x*scalew, height - t * heightScale * height);
	}
	mygl.end(false);
	
	mygl.flush();
}


}


