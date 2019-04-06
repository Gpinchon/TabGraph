APP_SRC			=	main.cpp			\
					callbacks.cpp

APP_RES_FILES	=	$(shell find ./res -type f)

APP_PATH		=	$(PWD)/

APP_NAME		=	Scop.exe

export

all:
	cd ../.. && $(MAKE) application

debug: APP_NAME = ScopD.exe
debug:
	cd ../.. && $(MAKE) application DEBUG=1