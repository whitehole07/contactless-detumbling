# Install script for directory: /home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/sunlinsol/spgmr/serial

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/opt/sundials")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
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

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/sunlinsol/spgmr/serial/test_sunlinsol_spgmr_serial.c;/opt/sundials/examples/sunlinsol/spgmr/serial/test_sunlinsol.h;/opt/sundials/examples/sunlinsol/spgmr/serial/test_sunlinsol.c")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/sunlinsol/spgmr/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/sunlinsol/spgmr/serial/test_sunlinsol_spgmr_serial.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/sunlinsol/spgmr/serial/../../test_sunlinsol.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/sunlinsol/spgmr/serial/../../test_sunlinsol.c"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/sunlinsol/spgmr/serial/test_sunlinsol_spgmr_serial.c;/opt/sundials/examples/sunlinsol/spgmr/serial/test_sunlinsol.h;/opt/sundials/examples/sunlinsol/spgmr/serial/test_sunlinsol.c")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/sunlinsol/spgmr/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/sunlinsol/spgmr/serial/test_sunlinsol_spgmr_serial.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/sunlinsol/spgmr/serial/../../test_sunlinsol.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/sunlinsol/spgmr/serial/../../test_sunlinsol.c"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/sunlinsol/spgmr/serial/test_sunlinsol_spgmr_serial.c;/opt/sundials/examples/sunlinsol/spgmr/serial/test_sunlinsol.h;/opt/sundials/examples/sunlinsol/spgmr/serial/test_sunlinsol.c")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/sunlinsol/spgmr/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/sunlinsol/spgmr/serial/test_sunlinsol_spgmr_serial.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/sunlinsol/spgmr/serial/../../test_sunlinsol.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/sunlinsol/spgmr/serial/../../test_sunlinsol.c"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/sunlinsol/spgmr/serial/test_sunlinsol_spgmr_serial.c;/opt/sundials/examples/sunlinsol/spgmr/serial/test_sunlinsol.h;/opt/sundials/examples/sunlinsol/spgmr/serial/test_sunlinsol.c")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/sunlinsol/spgmr/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/sunlinsol/spgmr/serial/test_sunlinsol_spgmr_serial.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/sunlinsol/spgmr/serial/../../test_sunlinsol.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/sunlinsol/spgmr/serial/../../test_sunlinsol.c"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/sunlinsol/spgmr/serial/CMakeLists.txt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/sunlinsol/spgmr/serial" TYPE FILE FILES "/home/whitehole/PycharmProjects/master_thesis/sundials/build-sundials-7.0.0/examples/sunlinsol/spgmr/serial/CMakeLists.txt")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/sunlinsol/spgmr/serial/Makefile")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/sunlinsol/spgmr/serial" TYPE FILE RENAME "Makefile" FILES "/home/whitehole/PycharmProjects/master_thesis/sundials/build-sundials-7.0.0/examples/sunlinsol/spgmr/serial/Makefile_ex")
endif()

