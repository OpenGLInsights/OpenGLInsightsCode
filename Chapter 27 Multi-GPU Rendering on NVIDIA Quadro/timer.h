#ifndef Timer_H
#define Timer_H

#include "commandLine.h"
/// Handle time measurement
class Timer
{
private:
    commandLineOptions _options;

    LARGE_INTEGER _frequency;
    LARGE_INTEGER _startCPUTime;

    unsigned int _frames;

    float _frameTimeCPU;

    // this is a singleton, hide constructors etc.
    Timer();
    Timer(const Timer&);
    Timer& operator=(const Timer&);
    ~Timer();

public:
    /// Get an instance of this class
    ///
    /// @return instance
    static Timer &getInstance()
    {
        static Timer instance; 

        return instance; 
    }

    bool init(const commandLineOptions *options);

    void start();
    bool update();

    void information(char *string, size_t stringlen);

};

#endif // Timer_H