/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

//shotgun!
//ls *.h -l | awk '{print $9}' | sed 's/\(.*\)/#include "\1"/' | sort > pyarlib.h

#ifndef PYARLIB_H
#define PYARLIB_H

//pyarlib version 1.0
#define PYARLIB_VERSION_MAJOR 1
#define PYARLIB_VERSION_MINOR 0

#include "prec.h"

#include "atlas.h"
#include "camera.h"
#include "fileutil.h"
#include "gpu.h"
#include "guifps.h"
#include "img.h"
#include "imgpng.h"
#include "immediate.h"
#include "includegl.h"
#include "jeltzfly.h"
#include "jeltzgui.h"
#include "jeltz.h"
#include "loader.h"
#include "matrix.h"
#include "matstack.h"
#include "meshctm.h"
#include "meshobj.h"
#include "mesh3ds.h"
#include "material.h"
#include "model.h"
#include "ninebox.h"
#include "png_loader.h"
#include "profile.h"
#include "quaternion.h"
#include "quickgui.h"
#include "shaderbuild.h"
#include "shader.h"
#include "shaderutil.h"
#include "text.h"
#include "texture.h"
#include "thread.h"
#include "util.h"
#include "vbomesh.h"
#include "vec.h"

#endif
