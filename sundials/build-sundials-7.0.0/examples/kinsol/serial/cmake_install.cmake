# Install script for directory: /home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial

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
   "/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.c;/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.c;/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.c;/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.c;/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.c;/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.c;/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.c;/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.c;/opt/sundials/examples/kinsol/serial/kinAnalytic_fp.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinAnalytic_fp.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinFerTron_dns.c;/opt/sundials/examples/kinsol/serial/kinFerTron_dns.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinFerTron_dns.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinFerTron_dns.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinFoodWeb_kry.c;/opt/sundials/examples/kinsol/serial/kinFoodWeb_kry.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinFoodWeb_kry.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinFoodWeb_kry.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinKrylovDemo_ls.c;/opt/sundials/examples/kinsol/serial/kinKrylovDemo_ls.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinKrylovDemo_ls.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinKrylovDemo_ls.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinLaplace_bnd.c;/opt/sundials/examples/kinsol/serial/kinLaplace_bnd.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinLaplace_bnd.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinLaplace_bnd.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinLaplace_picard_bnd.c;/opt/sundials/examples/kinsol/serial/kinLaplace_picard_bnd.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinLaplace_picard_bnd.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinLaplace_picard_bnd.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinLaplace_picard_kry.c;/opt/sundials/examples/kinsol/serial/kinLaplace_picard_kry.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinLaplace_picard_kry.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinLaplace_picard_kry.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinRoberts_fp.c;/opt/sundials/examples/kinsol/serial/kinRoberts_fp.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinRoberts_fp.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinRoberts_fp.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinRoboKin_dns.c;/opt/sundials/examples/kinsol/serial/kinRoboKin_dns.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinRoboKin_dns.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinRoboKin_dns.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/README")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/README")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/kinRoboKin_dns_stats.csv")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/kinsol/serial/kinRoboKin_dns_stats.csv")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/CMakeLists.txt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE FILES "/home/whitehole/PycharmProjects/master_thesis/sundials/build-sundials-7.0.0/examples/kinsol/serial/CMakeLists.txt")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/kinsol/serial/Makefile")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/kinsol/serial" TYPE FILE RENAME "Makefile" FILES "/home/whitehole/PycharmProjects/master_thesis/sundials/build-sundials-7.0.0/examples/kinsol/serial/Makefile_ex")
endif()

