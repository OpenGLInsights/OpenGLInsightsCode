/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef FILEUTIL_H
#define FILEUTIL_H

std::string basefilename(std::string filename);
std::string basefilepath(std::string filename);
bool fileExists(const char* filename);
bool readFile(std::string& str, const char* filename);
bool readUncomment(std::istream& stream, std::string& line);

#endif
