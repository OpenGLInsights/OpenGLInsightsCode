/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include <sys/stat.h> 

#include <string>
#include <fstream>

std::string basefilename(std::string filename)
{
	size_t p = filename.find_last_of(".");
	if ((int)p >= 0)
		return filename.substr(0, p);
	return filename;
}
std::string basefilepath(std::string filename)
{
	size_t p = filename.find_last_of("/");
	if ((int)p > 0)
		return filename.substr(0, p);
	return ".";
}
bool fileExists(const char* filename)
{ 
	struct stat info;
	return stat(filename, &info) == 0;
}

bool readFile(std::string& str, const char* filename)
{
	if (!filename) return false;
	std::ifstream ifile(filename, std::ios::in | std::ios::binary | std::ios::ate);
	if (!ifile.good()) return false;
	str.resize((unsigned int)ifile.tellg()); //reserve file size
	ifile.seekg(0); //go to start
	ifile.read((char*)&str[0], str.size());
	ifile.close();
	return true;
}

bool readUncomment(std::istream& stream, std::string& line)
{
	static bool commentBlock = false;
	if (!getline(stream, line))
		return false;
	
	//if currently within a block/c-style comment
	if (commentBlock)
	{
		int bcomment = line.find("*/");
		if (bcomment < 0)
		{
			line = "\n";
			return true;
		}
		else
			line = line.substr(bcomment+2);
		commentBlock = false;
	}
		
	//remove single line block/c-style comments
	while (true)
	{
		int first = line.find("/*");
		if (first < 0) break;
		int second = line.find("*/", first);
		if (second < 0) break;
		line = line.substr(0, first) + line.substr(second+2);
	}

	//remove "//" comments
	int lcomment = line.find("//");
	if (lcomment >= 0)
	{
		line = line.substr(0, lcomment);
	}
	
	//check for multiline block comments
	int bcomment = line.find("/*");
	if (bcomment >= 0)
	{
		line = line.substr(0, bcomment);
		commentBlock = true;
	}
	
	line += "\n";
	
	bool done = !stream.good();
	if (done) commentBlock = false;
	return !done;
}

