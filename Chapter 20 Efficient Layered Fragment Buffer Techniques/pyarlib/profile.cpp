/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include "includegl.h"
#include "profile.h"
#include "shader.h"

#include <assert.h>

Profile::Query::Query()
{
	timeDiff = -1;
}
Profile::Profile()
{
	ready = false;
	current = 0;
}
void Profile::clear()
{
	//FIXME: could free and clear queryObjs too
	ready = false;
	queries.clear();
	queryOrder.clear();
}
void Profile::restartQueries()
{
	current = 0;
	queryOrder.clear();
}
GLuint Profile::getNextQuery()
{
	if (current > 50)
	{
		printf("Error: too many queries. Did you forget to call Profile::begin()?\n");
		return 0;
	}

	//allocate another query if needed
	GLuint query;
	if ((int)queryObjs.size() < current + 1)
	{
		glGenQueries(1, &query);
		queryObjs.push_back(query);
	}
	else
		query = queryObjs[current];
	current++;
	return query;
}
void Profile::begin()
{
	assert(!CHECKERROR);
	
	if (ready)
	{
		ready = false;
	}

	if (queries.size())
	{
		//if the last query is done, they are all done
		GLint available;
		glGetQueryObjectiv(queryObjs[current-1], GL_QUERY_RESULT_AVAILABLE, &available);
		if (available)
		{
			//get the results
			timeStamps.resize(queryObjs.size());
			for (int i = 0; i < (int)queryObjs.size(); ++i)
			{
				CHECKERROR;
				GLuint64 result;
				glGetQueryObjectui64vEXT(queryObjs[i], GL_QUERY_RESULT, &result);
				timeStamps[i] = result;
				CHECKERROR;
			}
			
			//append the differences to the samples list
			for (QueryMap::iterator it = queries.begin(); it != queries.end(); ++it)
			{
				if (it->second.timeDiff < 1) continue;
				GLuint64 timediff = timeStamps[it->second.timeDiff] - timeStamps[it->second.timeDiff-1];
				it->second.times.push_back(timediff);
				if (it->second.times.size() > PROFILE_SAMPLES)
					it->second.times.pop_front();
				it->second.timeDiff = -1;
			}
			
			//start the next set of samples
			ready = true;
			restartQueries();
		}
	}
	else
		ready = true;
	
	if (ready)
	{
		glQueryCounter(getNextQuery(), GL_TIMESTAMP);
	}
}
void Profile::time(std::string name)
{
	assert(!CHECKERROR);
	
	if (ready)
	{
		//create query struct for "name" if there isn't one
		QueryMap::iterator q = queries.find(name);
		if (q == queries.end())
		{
			std::pair<QueryMap::iterator, bool> ret;
			ret = queries.insert(make_pair(name, Query()));
			q = ret.first;
		}
		
		//if "name" has already been set after begin(), ignore this call
		if (q->second.timeDiff > 0)
			return;
		
		//attach this timediff to "name"
		q->second.timeDiff = current;
		queryOrder.push_back(q->first);
		
		//move to the next query
		CHECKERROR;
		glQueryCounter(getNextQuery(), GL_TIMESTAMP);
		//NOTE: if this causes an error it's probably because you're using AMD
		//my guess is AMD won't allow two querys at the same time even of differing types
		CHECKERROR;
	}
}
float Profile::get(std::string name)
{
	QueryMap::iterator q = queries.find(name);
	if (q == queries.end() || q->second.times.size() == 0)
		return -1.0f;
	
	Times& t = q->second.times;
	GLuint64 result = 0;
	for (Times::iterator it = t.begin(); it != t.end(); ++it)
		result += *it;
	return (float)((double)result / (t.size() * 1000000));
}

std::vector<std::pair<std::string, float> > Profile::getAll()
{
	std::vector<std::pair<std::string, float> > ret;
	ret.reserve(queries.size());
	for (QueryMap::iterator it = queries.begin(); it != queries.end(); ++it)
	{
		ret.push_back(make_pair(it->first, get(it->first)));
	}
	return ret;
}
std::string Profile::toString()
{
	std::stringstream s;
	for (int i = 0; i < (int)queryOrder.size(); ++i)
	{
		QueryMap::iterator it = queries.find(queryOrder[i]);
		if (it != queries.end())
			s << it->first << ": " << get(it->first) << "\n";
	}
	return s.str();
}
