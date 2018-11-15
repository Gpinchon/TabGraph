# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/02/18 14:51:09 by gpinchon          #+#    #+#              #
#    Updated: 2018/11/15 14:40:38 by gpinchon         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	Scop
SRC			=	./src/scop42/main.cpp			\
				./src/scop42/callbacks.cpp		\
				./src/Window.cpp				\
				./src/Engine.cpp				\
				./src/Config.cpp				\
				./src/Environment.cpp			\
				./src/Object.cpp				\
				./src/ComputeObject.cpp			\
				./src/Events.cpp				\
				./src/Mouse.cpp					\
				./src/Keyboard.cpp				\
				./src/GameController.cpp		\
				./src/Cubemap.cpp				\
				./src/Node.cpp					\
				./src/Renderable.cpp			\
				./src/Render.cpp				\
				./src/Camera.cpp				\
				./src/Framebuffer.cpp			\
				./src/Light.cpp					\
				./src/Mesh.cpp					\
				./src/CubeMesh.cpp				\
				./src/Vgroup.cpp				\
				./src/VertexArray.cpp			\
				./src/Shader.cpp				\
				./src/Texture.cpp				\
				./src/TextureArray.cpp			\
				./src/Material.cpp				\
				./src/parser/tools.cpp			\
				./src/parser/OBJ.cpp			\
				./src/parser/FBX.cpp			\
				./src/parser/HDR.cpp			\
				./src/parser/BMP.cpp			\
				./src/parser/GLSL.cpp			\
				./src/parser/MTLLIB.cpp

OBJ			=	$(SRC:.cpp=.o)
HYPER_OBJ	=	final.o
CC			=	clang

INCLUDE_REP	=	./include				\
				./libs/vml/include		\

LIBDIR		=	./libs/vml/

LIBFILES	=	./libs/vml/libvml.a

INCLUDE		=	$(addprefix -I, $(INCLUDE_REP))
CXXFLAGS	=	-Ofast -std=c++1z -Wall -Wextra -Werror $(INCLUDE)
LINKFLAGS	=	-Wl,--allow-multiple-definition

NO_COLOR=\033[0m
OK_COLOR=\033[32;01m
OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)

ifeq ($(OS), Windows_NT)
OK_STRING	=	[OK]
NAME		=	Scop.exe
LIBS		=	-static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -lz -Wl,-Bdynamic $(addprefix -L , $(LIBDIR)) -lvml -lmingw32 -Wl,-Bdynamic -lSDL2main -lSDL2 -lglew32 -lopengl32
LINKFLAGS	=	-Wl,--allow-multiple-definition
CXXFLAGS	=	-Ofast -std=c++1z -Wall -Wextra -Werror $(INCLUDE)
else ifeq ($(shell uname -s), Darwin)
LIBS		=	$(addprefix -L , $(LIBDIR)) -lvml -lm -lGLEW -framework OpenGL -framework SDL2
INCLUDE		=	$(addprefix -I, $(INCLUDE_REP))
else
LIBS		=	$(addprefix -L , $(LIBDIR)) -lvml -lstdc++ -lpthread -lz -lm -lSDL2main -lSDL2 -lGLEW -lGL 
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
