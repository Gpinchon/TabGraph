# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/02/18 14:51:09 by gpinchon          #+#    #+#              #
#    Updated: 2017/02/18 23:16:06 by gpinchon         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	Scope
SRC			=	./src/main.c

OBJ			=	$(SRC:.c=.o)
CC			=	gcc

INCLUDE_REP	=	./include				\
				./libs/ezmem/include	\
				./libs/vml/include

LIBDIR		=	./libs/ezmem/	\
				./libs/vml/

INCLUDE		=	$(addprefix -I, $(INCLUDE_REP))

ifeq ($(OS), Windows_NT)
LIBS		=	$(addprefix -L , $(LIBDIR)) -lezmem -lvml -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lm
else ifeq ($(shell uname -s), Darwin)
LIBS		=	$(addprefix -L , $(LIBDIR)) -lezmem -lvml -L ~/.brew/lib -lSDL2 -lm
INCLUDE		=	-I ~/.brew/include $(INCLUDE)
else
LIBS		=	$(addprefix -L , $(LIBDIR)) -lezmem -lvml -ljson -lSDL2main -lSDL2 -lGL -lm
endif

CFLAGS		=	-Ofast -ffast-math -Wall -Wextra -Werror $(INCLUDE)

$(NAME): $(OBJ)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) && ) true
	$(CC) $(CFLAGS) $(OBJ) $(LIBS) -o $(NAME)


pull:
	git pull origin master
	git submodule update --init --recursive
	git submodule foreach git reset --hard origin/master

clean:
	rm -rf $(OBJ)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) clean && ) true

fclean:
	rm -rf $(OBJ) $(NAME)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) fclean && ) true

re: fclean $(NAME)

.PHONY: all clean fclean re