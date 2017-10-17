## FuRS

This is the project for CGU MSIST Course IST303 Software Development.
The project is to create software to support the activities of the registration clerk at “Future Rock Stars” (FuRS), a small, new, rock and roll training summer camp.
or now the purpose is to handle the responsibilities of clerk to receive and handle applications, mailing notices, checkin, assign bands and dormitories.

## Milestone 1.0
For the current milstone this project handles the process to receive applicatons,  process, and notify selected candidates.

## Technology

This project is implemented in C++ usin Qt Framework. Qt is a cross platform C++ framework with support for vast variety of features like cross platform GUI, database connections, Document generations etc. It has all the features required for this project.


## To build the project

To build the project install the latest version of Open Source Qt from https://www.qt.io/download-qt-for-application-development. It supports various compilers. Make sure you select the mingw for your installation.

1. Get the project from the repository.
2. Start the Qt creator IDE to build. link and run the project.
3. Open the .pro file of the project in to the Qt Creator. 
4. When loadin it first time configure it with the mingw KIT and compiler settings.
5. Select the build mode Debug or Release. Build the project.
6. Then click the Green trianhgle button to run the project.


## Tests

This project currently uses Manual testing

## Deploy
To deploy the project run $QTDIR/bin/windeployqt.exe with path to the application executable. It will pull all the dependencies and put it with the executable.
