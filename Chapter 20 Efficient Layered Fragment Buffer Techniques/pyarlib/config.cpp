/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */
#include "prec.h"
#include "config.h"
#include "resources.h"

#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>

using namespace std;

EMBED(defaultConfig, default.cfg);

Config* Config::instance = NULL;

Config::Value Config::createNullValue()
{
	Value v;
	v.i = 0;
	v.f = 0.0f;
	return v;
}

Config::Config()
{
	ifstream ifile(CONFIG_NAME);
	
	//check if config opened
	if (!ifile.is_open())
	{
		//write default config
		ofstream ofile(CONFIG_NAME);
		printf("%i %s\n", RESOURCELEN(defaultConfig), RESOURCE(defaultConfig));
		ofile.write(RESOURCE(defaultConfig), RESOURCELEN(defaultConfig));
		ofile.close();
		
		//try again
		ifile.open(CONFIG_NAME);
		if (!ifile.is_open())
		{
			cout << "Error: Config file " << CONFIG_NAME << " does not exist and cannot be created." << endl;
			return;
		}
	}
	
	string line;
	while (getline(ifile, line))
	{
		//extract key/value lines
		int p = line.find_first_of(":");
		if (p < 0)
			continue;

		//get the key
		string key = line.substr(0, p);
		++p;

		//strip whitespace AND STUPID WINDOWS CARRIAGE RETURNS
		while (p < (int)line.size() && line[p] == ' ')
			++p;
		int e = line.size() - 1;
		while (line[e] == ' ' || line[e] == '\r')
			--e;
		line = line.substr(p, e - p + 1);
		
		Value value;
		value.str = line;
		stringstream reader(line);
		reader >> value.i; reader.seekg(0);
		reader >> value.f; reader.seekg(0);
		
		values[key] = value;
	}
}
Config& Config::getSingleton()
{
	if (!instance)
		instance = new Config();
	return *instance;
}
int Config::get(std::string name)
{
	ValueMap::iterator it = getSingleton().values.find(name);
	if (it != getSingleton().values.end())
		return it->second.i;
	cout << "Error: Missing config value for " << name << endl;
	getSingleton().values[name] = createNullValue(); //don't print error again
	return 0;
}
float Config::getFloat(std::string name)
{
	ValueMap::iterator it = getSingleton().values.find(name);
	if (it != getSingleton().values.end())
		return it->second.f;
	cout << "Error: Missing config value for " << name << endl;
	getSingleton().values[name] = createNullValue(); //don't print error again
	return 0.0f;
}
std::string Config::getString(std::string name)
{
	ValueMap::iterator it = getSingleton().values.find(name);
	if (it != getSingleton().values.end())
		return it->second.str;
	cout << "Error: Missing config value for " << name << endl;
	getSingleton().values[name] = createNullValue(); //don't print error again
	return "";
}


