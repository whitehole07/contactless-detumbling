# Install script for directory: /home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial

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
   "/opt/sundials/examples/ida/serial/idaAnalytic_mels.c;/opt/sundials/examples/ida/serial/idaAnalytic_mels.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/ida/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaAnalytic_mels.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaAnalytic_mels.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/ida/serial/idaFoodWeb_bnd.c;/opt/sundials/examples/ida/serial/idaFoodWeb_bnd.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/ida/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaFoodWeb_bnd.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaFoodWeb_bnd.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/ida/serial/idaFoodWeb_kry.c;/opt/sundials/examples/ida/serial/idaFoodWeb_kry.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/ida/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaFoodWeb_kry.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaFoodWeb_kry.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/ida/serial/idaHeat2D_bnd.c;/opt/sundials/examples/ida/serial/idaHeat2D_bnd.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/ida/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaHeat2D_bnd.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaHeat2D_bnd.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/ida/serial/idaHeat2D_kry.c;/opt/sundials/examples/ida/serial/idaHeat2D_kry.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/ida/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaHeat2D_kry.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaHeat2D_kry.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/ida/serial/idaKrylovDemo_ls.c;/opt/sundials/examples/ida/serial/idaKrylovDemo_ls.out;/opt/sundials/examples/ida/serial/idaKrylovDemo_ls_1.out;/opt/sundials/examples/ida/serial/idaKrylovDemo_ls_2.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/ida/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaKrylovDemo_ls.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaKrylovDemo_ls.out"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaKrylovDemo_ls_1.out"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaKrylovDemo_ls_2.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/ida/serial/idaKrylovDemo_ls.c;/opt/sundials/examples/ida/serial/idaKrylovDemo_ls.out;/opt/sundials/examples/ida/serial/idaKrylovDemo_ls_1.out;/opt/sundials/examples/ida/serial/idaKrylovDemo_ls_2.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/ida/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaKrylovDemo_ls.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaKrylovDemo_ls.out"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaKrylovDemo_ls_1.out"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaKrylovDemo_ls_2.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/ida/serial/idaKrylovDemo_ls.c;/opt/sundials/examples/ida/serial/idaKrylovDemo_ls.out;/opt/sundials/examples/ida/serial/idaKrylovDemo_ls_1.out;/opt/sundials/examples/ida/serial/idaKrylovDemo_ls_2.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/ida/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaKrylovDemo_ls.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaKrylovDemo_ls.out"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaKrylovDemo_ls_1.out"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaKrylovDemo_ls_2.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/ida/serial/idaRoberts_dns.c;/opt/sundials/examples/ida/serial/idaRoberts_dns.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/ida/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaRoberts_dns.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaRoberts_dns.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/ida/serial/idaSlCrank_dns.c;/opt/sundials/examples/ida/serial/idaSlCrank_dns.out")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/ida/serial" TYPE FILE FILES
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaSlCrank_dns.c"
    "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaSlCrank_dns.out"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/ida/serial/README")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/ida/serial" TYPE FILE FILES "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/README")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/ida/serial/idaRoberts_dns_stats.csv")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/ida/serial" TYPE FILE FILES "/home/whitehole/PycharmProjects/master_thesis/sundials/sundials-7.0.0/examples/ida/serial/idaRoberts_dns_stats.csv")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/ida/serial/CMakeLists.txt")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/ida/serial" TYPE FILE FILES "/home/whitehole/PycharmProjects/master_thesis/sundials/build-sundials-7.0.0/examples/ida/serial/CMakeLists.txt")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/opt/sundials/examples/ida/serial/Makefile")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/opt/sundials/examples/ida/serial" TYPE FILE RENAME "Makefile" FILES "/home/whitehole/PycharmProjects/master_thesis/sundials/build-sundials-7.0.0/examples/ida/serial/Makefile_ex")
endif()

