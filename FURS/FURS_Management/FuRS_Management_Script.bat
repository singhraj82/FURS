rem Qmake the project file to generate make files.
%QTDIR%\bin\qmake.exe FURS_Management.pro
rem Compile the project.
%QTDIR%\..\..\Tools\QtCreator\bin\jom.exe -f Makefile.Debug
rem Get all the required dependencies.
%QTDIR%\bin\windeployqt.exe debug\FURS_Management.exe
rem Create folder that will keep letters.
mkdir ..\letters