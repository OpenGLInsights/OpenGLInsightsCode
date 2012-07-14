/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <set>

#include "jeltzgui.h"
#include "texture.h"
#include "profile.h"
#include "util.h"
#include "config.h"

#include <GL/gl.h>

static Profile profile;

JeltzGUI::JeltzGUI()
{
	visible = true;
	body.fill = QG::NONE;
	body.add(controls);
	body.stack = QG::HORIZONTAL;
	controls.add(fps);
	controls.add(fpsNum);
	controls.fill = QG::BOTH;
	fpsNum.fill = QG::NONE;
	fpsNum.border = 8;
	fpsNum.width = fps.width - fpsNum.padding.left - fpsNum.padding.right;
	ignoring = false;
}
JeltzGUI::~JeltzGUI()
{
}
void JeltzGUI::ignoreNextTime()
{
	ignoring = true;
}
void JeltzGUI::init()
{
/*
	QG::widgetBox.setImage(new QG::NineBoxImage("pyarlib/img/button.png", 38, 4));
	QG::labelBox.setImage(new QG::NineBoxImage("pyarlib/img/label.png", 38, 1));
	QG::buttonBox.setImage(new QG::NineBoxImage("pyarlib/img/button.png", 38, 4));
	QG::checkBox.setImage(new QG::NineBoxImage("pyarlib/img/check.png", 0, 2));
	QG::radioBox.setImage(new QG::NineBoxImage("pyarlib/img/radio.png", 0, 2));
	
	QG::fpsgraphBox.setImage(new QG::NineBoxImage("pyarlib/img/fps.png", 8, 1));
*/
	controls.loadImages(Config::getString("gui"));
}
void JeltzGUI::update(float dt)
{
	if (!ignoring && jeltz->focused())
		fps.update(dt);

	profile.time("GPUStuff");
	if (!ignoring)
		profile.begin();
	
	ignoring = false;
	
	if (fps.gotNewSample)
	{
		float allTime = fps.tpfLast * 1000.0f;
		float gpuTime = profile.get("GPUStuff");
		float gpuRatio = myclamp(gpuTime / allTime, 0.0f, 1.0f);
		//fpsNum.textf("fps: %.1f\nms: %.2f\ngpu: %.1f%%", fps.fpsLast, allTime, gpuRatio * 100.0f);
		fpsNum.textf("fps: %.1f\nms: %.2f", fps.fpsLast, allTime, gpuRatio * 100.0f);
	}
	
	if (jeltz->resized())
	{
		body.width = jeltz->winSize().x;
		body.height = jeltz->winSize().y;
		body.setDirty();
		modelview =
			mat44::translate(-1.0, 1.0, 0.0) *
			mat44::scale(1, -1, 1) *
			mat44::scale(2.0f/jeltz->winSize().x, 2.0f/jeltz->winSize().y, 1);
	}
	
	if (visible)
	{
		bool handled = false;
		if (jeltz->buttonDown("LButton"))
			if (body.mouseDown(jeltz->mousePos()))
				handled = true;
			
		if (jeltz->mouseMove() != vec2i(0))
			if (body.mouseMove(jeltz->mousePos()))
				handled = true;
			
		if (jeltz->buttonUp("LButton"))
			body.mouseUp(jeltz->mousePos());
		
		//FIXME: don't really like this - there's no control over which LButton event gets "used"
		if (handled)
			jeltz->use("LButton");
	}
}

void JeltzGUI::display()
{
	if (!visible)
		return;
	
	glPushAttrib(GL_ENABLE_BIT);
	glPushAttrib(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	QG::Widget::draw(modelview);
	CHECKERROR;
	
	glPopAttrib();
	glPopAttrib();
}

void JeltzGUI::cleanup()
{
	QG::Widget::releaseAll();
}

