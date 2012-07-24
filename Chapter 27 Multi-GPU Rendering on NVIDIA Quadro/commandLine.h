#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

/// Command line options
typedef struct
{
    bool _verbose;                  ///< print messages
	bool _singleGPU;				///< if in singleGPU mode
    unsigned int _numStreams;          ///< streams to use
    float _runTime;                 ///< seconds to run when in benchmark mode
    float _calibrationTime;         ///< seconds to wait before start measuring
    unsigned int _texWidth;         ///< offscreen texture width
    unsigned int _texHeight;        ///< offscreen texture height
} commandLineOptions;

bool parseCommandLine(char *szCmdLine[], commandLineOptions *op);

#endif // COMMAND_LINE_H