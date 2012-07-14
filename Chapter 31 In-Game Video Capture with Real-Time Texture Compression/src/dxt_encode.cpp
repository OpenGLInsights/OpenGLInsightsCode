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

#include "dxt_encode.h"
#include "base/base.h"
#include "base/tga_utils.h"
#include "base/base.h"

#include "dds.h"

#include <glm/gtc/matrix_transform.hpp>

#define LINEAR 0

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

dxt_encode::dxt_encode() : app(), _savedata(0)
{}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

dxt_encode::~dxt_encode() {}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void dxt_encode::start(const bool nogl)
{
	app::start();

    //_texorig = base::create_texture_from_file(SRC_LOCATION, "images/test2.tga", LINEAR);
    //_texorig = base::create_texture_from_file(SRC_LOCATION, "images/test-n.tga", LINEAR);
    _texorig = base::create_texture_from_file(SRC_LOCATION, "images/test-n.tga", LINEAR);
    //_texorig = base::create_texture_from_file(SRC_LOCATION, "images/test-small-orig.tga");
    //_texorig = base::create_texture_from_file(SRC_LOCATION, "images/kodim03.tga", LINEAR);

    glBindTexture(GL_TEXTURE_2D, _texorig);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &_w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &_h);
    glBindTexture(GL_TEXTURE_2D, 0);

    _texim = base::create_texture(_w/4, _h/4, base::PF_RGBA32UI, 0);
    _texdxt = base::create_texture(_w, _h, base::PF_DXT5, 0);
    _texfin = base::create_texture(_w, _h, base::PF_RGBA8, 0);

    glBindTexture(GL_TEXTURE_2D, _texfin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);


    glGenBuffers(1, &_vbodxt);
  	glBindBuffer(GL_ARRAY_BUFFER, _vbodxt);
	glBufferData(GL_ARRAY_BUFFER, _w*_h, 0, GL_STREAM_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


    _shv_unit = base::create_and_compile_shader(SRC_LOCATION, "shaders/vs.glsl", GL_VERTEX_SHADER);
    _shp_draw = base::create_and_compile_shader(SRC_LOCATION, "shaders/fs.glsl", GL_FRAGMENT_SHADER);
    _shp_dxt  = base::create_and_compile_shader(SRC_LOCATION, "shaders/dxt.glsl", GL_FRAGMENT_SHADER);

    _prg_dxt = base::create_program(_shv_unit, 0, _shp_dxt);
    base::link_program(SRC_LOCATION, _prg_dxt);

    _up_image = glGetUniformLocation(_prg_dxt, "image");
    _up_mode = glGetUniformLocation(_prg_dxt, "mode");

    _prg_draw = base::create_program(_shv_unit, 0, _shp_draw);
    base::link_program(SRC_LOCATION, _prg_draw);

    _up_imagedraw = glGetUniformLocation(_prg_draw, "image");
    _up_modedraw = glGetUniformLocation(_prg_draw, "mode");
    _up_to_srgb = glGetUniformLocation(_prg_draw, "to_srgb");


    glGenFramebuffers(1, &_fbo);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void dxt_encode::save_fbo_dxt5(const char* file)
{
    if(!_savedata)
        _savedata = ::malloc(_w*_h);

    glReadPixels(0, 0, _w/4, _h/4, GL_RGBA_INTEGER, GL_UNSIGNED_INT, _savedata);

    dds::save_texture(file, _w, _h, 1, base::PF_DXT5, _savedata);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void dxt_encode::save_fbo_rgba(const char* file)
{
    if(!_savedata)
        _savedata = ::malloc(_w*_h*4);

    glReadPixels(0, 0, _w, _h, GL_RGBA, GL_UNSIGNED_BYTE, _savedata);

    dds::save_texture(file, _w, _h, 1, base::PF_RGBA8, _savedata);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void dxt_encode::draw_frame()
{
    static int mode = 0;

    if(mode < 4)
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        //render to an intermediate uint4 texture
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texim, 0);
        glDisable(GL_FRAMEBUFFER_SRGB);

        glUseProgram(_prg_dxt);

        glUniform1i(_up_mode, mode);

        glUniform1i(_up_image, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texorig);

        glViewport(0, 0, _w/4, _h/4);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


        //read to vbo
        glReadBuffer(GL_COLOR_ATTACHMENT0);
	    glBindBuffer(GL_PIXEL_PACK_BUFFER, _vbodxt);
	    glReadPixels(0, 0, _w/4, _h/4, GL_RGBA_INTEGER, GL_UNSIGNED_INT, 0);
	    glBindBuffer(GL_PIXEL_PACK_BUFFER,0);
/*
        char fname[] = "testX.dds";
        fname[4] = '0'+mode;
        save_fbo_dxt5(fname);*/

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


        //copy to dxt tex
	    glBindTexture(GL_TEXTURE_2D, _texdxt);
	    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _vbodxt);

	    glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _w, _h,
            GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
            _w*_h, 0);

	    glBindTexture(GL_TEXTURE_2D, 0);
	    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);


        //draw to final texture
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texfin, 0);
        //if(LINEAR)
        //    glEnable(GL_FRAMEBUFFER_SRGB);

        glUseProgram(_prg_draw);

        glUniform1i(_up_modedraw, mode);
        glUniform1i(_up_to_srgb, LINEAR);

        glUniform1i(_up_imagedraw, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texdxt);

        glViewport(0, 0, _w, _h);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        {
#if LINEAR
            char fname[] = "test-lX.dds";
#else
            char fname[] = "test-sX.dds";
#endif
            fname[6] = char('0'+mode);
            save_fbo_rgba(fname);
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }


    //draw to screen
    glUseProgram(_prg_draw);

    glUniform1i(_up_modedraw, -1);
    glUniform1i(_up_to_srgb, 0);

    glUniform1i(_up_imagedraw, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texfin);

    glViewport(0, 0, get_wnd_width(), get_wnd_height());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    ++mode;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
