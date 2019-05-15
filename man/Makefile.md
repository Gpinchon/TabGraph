# Makefile

*tabGraph* Provides a complete building system, allowing for the generation of portable executables.
In order to use *tabGraph*, you need to setup your project with a Makefile calling *tabGraph*'s main Makefile.

# Exportable Makefile Variables

In order to allow for application building, several variable can be set.

##### APP_SRC (MANDATORY)

    The list of the source files of your project, path must be relative to your project's path

##### APP_NAME (MANDATORY)

    The name of the executable

##### APP_PATH (MANDATORY)

    The root of your application, usually set to $(PWD)/

##### APP_INCLUDE_PATH (OPTIONAL)

    The list of include paths relative to the application's path

##### APP_RES_FILES (OPTIONAL)

    The list of the application's resource files, they will be copied to the build folder

##### APP_LDLIBS (OPTIONAL)

    The list of libs to link to (such as -lpng)

##### APP_CXXFLAGS (OPTIONAL)

    The list of options to use with GCC for building the application,
    will be concanated with *tabGraph*'s default options

##### APP_HEADERS (OPTIONAL)

    The list of header files used by the application, allows for rebuild if a header changed

##### APP_SHADERS (OPTIONAL)

    The listof shader files used by the application, allows for rebuild if a shader changed

##### DEBUG (OPTIONAL)

    Set it to 1 in order to build the application in debug mod

# Example

```make
APP_NAME            =   Scop.exe
APP_SRC             =   src/main.cpp			\
                        src/callbacks.cpp
APP_INCLUDE_PATH    =   include
APP_RES_FILES       =   $(shell find ./res -type f)
APP_PATH            =   $(PWD)/

export

all:
    cd ../.. && $(MAKE) application

debug: APP_NAME = ScopD.exe
debug:
    cd ../.. && $(MAKE) application DEBUG=1
```