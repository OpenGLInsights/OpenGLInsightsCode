/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#ifndef PYARLIB_CONFIG_H
#define PYARLIB_CONFIG_H

#define CONFIG_NAME "config.cfg"

#include <map>
#include <string>

class Config
{
private:
	struct Value
	{
		std::string str;
		int i;
		float f;
	};
	typedef std::map<std::string, Value> ValueMap;
	ValueMap values;
	Config();
	static Value createNullValue();
	static Config* instance;
	static Config& getSingleton();
public:
	static int get(std::string name);
	static float getFloat(std::string name);
	static std::string getString(std::string name);
};

#endif
