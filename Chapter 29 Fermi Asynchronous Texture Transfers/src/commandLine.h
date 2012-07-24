#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

/// Command line options
typedef struct
{
    bool _verbose;                  ///< print messages
    unsigned int _stageMask;        ///< enabled stages
    unsigned int _numStreams;          ///< streams to use
	unsigned int _numStages;			//how many stages are there in this pipeline
    float _runTime;                 ///< seconds to run when in benchmark mode
    float _calibrationTime;         ///< seconds to wait before start measuring
	unsigned int _maxIterations;    ///< mandelbrot iterations
    unsigned int _inTexWidth;         ///< input texture width
    unsigned int _inTexHeight;        ///< input texture height
    unsigned int _outTexWidth;         ///< output texture width
    unsigned int _outTexHeight;        ///< output texture height
} commandLineOptions;

#define STAGE_UPLOAD_INDEX      0
#define STAGE_UPLOAD_MASK       (1 << STAGE_UPLOAD_INDEX)

#define STAGE_RENDER_INDEX      1
#define STAGE_RENDER_MASK       (1 << STAGE_RENDER_INDEX)

#define STAGE_READBACK_INDEX    2
#define STAGE_READBACK_MASK     (1 << STAGE_READBACK_INDEX)

bool parseCommandLine(char *szCmdLine[], commandLineOptions *op);

#endif // COMMAND_LINE_H