## FuRS

This is the project for CGU MSIST Course IST303 Software Development.
The project is to create software to support the activities of the registration clerk at “Future Rock Stars” (FuRS), a small, new, rock and roll training summer camp.
For now the purpose is to handle the responsibilities of clerk to receive and handle applications, mailing notices, checkin, assign bands and dormitories.

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

## Deploy Manually
To deploy the project run $QTDIR/bin/windeployqt.exe with path to the application executable. It will pull all the dependencies and put it with the executable.

## Milestone 2.0
1. Implement the check-in process for selected candidates.
2. Assignment of Dorms.
3. Assignment of Primary bands.
4. Assignment of secondary bands.
5. Provision to manage and maintain Waitlist.
6. Automated build.
7. Unit tests.
8. Automated process to create deployable product.

## Build and Deploy Automatically
1. Make sure that you have Qt SDK and Qt Creator installed.
2. Set your installed Qt path(folder that contains bin) to QTDIR. e.g QTDIR=C:\Qt\5.8\mingw53_32
3. Also add location of \Tools\mingw530_32\bin to the PATH. e.g. C:\Qt\Tools\mingw530_32\bin
3. Get the source code from repository.
4. Goto the source code folder and run FuRS_Management_Script.bat. This will create required project files, necessary folders and build the code in release mode, generate executable and also pull in all the required dependencies in to the release folder.
5. This will create a Deploy_FuRS folder with all necessary files. release folder contains the executable to be run.
6. This deploy folder can be used as a deliverable to run the project. There is a fors.bat file inside FURS folder. On clicking it launches the application. 

