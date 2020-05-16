ifeq ($(OS), Windows_NT)
APP_NAME			=	Scene.exe
else
APP_NAME			=	Scene
endif
APP_SRC				=	src/main.cpp
APP_INCLUDE_PATH	=	include
APP_RES_FILES		=	$(shell find ./res -type f)

APP_PATH			=	$(shell pwd)/

export

all:
	cd ../.. && $(MAKE) application

ifeq ($(OS), Windows_NT)
debug: APP_NAME = SceneD.exe
else
debug: APP_NAME = SceneD
endif
debug: APP_CXXFLAGS = -g
debug:
	cd ../.. && $(MAKE) application DEBUG=1