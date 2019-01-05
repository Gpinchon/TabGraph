# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/02/18 14:51:09 by gpinchon          #+#    #+#              #
#    Updated: 2019/01/05 09:21:33 by gpinchon         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

TEST			=	Scop
NAME			=	./bin/libTabGraph.a

#   Paths Declaration   #
SRC_PATH		=	./src/
SHADERS_PATH	=	./src/shaders/
HEADERS_PATH	=	./include/
# Paths Declaration End #

# Files Declaration #
SHADER_FILES	=	blur.frag				\
					deferred.frag			\
					deferred.vert			\
					depth.frag				\
					depth.vert				\
					empty.glsl				\
					forward.frag			\
					forward.vert			\
					forward_default.frag	\
					lighting.frag			\
					lighting_env.frag		\
					passthrough.frag		\
					passthrough.vert		\
					present.frag			\
					refraction.frag			\
					ssao.frag
HEADERS_FILES	=	AABB.hpp					\
					BoundingElement.hpp			\
					Camera.hpp					\
					ComputeObject.hpp			\
					Config.hpp					\
					Cubemap.hpp					\
					CubeMesh.hpp				\
					Engine.hpp					\
					Environment.hpp				\
					Errors.hpp					\
					Events.hpp					\
					Framebuffer.hpp				\
					GameController.hpp			\
					GLIncludes.hpp				\
					InputDevice.hpp				\
					Keyboard.hpp				\
					Light.hpp					\
					Material.hpp				\
					Mesh.hpp					\
					Mouse.hpp					\
					Node.hpp					\
					Object.hpp					\
					parser						\
					Render.hpp					\
					Renderable.hpp				\
					scop.hpp					\
					Shader.hpp					\
					Texture.hpp					\
					TextureArray.hpp			\
					VertexArray.hpp				\
					Vgroup.hpp					\
					Window.hpp					\
					parser/BMP.hpp				\
					parser/FBX.hpp				\
					parser/GLSL.hpp				\
					parser/HDR.hpp				\
					parser/InternalTools.hpp	\
					parser/MTLLIB.hpp			\
					parser/OBJ.hpp				\
					parser/TABSCENE.hpp
SRC_FILES		=	Camera.cpp			\
					ComputeObject.cpp	\
					Config.cpp			\
					Cubemap.cpp			\
					CubeMesh.cpp		\
					Engine.cpp			\
					Environment.cpp		\
					Events.cpp			\
					Framebuffer.cpp		\
					GameController.cpp	\
					Keyboard.cpp		\
					Light.cpp			\
					Material.cpp		\
					Mesh.cpp			\
					Mouse.cpp			\
					Node.cpp			\
					Object.cpp			\
					Render.cpp			\
					Renderable.cpp		\
					Shader.cpp			\
					Texture.cpp			\
					TextureArray.cpp	\
					VertexArray.cpp		\
					Vgroup.cpp			\
					Window.cpp			\
					parser/BMP.cpp		\
					parser/FBX.cpp		\
					parser/GLSL.cpp		\
					parser/HDR.cpp		\
					parser/MTLLIB.cpp	\
					parser/OBJ.cpp		\
					parser/tools.cpp	\
					render/shadow.cpp
# Files Declaration End #

#   Files Generation   #
SRC				=	$(addprefix $(SRC_PATH), $(SRC_FILES))
SHADERS			=	$(addprefix $(SHADERS_PATH), $(SHADERS_FILES))
HEADERS			=	$(addprefix $(HEADERS_PATH), $(HEADERS_FILES))
# Files Generation End #

TESTOBJ		=	./src/scop42/main.cpp			\
				./src/scop42/callbacks.cpp

OBJ			=	$(SRC:.cpp=.o)

CC			=	g++

INCLUDE_REP	=	./include				\
				./libs/vml/include

LIBDIR		=	./libs/vml/

LIBFILES	=	./libs/vml/libvml.a

INCLUDE		=	$(addprefix -I, $(INCLUDE_REP))
INCLUDE		+=	$(addprefix -I, $(SHADERS_PATH))
CXXFLAGS	=	-Ofast -std=c++17 -Wall -Wextra -Werror $(INCLUDE)
LINKFLAGS	=	-Wl,--allow-multiple-definition

NO_COLOR=\033[0m
OK_COLOR=\033[32;01m
OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)

ifeq ($(OS), Windows_NT)
OK_STRING	=	[OK]
TEST		=	Scop.exe
LIBS		=	-static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -lz -Wl,-Bdynamic $(addprefix -L , $(LIBDIR)) -lvml -lmingw32 -Wl,-Bdynamic -lSDL2main -lSDL2 -lglew32 -lopengl32
LINKFLAGS	=	-Wl,--allow-multiple-definition
CXXFLAGS	=	-Ofast -std=c++1z -Wall -Wextra -Werror $(INCLUDE)
else ifeq ($(shell uname -s), Darwin)
LIBS		=	$(addprefix -L , $(LIBDIR)) -lvml -lm -lGLEW -framework OpenGL -framework SDL2
INCLUDE		=	$(addprefix -I, $(INCLUDE_REP))
else
LIBS		=	$(addprefix -L , $(LIBDIR)) -lvml -lstdc++ -lpthread -lz -lm -lSDL2main -lSDL2 -lGLEW -lGL 
endif

$(NAME) : $(LIBFILES) $(OBJ)
	mkdir -p ./bin
	ar -rc $(NAME) $(OBJ)
	#$(CC) $(CXXFLAGS) $(OBJ) $(LINKFLAGS) $(LIBS) -o $(NAME)
	ranlib $(NAME)

test: $(LIBFILES) $(NAME) $(TESTOBJ)
	$(CC) $(CXXFLAGS) $(TESTOBJ) $(LINKFLAGS) -L ./bin -lTabGraph $(LIBS) -o $(TEST)

%.o: %.cpp $(HEADERS) $(SHADERS)
	@echo -n Compiling $@...
	@($(CC) $(CXXFLAGS) -o $@ -c $<)
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
	rm -rf $(OBJ)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) clean && ) true

fclean:
	rm -rf $(OBJ) $(NAME) $(TEST)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) fclean && ) true

re: fclean $(NAME) test

.PHONY: all clean fclean re
