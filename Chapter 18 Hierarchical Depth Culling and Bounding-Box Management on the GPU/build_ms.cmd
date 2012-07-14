@echo off
set NETVER=v3.5
set BOO="c:\Program Files\SharpDevelop\3.0\AddIns\AddIns\BackendBindings\BooBinding"
c:\windows\microsoft.net\framework\%NETVER%\MSBuild tools\tools.sln /p:BoocVerbosity=Info /p:BooBinPath=%BOO%
pause