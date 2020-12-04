cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0074 NEW)

#set(SDL2_BUILDING_LIBRARY ON)

#set(CMAKE_VERBOSE_MAKEFILE 1)
#set(CMAKE_FIND_DEBUG_MODE ON)
set(CMAKE_DEBUG_TARGET_PROPERTIES INCLUDE_DIRECTORIES)
set(CMAKE_DEBUG_TARGET_PROPERTIES INTERFACE_INCLUDE_DIRECTORIES)


set(TABGRAPH_SOURCE_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/src/Animation/AnimationChannel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Animation/Animation.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Animation/AnimationInterpolator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Animation/AnimationSampler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/AssetsParser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Buffer/BufferAccessor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Buffer/Buffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Buffer/BufferView.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Callback.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Camera/Camera.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Camera/FPSCamera.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Camera/OrbitCamera.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Common.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Component.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/ComputeObject.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Config.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Engine.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Environment.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Framebuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Event/Events.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Event/GameController.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Event/Inputdevice.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Event/Keyboard.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Event/Mouse.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Light/Light.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Light/DirectionnalLight.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Light/PointLight.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Light/SpotLight.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Material/Material.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Material/MaterialExtension.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Material/MetallicRoughness.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Material/SpecularGlossiness.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh/CapsuleMesh.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh/CubeMesh.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh/Geometry.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh/Mesh.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh/MeshSkin.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh/PlaneMesh.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh/SphereMesh.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Node.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Object.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/BoundingAABB.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/BoundingBox.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/BoundingElement.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/BoundingMesh.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/BoundingPlane.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/BoundingSphere.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/CollisionAlgorithmSAT.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/CollisionAlgorithmGJKRaycast.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/Intersection.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/PhysicsEngine.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/RigidBody.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Physics/SimplexSolver.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Quadtree.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Render.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Scene/Scene.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shader/GLUniformHelper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shader/Shader.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shader/ShaderExtension.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Shader/ShaderStage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/StackTracer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/Cubemap.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/Texture2D.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/TextureArray.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/TextureBuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/Texture.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Texture/TextureParser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Tools.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Transform.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Window.cpp
)

set(TABGRAPH_HEADER_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/include/Animation/AnimationChannel.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Animation/Animation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Animation/AnimationInterpolator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Animation/AnimationSampler.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Assets/AssetsParser.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/brdfLUT.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Buffer/BufferAccessor.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Buffer/BufferHelper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Buffer/Buffer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Buffer/BufferView.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Callback.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Camera/Camera.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Camera/FPSCamera.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Camera/OrbitCamera.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Common.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Component.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/ComputeObject.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Config.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Debug.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/DLLExport.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Engine.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Environment.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Framebuffer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Event/Signal.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Event/Events.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Event/GameController.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Event/InputDevice.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Event/Keyboard.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Event/Mouse.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Light/Light.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Light/DirectionnalLight.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Light/PointLight.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Light/SpotLight.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Material/Material.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Material/MaterialExtension.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Material/MetallicRoughness.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Material/SpecularGlossiness.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Mesh/CapsuleMesh.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Mesh/CubeMesh.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Mesh/Geometry.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Mesh/Mesh.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Mesh/MeshSkin.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Mesh/PlaneMesh.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Mesh/SphereMesh.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Node.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Object.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/BoundingAABB.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/BoundingBox.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/BoundingElement.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/BoundingMesh.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/BoundingPlane.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/BoundingSphere.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/CollisionAlgorithmSAT.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/CollisionAlgorithmGJKRaycast.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/IntersectFunctions.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/Intersection.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/PhysicsEngine.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/Ray.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/RigidBody.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Physics/SimplexSolver.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Quadtree.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Render.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Scene/Scene.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shader/GLUniformHelper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shader/Shader.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shader/ShaderExtension.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Shader/ShaderStage.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/StackTracer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/State.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/StateMachine.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Terrain/Terrain_GDAL.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Terrain/Terrain.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/Cubemap.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/Texture1D.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/Texture2D.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/TextureArray.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/TextureBuffer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/Texture.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/TextureParser.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Tools/Tools.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Transform.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Window.hpp
)

set(TABGRAPH_SHADER_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/blur.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/checkOpacity.glsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/compositing.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/deferred.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/deferred.vert
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/directionnalLight.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/forward.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/forward.vert
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/hzb.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/light.vert
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/material.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/metallicRoughness.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/passthrough.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/passthrough.vert
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/present.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/setInstanceID.glsl
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/specularGlossiness.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/ssao.frag
	${CMAKE_CURRENT_SOURCE_DIR}/shaders/SSR.frag
)

set(PLUGINS_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/BMP.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/BT.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/FBX/FBXDocument.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/FBX/FBXNode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/FBX/FBXObject.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/FBX/FBXProperty.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/FBX.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/GLSL.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/GLTF.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/HDR.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/MTLLIB.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/OBJ.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/tools.cpp
)

set(PLUGINS_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/BMP.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/FBX/FBXDocument.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/FBX/FBXNode.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/FBX/FBXObject.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/FBX/FBXProperty.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/FBX.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/GLSL.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/GLTF.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/HDR.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/MTLLIB.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/OBJ.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/InternalTools.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/Parser/TABSCENE.hpp
)

add_library(TabGraph STATIC ${TABGRAPH_SOURCE_FILES} ${TABGRAPH_HEADER_FILES} ${TABGRAPH_SHADER_FILES})

add_library(TabGraphPlugins OBJECT ${PLUGINS_SOURCES} ${PLUGINS_HEADERS})

target_include_directories(TabGraph PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/shaders"
	)
target_include_directories(TabGraph PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${ZLIB_INCLUDE_DIR}"
	)

target_include_directories(TabGraphPlugins PRIVATE
	"${RAPIDJSON_INCLUDE_DIR}"
	)
target_link_libraries(TabGraphPlugins PRIVATE TabGraph)
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message("${_variableName}=${${_variableName}}")
#endforeach()

target_link_libraries(TabGraph PUBLIC GLEW::GLEW)
target_link_libraries(TabGraph PUBLIC imagehlp)
target_link_libraries(TabGraph PUBLIC glm::glm)
target_link_libraries(TabGraph PUBLIC zlibstatic)
target_link_libraries(TabGraph PUBLIC SDL2::Core)
target_link_libraries(TabGraph PUBLIC SDL2::Image)

export(TARGETS TabGraph
       NAMESPACE "${CMAKE_PROJECT_NAME}-"
       APPEND FILE ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-targets.cmake)

#target_link_libraries(TabGraph ${RAPIDJSON_LIBRARIES})
#target_link_libraries(TabGraph ${GLEW_LIBRARIES})
#target_link_libraries(TabGraph ${ZLIB_LIBRARIES})

if(MSVC_IDE)
	set(CMAKE_DEBUG_POSTFIX "d")
	#source_group(Sources FILES ${TABGRAPH_SOURCE_FILES})
	#source_group(Include FILES ${TABGRAPH_HEADER_FILES})
	source_group(Shaders FILES ${TABGRAPH_SHADER_FILES})
	# Macro to preserve source files hierarchy in the IDE
	macro(GroupSources curdir)
		file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${curdir} ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/*)
	
		foreach(child ${children})
			if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/${child})
				GroupSources(${curdir}/${child})
			else()
				string(REPLACE "/" "\\" groupname ${curdir})
				string(REPLACE "src" "Sources" groupname ${groupname})
				string(REPLACE "include" "Includes" groupname ${groupname})
				#string(REPLACE "shaders" "Shaders" groupname ${groupname})
				source_group(${groupname} FILES ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/${child})
			endif()
		endforeach()
	endmacro()
	
	# Run macro
	GroupSources(src)
	GroupSources(include)
	#GroupSources(shaders)
endif()
