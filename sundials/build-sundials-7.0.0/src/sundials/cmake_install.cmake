# Install script for directory: /home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/src/sundials

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
  MESSAGE("
Install shared components
")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/whitehole/PycharmProjects/master_thesis/sundials/build-sundials-7.0.0/src/sundials/libsundials_core.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  include("/home/whitehole/PycharmProjects/master_thesis/sundials/build-sundials-7.0.0/src/sundials/CMakeFiles/sundials_core_static.dir/install-cxx-module-bmi-RelWithDebInfo.cmake" OPTIONAL)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libsundials_core.so.7.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libsundials_core.so.7"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "/opt/sundials/lib")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/build-sundials-7.0.0/src/sundials/libsundials_core.so.7.0.0"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/build-sundials-7.0.0/src/sundials/libsundials_core.so.7"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libsundials_core.so.7.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libsundials_core.so.7"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH ":::::::::::::::::"
           NEW_RPATH "/opt/sundials/lib")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/whitehole/PycharmProjects/master_thesis/sundials/build-sundials-7.0.0/src/sundials/libsundials_core.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  include("/home/whitehole/PycharmProjects/master_thesis/sundials/build-sundials-7.0.0/src/sundials/CMakeFiles/sundials_core_shared.dir/install-cxx-module-bmi-RelWithDebInfo.cmake" OPTIONAL)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/sundials" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_adaptcontroller.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_band.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_base.hpp"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_context.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_context.hpp"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_convertibleto.hpp"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_core.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_core.hpp"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_dense.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_direct.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_errors.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_iterative.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_linearsolver.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_linearsolver.hpp"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_logger.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_math.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_matrix.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_matrix.hpp"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_memory.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_memory.hpp"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_mpi_types.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_nonlinearsolver.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_nonlinearsolver.hpp"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_nvector.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_nvector.hpp"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_profiler.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_profiler.hpp"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_types_deprecated.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_types.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/sundials_version.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/sundials/priv" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/priv/sundials_context_impl.h"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/include/sundials/priv/sundials_errors_impl.h"
    )
endif()

