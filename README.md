# TabGraph

*tabGraph* is a full SceneGraph featuring a flexible Component system, giving a lot of freedome for adding new components to your scene and organizing it.

*tabGraph* Provides a SceneGraph and a complete building system, allowing for the generation of portable executables.

In order to use *tabGraph*, you need to setup your project with a Makefile calling *tabGraph*'s main Makefile.

The application will be built in `$(APP_PATH)/build/` by default

# Exportable Makefile Variables

In order to allow for application building, several variable can be set.

##### APP_SRC (MANDATORY)

- The list of the source files of your project
- Path must be relative to your project's path

##### APP_NAME (MANDATORY)

- The name of the executable to be built

##### APP_PATH (MANDATORY)

- The root of your application, usually set to $(PWD)/

##### APP_INCLUDE_PATH (OPTIONAL)

- The list of include paths relative to the application's path

##### APP_RES_FILES (OPTIONAL)

- The list of the application's resource files, they will be copied to the build folder

##### APP_LDLIBS (OPTIONAL)

- The list of libs to link to (such as -lpng)

##### APP_CXXFLAGS (OPTIONAL)

- The list of options to use with GCC for building the application,
- Will be concanated with *tabGraph*'s default options

##### APP_HEADERS (OPTIONAL)

- The list of header files used by the application
- Allows for rebuild if a header changed

##### APP_SHADERS (OPTIONAL)

- The list of shader files used by the application
- Allows for rebuild if a shader changed

##### DEBUG (OPTIONAL)

- DEFAULT VALUE = 0
- Set it to 1 in order to build the application in debug mod
- When building in debug mod, the `DEBUG_MOD` is defined,
- `DEBUGLOG` will be enabled
- `-g` option will be used for compilation

##### USE_GDAL (OPTIONAL)

- DEFAULT VALUE = 0
- Set it to 1 in order to build using GDAL library
- Set it to 0 to use internal parsing methods (with limited abilities) to parse terrains

# Example

```make
APP_NAME            =   Scop.exe
APP_SRC             =   src/main.cpp        \
                        src/callbacks.cpp
APP_INCLUDE_PATH    =   include
APP_RES_FILES       =   $(shell find ./res -type f)
APP_PATH            =   $(PWD)/

export

all:
    cd $(TABGRAPH_PATH) && $(MAKE) application

debug: APP_NAME = ScopD.exe
debug:
    cd $(TABGRAPH_PATH) && $(MAKE) application DEBUG=1
```
