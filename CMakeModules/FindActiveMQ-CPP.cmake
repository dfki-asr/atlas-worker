# - Find ActiveMQ-CPP 
#
# Once done this will define
#  ActiveMQ-CPP_FOUND - System has APR 
#  ActiveMQ-CPP_INCLUDE_DIRS - The APR include directories
#  ActiveMQ-CPP_LIBRARIES - The libraries needed to use APR 
#  ActiveMQ-CPP_DEFINITIONS - Compiler switches required for using APR

include(LibFindMacros)

libfind_package(ActiveMQ-CPP APR)
libfind_pkg_check_modules(ActiveMQ-CPP_PKGCONF activemq-cpp)

find_path(ActiveMQ-CPP_INCLUDE_DIR cms/Config.h
          DOC "ActiveMQ-CPP include files"
          HINTS ${ActiveMQ-CPP_PKGCONF_INCLUDE_DIRS})

find_library(ActiveMQ-CPP_LIBRARY NAMES activemq-cpp
             DOC "ActiveMQ-CPP library file"
             HINTS ${ActiveMQ-CPP_PKGCONF_LIBRARY_DIRS} )

libfind_process(ActiveMQ-CPP)
