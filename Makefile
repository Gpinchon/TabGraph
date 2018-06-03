# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/02/18 14:51:09 by gpinchon          #+#    #+#              #
#    Updated: 2018/06/03 18:30:02 by gpinchon         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	Scop
SRC			=	./src/main.cpp			\
				./src/event.cpp			\
				./src/cubemap.cpp			\
				./src/callbacks/callbacks.cpp		\
				./src/node/methods.cpp		\
				./src/window/methods.cpp			\
				./src/engine/methods.cpp			\
				./src/camera/methods.cpp			\
				./src/framebuffer/methods.cpp		\
				./src/material/methods.cpp		\
				./src/material/PBRmethods.cpp		\
				./src/mesh/methods.cpp			\
				./src/parser/tools.cpp			\
				./src/parser/obj/parser.cpp		\
				./src/parser/obj/parser1.cpp		\
				./src/parser/obj/parser2.cpp		\
				./src/parser/obj/parser3.cpp		\
				./src/parser/hdr/parser.cpp		\
				./src/parser/bmp/parser.cpp		\
				./src/parser/bmp/saver.cpp		\
				./src/parser/shaders/parser.cpp	\
				./src/parser/mtllib/parser.cpp	\
				./src/shader/methods.cpp			\
				./src/texture/blur.cpp		\
				./src/texture/methods.cpp		\
				./src/render/quad.cpp		\
				./src/render/scene.cpp	\
				./src/render/shadow.cpp	\
				./src/render/present.cpp

OBJ			=	$(SRC:.cpp=.o)
HYPER_OBJ	=	final.o
CC			=	g++

INCLUDE_REP	=	./include				\
				./libs/vml/include		\

LIBDIR		=	./libs/vml/

LIBFILES	=	./libs/vml/libvml.a

INCLUDE		=	$(addprefix -I, $(INCLUDE_REP))
CXXFLAGS	=	-Ofast -std=c++14 -Wall -Wextra -Werror $(INCLUDE)
LINKFLAGS	=	-Wl,--allow-multiple-definition

NO_COLOR=\033[0m
OK_COLOR=\033[32;01m
OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)

ifeq ($(OS), Windows_NT)
OK_STRING	=	[OK]
NAME		=	Scop.exe
LIBS		=	-static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic $(addprefix -L , $(LIBDIR)) -lvml -lmingw32 -Wl,-Bdynamic -lSDL2main -lSDL2 -lglew32 -lopengl32
else ifeq ($(shell uname -s), Darwin)
LIBS		=	$(addprefix -L , $(LIBDIR)) -lvml -lm -lGLEW -framework OpenGL -framework SDL2
INCLUDE		=	$(addprefix -I, $(INCLUDE_REP))
else
LIBS		=	$(addprefix -L , $(LIBDIR)) -lvml -lSDL2main -lSDL2 -lGL -lm -lGLEW
endif

$(NAME): $(LIBFILES) $(OBJ)
	$(CC) $(CXXFLAGS) $(OBJ) $(LINKFLAGS) $(LIBS) -o $(NAME)

hyper: $(LIBFILES) $(HYPER_OBJ)
	$(CC) $(CXXFLAGS) $(HYPER_OBJ) $(LIBS) -o $(addprefix Hyper, $(NAME))

%.o: %.cpp
	@echo -n Compiling $@...
	@($(CC) $(CXXFLAGS) -o $@ -c $<)
	@echo "$(OK_STRING)"

%.o: %.hyper
	@echo -n Compiling $@...
	@($(CC) -x c++ $(CXXFLAGS) -o $@ -c $<)
	@echo "$(OK_STRING)"

.INTERMEDIATE: final.hyper
final.hyper: $(SRC)
	@echo -n Generating $@...
	@(cat $^ > final.hyper)
	@echo "$(OK_STRING)"

./libs/ezmem/libezmem.a :
	$(MAKE) -C ./libs/ezmem/

./libs/vml/libvml.a :
	$(MAKE) -C ./libs/vml/

./libs/libft/libft.a :
	$(MAKE) -C ./libs/libft/

tidy:
	clang-tidy $(SRC) -checks=* -- $(CXXFLAGS) $(INCLUDE_REP)

pull:
	git pull
	git submodule update --init --recursive
	git submodule foreach git reset --hard origin/vml++
	git submodule foreach git pull origin vml++

clean:
	rm -rf $(OBJ) $(HYPER_OBJ)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) clean && ) true

fclean:
	rm -rf $(OBJ) $(HYPER_OBJ) $(NAME)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) fclean && ) true

re: fclean $(NAME)

.PHONY: all clean fclean re