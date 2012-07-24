#include "stdafx.h"
#include "commandLine.h"

// Get next word from command line
static void getNextWord(char **szCmdLine, char *word)
{
    while (isspace(**szCmdLine))
        ++(*szCmdLine);

    while (!isspace(**szCmdLine) && (**szCmdLine))
        *(word++) = *((*szCmdLine)++);

    *word = 0;
}

// Parse command line.
bool parseCommandLine(char *commandLine[], commandLineOptions *op)
{
    char word[100];

    op->_verbose = false;
    op->_stageMask = STAGE_UPLOAD_MASK | STAGE_RENDER_MASK | STAGE_READBACK_MASK;
	op->_numStages = 3;
    op->_numStreams = 2;
    op->_calibrationTime = 4.f;
    op->_runTime = 15.f;
    op->_maxIterations = 30;
    op->_inTexWidth = 4096;
    op->_inTexHeight = 2048;
	op->_outTexWidth = 4096;
    op->_outTexHeight = 2048;

    // Parse command line arguments
    while (*commandLine)
    {
        // Grab next token.
        getNextWord(commandLine, word);

        // Break at end of command line.
        if (strlen(word) == 0)
        {
            break;
        }
        else if (!_stricmp("-verbose", word))
        {
            op->_verbose = true;
        }
        else if (!_stricmp("-noUpload", word))
        {
            op->_stageMask &= ~STAGE_UPLOAD_MASK;
			op->_numStages--;
        }
        else if (!_stricmp("-noRender", word))
        {
            op->_stageMask &= ~STAGE_RENDER_MASK;
			op->_numStages--;
        }
        else if (!_stricmp("-noReadback", word))
        {
            op->_stageMask &= ~STAGE_READBACK_MASK;
			op->_numStages--;
        }
        else if (!_stricmp("-streams", word))
        {
            getNextWord(commandLine, word);
            op->_numStreams = atoi(word);
        }
        else if (!_stricmp("-runTime", word))
        {
            getNextWord(commandLine, word);
            op->_runTime = (float)atof(word);
        }
        else if (!_stricmp("-calibrationTime", word))
        {
            getNextWord(commandLine, word);
            op->_calibrationTime = (float)atof(word);
        }
        else if (!_stricmp("-maxIterations", word))
        {
            getNextWord(commandLine, word);
            op->_maxIterations = atoi(word);
            if(op->_maxIterations == 0)
                op->_maxIterations = 1;
            // user provided iterations turns off render calibration
        }
		else if (!_stricmp("-inSize", word))
        {
            getNextWord(commandLine, word);
            op->_inTexWidth = atoi(word);
            getNextWord(commandLine, word);
            op->_inTexHeight = atoi(word);
        }
        else if (!_stricmp("-outSize", word))
        {
            getNextWord(commandLine, word);
            op->_outTexWidth = atoi(word);
            getNextWord(commandLine, word);
            op->_outTexHeight = atoi(word);
        }
        else if (!_stricmp("-help", word))
        {
            char message[2048];

            sprintf_s(message, sizeof(message),
                "Copy Engine Benchmark\n"
                "Usage: CopyImageBench [-verbose] [-noUpload] [-noRender] [-noReadBack]\n"
                "                      [-streams n] [-runTime s] [-calibrationTime s]\n"
                "                      [-maxIterations n] [-size x y]\n"
                " -verbose              print messages\n"
                " -noUpload             don't do the upload stage\n"
                " -noRender             don't do the render stage\n"
                " -noReadBack           don't do the read back stage\n"
                " -streams n            use 'n' streams (default %d)\n"
                " -runTime s            when in benchmark mode run for 's' seconds\n"
                "                       (default %2.1f)\n"
                " -calibrationTime s    wait for 's' seconds before start measuring\n"
                "                       (default %2.1f)\n"
                " -maxIterations n      mandelbrot iterations (default %d), if this is not set\n"
                "                       automatic calibration is done\n"
                " -inSize x y          uploaded texture size (default %d %d)\n",
                " -outSize x y             readback texture size (default %d %d)\n",
                op->_numStreams,
                op->_runTime,
                op->_calibrationTime,
                op->_maxIterations,
                op->_inTexWidth,
                op->_inTexHeight,
                op->_outTexWidth,
                op->_outTexHeight
				);
            printf(message);
            MessageBox(NULL, message, "Infomation", MB_OK);
            exit(0);
        }
        else
        {
            return false;
        }
    }

    return true;
}
