# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/02/18 14:51:09 by gpinchon          #+#    #+#              #
#    Updated: 2019/03/27 22:03:04 by gpinchon         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			=	libTabGraph.a

#   Paths Declaration   #
OBJ_PATH		=	obj/
SRC_PATH		=	src/
BUILD_PATH		=	build/
HEADERS_PATH	=	include/
REL_PATH		=	release/
DBG_PATH		=	debug/
SHADERS_PATH	=	$(SRC_PATH)shaders/
RELOBJ_PATH		=	$(OBJ_PATH)$(REL_PATH)
DBGOBJ_PATH		=	$(OBJ_PATH)$(DBG_PATH)
RELBUILD_PATH	=	$(BUILD_PATH)$(REL_PATH)
DBGBUILD_PATH	=	$(BUILD_PATH)$(DBG_PATH)
# Paths Declaration End #

# Files Declaration #
SHADERS_FILES	=	blur.frag				\
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
					DLLExport.hpp				\
					Engine.hpp					\
					Environment.hpp				\
					Debug.hpp					\
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
					Shader.hpp					\
					Terrain.hpp					\
					Texture.hpp					\
					TextureArray.hpp			\
					VertexArray.hpp				\
					VertexBuffer.hpp			\
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
					Terrain.cpp			\
					Texture.cpp			\
					TextureArray.cpp	\
					VertexArray.cpp		\
					VertexBuffer.cpp	\
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

RES_FILES		=	$(shell find ./res -type f)
# Files Declaration End #

#   Files Generation   #
SRC				=	$(addprefix $(SRC_PATH), $(SRC_FILES))
SHADERS			=	$(addprefix $(SHADERS_PATH), $(SHADERS_FILES))
HEADERS			=	$(addprefix $(HEADERS_PATH), $(HEADERS_FILES))
OBJ				=	$(SRC_FILES:.cpp=.o)
RELOBJ			=	$(addprefix $(RELOBJ_PATH), $(OBJ))
DBGOBJ			=	$(addprefix $(DBGOBJ_PATH), $(OBJ))
# Files Generation End #

INCLUDE_PATH	=	./include				\
					./libs/vml/include

LIBDIR		=	./libs/vml/
LDFLAGS		+=	$(addprefix -L , $(LIBDIR))

LIBFILES	=	./libs/vml/libvml.a

CPPFLAGS	+=	$(addprefix -I, $(INCLUDE_PATH))
CPPFLAGS	+=	$(addprefix -I, $(SHADERS_PATH))
CXXFLAGS	+=	-std=c++17 -Wall -Wextra -Werror $(CPPFLAGS)
DBGFLAGS	=	-DDEBUG_MOD -g
RELFLAGS	=	-Ofast

NO_COLOR=\033[0m
OK_COLOR=\033[32;01m
OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)

ifeq ($(OS), Windows_NT)
OK_STRING	=	[OK]
LDLIBS		+=	-static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -lSDL2_image -limagehlp -ljpeg -lpng -ltiff -lwebp -lz -llzma -lmingw32 $(LDFLAGS) -lvml -Wl,-Bdynamic -lSDL2main -lSDL2 -lglew32 -lopengl32
else ifeq ($(shell uname -s), Darwin)
LDLIBS		+=	$(LDFLAGS) -lvml -lm -lGLEW -framework OpenGL -framework SDL2
else
LDLIBS		+=	$(LDFLAGS) -lvml -lstdc++ -lpthread -lz -lm -lSDL2main -lSDL2 -lGLEW -lGL 
endif

all: $(RELBUILD_PATH)$(NAME) $(DBGBUILD_PATH)$(NAME)

$(RELBUILD_PATH)$(NAME) : CXXFLAGS += $(RELFLAGS)
$(RELBUILD_PATH)$(NAME) : $(LIBFILES) $(RELOBJ)
	@(mkdir -p $(@D))
	ar -rc $(RELBUILD_PATH)$(NAME) $(RELOBJ)
	ranlib $(RELBUILD_PATH)$(NAME)

$(RELOBJ_PATH)%.o: $(SRC_PATH)%.cpp $(HEADERS) $(SHADERS)
	@(mkdir -p $(@D))
	@echo Compiling $@...
	@($(CXX) $(CXXFLAGS) -o $@ -c $<)
	@echo $@ compilation "$(OK_STRING)"

$(DBGBUILD_PATH)$(NAME) : CXXFLAGS += $(DBGFLAGS)
$(DBGBUILD_PATH)$(NAME) : $(LIBFILES) $(DBGOBJ)
	@(mkdir -p $(@D))
	ar -rc $(DBGBUILD_PATH)$(NAME) $(DBGOBJ)
	ranlib $(DBGBUILD_PATH)$(NAME)

$(DBGOBJ_PATH)%.o: $(SRC_PATH)%.cpp $(HEADERS) $(SHADERS)
	@(mkdir -p $(@D))
	@echo Compiling $@...
	@($(CXX) $(CXXFLAGS) -o $@ -c $<)
	@echo $@ compilation "$(OK_STRING)"

BUILD_APP_RES = $(addprefix $(APP_PATH)/build/, $(APP_RES_FILES))
APP_RES = $(addprefix $(APP_PATH), $(APP_RES_FILES))
APP_OBJ = $(addprefix $(APP_PATH)/obj/, $(APP_SRC:.cpp=.o))

info:
	@echo $(APP_RES)
	@echo $(BUILD_APP_RES)

$(BUILD_APP_RES): %: $(APP_RES)
	@(mkdir -p $(@D))
	@(cp $(patsubst $(APP_PATH)/build/%, $(APP_PATH)%,$@) $@)
	@echo Copied $(patsubst $(APP_PATH)%,%,$@) to $@

$(APP_PATH)/obj/%.o: $(APP_PATH)$(APP_SRCPATH)%.cpp $(APP_HEADERS) $(APP_SHADERS)
	@(mkdir -p $(@D))
	@echo Compiling $@...
	@($(CXX) $(CXXFLAGS) -o $@ -c $<)
	@echo $@ compilation "$(OK_STRING)"

$(APP_PATH)/build/$(APP_NAME): $(RELBUILD_PATH)$(NAME) $(RELBUILD_PATH)$(NAME) $(APP_OBJ)
	@(mkdir -p $(@D))
	@echo Compiling $@...
	$(CXX) $(CXXFLAGS) $(RELFLAGS) $(APP_OBJ) -L $(RELBUILD_PATH) -lTabGraph $(LDLIBS) -o $(APP_PATH)/build/$(APP_NAME)
	@echo $@ compilation "$(OK_STRING)"

application: $(APP_PATH)/build/$(APP_NAME) $(BUILD_APP_RES)
	./scripts/copyDlls.sh $(APP_PATH)/build/$(APP_NAME)


./libs/vml/libvml.a :
	$(MAKE) -C ./libs/vml/

tidy:
	#cppclean $(SRC) --include-path $(HEADERS_PATH)
	clang-tidy $(SRC) -checks=* -- $(CXXFLAGS) $(INCLUDE_PATH)

format:
	clang-format -i -style=WebKit $(SRC) $(HEADERS)

pull:
	git pull
	git submodule update --init --recursive
	git submodule foreach git reset --hard origin/vml++
	git submodule foreach git pull origin vml++

clean:
	rm -rf $(OBJ_PATH)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) clean && ) true

fclean:
	rm -rf $(BUILD_PATH) $(OBJ_PATH)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) fclean && ) true

re: fclean all

.PHONY: all clean fclean re
