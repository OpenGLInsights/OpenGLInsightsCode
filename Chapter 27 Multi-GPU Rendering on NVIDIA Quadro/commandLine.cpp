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

	op->_verbose = true;
	op->_numStreams = 2;
	op->_singleGPU = false;
	op->_calibrationTime = 4.f;
	op->_runTime = 15.f;
	op->_texWidth = 2048;
	op->_texHeight = 1024;

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
        else if (!_stricmp("-singleGPU", word))
        {
            op->_singleGPU = true;
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
		else if (!_stricmp("-size", word))
		{
			getNextWord(commandLine, word);
			op->_texWidth = atoi(word);
			getNextWord(commandLine, word);
			op->_texHeight = atoi(word);
		}
		else if (!_stricmp("-help", word))
		{
			char message[2048];

			sprintf_s(message, sizeof(message),
				"MultiGPU Benchmark\n"
				"Usage: MultiGPU [-verbose] [-streams n] [-runTime s] [-calibrationTime s] \n"
				"                      [-size x y] [-win winx winy]\n"
				" -verbose              print messages\n"
				" -streams n            use 'n' streams (default %d)\n"
				" -runTime s            when in benchmark mode run for 's' seconds\n"
				"                       (default %2.1f)\n"
				" -calibrationTime s    wait for 's' seconds before start measuring\n"
				"                       (default %2.1f)\n"
				" -size x y				texture size (default %d %d)\n",
				op->_numStreams,
				op->_runTime,
				op->_calibrationTime,
				op->_texWidth,
				op->_texHeight
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
