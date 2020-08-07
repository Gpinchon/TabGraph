# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/02/18 14:51:09 by gpinchon          #+#    #+#              #
#    Updated: 2020/05/04 16:21:08 by gpinchon         ###   ########.fr        #
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
					ssao.frag				\
					SSR.frag				\
					SSRMerge.frag			\
					SSRBlur.frag
HEADERS_FILES	=	\
					Animation/AnimationChannel.hpp \
					Animation/Animation.hpp \
					Animation/AnimationInterpolator.hpp \
					Animation/AnimationSampler.hpp \
					brdfLUT.hpp \
					Buffer/BufferAccessor.hpp \
					Buffer/BufferHelper.hpp \
					Buffer/Buffer.hpp \
					Buffer/BufferView.hpp \
					Callback.hpp \
					Camera/Camera.hpp \
					Camera/FPSCamera.hpp \
					Camera/OrbitCamera.hpp \
					Common.hpp \
					Component.hpp \
					ComputeObject.hpp \
					Config.hpp \
					Debug.hpp \
					DLLExport.hpp \
					Engine.hpp \
					Environment.hpp \
					Framebuffer.hpp \
					Input/Events.hpp \
					Input/GameController.hpp \
					Input/InputDevice.hpp \
					Input/Keyboard.hpp \
					Input/Mouse.hpp \
					Light/Light.hpp \
					Material.hpp \
					Mesh/CubeMesh.hpp \
					Mesh/Geometry.hpp \
					Mesh/Mesh.hpp \
					Mesh/MeshParser.hpp \
					Mesh/MeshSkin.hpp \
					Mesh/PlaneMesh.hpp \
					Mesh/SphereMesh.hpp \
					Node.hpp \
					Object.hpp \
					Parser/BMP.hpp \
					Parser/FBX/FBXDocument.hpp \
					Parser/FBX/FBXNode.hpp \
					Parser/FBX/FBXObject.hpp \
					Parser/FBX/FBXProperty.hpp \
					Parser/FBX.hpp \
					Parser/GLSL.hpp \
					Parser/GLTF.hpp \
					Parser/HDR.hpp \
					Parser/InternalTools.hpp \
					Parser/MTLLIB.hpp \
					Parser/OBJ.hpp \
					Parser/TABSCENE.hpp \
					Physics/BoundingAABB.hpp \
					Physics/BoundingBox.hpp \
					Physics/BoundingElement.hpp \
					Physics/BoundingMesh.hpp \
					Physics/BoundingPlane.hpp \
					Physics/BoundingSphere.hpp \
					Physics/CollisionAlgorithmSAT.hpp \
					Physics/CollisionAlgorithmGJKRaycast.hpp \
					Physics/IntersectFunctions.hpp \
					Physics/Intersection.hpp \
					Physics/PhysicsEngine.hpp \
					Physics/Ray.hpp \
					Physics/RigidBody.hpp \
					Physics/SimplexSolver.hpp \
					Quadtree.hpp \
					Render.hpp \
					Scene/Scene.hpp \
					Scene/SceneParser.hpp \
					Shader/GLUniformHelper.hpp \
					Shader/Shader.hpp \
					Shader/ShaderStage.hpp \
					StackTracer.hpp \
					State.hpp \
					StateMachine.hpp \
					Terrain/Terrain_GDAL.hpp \
					Terrain/Terrain.hpp \
					Texture/Cubemap.hpp \
					Texture/Texture1D.hpp \
					Texture/Texture2D.hpp \
					Texture/TextureArray.hpp \
					Texture/TextureBuffer.hpp \
					Texture/Texture.hpp \
					Texture/TextureParser.hpp \
					Tools.hpp \
					Transform.hpp \
					Window.hpp

SRC_FILES		=	\
					Animation/AnimationChannel.cpp \
					Animation/Animation.cpp \
					Animation/AnimationInterpolator.cpp \
					Animation/AnimationSampler.cpp \
					Buffer/BufferAccessor.cpp \
					Buffer/Buffer.cpp \
					Buffer/BufferView.cpp \
					Callback.cpp \
					Camera/Camera.cpp \
					Camera/FPSCamera.cpp \
					Camera/OrbitCamera.cpp \
					Common.cpp \
					ComputeObject.cpp \
					Config.cpp \
					Engine.cpp \
					Environment.cpp \
					Framebuffer.cpp \
					Input/Events.cpp \
					Input/GameController.cpp \
					Input/Inputdevice.cpp \
					Input/Keyboard.cpp \
					Input/Mouse.cpp \
					Light/Light.cpp \
					Material.cpp \
					Mesh/CubeMesh.cpp \
					Mesh/Geometry.cpp \
					Mesh/Mesh.cpp \
					Mesh/MeshParser.cpp \
					Mesh/MeshSkin.cpp \
					Mesh/PlaneMesh.cpp \
					Mesh/SphereMesh.cpp \
					Node.cpp \
					Object.cpp \
					Parser/BMP.cpp \
					Parser/BT.cpp \
					Parser/FBX.cpp \
					Parser/FBX/FBXDocument.cpp \
					Parser/FBX/FBXNode.cpp \
					Parser/FBX/FBXObject.cpp \
					Parser/FBX/FBXProperty.cpp \
					Parser/GLSL.cpp \
					Parser/GLTF.cpp \
					Parser/HDR.cpp \
					Parser/MTLLIB.cpp \
					Parser/OBJ.cpp \
					Parser/tools.cpp \
					Physics/BoundingAABB.cpp \
					Physics/BoundingBox.cpp \
					Physics/BoundingElement.cpp \
					Physics/BoundingMesh.cpp \
					Physics/BoundingPlane.cpp \
					Physics/BoundingSphere.cpp \
					Physics/CollisionAlgorithmSAT.cpp \
					Physics/CollisionAlgorithmGJKRaycast.cpp \
					Physics/Intersection.cpp \
					Physics/PhysicsEngine.cpp \
					Physics/RigidBody.cpp \
					Physics/SimplexSolver.cpp \
					Quadtree.cpp \
					Render.cpp \
					Scene/Scene.cpp \
					Scene/SceneParser.cpp \
					Shader/GLUniformHelper.cpp \
					Shader/Shader.cpp \
					Shader/ShaderStage.cpp \
					StackTracer.cpp \
					Texture/Cubemap.cpp \
					Texture/Texture2D.cpp \
					Texture/TextureArray.cpp \
					Texture/TextureBuffer.cpp \
					Texture/Texture.cpp \
					Texture/TextureParser.cpp \
					Tools.cpp \
					Transform.cpp \
					Window.cpp

RES_FILES		=	$(shell find ./res -type f)

ifeq ($(USE_GDAL), 1)
	SRC_FILES	+= Terrain/Terrain_GDAL.cpp
else
	SRC_FILES	+= Terrain/Terrain.cpp
endif

# Files Declaration End #

#   Files Generation   #
SRC				=	$(addprefix $(SRC_PATH), $(SRC_FILES))
SHADERS			=	$(addprefix $(SHADERS_PATH), $(SHADERS_FILES))
HEADERS			=	$(addprefix $(HEADERS_PATH), $(HEADERS_FILES))
OBJ				=	$(SRC_FILES:.cpp=.o)
# Files Generation End #

INCLUDE_PATH	=	./include				\
					./libs/include			\
					./libs/gdal/gdal/gcore	\
					./libs/gdal/gdal/port	\
					./libs/gdal/gdal/ogr	\
					./libs/rapidjson/include


ifeq ($(USE_GDAL), 1)
	LIBDIR		+=	./libs/gdal/gdal/
	LIBFILES	+=	./libs/gdal/gdal/libgdal.a
endif

LDFLAGS		+=	$(addprefix -L , $(LIBDIR))
CPPFLAGS	+=	-std=gnu++17
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
LDLIBS		+= $(LDFLAGS) -lstdc++ -lstdc++fs -pthread -lz -lm -lSDL2main -lSDL2 -lSDL2_image -lGLEW -lGL 
endif

ifeq ($(USE_GDAL), 1)
	CXXFLAGS += -DUSE_GDAL
endif

ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG_MOD -g -O1
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

ifeq ($(OS), Windows_NT)
application: $(APP_PATH)/build/$(APP_NAME) $(BUILD_APP_RES) $(BUILD_RES)
	./scripts/copyDlls.sh $(APP_PATH)/build/$(APP_NAME)
else
application: $(APP_PATH)/build/$(APP_NAME) $(BUILD_APP_RES) $(BUILD_RES)
endif


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

doc:
	doxygen doxyfile

.PHONY: all clean fclean re
