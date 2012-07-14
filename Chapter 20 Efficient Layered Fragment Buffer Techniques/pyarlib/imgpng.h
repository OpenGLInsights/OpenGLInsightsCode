/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef QI_IMG_PNG_H
#define QI_IMG_PNG_H

#include "img.h"

namespace QI
{
	struct ImagePNG : Image
	{
		ImagePNG();
		ImagePNG(std::string filename);
		virtual bool loadImage(std::string filename);
		virtual bool saveImage(std::string filename);
	};
}

#endif
