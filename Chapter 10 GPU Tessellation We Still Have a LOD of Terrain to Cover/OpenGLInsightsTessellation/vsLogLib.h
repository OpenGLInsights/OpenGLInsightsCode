/** ----------------------------------------------------------
 * \class VSLogLib
 *
 * Lighthouse3D
 *
 * VSLogLib - Very Simple Log Library
 *
 * \version 0.2.0
 *  - added streams 
 *  - usage of a define makes it possible to remove all
 *  logging from the application easily
 *	
 * \version 0.1.0
 *  - Initial Release
 *
 * This class provides a basic logging mechanism
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 ---------------------------------------------------------------*/

#ifndef __VSLogLib__
#define __VSLogLib__

#ifndef VSL_MODE
#define VSL_DEBUG 1
#define VSL_RELEASE 0
// set this value to VS_RELEASE to disable logging
#define VSL_MODE VSL_DEBUG
#endif

#include <vector>
#include <string>
#include <iostream>
#include <ostream>
#include <fstream>
#include <stdarg.h>


class VSLogLib {

public:

	VSLogLib();
	~VSLogLib();

	/// set an output stream
	void enableStream(std::ostream *outStream);
	/// disable output stream, keep messages in the log
	void disableStream();

	/**  Add a message, printf style
	  * \param format the same as the first parameter of printf
	  * \param ... the remaining params of printf
	*/
	void addMessage(std::string format, ...);

	/// Writes the log to a file
	void dumpToFile(std::string filename);

	/// returns a string with the logs contents
	std::string dumpToString();

	/// clear the log
	void clear();

private:

	/// The log itself
	std::vector<std::string> pLogVector;
	/// just a string to return values
	std::string pRes;
	/// aux string to avoid malloc/dealloc
	char pAux[256];
	/// the output stream
	std::ostream *pOuts;
	/// stream enabled status
	bool pStreamEnabled;

};

// This macro allow a simple usage of any log
// and when undefined it will remove all calls
// from the application
#if VSL_MODE == VSL_DEBUG
#define VSLOG(log, message, ...) \
{\
	(log.addMessage(message, ## __VA_ARGS__));\
};
#else
#define VSLOG(log, message, ...)
#endif


#endif