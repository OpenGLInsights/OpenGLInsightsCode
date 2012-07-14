
IMPORTANT!!!
Visual studio 2008 saves the "working directory" in the user file.
Given the user file is named based on the windows user,
the field will not be set and lfb.exe will run from this (the wrong) directory.
VS2010 does not have this problem anymore.

Long story short: run lfb.exe externally (eg just double click on the file after building)
OR:
- right click on the project lfb
- select Configuration Properties->Debugging
- enter "$(SolutionDir)../" in the "Working Directory" field, without quotes
- repeat for Debug and Release configurations as needed
