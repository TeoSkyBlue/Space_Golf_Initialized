# Install script for directory: C:/Users/TeoSkyBlue/source/repos/Space_Golf_Initialized/external/Simple-OpenGL-Image-Library

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/TeoSkyBlue/source/repos/Space_Golf_Initialized/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/TeoSkyBlue/source/repos/Space_Golf_Initialized/out/build/x64-Debug/external/Simple-OpenGL-Image-Library/SOIL_d.lib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SOIL" TYPE FILE FILES
    "C:/Users/TeoSkyBlue/source/repos/Space_Golf_Initialized/external/Simple-OpenGL-Image-Library/include/SOIL.h"
    "C:/Users/TeoSkyBlue/source/repos/Space_Golf_Initialized/external/Simple-OpenGL-Image-Library/include/image_DXT.h"
    "C:/Users/TeoSkyBlue/source/repos/Space_Golf_Initialized/external/Simple-OpenGL-Image-Library/include/image_helper.h"
    "C:/Users/TeoSkyBlue/source/repos/Space_Golf_Initialized/external/Simple-OpenGL-Image-Library/include/stb_image_aug.h"
    "C:/Users/TeoSkyBlue/source/repos/Space_Golf_Initialized/external/Simple-OpenGL-Image-Library/include/stbi_DDS_aug.h"
    "C:/Users/TeoSkyBlue/source/repos/Space_Golf_Initialized/external/Simple-OpenGL-Image-Library/include/stbi_DDS_aug_c.h"
    )
endif()

