Compiling the framework     {#compiling}
=======================

Linux
-----

The libraries eigen3, armadillo, boost and matio need to be installed.
The build options must be configured using cmake. To select the options ccmake can be used as a CLI interface. Afterwards make is used to compile. For example:

```bash
mkdir build
cd build
ccmake ..
make -j32
```

Alternatively, the build script buildtools/make_release.sh can be used.

Windows
-------

The windows version should be built from the git bash and requires an installation of Visual Studio or MinGW and cmake. The required libraries are collected in a folder called FrameworkExtensions that is synchronized from a network share (currently not working). The build script buildtools/make_release.sh works for Windows as well. If the framework extensions folder is not located at C:\\Data\\\%USERNAME%\\FrameworkExtensions, the cmake cache entry FRAMEWORK_EXTENSION_FOLDER must be set to the location of the folder.

Creating the FrameworkExtensions folder
---------------------------------------

To create the FrameworkExtensions folder, all needed libraries must be compiled and copied to the folder.

### Boost

The latest version of boost can be obtained from https://www.boost.org.  The downloaded archive contains a folder named boost with header files inside. This folder must be copied to FrameworkExtensions/boost/boost.

The standalones system2Symbolic, system2Tex and system2Matlab additionally require a compiled version of the boost thread library. The boost website provides instructions on how to compile the library. The b2 program can be called with option --with-thread to skip all libraries that are not needed. Boost:chrono is a dependency of Boost:thread, so it will also be built. The resulting binaries have names starting with libbost_thread or libboost_chrono and ending in .lib. They must be copied to FrameworkExtensions/lib.

### Matio

The matio documentation describes how to build the library using Visual Studio. The header file matio.h must be copied to the folder FrameworkExtensions/include and the compiled library to the folder FrameworkExtensions/lib.

### Eigen

Eigen is a header-only library, so no compiled binary is required. The source files must be copied to the include folder inside of FrameworkExtensions.

Cross Compiling
---------------

The windows version can also be built on linux using MinGW as a cross compiler. The appropriate toolchain file located in toolchain_files/ must be passed to cmake in the CMAKE_TOOLCHAIN_FILE option. Example:
```bash
ccmake -DCMAKE_TOOLCHAIN_FILE=../toolchain_files/ToolchainWin64.cmake ..
```

This can also be achieved by invoking the build script with the option -w.

The required windows libraries are needed for linking, so the FrameworkExtensions folder must be available on the linux host. The default location for cross compiling is /usr/x86_64-w64-mingw32/FrameworkExtensions and can be changed in the toolchain file. The names of boost files change depending on the compiler that is used, so boost should also be cross compiled using MinGW, otherwise the boost libraries will not be found by cmake. Boost is built using its own build tool that is bootstrapped from the boost download. To use the MinGW compiler, a config file named user-config.jam must be created in the user's home directory with the following line:

```bash
using gcc : mingw : x86_64-w64-mingw32-g++-posix ;
```

Building Boost.log requires a patch to two build files. This is described in https://stackoverflow.com/a/12486964. Afterwards Boost can be built by running

```bash
./bootstrap.sh mingw
./b2 toolset=gcc-mingw mc-compiler=windmc target-os=windows variant=release address-model=64 stage
```

The compiled libraries can be found in stage/lib and should be copied to the framework extensions folder.

Building the documentation
--------------------------

The documentation exists in two versions. The release documentation contains handwritten explanations. The development documentation also includes code documentation automatically created by doxygen. The documentation can be build by running make doc or make docRelease in the build directory after the framework has been built. The development documentation will be written to html/, the release version is in DoxygenRelease/html.

CMake Options
-------------

The cmake file provides severeal cached options that can be configured using ccmake or passed as command line options.

| Option                              | Explanation                                                                                    |
| ----------------------------------- | -----------                                                                                    |
| ARCH_TYPE                           | Either 64 or 32, depending ón the target system                                                |
| BUILD_AGING                         | Build all classes required for the aging simulation                                            |
| BUILD_AGING_SIMULATION              | Build a standalone for aging simulations                                                       |
| BUILD_DOCS                          | Generate this documentation using doxygen                                                      |
| BUILD_DOT_EXPORT                    | Build a standalone that creates dot files showing the electrical circuit                       |
| BUILD_EIGENWERTE                    | Build a standalone that calculates the Eigenwerte of a system                                  |
| BUILD_ELECTRICAL_SIMULATION         | Build a standalone for electrical simulations                                                  |
| BUILD_FOR_RT                        | Set compiler options needed for the realtime system, disable strings, streams and exceptions   |
| BUILD_GETTOTALCAPACITY              | Build a standalone that calculated the total capacity of the whole system                      |
| BUILD_NUMERIC                       | Build the numeric version of the ISEAFrame library                                             |
| BUILD_QUICKVERIFICATION             | Build a standalone that verifies XML files and determines the simulation type                  |
| BUILD_SIMULINK_MATRIX_MODEL         | Build mex files that create a simulink model from an XML file                                  |
| BUILD_SVG_EXPORT                    | Build a standalone that creates svg files showing the electrical circuit                       |
| BUILD_SYMBOLIC                      | Build the symbolic version of the ISEAFrame library                                            |
| BUILD_SYMBOLIC_2_TEX                | Build a standalone that creates tex files from a symbolic string                               |
| BUILD_SYSTEM_2_MATLAB               | Build a standalone that creates a matlab file of the equation system                           |
| BUILD_SYSTEM_2_SYMBOLIC             | Build a standalone that prints the symbolic equations describing a system                      |
| BUILD_SYSTEM_2_TEX                  | Build a standalone that creates tex files with the equations describing the system             |
| BUILD_S_FUNCTIONS                   | Create mex files for using the framework as a block in a simulink model                        |
| BUILD_THERMAL_ELECTRICAL_SIMULATION | Build a standalone for thermal-electrical simulations                                          |
| BUILD_THERMAL_SIMULATION            | Build a standalone for thermal simulations                                                     |
| BUILD_UNITTESTS                     | Build unittests                                                                                |
| BUILD_VISUALIZER                    | Build a standalone for creating csv files describing the thermal areas and volumes             |
| CMAKE_BUILD_TYPE                    | Either RELEASE or DEBUG, determines the level of optimization done by the compiler             |
| ERROR_LANG                          | Either EN or DE, sets the language of error messages                                           |
| MATRIX_IMPLEMENTATION               | Can be Eigen, Armadillo or Sparse (currently only Eigen). Sets which type of matrix ist used   |
| MATLAB_INCLUDE_DIR                  | Location of the matlab header files                                                            |
| MATLIAB_LINK_DIR                    | Location of the matlab libraries                                                               |
| SIMULINK_INCLUDE_DIR                | Location of the simulink header files                                                          |
| USE_BOOST_THREADS                   | Enable multithreading                                                                          |
| USE_DEBUG_GDB                       | Add debug symbols to the build so that gdb can display function and variable names             |
