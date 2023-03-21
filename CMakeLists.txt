###############################################################################
cmake_minimum_required(VERSION 3.0)
project(LIGHT_BLUEs_SPACE_GOLF)

###############################################################################

find_package(OpenGL REQUIRED)


set (CMAKE_CXX_STANDARD 17)
# c++11, -g option is used to export debug symbols for gdb
if(${CMAKE_CXX_COMPILER_ID} MATCHES GNU OR
    ${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  # Using C++17 on OSX requires using libc++ instead of libstd++.
  # libc++ is an implementation of the C++ standard library for OSX.
  if(APPLE)
    if(XCODE)
      set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17")
      set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -stdlib=libc++")
    endif()
  else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g")
  endif()
endif()



# define library postfix so that different builds will produce
# distinguished libraries
set(CMAKE_RELEASE_POSTFIX "_r" CACHE string "Release postfix")
set(CMAKE_DEBUG_POSTFIX "_d" CACHE string "Debug postfix")
set(CMAKE_RELWITHDEBINFO_POSTFIX "_rd" CACHE string
  "Release with debug info postfix")
set(CMAKE_MINSIZEREL_POSTFIX "_mr" CACHE string
  "Minimum size release postfix")
mark_as_advanced(CMAKE_RELEASE_POSTFIX)
mark_as_advanced(CMAKE_DEBUG_POSTFIX)
mark_as_advanced(CMAKE_RELWITHDEBINFO_POSTFIX)
mark_as_advanced(CMAKE_MINSIZEREL_POSTFIX)

# for rdm (emacs)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# for grouping into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
  message(FATAL_ERROR "Please select another Build Directory ! (and give it a
  clever name, like bin_Visual2012_64bits/)")
endif()
if(CMAKE_SOURCE_DIR MATCHES " ")
  message( "Your Source Directory contains spaces. If you experience problems
    when compiling, this can be the cause." )
endif()
if(CMAKE_BINARY_DIR MATCHES " ")
  message("Your Build Directory contains spaces. If you experience problems
  when compiling, this can be the cause.")
endif()

# Compile external dependencies
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

###############################################################################

########### Dear ImGui import code
set(IMGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
add_library(imgui 
        ${IMGUI_PATH}/imgui.cpp
        ${IMGUI_PATH}/imgui_draw.cpp
        ${IMGUI_PATH}/imgui_demo.cpp
        ${IMGUI_PATH}/imgui_tables.cpp
        ${IMGUI_PATH}/imgui_widgets.cpp
        ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp
        ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp)
target_compile_definitions(imgui PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GLEW)
target_include_directories(imgui PUBLIC ${IMGUI_PATH}/ ${IMGUI_PATH}/backends/)

target_link_libraries(imgui GLEW_1130 glfw)

################################################################################

include_directories(
  external/glfw-3.2.1/include/GLFW/
  external/glm-0.9.7.1/
  external/glew-1.13.0/include/
  external/Simple-OpenGL-Image-Library/include/
  external/tinyxml2/include/
  external/tinyobjloader/include/
  .
  )

set(ALL_LIBS
  ${OPENGL_LIBRARY}
  glfw
  GLEW_1130
  SOIL
  TINYXML2
  imgui
  )

add_definitions(
  -DTW_STATIC
  -DTW_NO_LIB_PRAGMA
  -DTW_NO_DIRECT3D
  -DGLEW_STATIC
  -D_CRT_SECURE_NO_WARNINGS
  )

###############################################################################
# Space_Golf_Initialized
add_executable(Space_Golf_Initialized
  Space_Golf_Initialized/Space_Golf_Initialized.cpp

  common/util.cpp
  common/util.h
  common/shader.cpp
  common/shader.h
  common/camera.cpp
  common/camera.h
  common/model.cpp
  common/model.h
  common/texture.cpp
  common/texture.h
  common/Collision.cpp
  common/Collision.h
  common/RigidBody.cpp
  common/RigidBody.h
  common/Sphere.cpp
  common/Sphere.h
  common/Light.cpp
  common/Light.h
  common/IntParticleEmitter.cpp
  common/IntParticleEmitter.h
  common/FountainEmitter.cpp
  common/FountainEmitter.h
  


  Space_Golf_Initialized/Lit_texture.frag
  Space_Golf_Initialized/Lit_texture.vert
  Space_Golf_Initialized/instanced_Items.vert
  Space_Golf_Initialized/instanced_Items.frag
  Space_Golf_Initialized/Depth.vert
  Space_Golf_Initialized/Depth.frag
  Space_Golf_Initialized/Depth_instanced.vert
  Space_Golf_Initialized/ShadowMapping.frag
  Space_Golf_Initialized/ShadowMapping.vert
  Space_Golf_Initialized/SimpleTexture.frag
  Space_Golf_Initialized/SimpleTexture.vert
  
  
  
  
  
  )
target_link_libraries(Space_Golf_Initialized
  ${ALL_LIBS}
  )
# Xcode and Visual working directories
set_target_properties(Space_Golf_Initialized
  PROPERTIES
  XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Space_Golf_Initialized/"
  PROJECT_LABEL "Space_Golf_Initialized"
  FOLDER "Exercise"
  )
create_target_launcher(Space_Golf_Initialized WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Space_Golf_Initialized/")
create_default_target_launcher(Space_Golf_Initialized WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Space_Golf_Initialized/")

###############################################################################

SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
SOURCE_GROUP(shaders_vert REGULAR_EXPRESSION ".*/*.vert$" )
SOURCE_GROUP(shaders_frag REGULAR_EXPRESSION ".*/*.frag$" )
SOURCE_GROUP(obj REGULAR_EXPRESSION ".*/.*obj$" )

###############################################################################
# copy
if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

  add_custom_command(
    TARGET Space_Golf_Initialized POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Space_Golf_Initialized${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/Space_Golf_Initialized/"
    )
    
elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
