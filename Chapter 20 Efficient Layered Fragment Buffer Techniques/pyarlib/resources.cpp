/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include "resources.h"

#ifdef _WIN32

#include <windows.h>
#include <tchar.h>
#include "resourcedefs.h"

#include <map>
#include <string>

//all this just for a null terminated string :(
std::map<std::string, std::string> nullTerminated;

const char* loadResource(const char* name)
{
	if (!name)
		return NULL;
	//std::cout << "loading resource: " << name << std::endl;

	//check if the cached null-termianted string exists
	std::map<std::string, std::string>::iterator it;
	it = nullTerminated.find(name);
	if (it != nullTerminated.end())
		return it->second.c_str();

	// :(
	WCHAR wname[64];
	int l = MultiByteToWideChar(0, 0, name, strlen(name), wname, 63);
	wname[l] = '\0';

	//more :(
	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, wname, _T("binary"));
	HGLOBAL rcData = LoadResource(handle, rc);
	int size = SizeofResource(handle, rc);

	//... vs, your resource embedding system disgusts me

	const char* dat = static_cast<const char*>(LockResource(rcData));

	//MUST EXIST!!
	assert(dat); //make sure you include resources.rc in your linked binary

	nullTerminated[name] = std::string(dat, size);
	return nullTerminated[name].c_str();
}

int loadResourceLen(const char* name)
{
	//check if the cached null-termianted string exists
	std::map<std::string, std::string>::iterator it;
	it = nullTerminated.find(name);
	if (it != nullTerminated.end())
		return it->second.size();

	loadResource(name);

	return nullTerminated[name].size();
}

#else

//linux doesn't need horrible loading functions

#endif
