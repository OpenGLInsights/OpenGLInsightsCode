/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef PROFILE_H
#define PROFILE_H

#define PROFILE_SAMPLES 20

#include <list>
#include <vector>
#include <map>
#include <string>

#include "includegl.h"

class Profile
{
	bool ready;
	typedef std::list<GLuint64> Times;
	struct Query
	{
		int timeDiff;
		Times times;
		Query();
	};
	typedef std::map<std::string, Query> QueryMap;
	std::vector<GLuint> queryObjs;
	std::vector<GLuint64> timeStamps;
	std::vector<std::string> queryOrder; //for toString
	QueryMap queries;
	
	int current;
	void restartQueries();
	GLuint getNextQuery();
public:
	Profile();
	void clear(); //removes all queries from name map
	void begin();
	void time(std::string name); //milliseconds
	float get(std::string name);
	std::vector<std::pair<std::string, float> > getAll();
	std::string toString();
};

#endif
