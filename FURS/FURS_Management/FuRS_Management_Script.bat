rem Qmake the project file to generate make files.
%QTDIR%\bin\qmake.exe FURS_Management.pro
rem Compile the project.
%QTDIR%\..\..\Tools\QtCreator\bin\jom.exe -f Makefile.Release
rem Get all the required dependencies.
%QTDIR%\bin\windeployqt.exe release\FURS_Management.exe
rem Create folder that will keep letters.
mkdir ..\letters
rem Create deployable folder
mkdir Deploy_FuRS
mkdir Deploy_FuRS\FURS
mkdir Deploy_FuRS\FURS\letters
mkdir Deploy_FuRS\FURS\FURS_Management
mkdir Deploy_FuRS\FURS\FURS_Management\release
xcopy /s release\* Deploy_FuRS\FURS\FURS_Management\release
xcopy furs_data Deploy_FuRS\FURS\FURS_Management
