/** ----------------------------------------------------------
 * \class VSLogLib
 *
 * Lighthouse3D
 *
 * VSLogLib - Very Simple Log Library
 *
 * \version 0.1.0
 * Initial Release
 *
 * This class provides a basic logging mechanism
 *
 * Full documentation at 
 * http://www.lighthouse3d.com/very-simple-libs
 *
 ---------------------------------------------------------------*/

#include "vsLogLib.h"


VSLogLib::VSLogLib(): pStreamEnabled(false) {

}

// cleans up
VSLogLib::~VSLogLib() {

	pLogVector.clear();
}


// clears the log
void
VSLogLib::clear() {

	pLogVector.clear();
}

// adds a message, printf style
void
VSLogLib::addMessage(std::string s, ...) {

	va_list args;
	va_start(args,s);
	vsnprintf( pAux, 256, s.c_str(), args );
	//vsprintf(pAux,s.c_str(), args);
	va_end(args);
	if (pStreamEnabled)
		*pOuts << pAux << "\n";
	else
		pLogVector.push_back(pAux);
}

// dumps the log contents to a file
void 
VSLogLib::dumpToFile(std::string filename) {

	std::ofstream file;
	file.open(filename.c_str());

	for (unsigned int i = 0; i < pLogVector.size(); ++i) {
		file << pLogVector[i] << "\n";
	}
	file.close();
}

// dumps the log contents to a string
std::string
VSLogLib::dumpToString() {

	pRes = "";

	for (unsigned int i = 0; i < pLogVector.size(); ++i) {

		pRes += pLogVector[i] + "\n";
	}

	return pRes;
}


void 
VSLogLib::disableStream() {

	pStreamEnabled = false;
}


void 
VSLogLib::enableStream(std::ostream *outStream) {

	// set the output stream
	if (!outStream)
		pOuts = (std::iostream *)&std::cout;
	else 
		pOuts = outStream;
	
	pStreamEnabled = true;
}