# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/02/18 14:51:09 by gpinchon          #+#    #+#              #
#    Updated: 2018/03/25 20:31:44 by gpinchon         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	Scope
SRC			=	./src/main.c			\
				./src/brdf.c			\
				./src/callbacks.c		\
				./src/callbacks1.c		\
				./src/callbacks2.c		\
				./src/event.c			\
				./src/window.c			\
				./src/framebuffer.c		\
				./src/framebuffer1.c	\
				./src/engine.c			\
				./src/camera.c			\
				./src/camera1.c			\
				./src/shader.c			\
				./src/cubemap.c			\
				./src/texture.c			\
				./src/texture1.c		\
				./src/texture2.c		\
				./src/transform.c		\
				./src/mesh.c			\
				./src/vgroup.c			\
				./src/material.c		\
				./src/material_get.c	\
				./src/material_get1.c	\
				./src/material_get2.c	\
				./src/material_get3.c	\
				./src/material_set.c	\
				./src/material_set1.c	\
				./src/material_set2.c	\
				./src/material_set3.c	\
				./src/parser/tools.c	\
				./src/parser/obj/parser.c	\
				./src/parser/bmp/parser.c	\
				./src/parser/bmp/saver.c	\
				./src/parser/shaders/parser.c	\
				./src/parser/mtllib/parser.c

OBJ			=	$(SRC:.c=.o)
CC			=	gcc

INCLUDE_REP	=	./include				\
				./libs/ezmem/include	\
				./libs/vml/include		\
				./libs/libft/include

LIBDIR		=	./libs/ezmem/	\
				./libs/vml/		\
				./libs/libft/

LIBFILES	=	./libs/ezmem/libezmem.a	\
				./libs/vml/libfml.a		\
				./libs/libft/libft.a

INCLUDE		=	$(addprefix -I, $(INCLUDE_REP))
CFLAGS		=	-Ofast -march=native -Wall -Wextra -Werror $(INCLUDE)

ifeq ($(OS), Windows_NT)
LIBS		=	$(addprefix -L , $(LIBDIR)) -lezmem -lvml -lft -lmingw32 -lSDL2main -lSDL2 -lm -lglew32 -lopengl32
else ifeq ($(shell uname -s), Darwin)
LIBS		=	$(addprefix -L , $(LIBDIR)) -lezmem -lvml -L ~/.brew/lib -lSDL2 -lm -lGLEW -lft -framework OpenGL
INCLUDE		=	-I ~/.brew/include $(addprefix -I, $(INCLUDE_REP))
CFLAGS		=	-Ofast -arch x86_64 -Wall -Wextra -Werror $(INCLUDE)
else
LIBS		=	$(addprefix -L , $(LIBDIR)) -lezmem -lvml -lSDL2main -lSDL2 -lGL -lm -lGLEW -lft
endif

$(NAME): $(LIBFILES) $(OBJ)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) && ) true
	$(CC) $(CFLAGS) $(OBJ) $(LIBS) -o $(NAME)

./libs/ezmem/libezmem.a :
	$(MAKE) -C ./libs/ezmem/

./libs/vml/libfml.a :
	$(MAKE) -C ./libs/vml/

./libs/libft/libft.a :
	$(MAKE) -C ./libs/libft/

pull:
	git pull
	git submodule update --init --recursive
	git submodule foreach git reset --hard origin/master
	git submodule foreach git pull origin master

clean:
	rm -rf $(OBJ)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) clean && ) true

fclean:
	rm -rf $(OBJ) $(NAME)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) fclean && ) true

re: fclean $(NAME)

.PHONY: all clean fclean re