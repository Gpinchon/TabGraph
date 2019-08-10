# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/02/18 14:51:09 by gpinchon          #+#    #+#              #
#    Updated: 2019/04/13 19:42:53 by gpinchon         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#NAME			=	libTabGraph.a

DEBUG		?= 0
USE_GDAL	?= 0

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
SHADERS_FILES	=	\
					blur.frag				\
					deferred.frag			\
					deferred.vert			\
					depth.frag				\
					depth.vert				\
					empty.glsl				\
					forward.frag			\
					forward.vert			\
					forward_default.frag	\
					hzb.frag				\
					lighting.frag			\
					lighting_env.frag		\
					passthrough.frag		\
					passthrough.vert		\
					present.frag			\
					refraction.frag			\
					ssao.frag
HEADERS_FILES	=	\
					AABB.hpp					\
					BoundingElement.hpp			\
					Camera.hpp					\
					Common.hpp					\
					ComputeObject.hpp			\
					Config.hpp					\
					Cubemap.hpp					\
					CubeMesh.hpp				\
					Debug.hpp					\
					DLLExport.hpp				\
					Engine.hpp					\
					Environment.hpp				\
					Events.hpp					\
					FPSCamera.hpp				\
					Framebuffer.hpp				\
					GameController.hpp			\
					InputDevice.hpp				\
					Keyboard.hpp				\
					Light.hpp					\
					Material.hpp				\
					Mesh.hpp					\
					MeshParser.hpp				\
					Mouse.hpp					\
					Node.hpp					\
					Object.hpp					\
					OrbitCamera.hpp				\
					parser/BMP.hpp				\
					parser/FBX.hpp				\
					parser/FBX/FBXNode.hpp		\
					parser/FBX/FBXProperty.hpp	\
					parser/FBX/FBXDocument.hpp	\
					parser/GLSL.hpp				\
					parser/HDR.hpp				\
					parser/InternalTools.hpp	\
					parser/MTLLIB.hpp			\
					parser/OBJ.hpp				\
					parser/TABSCENE.hpp			\
					Quadtree.hpp				\
					Render.hpp					\
					Renderable.hpp				\
					Shader.hpp					\
					StackTracer.hpp				\
					Terrain.hpp					\
					Texture.hpp					\
					TextureArray.hpp			\
					TextureParser.hpp			\
					VertexArray.hpp				\
					VertexBuffer.hpp			\
					Vgroup.hpp					\
					Window.hpp
SRC_FILES		=	\
					AABB.cpp					\
					Camera.cpp					\
					ComputeObject.cpp			\
					Common.cpp					\
					Config.cpp					\
					Cubemap.cpp					\
					CubeMesh.cpp				\
					Engine.cpp					\
					Environment.cpp				\
					Events.cpp					\
					FPSCamera.cpp				\
					Framebuffer.cpp				\
					GameController.cpp			\
					Keyboard.cpp				\
					Light.cpp					\
					Material.cpp				\
					Mesh.cpp					\
					MeshParser.cpp				\
					Mouse.cpp					\
					Node.cpp					\
					Object.cpp					\
					OrbitCamera.cpp				\
					parser/BMP.cpp				\
					parser/BT.cpp				\
					parser/FBX.cpp				\
					parser/FBX/FBXNode.cpp		\
					parser/FBX/FBXProperty.cpp	\
					parser/FBX/FBXDocument.cpp	\
					parser/GLSL.cpp				\
					parser/HDR.cpp				\
					parser/MTLLIB.cpp			\
					parser/OBJ.cpp				\
					parser/tools.cpp			\
					Quadtree.cpp				\
					Render.cpp					\
					Renderable.cpp				\
					Shader.cpp					\
					StackTracer.cpp				\
					Texture.cpp					\
					TextureArray.cpp			\
					TextureParser.cpp			\
					VertexArray.cpp				\
					VertexBuffer.cpp			\
					Vgroup.cpp					\
					Window.cpp

RES_FILES		=	$(shell find ./res -type f)

ifeq ($(USE_GDAL), 1)
	SRC_FILES	+= Terrain_GDAL.cpp
else
	SRC_FILES	+= Terrain.cpp
endif

# Files Declaration End #

#   Files Generation   #
SRC				=	$(addprefix $(SRC_PATH), $(SRC_FILES))
SHADERS			=	$(addprefix $(SHADERS_PATH), $(SHADERS_FILES))
HEADERS			=	$(addprefix $(HEADERS_PATH), $(HEADERS_FILES))
OBJ				=	$(SRC_FILES:.cpp=.o)
# Files Generation End #

INCLUDE_PATH	=	./include				\
					./libs/gdal/gdal/gcore	\
					./libs/gdal/gdal/port	\
					./libs/gdal/gdal/ogr


ifeq ($(USE_GDAL), 1)
	LIBDIR		+=	./libs/gdal/gdal/
	LIBFILES	+=	./libs/gdal/gdal/libgdal.a
endif

LDFLAGS		+=	$(addprefix -L , $(LIBDIR))
CPPFLAGS	+=	-std=c++17
CPPFLAGS	+=	$(addprefix -I, $(INCLUDE_PATH))
CPPFLAGS	+=	$(addprefix -I, $(SHADERS_PATH))
CXXFLAGS	+=	-Wall -Wextra -Werror $(CPPFLAGS)

NO_COLOR=\033[0m
OK_COLOR=\033[32;01m
OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)

ifeq ($(OS), Windows_NT)
OK_STRING	=	[OK]
	ifeq ($(USE_GDAL), 1)
		GDALLIBS	= -Wl,--allow-multiple-definition -Wl,-Bstatic -lgdal -lproj -lgeos -lsqlite3 -liconv -lwsock32 -lws2_32
	endif
LDLIBS		+= $(GDALLIBS) -static-libgcc -Wl,-Bstatic -lstdc++ -lpthread -lSDL2_image -limagehlp -ljpeg -lpng -lz -ltiff -lwebp -lzstd -llzma -lmingw32 $(LDFLAGS) -Wl,-Bdynamic -lSDL2main -lSDL2 -lglew32 -lopengl32
else ifeq ($(shell uname -s), Darwin)
LDLIBS		+= $(LDFLAGS) -lm -lGLEW -framework OpenGL -framework SDL2
else
LDLIBS		+= $(LDFLAGS) -lstdc++ -pthread -lz -lm -lSDL2main -lSDL2 -lGLEW -lGL 
endif

ifeq ($(USE_GDAL), 1)
	CXXFLAGS += -DUSE_GDAL
endif

ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG_MOD -g
	LIBPATH = $(DBGBUILD_PATH)
	LIBOBJ_PATH = $(DBGOBJ_PATH)
	LIBOBJ = $(addprefix $(LIBOBJ_PATH), $(OBJ))
all: $(LIBFILES) $(LIBOBJ)
else
	CXXFLAGS += -Ofast
	LIBPATH = $(RELBUILD_PATH)
	LIBOBJ_PATH = $(RELOBJ_PATH)
	LIBOBJ = $(addprefix $(LIBOBJ_PATH), $(OBJ))
all: $(LIBFILES) $(LIBOBJ)
	$(MAKE) DEBUG=1
endif

#$(LIBTAB) : $(LIBFILES) $(LIBOBJ)
#	@(mkdir -p $(@D))
#	ar -rc $(LIBTAB) $(LIBOBJ)
#	ranlib $(LIBTAB)

$(LIBOBJ_PATH)%.o: $(SRC_PATH)%.cpp $(LIBFILES) $(HEADERS) $(SHADERS)
	@(mkdir -p $(@D))
	@echo Compiling $@...
	@($(CXX) $(CXXFLAGS) -o $@ -c $<)
	@echo $@ compilation "$(OK_STRING)"

BUILD_APP_RES	= $(addprefix $(APP_PATH)/build/, $(APP_RES_FILES))
BUILD_RES		= $(addprefix $(APP_PATH)/build/, $(RES_FILES))
APP_RES = $(addprefix $(APP_PATH), $(APP_RES_FILES))
APP_OBJ = $(addprefix $(APP_PATH)/obj/, $(APP_SRC:.cpp=.o))
APP_CXXFLAGS += $(CPPFLAGS)
APP_CXXFLAGS += $(addprefix -I, $(addprefix $(APP_PATH), $(APP_INCLUDE_PATH)))

info:
	@echo $(APP_RES)
	@echo $(BUILD_APP_RES)
	@echo $(CXXFLAGS)

$(BUILD_RES): %: $(RES_FILES)
	@(mkdir -p $(@D))
	@(cp $(patsubst $(APP_PATH)/build/%, %,$@) $@)
	@echo Copied $(patsubst $(APP_PATH)%,%,$@) to $@

$(BUILD_APP_RES): %: $(APP_RES)
	@(mkdir -p $(@D))
	@(cp $(patsubst $(APP_PATH)/build/%, $(APP_PATH)%,$@) $@)
	@echo Copied $(patsubst $(APP_PATH)%,%,$@) to $@

$(APP_PATH)/obj/%.o: $(APP_PATH)$(APP_SRCPATH)%.cpp $(LIBOBJ) $(APP_HEADERS) $(APP_SHADERS)
	@(mkdir -p $(@D))
	@echo Compiling $@...
	@($(CXX) $(APP_CXXFLAGS) -o $@ -c $<)
	@echo $@ compilation "$(OK_STRING)"

$(APP_PATH)/build/$(APP_NAME): $(APP_OBJ)
	@(mkdir -p $(@D))
	@echo Compiling $@...
	@$(CXX) $(APP_CXXFLAGS) $(LIBOBJ) $(APP_OBJ) $(APP_LDLIBS) $(LDLIBS) -o $(APP_PATH)/build/$(APP_NAME)
	@echo $@ compilation "$(OK_STRING)"

application: $(APP_PATH)/build/$(APP_NAME) $(BUILD_APP_RES) $(BUILD_RES)
	./scripts/copyDlls.sh $(APP_PATH)/build/$(APP_NAME)

libraries: $(LIBFILES)


./libs/gdal/gdal/libgdal.a:
	sh ./scripts/minimalGdal.sh

tidy:
	#cppclean $(SRC) --include-path $(HEADERS_PATH)
	clang-tidy $(SRC) -checks=* -- $(CXXFLAGS) $(INCLUDE_PATH)

format:
	clang-format -i -style=WebKit $(SRC) $(HEADERS)

pull:
	git pull
	git submodule update --init --recursive
	git submodule update --recursive --remote

clean:
	rm -rf $(OBJ_PATH)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) clean && ) true

fclean:
	rm -rf $(BUILD_PATH) $(OBJ_PATH)
	$(foreach dir, $(LIBDIR), $(MAKE) -C $(dir) fclean && ) true

re: fclean all

.PHONY: all clean fclean re
