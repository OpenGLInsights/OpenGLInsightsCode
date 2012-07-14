README.txt Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt)

1. INTRODUCTION
===============

This program, lfb, implements a number of
Layered Fragment Buffer techniques to
demonstrate their performance.

The important files are:

	main.cpp - LFB implementation usage
	
	lfbRender.frag - render to lfb
	lfbDisplay.frag - read from lfb (and resolve transparency)
	
	lfb.h/cpp/glsl - base LFB class
		uses lfbZero.vert
		uses sorting.glsl
	lfbLinear.h/cpp/glsl - Linearized LFB implementation
		uses prefixSums.h/cpp/vert
		uses lfbClear.vert
	lfbLinked.h/cpp/glsl - Linked List LFB implementation
	lfbPages.h/cpp/glsl - Linked Pages LFB implementation
	lfbBasic.h/cpp/glsl - Brute Force LFB implementation

This demo uses pyarlib, which is a jumble of common,
sometimes reusable, code. Pyarlib is provides an
OpenGL demo framework with camera controls, GUI,
profiling and a few other utilities such as shader
management and preprocessing.

Pyarlib is not directly related to the LFB
code above and can be ignored.

2. COMPILING
============

Note: REQUIRES OpenGL 4.2
tested on NVIDIA GeForce 460, 560, 580 and Quadro 4000

The /lfb/vs2008/ and /lfb/vs2010/ directories contain
visual studio solution and project files.
The output, lfb.exe, is placed in /lfb/
Note that service pack 1 is needed for
visual studio 2008.

The /lfb/Makefile is linux only.
Linux versions of library dependencies
are not included and must be installed first.
To swap between using SDL 1.2 and 1.3,
alter "-lSDL13" in the Makefile and the
line "#include <SDL13/SDL.h>" in
/lfb/pyarlib/jeltz.h

SDL 1.3 source can be found here:
http://www.libsdl.org/hg.php

3. LICENSE
==========

This program is distributed under the terms of the GNU LGPL license.
See LICENSE.txt for details

4. CREDITS
==========

This program uses the following libraries, unmodified:
FreeType
	Copyright © 1996-2011 The FreeType
	Project (www.freetype.org).  All rights reserved.
	David Turner, Robert Wilhelm, and Werner Lemberg.
OpenGL Extension Wrangler (GLEW)
	Copyright (C) 2002-2007, Milan Ikits <milan ikits[]ieee org>
	Copyright (C) 2002-2007, Marcelo E. Magallon <mmagallo[]debian org>
	Copyright (C) 2002, Lev Povalahev
	All rights reserved.
Simple DirectMedia Layer (SDL)
	Copyright (C) 1997-2011 Sam Lantinga <slouken@libsdl.org>
libpng
	Version 1.2.37 Copyright (c) 1998-2009 Glenn Randers-Pehrson
	Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger
	Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.
zlib
	Copyright (C) 1995-2005 Jean-loup Gailly and Mark Adler
OpenCTM
	Copyright (c) 2009-2010 Marcus Geelnard

