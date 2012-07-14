/*
Copyright (C) 2011 by Brano Kemen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef __ASYNC_VBO_TRANSFERS_APP_SCENARIO1_H__
#define __ASYNC_VBO_TRANSFERS_APP_SCENARIO1_H__

#include "base/app.h"

#include <memory>
#include <assert.h>

#include <glm/glm.hpp>

//struct frame_context;

class dxt_encode
	: public base::app
{
protected:
	//std::auto_ptr<frame_context> _frame_context;
	//std::auto_ptr<scene> _scene;

public:
	dxt_encode();
	virtual ~dxt_encode();

    virtual int get_wnd_width() const override { return 512; }
	virtual int get_wnd_height() const override { return 512; }

    virtual const char* get_app_name() const override { return "DXT Encode"; }
	virtual const char* get_wnd_name() const override { return "DXT Encode"; }
	virtual const char* get_wnd_cls() const override { return "dxte"; }

	virtual void start(const bool nogl=false) override;
	virtual void draw_frame() override;

private:
	dxt_encode(const dxt_encode&);
	void operator =(const dxt_encode&);

    void save_fbo_dxt5(const char* file);
    void save_fbo_rgba(const char* file);


    int _w, _h;

    void* _savedata;

    unsigned int _texorig, _texdxt, _texim, _texfin;
    unsigned int _vbodxt, _fbo;
    unsigned int _shv_unit, _shp_draw, _shp_dxt;
    unsigned int _prg_dxt, _prg_draw;

    int _up_image, _up_mode, _up_imagedraw, _up_modedraw, _up_to_srgb;
};

#endif // __ASYNC_VBO_TRANSFERS_APP_SCENARIO1_H__
