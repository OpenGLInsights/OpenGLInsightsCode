/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef PREFIX_SUMS_H
#define PREFIX_SUMS_H

class ImageBuffer;

//turns the unsigned ints within data into their prefix sums
//and returns the total sum. it is expected that data be of size 2^n
unsigned int prefixSums(ImageBuffer* data);

#endif
