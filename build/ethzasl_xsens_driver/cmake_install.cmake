# Install script for directory: /home/ajwahir/grmc/catkin_ws_black/src/ethzasl_xsens_driver

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/ajwahir/grmc/catkin_ws_black/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/ajwahir/grmc/catkin_ws_black/build/ethzasl_xsens_driver/catkin_generated/installspace/xsens_driver.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/xsens_driver/cmake" TYPE FILE FILES
    "/home/ajwahir/grmc/catkin_ws_black/build/ethzasl_xsens_driver/catkin_generated/installspace/xsens_driverConfig.cmake"
    "/home/ajwahir/grmc/catkin_ws_black/build/ethzasl_xsens_driver/catkin_generated/installspace/xsens_driverConfig-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/xsens_driver" TYPE FILE FILES "/home/ajwahir/grmc/catkin_ws_black/src/ethzasl_xsens_driver/package.xml")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/xsens_driver" TYPE PROGRAM FILES
    "/home/ajwahir/grmc/catkin_ws_black/src/ethzasl_xsens_driver/nodes/mtnode.py"
    "/home/ajwahir/grmc/catkin_ws_black/src/ethzasl_xsens_driver/nodes/mtdef.py"
    "/home/ajwahir/grmc/catkin_ws_black/src/ethzasl_xsens_driver/nodes/mtdevice.py"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/xsens_driver" TYPE DIRECTORY FILES "/home/ajwahir/grmc/catkin_ws_black/src/ethzasl_xsens_driver/launch")
endif()

